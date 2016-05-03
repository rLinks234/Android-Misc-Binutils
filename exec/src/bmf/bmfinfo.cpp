#include "bmf.h"
#include "bitmap_file_fmt.h"

#define VERSION_STR "1.00"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"

using namespace std;

////////////////////////////////

/**
 * 
 */


class BmfStub {

private:

	FontCacheHeader header;

	string mFontName;
	vector<GlyphEntry> mGlyphEntries;
	vector<uint8_t> mBitmapData;	

	void editGlyphBounds(GlyphEntry& pGlyph);

public:

	void editFringePixels();
	void printBmfData();
	void writeBitmapToFile(const string& pOutput);

	static boost::shared_ptr<BmfStub> readBmf(const string& pFile);

};

#define BOUNDS_COLOR 100

////////////////////////////////

static boost::shared_ptr<FontCacheHeader> readOnlyHeader(FILE* pFile) {

	fseek(pFile, 0L, SEEK_END);
	const uint32_t input_file_length = static_cast<uint32_t>(ftell(pFile));

	if (input_file_length <= sizeof(FontCacheHeader) ) {

		print_errorln("Invalid input file (input_file_length <= sizeof(FontCacheHeader) )");
		return nullptr;

	}

	boost::shared_ptr<FontCacheHeader> header(new FontCacheHeader());

	rewind(pFile);
	fread(header.get(), sizeof(FontCacheHeader), 1, pFile);

	return header;

}

static void printHeader(FontCacheHeader& header) {

	printf("	magic			0x%08x [%u]\n", header.magic, header.magic);
	printf("	mMajorVersion		0x%08x [%u]\n", header.mMajorVersion, header.mMajorVersion);
	printf("	mMinorVersion		0x%08x [%u]\n", header.mMinorVersion, header.mMinorVersion);
	printf("	mID			0x%08x [%u]\n", header.mID, header.mID);
	printf("	mFlags			0x%8lx [%lu]\n", header.mFlags, header.mFlags);
	printf("	mNumberOfGlyphs		0x%08x [%u]\n", header.mNumberOfGlyphs, header.mNumberOfGlyphs);
	printf("	mBitmapWidth		0x%08x [%u]\n", header.mBitmapWidth, header.mBitmapWidth);
	printf("	mBitmapHeight		0x%08x [%u]\n", header.mBitmapHeight, header.mBitmapHeight);	
	printf("	mBytesPerPixel		0x%08x [%u]\n", header.mBytesPerPixel, header.mBytesPerPixel);
	printf("	mGlyphFontSize		0x%08x [%u]\n", header.mGlyphFontSize, header.mGlyphFontSize);
	printf("	mMaxAdvance		0x%08x [%u]\n", header.mMaxAdvance, header.mMaxAdvance);
	printf("	mDpi			0x%08x [%u]\n", header.mDpi, header.mDpi);

}

////////////////////////////////

void BmfStub::printBmfData() {

	printf("\nHeader Info:\n");
	printHeader(header);
	printf("\nFont Name: \"%s\"\n", mFontName.c_str());

}

boost::shared_ptr<BmfStub> BmfStub::readBmf(const string& pFile) {

	FILE* tFile = fopen(pFile.c_str(), "rb");

	if (!tFile) {
		print_errorln("BmfStub::readBmf() failed to read from file `%s` with error: %s", pFile.c_str(), strerror(errno));
	}

	boost::shared_ptr<BmfStub> stub(new BmfStub());

	// get input file size
	fseek(tFile, 0L, SEEK_END);
	const uint32_t input_file_length = static_cast<uint32_t>(ftell(tFile));

	if (input_file_length <= sizeof(FontCacheHeader) ) {

		print_errorln("Invalid input file (input_file_length <= sizeof(FontCacheHeader) )");
		fclose(tFile);

		return nullptr;

	}

	rewind(tFile);
	fread(&(stub->header), sizeof(FontCacheHeader), 1, tFile);

	////////////////
	// Get font name

	const uint32_t strNameBegin = sizeof(FontCacheHeader);
	uint32_t strNameLength = 0;
	char strLetter;

	do {

		fread(&strLetter, 1, 1, tFile);
		strNameLength++;

	} while(strLetter != '\0');

	fseek(tFile, strNameBegin, SEEK_SET);

	char* fontStrName = new char[strNameLength];
	fread(fontStrName, 1, strNameLength, tFile);

	stub->mFontName = fontStrName;

	delete[] fontStrName;

	const uint32_t bitmapBufferLength = ( stub->header.mBitmapWidth ) * ( stub->header.mBitmapHeight );
	const uint32_t numberOfGlyphs = stub->header.mNumberOfGlyphs;

	stub->mGlyphEntries.resize(numberOfGlyphs);
	stub->mBitmapData.resize(bitmapBufferLength, 0);

	fread( stub->mGlyphEntries.data(), sizeof(GlyphEntry), numberOfGlyphs, tFile);
	fread( stub->mBitmapData.data(), 1, bitmapBufferLength, tFile);

	fclose(tFile);

	return stub;

}

void BmfStub::editGlyphBounds(GlyphEntry& pGlyph) {

	const uint32_t startX = pGlyph.mBLx;
	const uint32_t startY = pGlyph.mTRy;
	const uint32_t stopX = pGlyph.mTRx;
	const uint32_t stopY = pGlyph.mBLy;

	const uint32_t width = header.mBitmapWidth;
	const uint32_t pxCount = ( header.mBitmapWidth * header.mBitmapHeight );

	const uint32_t topY = pxCount - (width * startY);
	const uint32_t bottomY = pxCount - (width * stopY);
	const uint32_t leftX = pxCount + startX;
	const uint32_t rightX = pxCount + stopX;

	// Horizontal Lines
	for (uint32_t i = startX, j = 0; i < stopX; i++, j++) {

		if (j % 3 == 0) {
			continue;
		} else {

			mBitmapData[topY + i] = BOUNDS_COLOR;
			mBitmapData[bottomY + i] = BOUNDS_COLOR;

		}

	}

	// Vertical Lines
	for (uint32_t i = startY, j = 0; i < stopY; i++, j++) {

		const uint32_t w = i * width;

		if (j % 3 == 0) {
			continue;
		} else {

			mBitmapData[leftX - w] = BOUNDS_COLOR;
			mBitmapData[rightX - w] = BOUNDS_COLOR;

		}

	}

}

void BmfStub::editFringePixels() {

	for (GlyphEntry& glyph : mGlyphEntries) {
		editGlyphBounds(glyph);
	}

}

void BmfStub::writeBitmapToFile(const string& pOutput) {
	writeToBitmap(mBitmapData.data(), header.mBitmapWidth, header.mBitmapHeight, pOutput.c_str());
}

////////////////////////////////

#define USAGE_EXTRA_STR "If no input argument is provided " 				\
						"(i.e. `-i` or `--input`) then\nthe first "			\
						"provided argument proceeding a flag argument\n" 	\
						"will be interpreted as the input argument\n\n"

static void process_g(const string& pInputFile, const string& pOutputFile, bool pShowGlyphBoundaries) {

	boost::shared_ptr<BmfStub> inputBmf = BmfStub::readBmf(pInputFile);

	if (!inputBmf) {
		return;
	}

	inputBmf->printBmfData();

	if (pShowGlyphBoundaries) {
		inputBmf->editFringePixels();
	}

	inputBmf->writeBitmapToFile(pOutputFile);

}

static void process_f(const string& pInputFile) {

	FILE* input_file = fopen(pInputFile.c_str(), "rb");

	if (!input_file) {

		print_errorln("process_f() failed to read from file `%s` with error: %s", 
			pInputFile.c_str(), strerror(errno));
		return;

	}

	boost::shared_ptr<FontCacheHeader> header = readOnlyHeader(input_file);

	if (!header) {

		print_errorln("Couldn't read header");
		fclose(input_file);

	}

	printHeader( *(header.get()) );

	fclose(input_file);

}

static int32_t dispatch(po::options_description& usage, po::variables_map& args) {

	bool generate = args["generate"].as<bool>();
	bool header = args["header"].as<bool>();

	if (header && generate) {

		print_errorln("cannot select both `-g` and `-f`");
		printUsage(usage, USAGE_EXTRA_STR);

		return 2;

	} else if (!(header || generate)) {

		print_errorln("must provide `-g` or `-f`");
		printUsage(usage, USAGE_EXTRA_STR);

		return 2;

	}

	const string& inputFile = args["input"].as<string>();

	if (generate) {

		bool provided_output = args.count("output") > 0;
		string outputFile;

		if (provided_output) {
			outputFile = args["output"].as<string>();
		} else {
			outputFile = "out.bmp";
		}

		process_g(inputFile, outputFile, args["show-bounds"].as<bool>());

	} else if (header) {
		process_f(inputFile);
	}

}

////////////////////////////////

int main(int argc, char** argv) {

	po::options_description general("General Options");
	general.add_options()
		("verbose,v", boost::program_options::bool_switch()->default_value(false), "Print extra info")
		("generate,g", boost::program_options::bool_switch()->default_value(false), "Generate bitmap from input .bmf file")
		("header,f", boost::program_options::bool_switch()->default_value(false), "Read .bmf header and exit")
		("show-bounds,s", boost::program_options::bool_switch()->default_value(false), "Show glyph boundaries (valid with -g)")
		("output,o", po::value<string>(), "output file")
		("input,i", po::value<string>()->required(), "input file (Required)")
	;

	try {

		po::options_description help = getHelpDescription();
		po::variables_map help_vm;

		po::command_line_parser help_parser(argc, argv);
		help_parser.options(help).allow_unregistered();

		po::basic_parsed_options<char> help_parsed = help_parser.run();

		store(help_parsed, help_vm);

		if (hasHelpOptions(help_vm)) {

			printUsage(general, USAGE_EXTRA_STR);
			return 0;

		} else if (hasVersion(help_vm)) {

			printf("%s\n", VERSION_STR);
			return 0;

		}

		po::options_description hidden("Hidden Options");
		hidden.add_options()
			("TODO", po::value< vector<string> >(), "IDK yet")
		;

		po::positional_options_description positional;
		positional.add("input", 1);

		po::options_description cmdline_options;
		cmdline_options.add(general).add(hidden);

		po::command_line_parser parser(argc, argv);
		parser.options(cmdline_options).allow_unregistered();

		po::basic_parsed_options<char> parsed = parser.run();
		po::variables_map vm;

		store(parsed, vm);

		notify(vm);

		dispatch(general, vm);

	} catch (po::error& perr) {

		print_errorln("\nError: %s", perr.what());
		printUsage(general, USAGE_EXTRA_STR);

		return 1;

	} catch(exception& e) {

		print_errorln("\n%s", e.what());
		return 1;

	} catch(boost::exception & ex) {

		print_errorln("\n%s", boost::diagnostic_information(ex).c_str());
		return 2;

	} catch (...) {

		print_errorln("\nCaught unknown exception...");
		return 4;

	}

	return 0;

}

#pragma clang diagnostic pop
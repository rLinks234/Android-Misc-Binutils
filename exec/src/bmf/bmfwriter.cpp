#include "ft2build.h"
#include "freetype/freetype.h"
#include FT_GLYPH_H

#include "bmf.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"

#define VERSION_STR "1.00"
#define MINIMUM_FONT_SIZE 4
#define DPI_DEFAULT 320
#define MIN_DPI 60
#define PIXEL_PADDING 4

static const vector<uint16_t> bmp_sizes = { 
	64, 96, 128, 192, 224, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096 
};

static const vector<uint16_t> bmp_2n_sizes = { 
	64, 128, 256, 512, 1024, 2048, 4096 
};


static const char* ASCII_LIST = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
static const string ttf_extension = ".ttf";

using namespace std;

////////////////////////////////

static int32_t try_get_dpi() {

	FILE* fp = popen("getprop ro.sf.lcd_density 2>/dev/null", "r");

	if (!fp) {
		return DPI_DEFAULT;
	}

	int32_t value = 0;
	fscanf(fp, "%d", &value);

	if (value <= 0) {
		value = DPI_DEFAULT;
	} else if (value < MIN_DPI) {
		value = MIN_DPI;
	}

	pclose(fp);

	return value;

}

static uint32_t hash_name(string& pString) {

	uint32_t hash = 5381;
	int32_t c;

	for (char& c : pString) {
		hash = ((hash << 5) + hash) + static_cast<int32_t>(c); // hash * 33 + c
	}

	return hash;

}

static bool is_digit(char pCharacter) {
	return (pCharacter >= '0') && (pCharacter <= '9');
}

static void print_stats(string& pFileName, uint32_t pFileLength, uint32_t pGlyphEntriesOffset, uint32_t pBitmapDataOffset) {

	printf("\nFile Name:			%s\n", pFileName.c_str());
	printf("File Size:			0x%08x [%u]\n", pFileLength, pFileLength);
	printf("Glyph Data offset:		0x%08x [%u]\n", pGlyphEntriesOffset, pGlyphEntriesOffset);
	printf("Bitmap Data offset:		0x%08x [%u]\n\n", pBitmapDataOffset, pBitmapDataOffset);

}

static void repl_out_if_needed(string& pInputFile, string& pOutputFile) {

	if ( (pOutputFile.length() == 1) && pOutputFile[0] == '.' ) {

		size_t indx = pInputFile.find_last_of('.');

		pOutputFile.assign(pInputFile, 0, indx);
		pOutputFile += ".bmf";

	}

}

static bool is_ttf(string const & pValue) {

	if (ttf_extension.size() > pValue.size()) {
    	return false;	
    }

	return std::equal(ttf_extension.rbegin(), ttf_extension.rend(), pValue.rbegin());

}

////////////////////////////////

class BmfWriter;
class BmfWriterTask;
class BackgroundWriterTask;

class BmfWriter final {

	bool isVerbose;
	bool bmpOnly2n;
	bool packBmps;

	uint16_t glyphFontSize;
	uint16_t dpi;
	uint32_t threads;

	vector<BmfWriterTask> jobs;
	FT_Library ftLibrary;

public:

	inline void set_powerOfTwoBitmapDimensions(bool pValue) {
		bmpOnly2n = pValue;
	}

	inline void set_packBitmaps(bool pValue) {
		packBmps = pValue;
	}

	inline bool verbose() const {
		return isVerbose;
	}

	inline bool powerOfTwoBitmapDimensions() const {
		return bmpOnly2n;
	}

	inline bool packBitmaps() const {
		return packBmps;
	}

	inline uint32_t numberOfThreads() const {
		return threads;
	}

	inline uint16_t getFontSize() const {
		return glyphFontSize;
	}

	inline uint16_t getDpi() const {
		return dpi;
	}

	inline FT_Library getLibrary() const {
		return ftLibrary;
	}

	void addJob(const string& pInput, const string& pOutput);
	int32_t run();

	BmfWriter(bool pVerbose, uint16_t pGlyphFontSize, uint16_t pDpi, uint32_t pNumberOfThreads)
		: isVerbose(pVerbose), bmpOnly2n(false), packBmps(false), glyphFontSize(pGlyphFontSize), dpi(pDpi), threads(pNumberOfThreads) {

		FT_Error error = FT_Init_FreeType( &ftLibrary );

		if(error) {
			print_errorln("FT_Init_FreeType() returned %d", error);
		}

	}

};

class BmfWriterTask {

	FT_Face ftFace;
	boost::shared_ptr<const BmfWriter> instance;

	string inputFile;
	string outputFile;

	vector<GlyphEntry> glyphs;
	vector<uint8_t> bitmap;

	uint16_t bitmap_width;
	uint16_t bitmap_height;

	uint16_t max_advance;

public:

	inline bool verbose() {
		return instance->verbose();
	}

	int32_t run();
	void renderToBitmap(uint16_t pGlyph, uint32_t pGlyphIndex, uint32_t& pCursorX, uint32_t& pCursorY);
	void writeToFile();
	void packBitmap(uint16_t pNewBitmapHeight);

	BmfWriterTask(
		FT_Face pFace, boost::shared_ptr<const BmfWriter> pInstance, const string& pInputFile, const string& pOutputFile
	)
		: ftFace(pFace), instance(pInstance), inputFile(pInputFile), outputFile(pOutputFile), 
		bitmap_width(0), bitmap_height(0) {

			if (!is_ttf(pInputFile)) {
				print_warnln("Warning: input file `%s` is an unknown font file", pInputFile.c_str());
			}

			repl_out_if_needed(inputFile, outputFile);

		}

};

class BackgroundWriterTask {

	friend class BmfWriter;

	boost::shared_ptr<const BmfWriter> instance;

	boost::shared_ptr<sem_t> semaphore;
	boost::shared_ptr<pthread_cond_t> cond;
	boost::shared_ptr<pthread_mutex_t> initial_mutex;

	vector<BmfWriterTask*> args;

	void backgroundRoutine();

public:

	inline void setContext(const BmfWriter* pInstance) {
		instance = boost::shared_ptr<const BmfWriter>(pInstance);
	}

	inline void addTask(BmfWriterTask* pTask) {
		args.push_back(pTask);
	}

	inline std::thread run() {
		return std::thread( 
			[=] { backgroundRoutine(); } // Lambda expression
		); 
	}

	BackgroundWriterTask() {}

};

////////////////////////////////

// BmfWriter

void BmfWriter::addJob(const string& pInput, const string& pOutput) {

	FT_Face thzFace;

	FT_New_Face( ftLibrary, pInput.c_str(), 0, &thzFace );

	jobs.push_back(
		BmfWriterTask(thzFace, boost::shared_ptr<const BmfWriter>(this), pInput, pOutput)
	);

}

int32_t BmfWriter::run() {

	const uint32_t thread_count = numberOfThreads();
	const uint32_t jobCount = jobs.size();

	// Run parallel
	if (thread_count > 0) {

		const uint32_t threads_to_spawn = thread_count > jobCount ? jobCount : thread_count;
		
		if (verbose()) {
			print_verboseln("spawning %u worker threads", threads_to_spawn);
		}

		pthread_mutex_t lock;
		boost::shared_ptr<pthread_cond_t> cond( new pthread_cond_t );
		boost::shared_ptr<sem_t> sem( new sem_t );

		pthread_mutex_init(&lock, nullptr);
		pthread_cond_init(cond.get(), nullptr);

		vector<BackgroundWriterTask> args = vector<BackgroundWriterTask>(threads_to_spawn);
		vector< boost::shared_ptr<pthread_mutex_t> > initialMutexes = vector< boost::shared_ptr<pthread_mutex_t> >(threads_to_spawn);

		uint32_t upper_ct = jobCount / threads_to_spawn;
		uint32_t lower_ct = upper_ct;

		const uint32_t difference = jobCount % threads_to_spawn;

		if (difference) {
			upper_ct++;
		}

		if (upper_ct == lower_ct) {

			for (uint32_t i = 0; i < threads_to_spawn; i++) {

				BackgroundWriterTask& task = args[i];

				task.setContext(this);

				task.args = vector<BmfWriterTask*>(upper_ct);
				task.semaphore = sem;
				task.cond = cond;

				boost::shared_ptr<pthread_mutex_t> init_mutex( new pthread_mutex_t );
				
				pthread_mutex_init(init_mutex.get(), nullptr);
				pthread_mutex_lock(init_mutex.get());

				initialMutexes.push_back(init_mutex);

				task.initial_mutex = init_mutex;
				
				for (uint32_t j = i; j < upper_ct; j++) {
					task.addTask( &(jobs[i * threads_to_spawn]) );
				}

				std::thread t = task.run();

			}

		} else {

			for (uint32_t i = 0; i < difference; i++) {

				BackgroundWriterTask& task = args[i];

				task.setContext(this);

				task.args = vector<BmfWriterTask*>(upper_ct);
				task.semaphore = sem;
				task.cond = cond;
				
				boost::shared_ptr<pthread_mutex_t> init_mutex( new pthread_mutex_t );

				pthread_mutex_init(init_mutex.get(), nullptr);
				pthread_mutex_lock(init_mutex.get());

				initialMutexes.push_back(init_mutex);

				task.initial_mutex = init_mutex;

				for (uint32_t j = 0; j < upper_ct; j++) {
					task.addTask( &(jobs[j * threads_to_spawn + i]) );
				}

				std::thread t = task.run();

			}

			for (uint32_t i = difference; i < threads_to_spawn; i++) {

				BackgroundWriterTask& task = args[i];

				task.setContext(this);

				task.args = vector<BmfWriterTask*>(lower_ct);
				task.semaphore = sem;
				task.cond = cond;
				
				boost::shared_ptr<pthread_mutex_t> init_mutex( new pthread_mutex_t );

				pthread_mutex_init(init_mutex.get(), nullptr);
				pthread_mutex_lock(init_mutex.get());

				initialMutexes.push_back(init_mutex);

				task.initial_mutex = init_mutex;

				for (uint32_t j = 0; j < lower_ct; j++) {
					task.addTask( &(jobs[j * threads_to_spawn + i]) );
				}

				std::thread t = task.run();

			}

		}

		// Allow all background threads to begin
		for (boost::shared_ptr<pthread_mutex_t>& init_mutex : initialMutexes) {
			pthread_mutex_unlock(init_mutex.get());
		}

		// clear all references
		initialMutexes.erase(initialMutexes.begin(), initialMutexes.end());

		pthread_cond_wait(cond.get(), &lock);

		pthread_cond_destroy(cond.get());
		pthread_mutex_destroy(&lock);
		sem_destroy(sem.get());

	} else {

		for (BmfWriterTask& task : jobs) {
			task.run();
		}

	}

	return 0;

}

// BmfWriterTask

void BmfWriterTask::writeToFile() {

	FontCacheHeader header;

	string family_name( ftFace->family_name );

	header.magic = FONTCACHEFILE_MAGIC;
	header.mMajorVersion = FONTCACHEFILE_MAJOR_VERSION;
	header.mMinorVersion = FONTCACHEFILE_MINOR_VERSION;
	header.mID = hash_name(family_name);
	header.mFlags = 0; // TODO

	header.mNumberOfGlyphs = static_cast<uint16_t>(glyphs.size());
	header.mBitmapWidth = static_cast<uint16_t>(bitmap_width);
	header.mBitmapHeight = static_cast<uint16_t>(bitmap_height);
	header.mBytesPerPixel = 1;
	header.mGlyphFontSize = instance->getFontSize();
	header.mMaxAdvance = max_advance;
	header.mDpi = instance->getDpi();

	char n = '\0';

	const uint32_t family_name_len = family_name.length();
	const uint32_t pxCount = bitmap_width * bitmap_height;

	const uint32_t glyphEntriesFileOffset = sizeof(FontCacheHeader) + family_name_len + 1;
	const uint32_t bitmapDataFileOffset = glyphEntriesFileOffset + ( sizeof(GlyphEntry) * glyphs.size() );
	const uint32_t file_length = bitmapDataFileOffset + pxCount;

	FILE* tFile = fopen(outputFile.c_str(), "wb+");

	if (!tFile) {
		print_errorln("BmfWriterTask::writeToFile() failed to write to file `%s` with error: %s", outputFile.c_str(), strerror(errno));
	}

	// write header
	fwrite(&header, sizeof(FontCacheHeader), 1, tFile);
	
	// write string name
	fwrite(ftFace->family_name, 1, family_name_len, tFile); 

	// write string name null terminator
	fwrite(&n, 1, 1, tFile);

	// Write glyph entries
	fwrite(glyphs.data(), sizeof(GlyphEntry), glyphs.size(), tFile);

	// Write bitmap data
	fwrite(bitmap.data(), 1, pxCount, tFile);

	if (instance->verbose()) {
		print_stats(outputFile, file_length, glyphEntriesFileOffset, bitmapDataFileOffset);
	}

	fclose(tFile);

}

int32_t BmfWriterTask::run() {

	const uint32_t text_length = strlen(ASCII_LIST);
	glyphs = vector<GlyphEntry>(text_length);

	FT_Error error = FT_New_Face( instance->getLibrary(), inputFile.c_str(), 0, &(ftFace) );

	if ( error == FT_Err_Unknown_File_Format ) {

		print_errorln("FT_New_Face() returned FT_Err_Unknown_File_Format. Font file name = %s", inputFile.c_str());
		return 1;

	} else if (error) {

		print_errorln("FT_New_Face() returned %d", error);
		return 1;

	}

	error = FT_Set_Char_Size( ftFace, 0, instance->getFontSize() * 64, instance->getDpi(), instance->getDpi() );

	FT_Glyph glyph;

	uint32_t currentX_px = PIXEL_PADDING - 1;
	uint32_t currentY_px = PIXEL_PADDING - 1;

	// calcuate needed bitmap size

	FT_Size_Metrics szMetrics = ftFace->size->metrics;

	const int32_t max_font_width = ( szMetrics.max_advance / 64 ) + PIXEL_PADDING;
	const int32_t max_font_height = ( (szMetrics.ascender - szMetrics.descender) / 64 ) + PIXEL_PADDING;

	uint16_t dimension = 0;

	const vector<uint16_t>& bmp_ref_sizes = instance->powerOfTwoBitmapDimensions() ? bmp_2n_sizes : bmp_sizes;

	for (uint16_t size : bmp_ref_sizes) {

		// Don't simplify since integer flooring matters here
		uint16_t calculate = (size / max_font_height) * (size / max_font_width);

		if (calculate > 96) {

			dimension = size;

			if (verbose()) {
				print_verboseln("choosing bitmap size of %ux%u", dimension, dimension);
			}

			break;
		
		}

	}

	if (!dimension) {

		print_errorln("Bitmap would be too large for font. Request smaller font size or DPI");
		return 4;

	}

	bitmap_height = dimension;
	bitmap_width = dimension;

	const uint32_t px = dimension * dimension;
	bitmap = vector<uint8_t>(px);

	bzero(bitmap.data(), px);

	if (verbose()) {
		print_verboseln("\tbeginning bitmap render [%s]", inputFile.c_str());
	}

	for ( uint32_t n = 0; n < text_length; n++ ) {

		const char c = ASCII_LIST[n];

		FT_Load_Char( ftFace, c, FT_LOAD_RENDER );
		FT_Get_Glyph( ftFace->glyph, &glyph );

		renderToBitmap( c, n, currentX_px, currentY_px );

		FT_Done_Glyph(glyph);

	}

	if (instance->packBitmaps()) {

		const uint16_t t_newHeight = currentY_px + max_font_height + PIXEL_PADDING;

		if (verbose()) {
			print_verboseln("packing bitmap as requested. height %u ---> %u\n", bitmap_height, t_newHeight);
		}

		packBitmap(t_newHeight);

	}

	if (verbose()) {
		print_verboseln("\tWriting out to file from [%s] ---> [%s]", inputFile.c_str(), outputFile.c_str());
	}

	writeToFile();

	FT_Done_Face( ftFace );

	return 0;

}

void BmfWriterTask::renderToBitmap(uint16_t pGlyph, uint32_t pGlyphIndex, uint32_t& pCursorX, uint32_t& pCursorY) {

	FT_GlyphSlot slot = ftFace->glyph;
	FT_Glyph_Metrics metrics = slot->metrics;
	FT_Bitmap* ft_bitmap = &(slot->bitmap);
	FT_Size size = ftFace->size;
	FT_Size_Metrics szMetrics = size->metrics;

	const uint32_t advance = metrics.horiAdvance / 64;

	const uint32_t bmpWidth = ft_bitmap->width;
	const uint32_t bmpHeight = ft_bitmap->rows;
	const uint32_t pitch = ft_bitmap->pitch;

	const uint32_t X_advance = advance >= bmpWidth ? advance : bmpWidth;
	const uint32_t Y_advance = (szMetrics.ascender - szMetrics.descender) / 64;

	max_advance = static_cast<uint16_t>(max_advance >= X_advance ? max_advance : X_advance);

	uint32_t cursorX = pCursorX;
	uint32_t cursorY = pCursorY;

	if ( ( pCursorX + X_advance + PIXEL_PADDING ) > bitmap_width ) {

		// need to shift down a line
		cursorX = PIXEL_PADDING - 1;
		cursorY += Y_advance + PIXEL_PADDING;

		pCursorY = cursorY;

	}

	const uint32_t pxCount = bitmap_width * bitmap_height;
	uint8_t* buffer = reinterpret_cast<uint8_t*>(ft_bitmap->buffer);

	// The first glyph will actually be written near the end of the bitmap vector
	// For instance, a (w,h) = (512,512) (size = 262,144) shall have its first glyph written starting at index:
	// index = 262144 - (PIXEL_PADDING - 1) * w + (PIXEL_PADDING - 1) 
	// for PIXEL_PADDING = 4, index = 260611

	for (uint32_t srcX = 0, dstX = cursorX; srcX < bmpWidth; srcX++, dstX++) {

		// positive pitch.
		// See http://www.freetype.org/freetype2/docs/glyphs/glyphs-7.html for explanation
		for (uint32_t srcY = 0, dstY = cursorY; srcY < bmpHeight; srcY++, dstY++) {
			bitmap[ pxCount - ( bitmap_width * dstY ) + dstX ] = static_cast<uint8_t>( buffer[ srcX + ( pitch * srcY ) ] );
		}

	}

	GlyphEntry& currentGlyphEntry = glyphs[pGlyphIndex];
	
	currentGlyphEntry.mGlyph = pGlyph;
	currentGlyphEntry.mBLx = cursorX;
	currentGlyphEntry.mBLy = cursorY + Y_advance;
	currentGlyphEntry.mTRx = cursorX + X_advance;
	currentGlyphEntry.mTRy = cursorY;

	pCursorX = cursorX + X_advance + PIXEL_PADDING;

}

void BmfWriterTask::packBitmap(uint16_t pNewBitmapHeight) {

	const uint32_t pxCount_new = pNewBitmapHeight * bitmap_width;
	const uint32_t offset = (bitmap_height * bitmap_width) - pxCount_new;

	vector<uint8_t> n_bitmap = vector<uint8_t>(pxCount_new);

	std::copy( bitmap.begin() + offset, bitmap.end(), n_bitmap.begin() );
	bitmap.swap(n_bitmap);

	bitmap_height = pNewBitmapHeight;

}

// BackgroundWriterTask

void BackgroundWriterTask::backgroundRoutine() {

	pthread_mutex_lock(initial_mutex.get());

	for (BmfWriterTask* task : args) {
		task->run();
	}

	sem_post(semaphore.get());

	int semVal;

	sem_getvalue(semaphore.get(), &semVal);

	if (semVal == instance->numberOfThreads()) {
		pthread_cond_signal(cond.get());
	}

	pthread_mutex_unlock(initial_mutex.get());
	pthread_mutex_destroy(initial_mutex.get());

}

////////////////////////////////

static boost::shared_ptr<BmfWriter> create_instance(po::variables_map& pArgs) {

	bool is_verbose = pArgs["verbose"].as<bool>();
	uint16_t fontSize = pArgs["size"].as<uint16_t>();
	uint16_t dpi = 0;
	uint32_t num_threads = 0;

	if (pArgs.count("dpi") > 0) {
		dpi = pArgs["dpi"].as<uint16_t>();
	}

	if (dpi < MIN_DPI) {
		dpi = try_get_dpi();
	}

	if (pArgs.count("jobs") > 0) {
		num_threads = pArgs["jobs"].as<uint32_t>();
	}

	if (is_verbose) {
		print_verboseln("[verbose mode]\nfont size: %u\ndpi: %u", fontSize, dpi);
	}	

	boost::shared_ptr<BmfWriter> instance(new BmfWriter(is_verbose, fontSize, dpi, num_threads));

	try {
		instance->set_powerOfTwoBitmapDimensions(pArgs["pwrtwo"].as<bool>());
	} catch (po::error& perr) {
		if (is_verbose) {
			print_warnln("Caught boost::program_options::error while trying to query `pwrtwo` existence. Exception:\n%s", perr.what());
		}
	} catch (std::exception& ex) {
		if (is_verbose) {
			print_warnln("Caught std::exception while trying to query `pwrtwo` existence. Exception:\n%s", ex.what());
		}
	} catch (...) {
		if (is_verbose) {
			print_warnln("Caught unknown exception while trying to query `pwrtwo` existence.");
		}		
	}

	try {
		instance->set_packBitmaps(pArgs["pack"].as<bool>());
	} catch (po::error& perr) {
		if (is_verbose) {
			print_warnln("Caught boost::program_options::error while trying to query `pack` existence. Exception:\n%s", perr.what());
		}
	} catch (std::exception& ex) {
		if (is_verbose) {
			print_warnln("Caught std::exception while trying to query `pack` existence. Exception:\n%s", ex.what());
		}
	} catch (...) {
		if (is_verbose) {
			print_warnln("Caught unknown exception while trying to query `pack` existence.");
		}
	}
	
	const vector<string>& inputFiles = pArgs["input"].as< std::vector<string> >();
	const vector<string>& outputFiles = pArgs["output"].as< std::vector<string> >();

	const size_t in_len = inputFiles.size();
	const size_t out_len = outputFiles.size();

	if ( outputFiles.size() == 0 ) {

		for(const string& input : inputFiles) {
			instance->addJob(input, ".");
		}

	} else if ( in_len < out_len ) {

		print_warnln("warning: ignoring last %zu output files specified (user provided too many)", outputFiles.size() - inputFiles.size() );
	
		const uint32_t count = inputFiles.size();

		for (uint32_t i = 0; i < count; i++) {
			instance->addJob(inputFiles[i], outputFiles[i]);
		}	

	} else {

		const uint32_t count = outputFiles.size();

		for (uint32_t i = 0; i < count; i++) {
			instance->addJob(inputFiles[i], outputFiles[i]);
		}

		for (uint32_t i = count; i < inputFiles.size(); i++) {
			instance->addJob(inputFiles[i], ".");
		}

	}

	return instance;

}

#define USAGE_EXTRA_STR "To specify an output file with the same name" 		\
						" as its input file (with .bmf extension), " 		\
						"simply pass `.` as the file name\n\n"

int32_t main(int32_t argc, char* argv[]) {

	po::options_description general("General Options");
	general.add_options()
		("verbose,v", po::bool_switch()->default_value(false), "print extra info while running")
		("pack,p", po::bool_switch()->default_value(false), "pack bitmap size to map edge (default = false)")
		("pwrtwo,2n", po::bool_switch()->default_value(false), "restrict bitmap output dimensions to 2^n (default = false)")
		("dpi,d", po::value<uint16_t>(), "font dpi")
		("jobs,j", po::value<uint16_t>(), "jobs (thread) count")
		("output,o", po::value< vector<string> >()->multitoken(), "specify output files")
		("size,f", po::value<uint16_t>()->required(), "font size (Required)")
		("input,i", po::value< vector<string> >()->multitoken()->required(), "specify input files (Required)")
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

		po::options_description cmdline_options;
		cmdline_options.add(general).add(hidden);

		po::command_line_parser parser(argc, argv);
		parser.options(cmdline_options).allow_unregistered();

		po::basic_parsed_options<char> parsed = parser.run();
		po::variables_map vm;

		store(parsed, vm);

		notify(vm);

		boost::shared_ptr<BmfWriter> instance = create_instance(vm);

		int32_t retCode = instance->run();

		return retCode;

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

}

#pragma clang diagnostic pop
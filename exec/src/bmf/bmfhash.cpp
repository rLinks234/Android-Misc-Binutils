#include "ft2build.h"
#include "freetype/freetype.h"
#include FT_GLYPH_H

#include "bmf.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"

#define VERSION_STR "1.00"

using namespace std;

static int32_t run(std::string& pFileLocation) {

	FT_Library ftLib;
	FT_Error error = FT_Init_FreeType(&ftLib);

	if (error) {

		printf("0\n");
		return error;

	}

	FT_Face ftFace;
	error = FT_New_Face(ftLib, pFileLocation.c_str(), 0, &ftFace);

	if (error) {

		printf("0\n");
		return error;

	}

	string hash_string(ftFace->family_name);

	// Per FreeType API, this can be null
	if (ftFace->style_name != nullptr) {
		hash_string.append(ftFace->style_name);
	}

	uint32_t id = hash_name(hash_string);

	printf("%u\n", id);

	return 0;

}

////////////////////////////////

int32_t main(int32_t argc, char* argv[]) {

	po::options_description general("General Options");
	general.add_options()
		("input,i", po::value<string>()->required(), "specify input file (Required)")
	;

	try {

		po::options_description help = getHelpDescription();
		po::variables_map help_vm;

		po::command_line_parser help_parser(argc, argv);
		help_parser.options(help).allow_unregistered();

		po::basic_parsed_options<char> help_parsed = help_parser.run();

		store(help_parsed, help_vm);

		if (hasHelpOptions(help_vm)) {

			printUsage(general, "");
			return 0;

		} else if (hasVersion(help_vm)) {

			printf("%s\n", VERSION_STR);
			return 0;

		}

		po::positional_options_description p;
		p.add("input", 1);

		po::options_description cmdline_options;
		cmdline_options.add(general);

		po::command_line_parser parser(argc, argv);
		parser.options(cmdline_options).positional(p).allow_unregistered();

		po::basic_parsed_options<char> parsed = parser.run();
		po::variables_map vm;

		store(parsed, vm);
		notify(vm);

		string f = vm["input"].as<string>();
		int32_t retCode = run(f);

		return retCode;

	} catch (po::error& perr) {

		print_errorln("\nError: %s", perr.what());
		printUsage(general, "");

		return 1;

	} catch(exception& e) {

		print_errorln("\n%s", e.what());
		return 1;

	} catch(boost::exception & ex) {

		print_errorln("\n%s", boost::diagnostic_information(ex).c_str());
		return 2;

	} catch (...) {

		print_errorln("\nCaught unknown exception");
		return 4;

	}

}

#pragma clang diagnostic pop
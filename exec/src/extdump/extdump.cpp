#include "extdump.h"

////////////////////////////////

static ExtDumpInstance getFreshDump() {

	ExtDumpInstance ret;
	
	ret.asciiPrint = false;
	ret.ps = kStdOut;
	ret.outputFile = nullptr;
	ret.clHandle = nullptr;
	ret.glesHandle = nullptr;
	ret.eglHandle = nullptr;

	ret.clPlatformCount = 0;
	ret.clPlatforms = nullptr;
	ret.glRuntime = nullptr;

	return ret;

}

////////////////////////////////


static void _printAscii(ExtDumpInstance* pInstance, FILE* pOutput) {

	_cl_asciiPrintInfo(pInstance->clPlatforms, pInstance->clPlatformCount, pOutput);
	_gl_asciiPrintInfo(pInstance->glRuntime, pOutput);

}

static void _printBinary(ExtDumpInstance* pInstance, FILE* pOutput) {

}

////////////////////////////////

static FILE* createDescriptor(ExtDumpInstance* pInstance) {

	if (pInstance->ps == kTxtFile) {
		return fopen(pInstance->outputFile, "w+");
	} else if (pInstance->ps == kBinFile) {
		return fopen(pInstance->outputFile, "wb+");
	} else {
		return stdout;
	}

}

static void process(ExtDumpInstance* pInstance) {

	FILE* prnt = createDescriptor(pInstance);

	// Better huge than not large enough
	const uint32_t infoBufferCapacity = 16192;
	void* infoBuffer = malloc(infoBufferCapacity);
	// Set all data initially to zero
	bzero(infoBuffer, infoBufferCapacity);

	_process_GL(pInstance, infoBuffer, infoBufferCapacity);
	_process_CL(pInstance, infoBuffer, infoBufferCapacity);

	if (pInstance->asciiPrint) {
		_printAscii(pInstance, prnt);
	} else {
		_printBinary(pInstance, prnt);
	}

	if (prnt != stdout) {
		fclose(prnt);
	}

	free(infoBuffer);

}

////////////////////////////////

static void printUsage() {

	printf("\nUSAGE:\n%s ...\n\n", PROGRAM_NAME);
	printf("No arguments prints to stdout\n\n");
	printf("-f FILE\nPrints extensions to FILE in ascii text\n\n");
	printf("-fb FILE\nPrints extensions to FILE in binary format (see spec for file format)\n\n");
	printf("-b\nPrints extensions to stdout in binary format (see spec for binary format)\n\n");
	printf("-cl LIB -egl LIB -gles LIB\nSpecify which libraries to load\n\n");
	printf("--help\nPrint this message\n");

}

////////////////////////////////

static void parseArgs(int argc, char** argv) {

	bool isAsciiMode = true;
	uint32_t fileOutSpecified = 0;
	uint32_t fileOutSpecifiedAt = 0;

	uint32_t clSpecified = 0;
	uint32_t clSpecifiedAt = 0;
	
	uint32_t eglSpecified = 0;
	uint32_t eglSpecifiedAt = 0;
	
	uint32_t glesSpecified = 0;
	uint32_t glesSpecifiedAt = 0;

	// Loop 1 to ensure --help isn't specified
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0) {

			printUsage();
			return;

		}
	}

	for (int i = 1; i < argc; i++) {

		bool found = false;

		if (strcmp(argv[i], "-f") == 0) {

			fileOutSpecified++;
			fileOutSpecifiedAt = i;
			isAsciiMode = true;
			found = true;

		} else if (strcmp(argv[i], "-fb") == 0) {

			fileOutSpecified++;
			fileOutSpecifiedAt = i;
			isAsciiMode = false;
			found = true;

		} else if (strcmp(argv[i], "-b") == 0) {

			fileOutSpecified++;
			fileOutSpecifiedAt = i;
			isAsciiMode = false;
			found = true;

		} else if (strcmp(argv[i], "-cl") == 0) {

			clSpecified++;
			clSpecifiedAt = i;
			found = true;

		} else if (strcmp(argv[i], "-egl") == 0) {

			eglSpecified++;
			eglSpecifiedAt = i;
			found = true;

		} else if (strcmp(argv[i], "-gles") == 0) {

			glesSpecified++;
			glesSpecifiedAt = i;
			found = true;

		}

		if (found && i == (argc - 1) ) {

			printf("Expected an argument following %s\n", argv[i]);
			printUsage();

			return;

		}

	}

	bool specifiedTwice = false;

	if (fileOutSpecified > 1) {

		specifiedTwice = true;
		printf("-[f][b] specified more than once\n");

	}

	if (clSpecified > 1) {

		specifiedTwice = true;
		printf("-cl specified more than once\n");

	}

	if (eglSpecified > 1) {

		specifiedTwice = true;
		printf("-egl specified more than once\n");

	}

	if (glesSpecified > 1) {

		specifiedTwice = true;
		printf("-gles specified more than once\n");

	}

	if (specifiedTwice) {

		printUsage();
		return;

	}

	// Setting up parameters

	ExtDumpInstance dump = getFreshDump();

	if ( (fileOutSpecified == 1) && ( strcmp(argv[fileOutSpecifiedAt], "-b" ) != 0 ) ) {

		const uint32_t name_len = strlen(argv[fileOutSpecifiedAt + 1]);
		char* name = new char[name_len + 1];
		name[name_len] = '\0';
		memcpy(name, argv[fileOutSpecifiedAt + 1], name_len);

		dump.outputFile = name;

		if (isAsciiMode) {
			dump.ps = kTxtFile;
		} else {
			dump.ps = kBinFile;
		}

	} else {
		dump.ps = kStdOut;
	}

	dump.asciiPrint = isAsciiMode;

	if (clSpecified) {
		dump.clHandle = dlopen(argv[clSpecifiedAt + 1], RTLD_NOW | RTLD_GLOBAL);
	} else {
		dump.clHandle = dlopen(CL_LIB, RTLD_NOW | RTLD_GLOBAL);
	}

	if (eglSpecified) {
		dump.eglHandle = dlopen(argv[eglSpecifiedAt + 1], RTLD_NOW | RTLD_GLOBAL);
	} else {
		dump.eglHandle = dlopen(EGL_LIB, RTLD_NOW | RTLD_GLOBAL);	
	}

	if (glesSpecified) {
		dump.glesHandle = dlopen(argv[glesSpecifiedAt + 1], RTLD_NOW | RTLD_GLOBAL);
	} else {
		dump.glesHandle = dlopen(GLES_LIB, RTLD_NOW | RTLD_GLOBAL);
	}

	process(&dump);

}

////////////////////////////////

int main(int argc, char** argv) {
	parseArgs(argc, argv);
}
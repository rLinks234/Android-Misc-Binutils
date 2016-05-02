#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PROGRAM_NAME "extdump"

#define CL_LIB "libOpenCL.so"
#define EGL_LIB "libEGL.so"
#define GLES_LIB "libGLESv2.so"

#include "apistub.h"

////////////////////////////////

struct CLDeviceInfo {

	char* name;
	char* vendor;
	uint32_t vendorID;
	char* version;
	char* profile;
	char* cVersion;
	char* driverVersion;
	char* extensions;
	uint32_t imageSupport;
	uint32_t maxComputeUnits;
	uint32_t maxConstantArgs;
	uint32_t maxWorkItemDimensions;
	uint32_t maxWorkGroupSize;
	uint32_t* maxWorkItemSizes;
	uint64_t localMemSize;
	uint64_t globalMemSize;
	uint64_t maxMemAllocSize;

	~CLDeviceInfo() {

		if (name) {
			delete[] name;
		}

		if (vendor) {
			delete[] vendor;
		}

		if (version) {
			delete[] version;
		}

		if (profile) {
			delete[] profile;
		}

		if (cVersion) {
			delete[] cVersion;
		}

		if (driverVersion) {
			delete[] driverVersion;
		}

		if (extensions) {
			delete[] extensions;
		}

	}

};

struct CLPlatformInfo {

	char* profile;
	char* version;
	char* name;
	char* vendor;
	char* extensions;
	
	uint32_t deviceCount;
	CLDeviceInfo* devices;

	~CLPlatformInfo() {

		if (devices) {
			delete[] devices;
		}

	}

};

struct GLRuntimeInfo {

	char* eglVendorString;
	char* eglVersionString;
	char* eglExtensionsString;
	char* glVendorString;
	char* glRendererString;
	char* glVersionString;
	char* glExtensionsString;
	char* glShadingLanguageVersionsString;

	int32_t glMaxCombinedTextureImageUnitsValue;
	int32_t glMaxCubeMapTextureSizeValue;
	int32_t glMaxFragmentUniformVectorsValue;
	int32_t glMaxRenderbufferSize;
	int32_t glMaxTextureImageUnitsValue;
	int32_t glMaxTextureSizeValue;
	int32_t glMaxVaryingVectorsValue;
	int32_t glMaxVertexAttribsValue;
	int32_t glMaxVertexTextureImageUnitsValue;
	int32_t glMaxVertexUniformVectorsValue;
	int32_t glMaxViewportDims[2];
	int32_t glMaxCompressedTextureFormatsValue;
	int32_t glMaxShaderBinaryFormatsValue;

	~GLRuntimeInfo() {

		if (eglVendorString) {
			delete[] eglVendorString;
		}

		if (eglVersionString) {
			delete[] eglVersionString;
		}

		if (eglExtensionsString) {
			delete[] eglExtensionsString;
		}

		if (glVendorString) {
			delete[] glVendorString;
		}

		if (glRendererString) {
			delete[] glRendererString;
		}

		if (glVersionString) {
			delete[] glVersionString;
		}

		if (glExtensionsString) {
			delete[] glExtensionsString;
		}

		if (glShadingLanguageVersionsString) {
			delete[] glShadingLanguageVersionsString;
		}

	}

};

enum PrintStrategy {

	kStdOut = 0,
	kTxtFile = 1,
	kBinFile = 2

};

struct ExtDumpInstance {

	bool asciiPrint;
	PrintStrategy ps;
	char* outputFile;
	void* clHandle;
	void* eglHandle;
	void* glesHandle;

	uint32_t clPlatformCount;
	CLPlatformInfo* clPlatforms;
	GLRuntimeInfo* glRuntime;

	~ExtDumpInstance() {

		if (clPlatforms) {
			delete[] clPlatforms;
		}

		if (glRuntime) {
			delete glRuntime;
		}

	}

};

////////////////////////////////

#define EXTDUMPMAGIC 0x454D

struct BinaryHeader {

	/**
	 *
	 * The magic, which is represented as an unsigned
	 * 4 byte integer equal to 0x454D
	 */
	uint32_t magic;

	/**
	 *
	 * The offset from the beginning of the file
	 * where the beginning of the OpenCL data is stored.
	 * If this platform doesn't have OpenCL, this will be 0. 
	 */
	uint32_t openCLoffset;

	/**
	 * 
	 * The offset from the beginning of the file
	 * where the beginning of the GLES/EGL data 
	 * is stored. This data is stored until the end of the file.
	 */
	uint32_t glOffset;

	/**
	 * 
	 * Size in bytes of the OpenCL section.
	 * If this is 0, then this device doesn't have
	 * OpenCL, or no platform was found.
	 */
	uint32_t clSize;

} __attribute__((packed));

struct CLDeviceHeader {

} __attribute__((packed));

struct CLPlatformHeader {



} __attribute__((packed));

struct BinaryCLHeader {

	uint32_t numberOfPlatforms;
	CLPlatformHeader* platforms;

} __attribute__((packed));

////////////////////////////////

void _gl_binaryPrintInfo(GLRuntimeInfo* pRuntime, FILE* pOutput);
void _gl_asciiPrintInfo(GLRuntimeInfo* pRuntime, FILE* pOutput);
void _destroyGL_context(void* pDisplay, void* pSurface, void* pContext);
void _setupGL_context(void** _config, void** _surface, void** _dpy, void** _context);
void _queryAPI_GL(ExtDumpInstance* pInstance, void* pDisplay, void* pInfoBuffer, uint32_t pInfoBufferCapacity);
void _process_GL(ExtDumpInstance* pInstance, void* pInfoBuffer, uint32_t pInfoBufferCapacity);

void _process_CL(ExtDumpInstance* pInstance, void* pInfoBuffer, uint32_t pInfoBufferCapacity);
void _clPopulate(cl_device_id pDevice, CLDeviceInfo* pDeviceInfo, void* pInfoBuffer, uint32_t pInfoBufferCapacity);
void _cl_asciiPrintInfo(CLPlatformInfo* pPlatforms, uint32_t pPlatformCount, FILE* pOutput);
void _cl_asciiPrintPlatform(CLPlatformInfo* pPlatform, uint32_t pIndex, FILE* pOutput);
void _cl_asciiPrintDevice(CLDeviceInfo* pDevice, uint32_t pIndex, FILE* pOutput);
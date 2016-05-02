#include "extdump.h"

// TODO
void _gl_binaryPrintInfo(GLRuntimeInfo* pRuntime, FILE* pOutput) {

}

void _gl_asciiPrintInfo(GLRuntimeInfo* pRuntime, FILE* pOutput) {

	fprintf(pOutput, "\nEGL and OpenGL ES Platform\n\n");
	fprintf(pOutput, "EGL Vendor: %s\n", pRuntime->eglVendorString);
	fprintf(pOutput, "EGL Version: %s\n", pRuntime->eglVersionString);
	fprintf(pOutput, "GL Vendor: %s\n", pRuntime->glVendorString);
	fprintf(pOutput, "GL Renderer: %s\n", pRuntime->glRendererString);
	fprintf(pOutput, "GL Version: %s\n", pRuntime->glVersionString);
	fprintf(pOutput, "GL Extensions: %s\n", pRuntime->glExtensionsString);
	fprintf(pOutput, "GL Shading Language Version: %s\n", pRuntime->glShadingLanguageVersionsString);

	fprintf(pOutput, "Max Combined Txt Image Units: %d\n", pRuntime->glMaxCombinedTextureImageUnitsValue);
	fprintf(pOutput, "Max Cubemap Txt Size: %d\n", pRuntime->glMaxCubeMapTextureSizeValue);
	fprintf(pOutput, "Max Fragment Uniform Vectors: %d\n", pRuntime->glMaxFragmentUniformVectorsValue);
	fprintf(pOutput, "Max Renderbuffer Size: %d\n", pRuntime->glMaxRenderbufferSize);
	fprintf(pOutput, "Max Txt Image Units: %d\n", pRuntime->glMaxTextureImageUnitsValue);
	fprintf(pOutput, "Max Texture Size: %d\n", pRuntime->glMaxTextureSizeValue);
	fprintf(pOutput, "Max Varying Vectors: %d\n", pRuntime->glMaxVaryingVectorsValue);
	fprintf(pOutput, "Max Vertex Attribs: %d\n", pRuntime->glMaxVertexAttribsValue);
	fprintf(pOutput, "Max Vertex Txt Image Units: %d\n", pRuntime->glMaxVertexTextureImageUnitsValue);
	fprintf(pOutput, "Max Vertex Uniform Vectors: %d\n", pRuntime->glMaxVertexUniformVectorsValue);
	fprintf(pOutput, "Max Viewport Dimensions: [%d, %d]\n", pRuntime->glMaxViewportDims[0], pRuntime->glMaxViewportDims[1]);
	fprintf(pOutput, "Max Compressed Txt Formats: %d\n", pRuntime->glMaxCompressedTextureFormatsValue);
	fprintf(pOutput, "Max Shader Binary Formats: %d\n", pRuntime->glMaxShaderBinaryFormatsValue);

}

////////////////////////////////

void _destroyGL_context(void* pDisplay, void* pSurface, void* pContext) {

	eglDestroySurface(pDisplay, pSurface);
	eglDestroyContext(pDisplay, pContext);

}

void _setupGL_context(void** _config, void** _surface, void** _dpy, void** _context) {

	void* config;
	void* surface;
	void* dpy;
	void* context;

	int32_t cfg_attrs[] = {

		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_BLUE_SIZE, 5,
		EGL_GREEN_SIZE, 6,
		EGL_RED_SIZE, 5,
		EGL_NONE
	
	};

	int32_t ctx_attrs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
	};

	int32_t surf_attrs[] = {

		EGL_WIDTH, 128,
		EGL_HEIGHT, 128,
		EGL_NONE

	};

	dpy = eglGetDisplay(nullptr);

	if (!eglInitialize(dpy, nullptr, nullptr)) {

		printf("eglInitialize() returned false!\n");
		return;

	}

	int32_t num_configs = -1;

	if (!eglChooseConfig(dpy, cfg_attrs, &config, 1, &num_configs)) {

		printf("eglChooseConfig() returned false!\n");
		return;

	}

	context = eglCreateContext(dpy, config, nullptr, ctx_attrs);

	if (!context) {

		printf("eglCreateContext() returned null!\n");
		return;

	}

	surface = eglCreatePbufferSurface(dpy, config, surf_attrs);

	if (!surface) {

		printf("eglCreateWindowSurface() returned null!\n");
		return;

	}

	eglMakeCurrent(dpy, surface, surface, context);

	*_config = config;
	*_surface = surface;
	*_dpy = dpy;
	*_context = context;

}

////////////////////////////////

void _queryAPI_GL(ExtDumpInstance* pInstance, void* pDisplay, void* pInfoBuffer, uint32_t pInfoBufferCapacity) {

	// EGL query string
	const char* eglVendorString = eglQueryString(pDisplay, EGL_VENDOR);
	const char* eglVersionString = eglQueryString(pDisplay, EGL_VERSION);
	const char* eglExtensionsString = eglQueryString(pDisplay, EGL_EXTENSIONS);

	// GL get string
	const char* glVendorString = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	const char* glRendererString = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	const char* glVersionString = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	const char* glExtensionsString = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
	const char* glShadingLanguageVersionsString = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

	// GL get
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

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &glMaxCombinedTextureImageUnitsValue);
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &glMaxCubeMapTextureSizeValue);
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &glMaxFragmentUniformVectorsValue);
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &glMaxRenderbufferSize);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &glMaxTextureImageUnitsValue);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTextureSizeValue);
	glGetIntegerv(GL_MAX_VARYING_VECTORS, &glMaxVaryingVectorsValue);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &glMaxVertexAttribsValue);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &glMaxVertexTextureImageUnitsValue);
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &glMaxVertexUniformVectorsValue);
	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, glMaxViewportDims);
	glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &glMaxCompressedTextureFormatsValue);
	glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &glMaxShaderBinaryFormatsValue);

	// Copy values into struct
	GLRuntimeInfo* runtime = new GLRuntimeInfo();

	char* infoBuffer;

	uint32_t infoBufferLength = strlen(eglVendorString);
	infoBuffer = new char[infoBufferLength + 1];
	infoBuffer[infoBufferLength] = '\0';
	memcpy(infoBuffer, eglVendorString, infoBufferLength);
	runtime->eglVendorString = infoBuffer;

	infoBufferLength = strlen(eglVersionString);
	infoBuffer = new char[infoBufferLength + 1];
	infoBuffer[infoBufferLength] = '\0';
	memcpy(infoBuffer, eglVersionString, infoBufferLength);
	runtime->eglVersionString = infoBuffer;

	infoBufferLength = strlen(eglExtensionsString);
	infoBuffer = new char[infoBufferLength + 1];
	infoBuffer[infoBufferLength] = '\0';
	memcpy(infoBuffer, eglExtensionsString, infoBufferLength);
	runtime->eglExtensionsString = infoBuffer;

	// these are a bit different since they're unsigned and not signed chars
	infoBufferLength = strlen(glVendorString);
	infoBuffer = new char[infoBufferLength + 1];
	infoBuffer[infoBufferLength] = '\0';
	memcpy(infoBuffer, glVendorString, infoBufferLength);
	runtime->glVendorString = infoBuffer;

	infoBufferLength = strlen(glRendererString);
	infoBuffer = new char[infoBufferLength + 1];
	infoBuffer[infoBufferLength] = '\0';
	memcpy(infoBuffer, glRendererString, infoBufferLength);
	runtime->glRendererString = infoBuffer;

	infoBufferLength = strlen(glVersionString);
	infoBuffer = new char[infoBufferLength + 1];
	infoBuffer[infoBufferLength] = '\0';
	memcpy(infoBuffer, glVersionString, infoBufferLength);
	runtime->glVersionString = infoBuffer;

	infoBufferLength = strlen(glExtensionsString);
	infoBuffer = new char[infoBufferLength + 1];
	infoBuffer[infoBufferLength] = '\0';
	memcpy(infoBuffer, glExtensionsString, infoBufferLength);
	runtime->glExtensionsString = infoBuffer;

	infoBufferLength = strlen(glShadingLanguageVersionsString);
	infoBuffer = new char[infoBufferLength + 1];
	infoBuffer[infoBufferLength] = '\0';
	memcpy(infoBuffer, glShadingLanguageVersionsString, infoBufferLength);
	runtime->glShadingLanguageVersionsString = infoBuffer;

	// Int values
	runtime->glMaxCombinedTextureImageUnitsValue = glMaxCombinedTextureImageUnitsValue;
	runtime->glMaxCubeMapTextureSizeValue = glMaxCubeMapTextureSizeValue;
	runtime->glMaxFragmentUniformVectorsValue = glMaxFragmentUniformVectorsValue;
	runtime->glMaxRenderbufferSize = glMaxRenderbufferSize;
	runtime->glMaxTextureImageUnitsValue = glMaxTextureImageUnitsValue;
	runtime->glMaxTextureSizeValue = glMaxTextureSizeValue;
	runtime->glMaxVaryingVectorsValue = glMaxVaryingVectorsValue;
	runtime->glMaxVertexAttribsValue = glMaxVertexAttribsValue;
	runtime->glMaxVertexTextureImageUnitsValue = glMaxVertexTextureImageUnitsValue;
	runtime->glMaxVertexUniformVectorsValue = glMaxVertexUniformVectorsValue;
	runtime->glMaxViewportDims[0] = glMaxViewportDims[0];
	runtime->glMaxViewportDims[1] = glMaxViewportDims[1];
	runtime->glMaxCompressedTextureFormatsValue = glMaxCompressedTextureFormatsValue;
	runtime->glMaxShaderBinaryFormatsValue = glMaxShaderBinaryFormatsValue;

	pInstance->glRuntime = runtime;
	
}


////////////////////////////////

void _process_GL(ExtDumpInstance* pInstance, void* pInfoBuffer, uint32_t pInfoBufferCapacity) {

	void* config;
	void* surface;
	void* dpy;
	void* context;

	if ( (!pInstance->eglHandle) || (!pInstance->glesHandle) ) {
		return;
	} else {

		eglGetDisplay = reinterpret_cast<_EGL_GetDisplay_t>(dlsym(pInstance->eglHandle, "eglGetDisplay"));
		eglInitialize = reinterpret_cast<_EGL_Initialize_t>(dlsym(pInstance->eglHandle, "eglInitialize"));
		eglChooseConfig = reinterpret_cast<_EGL_ChooseConfig_t>(dlsym(pInstance->eglHandle, "eglChooseConfig"));
		eglCreateContext = reinterpret_cast<_EGL_CreateContext_t>(dlsym(pInstance->eglHandle, "eglCreateContext"));
		eglDestroyContext = reinterpret_cast<_EGL_DestroyContext_t>(dlsym(pInstance->eglHandle, "eglDestroyContext"));
		eglCreatePbufferSurface = reinterpret_cast<_EGL_CreatePbufferSurface_t>(dlsym(pInstance->eglHandle, "eglCreatePbufferSurface"));
		eglMakeCurrent = reinterpret_cast<_EGL_MakeCurrent_t>(dlsym(pInstance->eglHandle, "eglMakeCurrent"));
		eglDestroySurface = reinterpret_cast<_EGL_DestroySurface_t>(dlsym(pInstance->eglHandle, "eglDestroySurface"));
		eglQueryString = reinterpret_cast<_EGL_QueryString_t>(dlsym(pInstance->eglHandle, "eglQueryString"));

		glGetString = reinterpret_cast<_GL_GetString_t>(dlsym(pInstance->glesHandle, "glGetString"));
		glGetBooleanv = reinterpret_cast<_GL_GetBooleanv_t>(dlsym(pInstance->glesHandle, "glGetBooleanv"));
		glGetFloatv = reinterpret_cast<_GL_GetFloatv_t>(dlsym(pInstance->glesHandle, "glGetFloatv"));
		glGetIntegerv = reinterpret_cast<_GL_GetIntegerv_t>(dlsym(pInstance->glesHandle, "glGetIntegerv"));

	}

	_setupGL_context(&config, &surface, &dpy, &context);
	_queryAPI_GL(pInstance, dpy, pInfoBuffer, pInfoBufferCapacity);
	_destroyGL_context(dpy, surface, context);

}
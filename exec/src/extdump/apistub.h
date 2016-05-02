// OpenCL

#define CL_DEVICE_TYPE_ALL 0xFFFFFFFF

#define CL_PLATFORM_VENDOR 0x0903

// --------------------------------
// CL Platform Queries

#define CL_PLATFORM_PROFILE 0x0900
#define CL_PLATFORM_VERSION 0x0901
#define CL_PLATFORM_NAME 0x0902
#define CL_PLATFORM_VENDOR 0x0903
#define CL_PLATFORM_EXTENSIONS 0x0904

// --------------------------------
// CL Device Queries

// char*
#define CL_DEVICE_NAME 0x102B
// char*
#define CL_DEVICE_VENDOR 0x102C
// uint32_t
#define CL_DEVICE_VENDOR_ID 0x1001
// char*
#define CL_DEVICE_VERSION 0x102F
// char*
#define CL_DEVICE_PROFILE 0x102E
// char*
#define CL_DEVICE_OPENCL_C_VERSION 0x103D
// char*
#define CL_DRIVER_VERSION 0x102D
// char*
#define CL_DEVICE_EXTENSIONS 0x1030
// uint32_t
#define CL_DEVICE_IMAGE_SUPPORT 0x1016
// uint32_t
#define CL_DEVICE_MAX_COMPUTE_UNITS 0x1002
// uint32_t
#define CL_DEVICE_MAX_CONSTANT_ARGS 0x1021
// uint32_t
#define CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS 0x1003
// size_t
#define CL_DEVICE_MAX_WORK_GROUP_SIZE 0x1004
// size_t*
#define CL_DEVICE_MAX_WORK_ITEM_SIZES 0x1005
// uint64_t
#define CL_DEVICE_LOCAL_MEM_SIZE 0x1023
// uint64_t
#define CL_DEVICE_GLOBAL_MEM_SIZE 0x101F
// uint64_t
#define CL_DEVICE_MAX_MEM_ALLOC_SIZE 0x1010

typedef struct _cl_platform_id* cl_platform_id;
typedef struct _cl_device_id* cl_device_id;

typedef int32_t (*_CL_GetPlatformIDs_t)(uint32_t, cl_platform_id*, uint32_t*);
typedef int32_t (*_CL_GetDeviceIDs_t)(cl_platform_id, uint64_t, uint32_t, cl_device_id*, uint32_t*);
typedef int32_t (*_CL_GetPlatformInfo_t)(cl_platform_id, uint32_t, size_t, void*, size_t*);
typedef int32_t (*_CL_GetDeviceInfo_t)(cl_device_id, uint32_t, size_t, void*, size_t*);

static _CL_GetPlatformIDs_t clGetPlatformIDs;
static _CL_GetDeviceIDs_t clGetDeviceIDs;
static _CL_GetPlatformInfo_t clGetPlatformInfo;
static _CL_GetDeviceInfo_t clGetDeviceInfo;

////////////////////////////////

// EGL

#define EGL_NONE 0x3038
#define EGL_OPENGL_ES2_BIT 0x0004
#define EGL_CONTEXT_CLIENT_VERSION 0x3098
#define EGL_RENDERABLE_TYPE 0x3040
#define EGL_BLUE_SIZE 0x3022
#define EGL_GREEN_SIZE 0x3023
#define EGL_RED_SIZE 0x3024
#define EGL_HEIGHT 0x3056
#define EGL_WIDTH 0x3057

#define EGL_VENDOR 0x3053
#define EGL_VERSION 0x3054
#define EGL_EXTENSIONS 0x3055

typedef void* (*_EGL_GetDisplay_t)(void*);
typedef uint32_t (*_EGL_Initialize_t)(void*, int32_t*, int32_t*);
typedef uint32_t (*_EGL_ChooseConfig_t)(void*, const int32_t*, void**, int32_t, int32_t*);
typedef void* (*_EGL_CreateContext_t)(void*, void*, void*, const int32_t*);
typedef uint32_t (*_EGL_DestroyContext_t)(void*, void*);
typedef void* (*_EGL_CreatePbufferSurface_t)(void*, void*, const int32_t*);
typedef uint32_t (*_EGL_MakeCurrent_t)(void*, void*, void*, void*);
typedef uint32_t (*_EGL_DestroySurface_t)(void*, void*);
typedef char const* (*_EGL_QueryString_t)(void*, int32_t);

static _EGL_GetDisplay_t eglGetDisplay;
static _EGL_Initialize_t eglInitialize;
static _EGL_ChooseConfig_t eglChooseConfig;
static _EGL_CreateContext_t eglCreateContext;
static _EGL_DestroyContext_t eglDestroyContext;
static _EGL_CreatePbufferSurface_t eglCreatePbufferSurface;
static _EGL_MakeCurrent_t eglMakeCurrent;
static _EGL_DestroySurface_t eglDestroySurface;
static _EGL_QueryString_t eglQueryString;

// OpenGL ES

// glGetString() params
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
// glGet() params
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS 0x8DFD
#define GL_MAX_RENDERBUFFER_SIZE 0x84E8
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_MAX_VARYING_VECTORS 0x8DFC
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_VERTEX_UNIFORM_VECTORS 0x8DFB
#define GL_MAX_VIEWPORT_DIMS 0x0D3A
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_NUM_SHADER_BINARY_FORMATS 0x8DF9

typedef const uint8_t* (*_GL_GetString_t)(uint32_t);
typedef void (*_GL_GetBooleanv_t)(uint32_t, uint32_t*);
typedef void (*_GL_GetFloatv_t)(uint32_t, float*);
typedef void (*_GL_GetIntegerv_t)(uint32_t, int32_t*);

static _GL_GetString_t glGetString;
static _GL_GetBooleanv_t glGetBooleanv;
static _GL_GetFloatv_t glGetFloatv;
static _GL_GetIntegerv_t glGetIntegerv;
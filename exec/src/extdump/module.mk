include $(CLEAR_VARS)

LOCAL_MODULE :=extdump

LOCAL_SRC_FILES :=extdump/extdump.cpp extdump/cl_wrapper.cpp extdump/gl_wrapper.cpp

include $(EXEC_MK)
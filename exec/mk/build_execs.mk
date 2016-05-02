LOCAL_LDLIBS +=-s -Wl,--gc-sections

#To save for recompiling this for PIE required devices (AKA API > 15)
$(call save-locals)

LOCAL_MODULE :=$(LOCAL_MODULE)_n

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

$(call restore-locals)

LOCAL_CPPFLAGS +=$(PIE_FLAGS)

include $(BUILD_EXECUTABLE)
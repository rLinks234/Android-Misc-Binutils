include $(AJDT_MK_DIR)/import_boost.mk

include $(CLEAR_VARS)

LOCAL_MODULE :=ft2-prebuilt
LOCAL_SRC_FILES :=$(AJDT_LIB_DIR)/$(TARGET_ARCH_ABI)/libfreetype2-static.a

include $(PREBUILT_STATIC_LIBRARY)
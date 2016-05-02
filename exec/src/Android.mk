LOCAL_PATH :=$(call my-dir)

include $(CLEAR_VARS)

include mk/prebuilts.mk
include $(LOCAL_PATH)/extdump/module.mk
include $(LOCAL_PATH)/bmf/module.mk
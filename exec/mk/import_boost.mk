SAVE_PATH :=$(LOCAL_PATH)

#include $(BOOST_ROOT)/Android.mk
$(call import-module,boost/1.59.0)

# Now switch back to our previous config
#NDK_APP_DST_DIR :=$(SAVE_NDK_APP_DST_DIR)
LOCAL_PATH :=$(SAVE_PATH)
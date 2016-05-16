include $(CLEAR_VARS)

LOCAL_MODULE :=bmfinfo

LOCAL_SRC_FILES :=bmf/bmfinfo.cpp

LOCAL_STATIC_LIBRARIES :=boost_program_options_static

include $(EXEC_MK)

include $(CLEAR_VARS)

LOCAL_MODULE :=bmfwriter

LOCAL_STATIC_LIBRARIES :=ft2-prebuilt boost_program_options_static

LOCAL_SRC_FILES :=bmf/bmfwriter.cpp

include $(EXEC_MK)

include $(CLEAR_VARS)

LOCAL_MODULE :=bmfhash

LOCAL_STATIC_LIBRARIES :=ft2-prebuilt boost_program_options_static

LOCAL_SRC_FILES :=bmf/bmfhash.cpp

include $(EXEC_MK)

include $(CLEAR_VARS)

LOCAL_MODULE :=bmfpack

LOCAL_STATIC_LIBRARIES :=ft2-prebuilt boost_program_options_static

LOCAL_SRC_FILES :=bmf/bmfpack.cpp

include $(EXEC_MK)

include $(CLEAR_VARS)

LOCAL_MODULE :=fontstyle

LOCAL_STATIC_LIBRARIES :=ft2-prebuilt boost_program_options_static

LOCAL_SRC_FILES :=bmf/fontstyle.cpp

include $(EXEC_MK)
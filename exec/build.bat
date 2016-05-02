@echo OFF

ndk-build -j%NUMBER_OF_PROCESSORS% NDK_PROJECT_PATH=%~dp0/src APP_BUILD_SCRIPT=%~dp0/src/Android.mk NDK_APPLICATION_MK=%~dp0/src/Application.mk NDK_LIBS_OUT=%~dp0/bin NDK_OUT=%~dp0/obj %*
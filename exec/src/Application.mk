#APP_ABI :=armeabi-v7a x86 x86_64 arm64-v8a
APP_ABI :=armeabi-v7a
NDK_TOOLCHAIN_VERSION :=clang
APP_PLATFORM :=android-14
APP_OPTIM :=release
APP_CPPFLAGS +=-frtti -fexceptions -fdata-sections -ffunction-sections -Os -std=c++14 -fvisibility=hidden
APP_STL :=c++_static

PIE_FLAGS :=-fPIE -Wl,-pie

AJDT_MK_DIR :=$(abspath $(dir $(abspath $(lastword $(MAKEFILE_LIST))))/../mk)
AJDT_LIB_DIR :=$(abspath $(dir $(abspath $(lastword $(MAKEFILE_LIST))))/../../lib)

EXEC_MK :=$(AJDT_MK_DIR)/build_execs.mk

# ----------------
# For Boost

BOOST_ROOT :=

ifeq ($(crystax-dir),)
    ifeq ($(CRYSTAX_HOME),)
        $(error use CrystaX NDK or use CRYSTAX_HOME environment variable to point to its root)
    else
        BOOST_ROOT :=$(CRYSTAX_HOME)/sources/boost/1.59.0
    endif
else
    BOOST_ROOT :=$(NDK_ROOT)/sources/boost/1.59.0
endif

APP_CPPFLAGS +=-D_LIBCPP_HAS_C11_FEATURES

APP_CPPFLAGS +=-I$(BOOST_ROOT)/include \
-I$(abspath $(dir $(abspath $(lastword $(MAKEFILE_LIST))))/../include)

# ----------------
# Functions

save-locals = $(foreach __varname,$(filter-out PATH,$(modules-LOCALS)),$(eval AJDT_$(__varname) := $(LOCAL_$(__varname))))
restore-locals = $(foreach __varname,$(filter-out PATH,$(modules-LOCALS)),$(eval LOCAL_$(__varname) := $(AJDT_$(__varname))))

save-locals-to = $(foreach __varname,$(filter-out PATH,$(modules-LOCALS)),$(eval $1_$(__varname) := $(LOCAL_$(__varname))))
restore-locals-from = $(foreach __varname,$(filter-out PATH,$(modules-LOCALS)),$(eval LOCAL_$(__varname) := $($1_$(__varname))))
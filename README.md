TODO - write brief readmes for (i) bmfwriter (ii) bmfinfo (iii) extdump

Notes:

	1) CrystaX-NDK 10.3.1 or greater is required
		i)	CrystaX is REQUIRED since we will be using boost C++ libraries, which 
			do not easily compile with NDK bionic libraries. So unless you are willing
			to implement `wchar_t` functions and misc other missing pieces in libc, use
			CrystaX :)

	2) GNU Make 3.81 (newer is fine, or just use Make that comes with NDK)

	3) g++ 4.9 is recommended if compiling for Linux 
		i)	This is recommended since I ran into GCC 5.0 C++ STL ABI unresolved symbols 
			(C++11 changed container specs & GCC inlines a namespace for C++11 compliant code)
			with the boost 1.58 library that I had on my Ubuntu 16.04 machine

		ii) clang may work, but be certain to specify a GCC toolchain compliant with the 
			boost C++ ABI on your Linux machine.

		iii) More info regarding the GCC ABI change can be seen in the following link:
			https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_dual_abi.html

	4) Android device, capable of running executables

		i)	Both PIE (position independent executable) and non-PIE versions of binaries are made. 
			For *most* Android devices with API level greater than 15, you will need the PIE 
			binaries. All compiled executables suffixed with `_n` are NON-PIE executables, which
			may be used for older devices that cannot run PIE code.

		ii) On an Android L (API lvl = 21) device, it was found that linking executables with a
			shared version of the STL, e.g. `libc++_shared` would cause C++ exceptions to go
			uncaught, even within a try...catch block. Upon further testing, a defect may be filed
			with CrystaX, NDK, ld, or clang. Be cautious and use the llvm static version, `libc++_static`
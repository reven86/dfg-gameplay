NDK_ROOT="/cygdrive/e/_work/_projects/android-ndk-r10c"
NDK_TOOLCHAIN_BIN="$NDK_ROOT/toolchains/x86-4.8/prebuilt/windows-x86_64/bin"
SDK_ROOT="$NDK_ROOT/platforms/android-9/arch-x86"
ICU_PATH="../../../../icu4c-51_2-src/icu"
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -DU_USING_ICU_NAMESPACE=1 -DU_DISABLE_RENAMING=1 -DU_HAVE_NL_LANGINFO_CODESET=0 -DU_TIMEZONE=0 -DU_ENABLE_DYLOAD=0  -DU_DISABLE_RENAMING=1"
NDK_ROOT_WIN="E:/_Work/_projects/android-ndk-r10c"
SDK_ROOT_WIN="$NDK_ROOT_WIN/platforms/android-9/arch-x86"
export CPPFLAGS="--sysroot=$SDK_ROOT_WIN -O3 -D__STDC_INT64__ $ICU_FLAGS -I$SDK_ROOT/usr/include/ -I$NDK_ROOT_WIN/sources/cxx-stl/gnu-libstdc++/4.8/include/ -I$NDK_ROOT_WIN/sources/cxx-stl/gnu-libstdc++/4.8/libs/x86/include/"
export LDFLAGS="--sysroot=$SDK_ROOT_WIN -lc -lstdc++ -lgnustl_shared -Wl,-rpath-link=$SDK_ROOT_WIN/usr/lib -L$SDK_ROOT_WIN/usr/lib -L$NDK_ROOT_WIN/sources/cxx-stl/gnu-libstdc++/4.8/libs/x86"
export CC=$NDK_TOOLCHAIN_BIN/i686-linux-android-gcc-4.8
export CXX=$NDK_TOOLCHAIN_BIN/i686-linux-android-g++
export RANLIB=$NDK_TOOLCHAIN_BIN/i686-linux-android-ranlib
export AR=$NDK_TOOLCHAIN_BIN/i686-linux-android-ar
../../source/configure --with-cross-build=/cygdrive/e/_Work/icu4c-51_2-src/icu/build/icu-release-static-win32-vs2013 --enable-extras=no --enable-strict=no --enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no --disable-renaming --host=x86-linux-android

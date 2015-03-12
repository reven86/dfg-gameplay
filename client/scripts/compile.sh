cd Downloads/
ls
cd icu
ls
cd source/
chmod +x runConfigureICU config install-sh
runConfigureICU --help
bash runConfigureICU --help
bash runConfigureICU MacOSX --enable-static
/usr/bin/gnumake
gmake
make
/usr/bin/gnumake
bash runConfigureICU MacOSX --enable-static -arch i386
bash runConfigureICU --help
CXXFLAGS=i386
CXXFLAGS
bash CXXFLAGS=i386 runConfigureICU MacOSX --enable-static -arch
bash CXXFLAGS=i386 runConfigureICU MacOSX --enable-static
CXXFLAGS=i386 bash runConfigureICU MacOSX --enable-static
CXXFLAGS=-arch i386 bash runConfigureICU MacOSX --enable-static
CXXFLAGS="-arch i386" bash runConfigureICU MacOSX --enable-static
bash runConfigureICU MacOSX --enable-static --with-library-bits=32
/usr/bin/gnumake
bash runConfigureICU MacOSX --enable-static --with-library-bits=32
/usr/bin/gnumake --always-make
bash runConfigureICU MacOSX --enable-static --with-library-bits=32
/usr/bin/gnumake --always-make
/usr/bin/gnumake
/usr/bin/gnumake -B
/usr/bin/gnumake
/usr/bin/gnumake
top
ls
cd Documents/Projects/
ls
/Users/reven86/Documents/Projects/libyaml-multiplatform/_xcode/Build/Products/Release/libyaml.a
export CC="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/llvm-gcc-4.2"
export CFLAGS="-arch armv7 -arch armv7s -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
export LDFLAGS="-arch armv7 -arch armv7s -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
curl -O http://curl.haxx.se/download/curl-7.30.0.tar.gz
tar -xzf curl-7.31.0.tar.gz
tar -xzf curl-7.30.0.tar.gz
cd curl-7.30.0
export CFLAGS="-arch armv7 -arch armv7s -arch i386 -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
export LDFLAGS="-arch armv7 -arch armv7s -arch i386 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
./configure --disable-shared --enable-static --disable-dependency-tracking --host=armv7-apple-darwin
cd Documents/Projects/
cd curl-7.30.0
./configure --disable-shared --enable-static --disable-dependency-tracking --host=armv7-apple-darwin
export CC="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/llvm-gcc-4.2"
export CFLAGS="-arch armv7 -arch armv7s -arch i386 -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
export LDFLAGS="-arch armv7 -arch armv7s -arch i386 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
./configure --disable-shared --enable-static --disable-dependency-tracking --host=armv7-apple-darwin
ls
cd ~/Downloads/icu
ls
mkdir build_icu_ios
cd build_icu_ios/
DEVROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer
SDKROOT=$DEVROOT/SDKs/iPhoneOS6.1.sdk
SYSROOT=$SDKROOT
ls ..
ICU_PATH=..
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 \
-fno-short-wchar -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
export CPPFLAGS="-I$SDKROOT/usr/include/ -I$SDKROOT/usr/include/ -I./include/ \
-miphoneos-version-min=2.2 $ICU_FLAGS -pipe -no-cpp-precomp -isysroot $SDKROOT"
export CC="$DEVROOT/usr/llvm-gcc-4.2/bin/arm-apple-darwin10-llvm-gcc-4.2"
export CXX="$DEVROOT/usr/llvm-gcc-4.2/bin/arm-apple-darwin10-llvm-g++-4.2"
export LDFLAGS="-L$SDKROOT/usr/lib/ -isysroot $SDKROOT -Wl,-dead_strip \
-miphoneos-version-min=2.0"
sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static
das
export MY_DIR=~/Downloads/icu
cd MY_DIR
cd $MY_DIR
mkdir build_icu_mac
cd build_icu_mac
ICUSRC_SOURCES=$MYDIR/icu-51.2
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
ICUSRC_SOURCES=$MYDIR
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
ls
cd ..
cd build_icu_mac
export ICUSRC_PATH=$MYDIR
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
export ICUSRC_PATH=$MY_DIR
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
cd $MY_DIR
cd $ICUSOURCE_PATH
export MY_DIR=~/Downloads/icu
cd $MY_DIR
cd build_icu_mac
export MY_DIR=~/Downloads/icu
cd $MY_DIR
cd build_icu_mac
ICUSRC_PATH=$MY_DIR
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make
cd ../build_icu_ios/
DEVROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer
SDKROOT=$DEVROOT/SDKs/iPhoneOS6.1.sdk
SYSROOT=$SDKROOT
ICU_PATH=$MY_DIR
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 \
-fno-short-wchar -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
export CPPFLAGS="-I$SDKROOT/usr/include/ -I$SDKROOT/usr/include/ -I./include/ \
-miphoneos-version-min=2.2 $ICU_FLAGS -pipe -no-cpp-precomp -isysroot $SDKROOT"
export CC="$DEVROOT/usr/llvm-gcc-4.2/bin/arm-apple-darwin10-llvm-gcc-4.2"
export CXX="$DEVROOT/usr/llvm-gcc-4.2/bin/arm-apple-darwin10-llvm-g++-4.2"
export LDFLAGS="-L$SDKROOT/usr/lib/ -isysroot $SDKROOT -Wl,-dead_strip \
-miphoneos-version-min=2.0"
sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static --disable-shared -with-cross-build=$MY_DIR/icu_build_mac --prefix=$PWD/icu_build
sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static --disable-shared -with-cross-build=$MY_DIR/build_icu_mac --prefix=$PWD/icu_build
gnumake
export MY_DIR=~/Downloads/icu
cd $MY_DIR
mkdir build_icu_mac
cd build_icu_mac
ls
ICUSRC_PATH=$MY_DIR
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no -arch i386
sh $ICUSRC_PATH/source/runConfigureICU --help
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1 p"
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1 -arch i386"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
export LDFLAGS="-arch i386"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
make install
cd /Users/reven86/Documents/Projects/dfg-story-of-dragons/Client/Source/_xcode/../../../ThirdParty/icu/lib/macosx\
/Users/reven86/Documents/Projects/dfg-story-of-dragons/Client/Source/_xcode/../../../ThirdParty/icu/lib/macosx
export MY_DIR=~/Downloads/icu
cd $MY_DIR
cd build_icu_mac
ICUSRC_PATH=$MY_DIR
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=0 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1 -arch i386"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
export LDFLAGS="-arch i386"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCO -arch i386"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
make install
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
make install
make -j4
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1 -arch i386"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
make install
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1 -arch i386"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1 -arch i386"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
make install
cd $MY_DIR
mkdir build_icu_simulator
cd build_icu_simulator/
DEVROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer
SDKROOT=$DEVROOT/SDKs/iPhoneSimulator6.1.sdk
SYSROOT=$SDKROOT
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 \
-DU_USING_ICU_NAMESPACE=1 -fno-short-enums -DU_HAVE_NL_LANGINFO_CODESET=0 \
-D__STDC_INT64__ -DU_TIMEZONE=0 -DUCONFIG_NO_LEGACY_CONVERSION=1 \
-DUCONFIG_NO_BREAK_ITERATION=1 -DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 \
-DUCONFIG_NO_TRANSLITERATION=0 -DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 \
-DU_USING_ICU_NAMESPACE=1 -fno-short-enums -DU_HAVE_NL_LANGINFO_CODESET=0 \
-D__STDC_INT64__ -DU_TIMEZONE=0 -DUCONFIG_NO_LEGACY_CONVERSION=1 \
-DUCONFIG_NO_BREAK_ITERATION=1 -DUCONFIG_NO_COLLATION=1 \
-DUCONFIG_NO_TRANSLITERATION=0 -DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
export CPPFLAGS="-I$SDKROOT/usr/include/ -I$SDKROOT/usr/include/ -I./include/ \
-miphoneos-version-min=2.2 $ICU_FLAGS -pipe -arch i386 -no-cpp-precomp \
-isysroot $SDKROOT"
export LDFLAGS="-arch i386 -L$SDKROOT/usr/lib/ -isysroot $SDKROOT \
-Wl,-dead_strip -miphoneos-version-min=2.0"export LDFLAGS="-arch i386 -L$SDKROOT/usr/lib/ -isysroot $SDKROOT \
-Wl,-dead_strip -miphoneos-version-min=2.0"export LDFLAGS="-arch i386 -L$SDKROOT/usr/lib/ -isysroot $SDKROOT \
-Wl,-dead_strip -miphoneos-version-min=2.0"
export LDFLAGS="-arch i386 -L$SDKROOT/usr/lib/ -isysroot $SDKROOT \
-Wl,-dead_strip -miphoneos-version-min=2.0"
sh $ICU_PATH/source/configure --host=i686-apple-darwin11 --enable-static --disable-shared -with-cross-build=$ICU_PATH/build_icu_mac --prefix=$PWD/icu_build
ICU_PATH
ICU_PATH=$MY_DIR
sh $ICU_PATH/source/configure --host=i686-apple-darwin11 --enable-static --disable-shared -with-cross-build=$ICU_PATH/build_icu_mac --prefix=$PWD/icu_build
make -j4
make install
cd ../build_icu_ios/
DEVROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer
SDKROOT=$DEVROOT/SDKs/iPhoneOS5.1.sdk
SYSROOT=$SDKROOT
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 \
-fno-short-wchar -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 -fno-short-wchar -DU_USING_ICU_NAMESPACE=1 -fno-short-enums -DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 -DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 -DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_TRANSLITERATION=0
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 \
-fno-short-wchar -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 -fno-short-wchar -DU_USING_ICU_NAMESPACE=1 -fno-short-enums -DU_HAVE_NL_LANGINFO_CODESET=0 -D__S[B
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 \
-fno-short-wchar -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 -fno-short-wchar -DU_USING_ICU_NAMESPACE=1 -fno-short-enums -DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 -DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 -DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_TRANSLITERATION=0 .DS_Store .DS_Store
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 \
-fno-short-wchar -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 \
-fno-short-wchar -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
export CPPFLAGS="-I$SDKROOT/usr/include/ -I$SDKROOT/usr/include/ -I./include/ \
-miphoneos-version-min=2.2 $ICU_FLAGS -pipe -no-cpp-precomp -isysroot $SDKROOT"
export CC="$DEVROOT/usr/llvm-gcc-4.2/bin/arm-apple-darwin10-llvm-gcc-4.2"
export CXX="$DEVROOT/usr/llvm-gcc-4.2/bin/arm-apple-darwin10-llvm-g++-4.2"
export LDFLAGS="-L$SDKROOT/usr/lib/ -isysroot $SDKROOT -Wl,-dead_strip \
-miphoneos-version-min=2.0"sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static --disable-shared -with-cross-build=$MY_DIR/icu_build_mac --prefix=$PWD/icu_build
export LDFLAGS="-L$SDKROOT/usr/lib/ -isysroot $SDKROOT -Wl,-dead_strip \
-miphoneos-version-min=2.0"
sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static --disable-shared -with-cross-build=$MY_DIR/icu_build_mac --prefix=$PWD/icu_build
sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static --disable-shared -with-cross-build=$MY_DIR/build_icu_mac --prefix=$PWD/icu_build
sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static --disable-shared -with-cross-build=$MY_DIR/build_icu_mac --prefix=$PWD/icu_build
ls /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/
ls /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.simulator/Developer/usr/
/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/llvm-gcc-4.2/bin/arm-apple-darwin10-llvm-gcc-4.2
ls /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr
ls /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/llvm-gcc-4.2
ls /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/llvm-gcc-5.0
/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/llvm-gcc-5.0/bin/arm-apple-darwin10-llvm-gcc-5.0
xcrun -find -sdk iphoneos clang
xcrun -find -sdk iphoneos clang++
export CC="$DEVROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
export CXX="$DEVROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++"
sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static --disable-shared -with-cross-build=$MY_DIR/build_icu_mac --prefix=$PWD/icu_build
DEVROOT=/Applications/Xcode.app/Contents/Developer/
DEVROOT=/Applications/Xcode.app/Contents/Developer
export CC="$DEVROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
export CXX="$DEVROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++"
sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static --disable-shared -with-cross-build=$MY_DIR/build_icu_mac --prefix=$PWD/icu_build
xcrun -find -sdk iphoneos clang
sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static --disable-shared -with-cross-build=$MY_DIR/build_icu_mac --prefix=$PWD/icu_build
xcrun -find -sdk iphonesimulator clang
xcrun -find -sdk iphoneos -arch armv7 clang
export CPPFLAGS="-I$SDKROOT/usr/include/ -I$SDKROOT/usr/include/ -I./include/ \
-miphoneos-version-min=2.2 $ICU_FLAGS -arch armv7 -pipe -no-cpp-precomp -isysroot $SDKROOT"
export LDFLAGS="-L$SDKROOT/usr/lib/ -isysroot $SDKROOT -Wl,-dead_strip \
-miphoneos-version-min=2.0 -arch armv7"
sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static --disable-shared -with-cross-build=$MY_DIR/build_icu_mac --prefix=$PWD/icu_build --arch=arm
sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static --disable-shared -with-cross-build=$MY_DIR/build_icu_mac --prefix=$PWD/icu_build
make -j4
make install
defaults write com.apple.appstore ShowDebugMenu -bool true
sudo defaults delete /Library/Preferences/com.apple.SoftwareUpdate.plist RecommendedUpdates ; sudo defaults delete /Library/Preferences/com.apple.SoftwareUpdate.plist DidRegisterLocalUpdates
cd ~/Documents/Projects/GamePlay/
sh install.sh
cd ~/Downloads/icu
ls
export MY_DIR=~/Downloads/icu
cd $MY_DIR
cd build_icu_mac
ICUSRC_PATH=$MY_DIR
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
make install
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1 -arch x86_64"
export LDFLAGS="-arch x86_64"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
make install
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=0 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
make install
cd /Users/reven86/Documents/Projects/wot-tank-explorer/Client/Source/_xcode/../../../ThirdParty/zip/lib/ios
ls
cd Documents/Projects/GamePlay/
sh install.sh
top
codesign ->
codesign -?
cd Downloads/
codesign -s "Developer ID Application" --deep Story\ of\ Dragons.app
codesign -s "Developer ID Application" -f --deep Story\ of\ Dragons.app
Andrews-Mac-mini:~ reven86$

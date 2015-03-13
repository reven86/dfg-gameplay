export MY_DIR=~/Downloads/icu
cd $MY_DIR
cd build_icu_mac
ICUSRC_PATH=$MY_DIR
export CPPFLAGS="-O3 -DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_FORMATTING=0 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1 -arch i386"
export LDFLAGS="-arch i386"
sh $ICUSRC_PATH/source/runConfigureICU MacOSX --prefix=$PWD/icu_build --enable-extras=no --enable-strict=no -enable-static --enable-shared=no --enable-tests=no --enable-samples=no --enable-dyload=no
make -j4
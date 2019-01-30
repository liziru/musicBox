#!/bin/sh
OUT_DIR="`pwd`/out"
echo $OUT_DIR
mkdir $OUT_DIR
./configure --host=aarch64-linux-gnu \
	--prefix=$OUT_DIR
make

make check

make install
cp -a ${OUT_DIR}/include/log4cpp ../../../out/include/
cp ${OUT_DIR}/lib/liblog4cpp.so.5.0.6 ../../../out/lib/
cp ${OUT_DIR}/../../myLog.h ../../../out/include/
ln -s liblog4cpp.so.5.0.6 ../../../out/lib/liblog4cpp.so
ln -s liblog4cpp.so.5.0.6 ../../../out/lib/liblog4cpp.so.5
#!/bin/bash

CC=g++

echo "use OSTYPE:"
if [ "$OSTYPE" = "linux-gnu" ]; then
  echo -e "Linux platform\n"
elif [ "$OSTYPE" = "msys" ]; then
  echo -e "Windows platform: MinGW\n"
fi

INC=$PIDSDK_DIR/inc
JNI_INC=$JAVA_HOME/include

if [ "$OSTYPE" = "linux-gnu" ]; then
  JNI_INC_SUB=$JNI_INC/linux
  IRIS_ENGINE=$PIDSDK_DIR/lib
  OUTPUT_FILE=./libirisenginehelper.so
elif [[ $OSTYPE == "msys" || $OSTYPE == "cygwin" ]]; then
  JNI_INC_SUB=$JNI_INC/win32
  IRIS_ENGINE=$PIDSDK_DIR/bin
  OUTPUT_FILE=./irisenginehelper.dll
fi

SRC_FILES=IrisEngineHelper.cpp

set -x

$CC -o $OUTPUT_FILE \
  -shared -fPIC \
  -I "$JNI_INC" \
  -I "$JNI_INC_SUB" \
  -I "$INC" \
  -L "$IRIS_ENGINE" -lirisengine \
  $SRC_FILES

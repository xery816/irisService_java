# 1. Requirement

    JDK >= 1.8 (correto jdk with javafx)
    pidsdk >= 3.23.6

# 2. Build
## 2.1 pre-build
```bash
export PIDSDK_DIR=<pidsdk directory>
```

## 2.1 build libirisenginehelper.so
```
# libirisenginehelper.so is already prebuilt with g++ on Ubuntu and put in <pidsdk-java root directory>/libs/ubuntu-x64
# if you don't want to build it, then skip this step
#
export JAVA_HOME=/usr/lib/jvm/java-1.8.0-amazon-corretto
cd <pidsdk-java root directory>/pidsdk-java/libIrisHelper/src/main/cpp/IrisEngineHelper
./build.sh
```

## 2.3 post-build
```
cp libirisenginehelper.so $PIDSDK_DIR/lib
```

## 2.2 build jar


# 3. Run


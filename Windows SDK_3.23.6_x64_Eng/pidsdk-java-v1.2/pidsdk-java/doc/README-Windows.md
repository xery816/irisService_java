# 1. Requirement

    JDK >= 1.8 (correto jdk with javafx)
    win_pidsdk(x64) >= 3.23.6


# 2. Build
## 2.1 pre-build
```bash
export PIDSDK_DIR=<win_pidsdk directory>
```

## 2.2 build irisenginehelper.dll
```
# irisenginehelper.dll is already prebuilt with MinGW64 and put in <pidsdk-java root directory>/libs/windows-x64
# MinGW64 build tools: [msys64] + [x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0]
# if you don't want to build it, then skip this step
#
cd <pidsdk-java root directory>/pidsdk-java/libIrisHelper/src/main/cpp/IrisEngineHelper
./build.sh
```

## 2.3 post-build
```
cp irisenginehelper.dll $PIDSDK_DIR/bin
```

## 2.4 build jar

```powershell
# run in powershell
$env:PIDSDK_DIR=<win_pidsdk directory>
cd <pidsdk-java root directory>/pidsdk-java
.\gradlew build
cp .\app\build\libs\app-1.0.jar $env:PIDSDK_DIR\bin
cp .\libIrisHelper\build\libs\libIrisHelper.jar $env:PIDSDK_DIR\bin
```


# 3. Run
```powershell
# run in powershell
cd $env:PIDSDK_DIR\bin
java -cp "app-1.0.jar;libIrisHelper.jar" cn.simbok.demo.App
```

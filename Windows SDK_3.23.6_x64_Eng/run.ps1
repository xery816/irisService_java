# Iris Java Demo Run Script
# Build and Run

$env:PIDSDK_DIR = "F:\iris_java\Windows SDK_3.23.6_x64_Eng\win_pidsdk_3.23.6_x64"

Write-Host "========================================"
Write-Host "  Iris Java Demo Build and Run"
Write-Host "========================================"
Write-Host ""

# Check SDK directory
if (-Not (Test-Path $env:PIDSDK_DIR)) {
    Write-Host "ERROR: SDK directory not found: $env:PIDSDK_DIR" -ForegroundColor Red
    pause
    exit 1
}

# Go to project directory
$ProjectDir = "F:\iris_java\Windows SDK_3.23.6_x64_Eng\pidsdk-java-v1.2\pidsdk-java"
if (-Not (Test-Path $ProjectDir)) {
    Write-Host "ERROR: Project directory not found: $ProjectDir" -ForegroundColor Red
    pause
    exit 1
}

Write-Host "[1/4] Entering project directory..." -ForegroundColor Green
Set-Location $ProjectDir

# Clean first (to clear any cached errors)
Write-Host "[2/4] Cleaning previous build..." -ForegroundColor Yellow
& .\gradlew clean --no-daemon

# Build project
Write-Host "[3/4] Building project..." -ForegroundColor Green
Write-Host "This may take a while on first run (downloading dependencies)..." -ForegroundColor Gray
& .\gradlew build --no-daemon

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "ERROR: Build failed!" -ForegroundColor Red
    Write-Host "Please check the error messages above." -ForegroundColor Yellow
    pause
    exit 1
}

Write-Host "Build successful!" -ForegroundColor Green
Write-Host ""

# Copy files
Write-Host "[4/4] Copying files and starting application..." -ForegroundColor Green

$AppJar = ".\app\build\libs\app-1.0.jar"
$HelperJar = ".\libIrisHelper\build\libs\libIrisHelper.jar"
$TargetBin = "$env:PIDSDK_DIR\bin"

if (Test-Path $AppJar) {
    Copy-Item $AppJar $TargetBin -Force
    Write-Host "  Copied: app-1.0.jar" -ForegroundColor Gray
} else {
    Write-Host "  WARNING: Not found $AppJar" -ForegroundColor Yellow
}

if (Test-Path $HelperJar) {
    Copy-Item $HelperJar $TargetBin -Force
    Write-Host "  Copied: libIrisHelper.jar" -ForegroundColor Gray
} else {
    Write-Host "  WARNING: Not found $HelperJar" -ForegroundColor Yellow
}

# Copy JNI library
$JniDll = "..\libs\windows-x64\irisenginehelper.dll"
if (Test-Path $JniDll) {
    Copy-Item $JniDll $TargetBin -Force
    Write-Host "  Copied: irisenginehelper.dll" -ForegroundColor Gray
}

Write-Host ""
Write-Host "Starting application..." -ForegroundColor Green
Set-Location $TargetBin

java -cp "app-1.0.jar;libIrisHelper.jar" cn.simbok.demo.App

Write-Host ""
Write-Host "Application exited." -ForegroundColor Gray
pause

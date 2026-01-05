# Build Only Script (No Run)

$env:PIDSDK_DIR = "F:\iris_java\Windows SDK_3.23.6_x64_Eng\win_pidsdk_3.23.6_x64"

Write-Host "Starting build..." -ForegroundColor Green

Set-Location "F:\iris_java\Windows SDK_3.23.6_x64_Eng\pidsdk-java-v1.2\pidsdk-java"

# Clean old build
Write-Host "Cleaning old build..." -ForegroundColor Yellow
& .\gradlew clean

# Build project
Write-Host "Building project..." -ForegroundColor Green
& .\gradlew build

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful!" -ForegroundColor Green
    
    # Copy files
    Write-Host "Copying JAR files..." -ForegroundColor Green
    Copy-Item .\app\build\libs\app-1.0.jar "$env:PIDSDK_DIR\bin\" -Force
    Copy-Item .\libIrisHelper\build\libs\libIrisHelper.jar "$env:PIDSDK_DIR\bin\" -Force
    
    # Copy JNI library
    $JniDll = "..\libs\windows-x64\irisenginehelper.dll"
    if (Test-Path $JniDll) {
        Copy-Item $JniDll "$env:PIDSDK_DIR\bin\" -Force
        Write-Host "Copied JNI library" -ForegroundColor Gray
    }
    
    Write-Host ""
    Write-Host "All files copied to: $env:PIDSDK_DIR\bin\" -ForegroundColor Cyan
} else {
    Write-Host "Build failed!" -ForegroundColor Red
}

Write-Host ""
pause

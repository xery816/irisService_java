# Run Only Script (No Build)

$env:PIDSDK_DIR = "F:\iris_java\Windows SDK_3.23.6_x64_Eng\win_pidsdk_3.23.6_x64"

Write-Host "Starting Iris Recognition Application..." -ForegroundColor Green
Write-Host ""

# Check JAR files exist
$BinDir = "$env:PIDSDK_DIR\bin"
$AppJar = "$BinDir\app-1.0.jar"
$HelperJar = "$BinDir\libIrisHelper.jar"

if (-Not (Test-Path $AppJar)) {
    Write-Host "ERROR: app-1.0.jar not found" -ForegroundColor Red
    Write-Host "Please run build.ps1 or run.ps1 first!" -ForegroundColor Yellow
    pause
    exit 1
}

if (-Not (Test-Path $HelperJar)) {
    Write-Host "ERROR: libIrisHelper.jar not found" -ForegroundColor Red
    Write-Host "Please run build.ps1 or run.ps1 first!" -ForegroundColor Yellow
    pause
    exit 1
}

Set-Location $BinDir

Write-Host "Working directory: $BinDir" -ForegroundColor Gray
Write-Host "Command: java -cp app-1.0.jar;libIrisHelper.jar cn.simbok.demo.App" -ForegroundColor Gray
Write-Host ""

java -cp "app-1.0.jar;libIrisHelper.jar" cn.simbok.demo.App

Write-Host ""
Write-Host "Application exited." -ForegroundColor Gray
pause

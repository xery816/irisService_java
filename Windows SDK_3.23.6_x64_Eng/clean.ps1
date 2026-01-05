# Clean Build Files Script

$ProjectDir = "F:\iris_java\Windows SDK_3.23.6_x64_Eng\pidsdk-java-v1.2\pidsdk-java"

Write-Host "Cleaning project build files..." -ForegroundColor Yellow

Set-Location $ProjectDir

# Gradle clean
& .\gradlew clean

if ($LASTEXITCODE -eq 0) {
    Write-Host "Clean completed!" -ForegroundColor Green
} else {
    Write-Host "Clean failed!" -ForegroundColor Red
}

# Optional: delete JAR files in bin directory
$DeleteBinJars = Read-Host "Delete JAR files in SDK bin directory? (y/n)"
if ($DeleteBinJars -eq "y" -or $DeleteBinJars -eq "Y") {
    $env:PIDSDK_DIR = "F:\iris_java\Windows SDK_3.23.6_x64_Eng\win_pidsdk_3.23.6_x64"
    $BinDir = "$env:PIDSDK_DIR\bin"
    
    Remove-Item "$BinDir\app-1.0.jar" -ErrorAction SilentlyContinue
    Remove-Item "$BinDir\libIrisHelper.jar" -ErrorAction SilentlyContinue
    
    Write-Host "Deleted JAR files from bin directory" -ForegroundColor Green
}

Write-Host ""
pause

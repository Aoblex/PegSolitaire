# PowerShell script to create and push a new release tag
# Usage: .\create-release.ps1 [version]
# Example: .\create-release.ps1 1.0.0

param(
    [Parameter(Mandatory=$true)]
    [string]$Version
)

# Validate version format (semantic versioning)
if (-not ($Version -match '^\d+\.\d+\.\d+(-\w+)?$')) {
    Write-Host "❌ Invalid version format. Please use semantic versioning (e.g., 1.0.0, 1.2.3-beta)" -ForegroundColor Red
    exit 1
}

$tagName = "v$Version"

Write-Host "🏷️  Creating release tag: $tagName" -ForegroundColor Green

# Check if tag already exists
$existingTag = git tag -l $tagName
if ($existingTag) {
    Write-Host "❌ Tag $tagName already exists!" -ForegroundColor Red
    exit 1
}

# Check if there are uncommitted changes
$status = git status --porcelain
if ($status) {
    Write-Host "⚠️  Warning: You have uncommitted changes:" -ForegroundColor Yellow
    git status --short
    
    $continue = Read-Host "Do you want to continue? (y/N)"
    if ($continue -ne 'y' -and $continue -ne 'Y') {
        Write-Host "Release creation cancelled." -ForegroundColor Yellow
        exit 0
    }
}

# Update version in CMakeLists.txt
Write-Host "📝 Updating version in CMakeLists.txt..." -ForegroundColor Blue
$cmakeContent = Get-Content "CMakeLists.txt" -Raw
$cmakeContent = $cmakeContent -replace 'project\(solitaire VERSION [\d\.]+', "project(solitaire VERSION $Version"
$cmakeContent | Set-Content "CMakeLists.txt" -NoNewline

# Check if version was updated
$updatedContent = Get-Content "CMakeLists.txt" -Raw
if ($updatedContent -match "project\(solitaire VERSION $Version") {
    Write-Host "✅ Version updated in CMakeLists.txt" -ForegroundColor Green
} else {
    Write-Host "⚠️  Warning: Could not update version in CMakeLists.txt" -ForegroundColor Yellow
}

# Create git tag
Write-Host "🏷️  Creating git tag..." -ForegroundColor Blue

# Add updated files if any
git add CMakeLists.txt

# Commit version update if there are changes
$changes = git diff --staged
if ($changes) {
    git commit -m "chore: bump version to $Version"
    Write-Host "✅ Version update committed" -ForegroundColor Green
}

# Create annotated tag
$tagMessage = @"
Release $tagName

Features:
- Beautiful Peg Solitaire game with multiple board types
- Intuitive mouse and keyboard controls  
- AI-powered strategy hints
- Win/lose detection with replay options
- Fullscreen mode support
- Modern Qt6-based interface

Game Modes:
- English (Classic), Diamond, Square, Anti-Peg, Endgame

System Requirements:
- Windows 10 or later (64-bit)
- 100 MB free disk space
"@

git tag -a $tagName -m $tagMessage

if ($LASTEXITCODE -eq 0) {
    Write-Host "✅ Tag $tagName created successfully" -ForegroundColor Green
    
    # Ask if user wants to push the tag
    $pushTag = Read-Host "Do you want to push the tag to trigger the release build? (Y/n)"
    if ($pushTag -ne 'n' -and $pushTag -ne 'N') {
        Write-Host "🚀 Pushing tag to origin..." -ForegroundColor Blue
        git push origin $tagName
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✅ Tag pushed successfully!" -ForegroundColor Green
            Write-Host "🎉 Release workflow should start shortly on GitHub Actions" -ForegroundColor Cyan
            Write-Host "📦 Check the Actions tab on GitHub to monitor the build progress" -ForegroundColor Cyan
        } else {
            Write-Host "❌ Failed to push tag" -ForegroundColor Red
        }
    }
} else {
    Write-Host "❌ Failed to create tag" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "🎯 Next steps:" -ForegroundColor Cyan
Write-Host "1. Monitor the GitHub Actions workflow" -ForegroundColor White
Write-Host "2. Check the Releases page once the build completes" -ForegroundColor White
Write-Host "3. Download and test the generated Windows executable" -ForegroundColor White

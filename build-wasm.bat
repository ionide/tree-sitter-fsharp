@echo off
REM Build WASM files for tree-sitter-fsharp

if not exist wasm mkdir wasm

echo Building WASM for fsharp grammar...
cd fsharp
call npx tree-sitter build --wasm
if errorlevel 1 (
    echo Failed to build fsharp WASM
    cd ..
    exit /b 1
)
move /Y tree-sitter-fsharp.wasm ..\wasm\ >nul 2>&1
cd ..

echo Building WASM for fsharp_signature grammar...
cd fsharp_signature
call npx tree-sitter build --wasm
if errorlevel 1 (
    echo Failed to build fsharp_signature WASM
    cd ..
    exit /b 1
)
move /Y tree-sitter-fsharp_signature.wasm ..\wasm\ >nul 2>&1
cd ..

echo.
echo WASM files built successfully in wasm\ directory:
if exist wasm\tree-sitter-fsharp.wasm (
    echo   - wasm\tree-sitter-fsharp.wasm
)
if exist wasm\tree-sitter-fsharp_signature.wasm (
    echo   - wasm\tree-sitter-fsharp_signature.wasm
)
echo.

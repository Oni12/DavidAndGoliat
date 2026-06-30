@echo off
REM Compilacion directa sin CMake (MSYS2/MinGW)
g++ src/main.cpp src/game.cpp src/player.cpp src/level1.cpp src/level2.cpp src/level3.cpp src/tiles.cpp src/utils.cpp -o "David_Goliat.exe" -I"C:/msys64/ucrt64/include" -L"C:/msys64/ucrt64/lib" -lraylib -lopengl32 -lgdi32 -lwinmm -std=c++17
if %errorlevel% equ 0 (
    echo Compilacion exitosa!
    echo Ejecuta David_Goliat.exe
) else (
    echo Error en la compilacion
)
pause

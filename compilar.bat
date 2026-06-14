@echo off
setlocal enabledelayedexpansion

REM comentario
:: comentario

for %%I in (.) do set "CARPETA=%%~nxI"

echo .
echo Compilando el proyecto
echo .

for %%F in (*.asm) do (
    echo Compilando %%F ...
    echo .
    nasm -f win64 "%%F" -o "%%~nF.obj"
    if errorlevel 1 (
        echo Error al compilar Nasm 
        exit /b 1
    )
)

::guardemos en vectores los .obj y los .c
set "OBJS="
for %%F in (*.obj) do (
    set "OBJS=!OBJS! %%F"
)

set "CSRC="
for %%F in (*.c) do (
    set "CSRC=!CSRC! %%F"
)

::enlazar en gcc
echo .
echo Compilando en GCC 

gcc %CSRC% %OBJS% -o "%CARPETA%.exe"
if errorlevel 1 (
    echo Error al compilar GCC 
    exit /b 1
)

echo.
echo Finalizado el compilado
endlocal
pause
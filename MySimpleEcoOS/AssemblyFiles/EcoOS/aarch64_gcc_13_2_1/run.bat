@echo off
echo Запуск MySimpleEcoOS через QEMU...
echo.

REM Проверка наличия kernel8.img
if not exist "%~dp0kernel8.img" (
    echo ОШИБКА: Файл kernel8.img не найден!
    echo Необходимо сначала собрать проект: make -f Makefile
    pause
    exit /b 1
)

REM Проверка переменной окружения ECO_TOOLCHAIN
if "%ECO_TOOLCHAIN%"=="" (
    echo ОШИБКА: Переменная окружения ECO_TOOLCHAIN не установлена!
    pause
    exit /b 1
)

REM Проверка наличия QEMU
if not exist "%ECO_TOOLCHAIN%\qemu_2_12\qemu\qemu-system-aarch64.exe" (
    echo ОШИБКА: QEMU не найден по пути: %ECO_TOOLCHAIN%\qemu_2_12\qemu\qemu-system-aarch64.exe
    pause
    exit /b 1
)

echo Запуск QEMU...
echo.
"%ECO_TOOLCHAIN%\qemu_2_12\qemu\qemu-system-aarch64.exe" -M raspi3 -serial stdio -kernel "%~dp0kernel8.img"

pause
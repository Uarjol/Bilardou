@echo off
g++ -g main.cpp -o main.exe -I "C:\SFML-3.0.2\include" -L "C:\SFML-3.0.2\lib" -DSFML_STATIC -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lfreetype -lopengl32 -lwinmm -lgdi32 -lws2_32 -static-libgcc -static-libstdc++ -static
if %errorlevel% == 0 (
    echo Compilacion exitosa
    main.exe
) else (
    echo Error de compilacion
)
pause
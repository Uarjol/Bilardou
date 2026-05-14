import struct

# Leer la fuente
with open(r"C:\Windows\Fonts\comic.ttf", "rb") as f:
    data = f.read()

# Generar el archivo .h
with open("comic.h", "w") as f:
    f.write("#pragma once\n")
    f.write(f"const unsigned int comicFontSize = {len(data)};\n")
    f.write("const unsigned char comicFont[] = {\n    ")
    
    bytes_list = [str(b) for b in data]
    # Escribir de a 16 bytes por linea
    for i, b in enumerate(bytes_list):
        f.write(b)
        if i < len(bytes_list) - 1:
            f.write(",")
        if (i + 1) % 16 == 0:
            f.write("\n    ")
    
    f.write("\n};\n")

print("comic.h generado exitosamente!")



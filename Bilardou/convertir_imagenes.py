import os

imagenes = [
    # Mascota base
    "bilardobasebu", "bilardobasesuciobu", "bilardobasedormidobu", "bilardobasesuciodormidobu",
    # Mascota super sayajin
    "bilardosupersayajinbu", "bilardosupersayajinsuciobu", "bilardosupersayajindormidobu", "bilardosupersayajinsuciodormidobu",
    # Mascota pomni
    "bilardopomnibu", "bilardopomnisuciobu", "bilardopomnidormidobu", "bilardopomnisuciodormidobu",
    # Mascota exe
    "bilardoexebu", "bilardoexesuciobu", "bilardoexedormidobu", "bilardoexesuciodormidobu",
    # Mascota cuphead
    "bilardocupheadbu", "bilardocupheadsuciobu", "bilardocupheaddormidobu", "bilardocupheadsuciodormidobu",
    # Mascota san
    "bilardosanbu", "bilardosansuciobu", "bilardosandormidobu", "bilardosansuciodormidobu",
    # Cabezas para juegos
    "bilardobasecabezabu", "bilardosupersayajincabezabu", "bilardopomnicabezabu",
    "bilardoexecabezabu", "bilardocupheadcabezabu", "bilardosancabezabu",
    # Skins bloqueadas
    "bilardosupersayajinnobu", "bilardopomninobu", "bilardoexenobu", "bilardocupheadnobu",
    "bilardosannobu",
    # Elementos habitaciones
    "veladorapagadobu", "veladorprendido", "esponjabu", "heladerabu",
    "joystickbu", "armariobu", "camabu",
    # Comida
    "comidaparaperrosbu", "raviolesbu", "milanesabu", "pizzabu",
    "asadodecesarbu", "heladobu", "cactusbu",
    # Juego 2
    "cactusjuego1bu", "cactusjuego2bu", "cactusjuego3bu", "arena",
    # Juego 4
    "arcobu", "pelotabu",
]

carpeta = "assetsbu"

with open("imagenes.h", "w") as out:
    out.write("#pragma once\n\n")
    for nombre in imagenes:
        ruta = os.path.join(carpeta, nombre + ".png")
        if not os.path.exists(ruta):
            print(f"No encontrado: {ruta}")
            continue
        with open(ruta, "rb") as f:
            data = f.read()
        out.write(f"const unsigned int {nombre}_size = {len(data)};\n")
        out.write(f"const unsigned char {nombre}[] = {{\n    ")
        for i, b in enumerate(data):
            out.write(str(b) + ",")
            if (i + 1) % 16 == 0:
                out.write("\n    ")
        out.write("\n};\n\n")
        print(f"OK: {ruta}")

print("\nimagenes.h generado!")
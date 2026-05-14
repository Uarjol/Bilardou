#pragma once
#include <SFML/Graphics.hpp>
#include "imagenes.h"

struct Assets {
    // Mascota por skin y estado (0=normal, 1=sucio, 2=dormido, 3=sucio+dormido)
    sf::Texture mascota[6][4];

    // Cabezas para juegos
    sf::Texture cabeza[6];
    sf::Texture bloqueada[5];

    // Elementos habitaciones
    sf::Texture veladorApagado;
    sf::Texture veladorPrendido;
    sf::Texture esponja;
    sf::Texture heladera;
    sf::Texture joystick;
    sf::Texture armario;
    sf::Texture cama;

    // juego 2
    sf::Texture cactus1;
    sf::Texture cactus2;
    sf::Texture cactus3;
    sf::Texture arenaTex;

    // juego 4
    sf::Texture arco;
    sf::Texture pelota;

    // Comida
    sf::Texture comida[7];

    void cargar()
    {
        // Mascota
        mascota[0][0].loadFromMemory(bilardobasebu,                    bilardobasebu_size);
        mascota[0][1].loadFromMemory(bilardobasesuciobu,               bilardobasesuciobu_size);
        mascota[0][2].loadFromMemory(bilardobasedormidobu,             bilardobasedormidobu_size);
        mascota[0][3].loadFromMemory(bilardobasesuciodormidobu,        bilardobasesuciodormidobu_size);

        mascota[1][0].loadFromMemory(bilardosupersayajinbu,            bilardosupersayajinbu_size);
        mascota[1][1].loadFromMemory(bilardosupersayajinsuciobu,       bilardosupersayajinsuciobu_size);
        mascota[1][2].loadFromMemory(bilardosupersayajindormidobu,     bilardosupersayajindormidobu_size);
        mascota[1][3].loadFromMemory(bilardosupersayajinsuciodormidobu,bilardosupersayajinsuciodormidobu_size);

        mascota[2][0].loadFromMemory(bilardopomnibu,                   bilardopomnibu_size);
        mascota[2][1].loadFromMemory(bilardopomnisuciobu,              bilardopomnisuciobu_size);
        mascota[2][2].loadFromMemory(bilardopomnidormidobu,            bilardopomnidormidobu_size);
        mascota[2][3].loadFromMemory(bilardopomnisuciodormidobu,       bilardopomnisuciodormidobu_size);

        mascota[3][0].loadFromMemory(bilardoexebu,                     bilardoexebu_size);
        mascota[3][1].loadFromMemory(bilardoexesuciobu,                bilardoexesuciobu_size);
        mascota[3][2].loadFromMemory(bilardoexedormidobu,              bilardoexedormidobu_size);
        mascota[3][3].loadFromMemory(bilardoexesuciodormidobu,         bilardoexesuciodormidobu_size);

        mascota[4][0].loadFromMemory(bilardocupheadbu,                 bilardocupheadbu_size);
        mascota[4][1].loadFromMemory(bilardocupheadsuciobu,            bilardocupheadsuciobu_size);
        mascota[4][2].loadFromMemory(bilardocupheaddormidobu,          bilardocupheaddormidobu_size);
        mascota[4][3].loadFromMemory(bilardocupheadsuciodormidobu,     bilardocupheadsuciodormidobu_size);

        mascota[5][0].loadFromMemory(bilardosanbu,                     bilardosanbu_size);
        mascota[5][1].loadFromMemory(bilardosansuciobu,                bilardosansuciobu_size);
        mascota[5][2].loadFromMemory(bilardosandormidobu,              bilardosandormidobu_size);
        mascota[5][3].loadFromMemory(bilardosansuciodormidobu,         bilardosansuciodormidobu_size);

        // Cabezas
        cabeza[0].loadFromMemory(bilardobasecabezabu,      bilardobasecabezabu_size);
        cabeza[1].loadFromMemory(bilardosupersayajincabezabu, bilardosupersayajincabezabu_size);
        cabeza[2].loadFromMemory(bilardopomnicabezabu,    bilardopomnicabezabu_size);
        cabeza[3].loadFromMemory(bilardoexecabezabu,       bilardoexecabezabu_size);
        cabeza[4].loadFromMemory(bilardocupheadcabezabu,   bilardocupheadcabezabu_size);
        cabeza[5].loadFromMemory(bilardosancabezabu,       bilardosancabezabu_size);

        bloqueada[0].loadFromMemory(bilardosupersayajinnobu, bilardosupersayajinnobu_size);
        bloqueada[1].loadFromMemory(bilardopomninobu,        bilardopomninobu_size);
        bloqueada[2].loadFromMemory(bilardoexenobu,          bilardoexenobu_size);
        bloqueada[3].loadFromMemory(bilardocupheadnobu,      bilardocupheadnobu_size);
        bloqueada[4].loadFromMemory(bilardosannobu,          bilardosannobu_size);

        // Elementos habitaciones
        veladorApagado.loadFromMemory(veladorapagadobu,  veladorapagadobu_size);
        veladorPrendido.loadFromMemory(veladorprendido,  veladorprendido_size);
        esponja.loadFromMemory(esponjabu,                esponjabu_size);
        heladera.loadFromMemory(heladerabu,              heladerabu_size);
        joystick.loadFromMemory(joystickbu,              joystickbu_size);
        armario.loadFromMemory(armariobu,                armariobu_size);
        cama.loadFromMemory(camabu,                      camabu_size);

        // Comida
        comida[0].loadFromMemory(comidaparaperrosbu, comidaparaperrosbu_size);
        comida[1].loadFromMemory(raviolesbu,         raviolesbu_size);
        comida[2].loadFromMemory(milanesabu,         milanesabu_size);
        comida[3].loadFromMemory(pizzabu,            pizzabu_size);
        comida[4].loadFromMemory(asadodecesarbu,     asadodecesarbu_size);
        comida[5].loadFromMemory(heladobu,           heladobu_size);
        comida[6].loadFromMemory(cactusbu,           cactusbu_size);

        // Juego 2
        cactus1.loadFromMemory(cactusjuego1bu,   cactusjuego1bu_size);
        cactus2.loadFromMemory(cactusjuego2bu,   cactusjuego2bu_size);
        cactus3.loadFromMemory(cactusjuego3bu,   cactusjuego3bu_size);
        arenaTex.loadFromMemory(arena,   arena_size);

        // Juego 4
        arco.loadFromMemory(arcobu,   arcobu_size);
        pelota.loadFromMemory(pelotabu, pelotabu_size);
    }
};
#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct Stat {
    float valor;
    sf::String etiqueta;
};

struct Comida {
    sf::String nombre;
    int precio;
    float hambre;
    int cantidad;
    sf::Color color;
    float escalaMenu;
    float escalaInventario;
};

struct Skin {
    sf::String nombre;
    int precio;
    bool comprada;
    sf::Color colorPlaceholder;
    sf::Color colorSucio;
};
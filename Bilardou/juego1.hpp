#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "tipos.hpp"

struct ItemCayendo {
    sf::Sprite forma;
    float velocidad;
    int indiceComida;

    ItemCayendo(sf::Texture& tex) : forma(tex), velocidad(0.f), indiceComida(0) {}
};

struct EstadoJuego1 {
    std::vector<ItemCayendo> items;
    float tiempoSpawn;
    int puntos;
    int vidas;
    bool activo;
    bool terminado;
    float velocidadBase;
    int record;
};

void iniciarJuego1(EstadoJuego1& estado)
{
    estado.items.clear();
    estado.tiempoSpawn = 0.f;
    estado.puntos = 0;
    estado.vidas = 3;
    estado.activo = true;
    estado.terminado = false;
    estado.velocidadBase = 150.f;
    std::srand((unsigned)std::time(nullptr));
}

void actualizarJuego1(EstadoJuego1& estado, float dt, sf::Sprite& cabeza,
                      std::vector<Comida>& menu, int& monedas, Stat& felicidad,
                      sf::Texture texturasComida[])
{
    if (!estado.activo || estado.terminado) return;

    // Mover cabeza con teclado
    sf::Vector2f posCabeza = cabeza.getPosition();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        posCabeza.x -= 300.f * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        posCabeza.x += 300.f * dt;

    posCabeza.x = std::max(0.f, std::min(540.f, posCabeza.x));
    cabeza.setPosition(posCabeza);

    // Velocidad aumenta progresivamente
    estado.velocidadBase += dt * 5.f;

    // Spawn de items - cantidad aumenta con el tiempo
    estado.tiempoSpawn -= dt;
    if (estado.tiempoSpawn <= 0.f)
    {
        float intervalo = std::max(0.4f, 1.5f - estado.velocidadBase / 500.f);
        estado.tiempoSpawn = intervalo;

        int cantidadSpawn = 1 + (int)(estado.velocidadBase / 300.f);
        cantidadSpawn = std::min(cantidadSpawn, 4);

        for (int s = 0; s < cantidadSpawn; s++)
        {
            int idx = std::rand() % menu.size();
            ItemCayendo item(texturasComida[idx]);
            item.forma = sf::Sprite(texturasComida[idx]);
            sf::Vector2u tamComida = texturasComida[idx].getSize();
            float escalaComida = menu[idx].escalaInventario;
            item.forma.setScale(sf::Vector2f(escalaComida, escalaComida));
            item.forma.setPosition(sf::Vector2f(50.f + (float)(std::rand() % 500), -40.f - s * 60.f));
            item.velocidad = estado.velocidadBase + (float)(std::rand() % 80);
            item.indiceComida = idx;
            estado.items.push_back(item);
        }
    }

    // Mover items y detectar colisiones
    for (int i = (int)estado.items.size() - 1; i >= 0; i--)
    {
        estado.items[i].forma.move(sf::Vector2f(0.f, estado.items[i].velocidad * dt));
        estado.items[i].forma.rotate(sf::degrees(45.f * dt));

        if (cabeza.getGlobalBounds().findIntersection(estado.items[i].forma.getGlobalBounds()).has_value())
        {
            estado.puntos += 10;
            estado.items.erase(estado.items.begin() + i);
        }
        else if (estado.items[i].forma.getPosition().y > 900.f)
        {
            estado.vidas--;
            estado.items.erase(estado.items.begin() + i);
            if (estado.vidas <= 0)
            {
                estado.activo = false;
                estado.terminado = true;
                int monedasGanadas = estado.puntos / 20;
                monedas += monedasGanadas;
                felicidad.valor = std::min(100.f, felicidad.valor + 20.f);
                if (estado.puntos > estado.record)
                    estado.record = estado.puntos;
            }
        }
    }
}

void dibujarJuego1(sf::RenderWindow& window, EstadoJuego1& estado,
                   sf::Sprite& cabeza, sf::Font& fuente)
{
    if (!estado.activo && !estado.terminado) return;

    window.clear(sf::Color(30, 30, 30));

    if (estado.activo)
    {
        for (auto& item : estado.items)
            window.draw(item.forma);

        window.draw(cabeza);

        // Puntos
        sf::Text textoPuntos(fuente, L"Puntos: " + std::to_wstring(estado.puntos), 24);
        textoPuntos.setFillColor(sf::Color::White);
        textoPuntos.setPosition(sf::Vector2f(10.f, 10.f));
        window.draw(textoPuntos);

        // Monedas que llevas
        sf::Text textoMonedas(fuente, L"Monedas: " + std::to_wstring(estado.puntos / 20), 24);
        textoMonedas.setFillColor(sf::Color(255, 215, 0));
        textoMonedas.setPosition(sf::Vector2f(10.f, 40.f));
        window.draw(textoMonedas);

        // Vidas
        sf::Text textoVidas(fuente, L"Vidas: " + std::to_wstring(estado.vidas), 24);
        textoVidas.setFillColor(sf::Color(220, 0, 0));
        textoVidas.setPosition(sf::Vector2f(10.f, 70.f));
        window.draw(textoVidas);

        // Record
        sf::Text textoRecord(fuente, L"Record: " + std::to_wstring(estado.record), 20);
        textoRecord.setFillColor(sf::Color(200, 200, 200));
        textoRecord.setPosition(sf::Vector2f(400.f, 10.f));
        window.draw(textoRecord);
    }
    else if (estado.terminado)
    {
        int monedasGanadas = estado.puntos / 20;

        auto centrarTexto = [&](sf::Text& texto, float y) {
            sf::FloatRect b = texto.getLocalBounds();
            texto.setPosition(sf::Vector2f(300.f - b.size.x / 2.f, y));
        };

        sf::Text textoFin(fuente, L"FRACA", 36);
        textoFin.setFillColor(sf::Color::White);
        centrarTexto(textoFin, 280.f);
        window.draw(textoFin);

        sf::Text textoPuntosFin(fuente, L"Puntos: " + std::to_wstring(estado.puntos), 28);
        textoPuntosFin.setFillColor(sf::Color::White);
        centrarTexto(textoPuntosFin, 370.f);
        window.draw(textoPuntosFin);

        sf::Text textoMonedasFin(fuente, L"Monedas ganadas: " + std::to_wstring(monedasGanadas), 28);
        textoMonedasFin.setFillColor(sf::Color(255, 215, 0));
        centrarTexto(textoMonedasFin, 420.f);
        window.draw(textoMonedasFin);

        sf::Text textoRecordFin(fuente, L"Record: " + std::to_wstring(estado.record), 24);
        textoRecordFin.setFillColor(sf::Color(200, 200, 200));
        centrarTexto(textoRecordFin, 470.f);
        window.draw(textoRecordFin);

        sf::Text textoVolver(fuente, L"Click para volver", 24);
        textoVolver.setFillColor(sf::Color(200, 200, 200));
        centrarTexto(textoVolver, 540.f);
        window.draw(textoVolver);

        sf::Text textoReiniciar(fuente, L"Espacio para jugar de nuevo", 24);
        textoReiniciar.setFillColor(sf::Color(100, 220, 100));
        centrarTexto(textoReiniciar, 580.f);
        window.draw(textoReiniciar);
    }
}
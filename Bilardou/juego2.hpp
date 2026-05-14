#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "tipos.hpp"

struct Cactus {
    sf::Sprite sprite;
    float x;
     Cactus(sf::Texture& tex) : sprite(tex), x(0.f) {}
};


struct EstadoJuego2 {
    std::vector<Cactus> cactus;
    float tiempoJuego;
    float tiempoSpawn;
    float velocidad;
    float gravedad;
    float velY;
    float posY;
    float suelo;
    bool enSuelo;
    bool activo;
    bool terminado;
    int record;
};

void iniciarJuego2(EstadoJuego2& estado)
{
    estado.cactus.clear();
    estado.tiempoJuego  = 0.f;
    estado.tiempoSpawn  = 2.f;
    estado.velocidad    = 200.f;
    estado.gravedad     = 1800.f;
    estado.velY         = 0.f;
    estado.suelo        = 750.f;
    estado.posY         = estado.suelo;
    estado.enSuelo      = true;
    estado.activo       = true;
    estado.terminado    = false;
    std::srand((unsigned)std::time(nullptr));
}

void saltar(EstadoJuego2& estado)
{
    if (estado.enSuelo)
    {
        estado.velY    = -700.f;
        estado.enSuelo = false;
    }
}

void mantenerSalto(EstadoJuego2& estado)
{
    if (!estado.enSuelo && estado.velY < -200.f)
        estado.velY -= 800.f * 0.016f;
}

void actualizarJuego2(EstadoJuego2& estado, float dt,
                      int& monedas, Stat& felicidad,
                      sf::Texture& texCactus1, sf::Texture& texCactus2, sf::Texture& texCactus3,
                      sf::Sprite& cabeza)
{
    if (!estado.activo || estado.terminado) return;

    estado.tiempoJuego += dt;

    // Dificultad progresiva
    estado.velocidad = 200.f + estado.tiempoJuego * 10.f;

    // Gravedad
    estado.velY  += estado.gravedad * dt;
    estado.posY  += estado.velY * dt;

    if (estado.posY >= estado.suelo)
    {
        estado.posY    = estado.suelo;
        estado.velY    = 0.f;
        estado.enSuelo = true;
    }

    // Spawn de cactus
    estado.tiempoSpawn -= dt;
    if (estado.tiempoSpawn <= 0.f)
    {
        float intervalo = std::max(0.8f, 2.f - estado.tiempoJuego * 0.03f);
        estado.tiempoSpawn = intervalo;

        int tipo = std::rand() % 3;
        sf::Texture* tex = &texCactus1;
        if (tipo == 1) tex = &texCactus2;
        if (tipo == 2) tex = &texCactus3;

        Cactus c(*tex);
        float escala = 50.f / tex->getSize().y;
        if (tipo == 1) escala = 80.f / tex->getSize().y;
        if (tipo == 2) escala = 100.f / tex->getSize().y;
        c.sprite.setScale(sf::Vector2f(escala, escala));
        c.x = 650.f;
        float altoC = c.sprite.getGlobalBounds().size.y;
        c.sprite.setPosition(sf::Vector2f(c.x, estado.suelo - altoC));
        estado.cactus.push_back(c);
    }

    // Mover cactus
    for (int i = (int)estado.cactus.size() - 1; i >= 0; i--)
    {
        estado.cactus[i].x -= estado.velocidad * dt;
        float alto = estado.cactus[i].sprite.getGlobalBounds().size.y;
        estado.cactus[i].sprite.setPosition(sf::Vector2f(estado.cactus[i].x, estado.suelo - alto));

        // Eliminar si sale por la izquierda
        if (estado.cactus[i].x < -100.f)
        {
            estado.cactus.erase(estado.cactus.begin() + i);
            continue;
        }

        // Colision con la cabeza
        sf::FloatRect boundsCabeza = cabeza.getGlobalBounds();
        sf::FloatRect boundsCactus = estado.cactus[i].sprite.getGlobalBounds();

        // Hitbox reducida para ser mas justo
        sf::FloatRect hitboxCabeza(
            sf::Vector2f(boundsCabeza.position.x + boundsCabeza.size.x * 0.2f,
                         boundsCabeza.position.y + boundsCabeza.size.y * 0.2f),
            sf::Vector2f(boundsCabeza.size.x * 0.6f, boundsCabeza.size.y * 0.6f)
        );
        sf::FloatRect hitboxCactus(
            sf::Vector2f(boundsCactus.position.x + boundsCactus.size.x * 0.2f,
                         boundsCactus.position.y),
            sf::Vector2f(boundsCactus.size.x * 0.6f, boundsCactus.size.y)
        );

        if (hitboxCabeza.findIntersection(hitboxCactus).has_value())
        {
            estado.activo    = false;
            estado.terminado = true;
            int puntaje = (int)estado.tiempoJuego;
            monedas += puntaje / 3;
            felicidad.valor = std::min(100.f, felicidad.valor + 15.f);
            if (puntaje > estado.record)
                estado.record = puntaje;
        }
    }
}

void dibujarJuego2(sf::RenderWindow& window, EstadoJuego2& estado,
                   sf::Sprite& cabeza, sf::Font& fuente,
                   sf::Texture& texArena)
{
    if (!estado.activo && !estado.terminado) return;

    window.clear(sf::Color(135, 206, 235));

    auto centrar = [&](sf::Text& t, float y) {
        sf::FloatRect b = t.getLocalBounds();
        t.setPosition(sf::Vector2f(300.f - b.size.x / 2.f, y));
    };

    if (estado.activo)
    {
        // Suelo con arena
        sf::Sprite arena(texArena);
        sf::Vector2u tamArena = texArena.getSize();
        float escalaArena = 600.f / tamArena.x;
        arena.setScale(sf::Vector2f(escalaArena, escalaArena));
        float altoArena = tamArena.y * escalaArena;
        arena.setPosition(sf::Vector2f(0.f, estado.suelo));
        window.draw(arena);

        // Cactus
        for (auto& c : estado.cactus)
            window.draw(c.sprite);

        // Cabeza de Bilardo
        sf::FloatRect bounds = cabeza.getGlobalBounds();
        cabeza.setPosition(sf::Vector2f(100.f - bounds.size.x / 2.f, estado.posY - bounds.size.y));
        window.draw(cabeza);

        // HUD
        sf::Text textoTiempo(fuente, L"Tiempo: " + std::to_wstring((int)estado.tiempoJuego) + L"s", 24);
        textoTiempo.setFillColor(sf::Color::White);
        textoTiempo.setPosition(sf::Vector2f(10.f, 10.f));
        window.draw(textoTiempo);

        sf::Text textoRecord(fuente, L"Record: " + std::to_wstring(estado.record) + L"s", 20);
        textoRecord.setFillColor(sf::Color(200, 200, 200));
        textoRecord.setPosition(sf::Vector2f(400.f, 10.f));
        window.draw(textoRecord);

        sf::Text textoMonedas(fuente, L"Monedas: " + std::to_wstring((int)estado.tiempoJuego / 3), 24);
        textoMonedas.setFillColor(sf::Color(255, 215, 0));
        textoMonedas.setPosition(sf::Vector2f(10.f, 40.f));
        window.draw(textoMonedas);
    }
    else if (estado.terminado)
    {
        int puntaje = (int)estado.tiempoJuego;

        sf::Text textoFin(fuente, L"Juego terminado!", 36);
        textoFin.setFillColor(sf::Color::White);
        centrar(textoFin, 280.f);
        window.draw(textoFin);

        sf::Text textoTiempoFin(fuente, L"Tiempo: " + std::to_wstring(puntaje) + L"s", 28);
        textoTiempoFin.setFillColor(sf::Color::White);
        centrar(textoTiempoFin, 350.f);
        window.draw(textoTiempoFin);

        sf::Text textoMonedasFin(fuente, L"Monedas ganadas: " + std::to_wstring(puntaje / 3), 28);
        textoMonedasFin.setFillColor(sf::Color(255, 215, 0));
        centrar(textoMonedasFin, 400.f);
        window.draw(textoMonedasFin);

        sf::Text textoRecordFin(fuente, L"Record: " + std::to_wstring(estado.record) + L"s", 24);
        textoRecordFin.setFillColor(sf::Color(200, 200, 200));
        centrar(textoRecordFin, 450.f);
        window.draw(textoRecordFin);

        sf::Text textoVolver(fuente, L"Click para volver", 24);
        textoVolver.setFillColor(sf::Color(200, 200, 200));
        centrar(textoVolver, 530.f);
        window.draw(textoVolver);

        sf::Text textoReiniciar(fuente, L"Espacio para jugar de nuevo", 24);
        textoReiniciar.setFillColor(sf::Color(100, 220, 100));
        centrar(textoReiniciar, 570.f);
        window.draw(textoReiniciar);
    }
}
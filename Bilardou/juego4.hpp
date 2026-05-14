#pragma once
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include "tipos.hpp"

enum class FaseJuego4 { Esperando, MostrandoFlecha, PelotaMoviendose, Resultado };

struct EstadoJuego4 {
    int penalActual;
    int atajadas;
    int goles;
    int record;
    bool activo;
    bool terminado;
    int sectorDestino;
    int sectorElegido;
    float tiempoFase;
    FaseJuego4 fase;
    sf::Vector2f posPelota;
    sf::Vector2f posDestino;
    bool atajo;
};

// Posiciones de los 6 sectores del arco (3 columnas x 2 filas)
sf::Vector2f getSectorPos(int sector)
{
    float inicioX = 110.f;
    float inicioY = 300.f;
    float anchoSector = 126.f;
    float altoSector  = 100.f;

    float x = inicioX + (sector % 3) * anchoSector + anchoSector / 2.f;
    float y = inicioY + (sector / 3) * altoSector  + altoSector  / 2.f;
    return sf::Vector2f(x, y);
}

void iniciarJuego4(EstadoJuego4& estado)
{
    estado.penalActual  = 0;
    estado.atajadas     = 0;
    estado.goles        = 0;
    estado.activo       = true;
    estado.terminado    = false;
    estado.sectorDestino = -1;
    estado.sectorElegido = -1;
    estado.tiempoFase   = 0.f;
    estado.fase         = FaseJuego4::Esperando;
    estado.posPelota    = sf::Vector2f(300.f, 700.f);
    estado.atajo        = false;
    std::srand((unsigned)std::time(nullptr));
}

void actualizarJuego4(EstadoJuego4& estado, float dt, int& monedas, Stat& felicidad)
{
    if (!estado.activo || estado.terminado) return;

    estado.tiempoFase += dt;

    if (estado.fase == FaseJuego4::Esperando)
    {
        if (estado.tiempoFase >= 1.5f)
        {
            estado.sectorDestino = std::rand() % 6;
            estado.sectorElegido = -1;
            estado.atajo = false;
            estado.posDestino = getSectorPos(estado.sectorDestino);
            estado.tiempoFase = 0.f;
            estado.fase = FaseJuego4::MostrandoFlecha;
        }
    }
    else if (estado.fase == FaseJuego4::MostrandoFlecha)
    {
        float tiempoReaccion = 0.9f - estado.penalActual * 0.07f;
        tiempoReaccion = std::max(0.15f, tiempoReaccion);
        if (estado.tiempoFase >= tiempoReaccion)
        {
            estado.tiempoFase = 0.f;
            estado.fase = FaseJuego4::PelotaMoviendose;
        }
    }
    else if (estado.fase == FaseJuego4::PelotaMoviendose)
    {
        float velocidad = 400.f + estado.penalActual * 100.f;
        sf::Vector2f dir = estado.posDestino - estado.posPelota;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        if (dist > 5.f)
        {
            dir /= dist;
            estado.posPelota += dir * velocidad * dt;
        }
        else
        {
            estado.posPelota = estado.posDestino;
            estado.atajo = (estado.sectorElegido == estado.sectorDestino);

            if (estado.atajo)
                estado.atajadas++;
            else
                estado.goles++;

            estado.penalActual++;
            estado.tiempoFase = 0.f;
            estado.fase = FaseJuego4::Resultado;
        }
    }
    else if (estado.fase == FaseJuego4::Resultado)
    {
        if (estado.tiempoFase >= 1.5f)
        {
            if (estado.penalActual >= 10)
            {
                estado.activo    = false;
                estado.terminado = true;
                int monedasGanadas = estado.atajadas * 3;
                monedas += monedasGanadas;
                felicidad.valor = std::min(100.f, felicidad.valor + estado.atajadas * 5.f);
                if (estado.atajadas > estado.record)
                    estado.record = estado.atajadas;
            }
            else
            {
                estado.posPelota = sf::Vector2f(300.f, 700.f);
                estado.tiempoFase = 0.f;
                estado.fase = FaseJuego4::Esperando;
            }
        }
    }
}

void procesarClickJuego4(EstadoJuego4& estado, sf::Vector2f pos)
{
    if (estado.fase != FaseJuego4::MostrandoFlecha &&
        estado.fase != FaseJuego4::PelotaMoviendose) return;
    if (estado.sectorElegido != -1) return;

    for (int i = 0; i < 6; i++)
    {
        sf::Vector2f centro = getSectorPos(i);
        sf::FloatRect sector(sf::Vector2f(centro.x - 63.f, centro.y - 50.f), sf::Vector2f(126.f, 100.f));
        if (sector.contains(pos))
        {
            estado.sectorElegido = i;
            break;
        }
    }
}

void dibujarJuego4(sf::RenderWindow& window, EstadoJuego4& estado,
                   sf::Sprite& cabeza, sf::Font& fuente,
                   sf::Texture& texArco, sf::Texture& texPelota)
{
    if (!estado.activo && !estado.terminado) return;

    window.clear(sf::Color(34, 139, 34));

    auto centrar = [&](sf::Text& t, float y) {
        sf::FloatRect b = t.getLocalBounds();
        t.setPosition(sf::Vector2f(300.f - b.size.x / 2.f, y));
    };

    if (estado.activo)
    {
        // Arco
// Sectores marcados en rojo
// Solo marcar el sector destino durante el lanzamiento

        // Arco con imagen
        sf::Sprite sprArco(texArco);
        sf::Vector2u tamArco = texArco.getSize();
        float escalaArco = 380.f / tamArco.x;
        sprArco.setScale(sf::Vector2f(escalaArco, escalaArco));
        sprArco.setPosition(sf::Vector2f(110.f, 300.f));
        window.draw(sprArco);

        if (estado.fase == FaseJuego4::MostrandoFlecha ||
            estado.fase == FaseJuego4::PelotaMoviendose ||
            estado.fase == FaseJuego4::Resultado)
        {
            sf::Vector2f centro = getSectorPos(estado.sectorDestino);
            sf::RectangleShape sector(sf::Vector2f(116.f, 100.f));
            sector.setPosition(sf::Vector2f(centro.x - 58.f, centro.y - 38.f));
            sector.setOutlineThickness(0.f);

            if (estado.fase == FaseJuego4::Resultado && estado.atajo)
                sector.setFillColor(sf::Color(100, 255, 100, 120));
            else
                sector.setFillColor(sf::Color(255, 0, 0, 120));

            window.draw(sector);
        }

        // Sector elegido por el jugador en amarillo
        if (estado.sectorElegido != -1 && estado.sectorElegido != estado.sectorDestino)
        {
            sf::Vector2f centro = getSectorPos(estado.sectorElegido);
            sf::RectangleShape sector(sf::Vector2f(116.f, 100.f));
            sector.setPosition(sf::Vector2f(centro.x - 58.f, centro.y - 38.f));
            sector.setFillColor(sf::Color(255, 255, 0, 120));
            sector.setOutlineThickness(0.f);
            window.draw(sector);
        }

        // Cabeza de Bilardo como arquero
        if (estado.sectorElegido != -1)
        {
            sf::Vector2f posCab = getSectorPos(estado.sectorElegido);
            sf::FloatRect bounds = cabeza.getGlobalBounds();
            cabeza.setPosition(sf::Vector2f(posCab.x - bounds.size.x / 2.f, posCab.y - bounds.size.y / 2.f));
        }
        else
        {
            sf::FloatRect bounds = cabeza.getGlobalBounds();
            cabeza.setPosition(sf::Vector2f(300.f - bounds.size.x / 2.f, 430.f));
        }
        window.draw(cabeza);

        // Pelota
        sf::Sprite sprPelota(texPelota);
        sf::Vector2u tamPelota = texPelota.getSize();
        float escalaPelota = 40.f / tamPelota.x;
        sprPelota.setScale(sf::Vector2f(escalaPelota, escalaPelota));
        sprPelota.setOrigin(sf::Vector2f(tamPelota.x / 2.f, tamPelota.y / 2.f));
        sprPelota.setPosition(estado.posPelota);
        if (estado.fase == FaseJuego4::PelotaMoviendose)
            sprPelota.setRotation(sf::degrees(estado.tiempoFase * 300.f));
        window.draw(sprPelota);

        // Resultado del penal
        if (estado.fase == FaseJuego4::Resultado)
        {
            sf::Text textoRes(fuente, estado.atajo ? L"OAAA" : L"Golazo", 40);
            textoRes.setFillColor(estado.atajo ? sf::Color(100, 255, 100) : sf::Color(255, 80, 80));
            centrar(textoRes, 220.f);
            window.draw(textoRes);
        }

        // HUD
        sf::Text textoAtajadas(fuente, L"Atajadas: " + std::to_wstring(estado.atajadas), 24);
        textoAtajadas.setFillColor(sf::Color::White);
        textoAtajadas.setPosition(sf::Vector2f(10.f, 10.f));
        window.draw(textoAtajadas);

        sf::Text textoGoles(fuente, L"Goles: " + std::to_wstring(estado.goles), 24);
        textoGoles.setFillColor(sf::Color(255, 80, 80));
        textoGoles.setPosition(sf::Vector2f(10.f, 40.f));
        window.draw(textoGoles);

        sf::Text textoPenales(fuente, L"Penal: " + std::to_wstring(estado.penalActual) + L"/10", 24);
        textoPenales.setFillColor(sf::Color::White);
        textoPenales.setPosition(sf::Vector2f(10.f, 70.f));
        window.draw(textoPenales);

        sf::Text textoRecord(fuente, L"Record: " + std::to_wstring(estado.record), 20);
        textoRecord.setFillColor(sf::Color(200, 200, 200));
        textoRecord.setPosition(sf::Vector2f(400.f, 10.f));
        window.draw(textoRecord);

        // Instruccion
        if (estado.fase == FaseJuego4::MostrandoFlecha)
        {
            sf::Text textoInst(fuente, L"Elegí bien", 24);
            textoInst.setFillColor(sf::Color::Yellow);
            centrar(textoInst, 250.f);
            window.draw(textoInst);
        }
        else if (estado.fase == FaseJuego4::Esperando)
        {
            sf::Text textoInst(fuente, L"Ojo que va", 24);
            textoInst.setFillColor(sf::Color::Yellow);
            centrar(textoInst, 250.f);
            window.draw(textoInst);
        }
    }
    else if (estado.terminado)
    {
        int monedasGanadas = estado.atajadas * 3;

        sf::Text textoFin(fuente, L"GG", 36);
        textoFin.setFillColor(sf::Color::White);
        centrar(textoFin, 280.f);
        window.draw(textoFin);

        sf::Text textoAtajadasFin(fuente, L"Atajadas: " + std::to_wstring(estado.atajadas) + L"/10", 28);
        textoAtajadasFin.setFillColor(sf::Color(100, 255, 100));
        centrar(textoAtajadasFin, 340.f);
        window.draw(textoAtajadasFin);

        sf::Text textoGolesFin(fuente, L"Golazos que te metieron: " + std::to_wstring(estado.goles), 28);
        textoGolesFin.setFillColor(sf::Color(255, 80, 80));
        centrar(textoGolesFin, 390.f);
        window.draw(textoGolesFin);

        sf::Text textoMonedasFin(fuente, L"Monedas ganadas: " + std::to_wstring(monedasGanadas), 28);
        textoMonedasFin.setFillColor(sf::Color(255, 215, 0));
        centrar(textoMonedasFin, 440.f);
        window.draw(textoMonedasFin);

        sf::Text textoRecordFin(fuente, L"Record: " + std::to_wstring(estado.record), 24);
        textoRecordFin.setFillColor(sf::Color(200, 200, 200));
        centrar(textoRecordFin, 490.f);
        window.draw(textoRecordFin);

        sf::Text textoVolver(fuente, L"Click para volver", 24);
        textoVolver.setFillColor(sf::Color(200, 200, 200));
        centrar(textoVolver, 560.f);
        window.draw(textoVolver);

        sf::Text textoReiniciar(fuente, L"Espacio para jugar de nuevo", 24);
        textoReiniciar.setFillColor(sf::Color(100, 220, 100));
        centrar(textoReiniciar, 600.f);
        window.draw(textoReiniciar);
    }
}
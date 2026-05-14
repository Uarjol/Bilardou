#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cmath>
#include "assets.hpp"
#include "tipos.hpp"
#include "comic.h"
#include "juego1.hpp"
#include "juego2.hpp"
#include "juego4.hpp"


sf::Color colorSegunValor(float valor)
{
    if (valor > 80.f) return sf::Color(0, 180, 0);
    if (valor > 60.f) return sf::Color(100, 220, 100);
    if (valor > 40.f) return sf::Color(255, 220, 0);
    if (valor > 20.f) return sf::Color(255, 140, 0);
    return sf::Color(220, 0, 0);
}

void dibujarBarra(sf::RenderWindow& window, Stat stat, float x, float y, float ancho, float alto, sf::Font& fuente, sf::Vector2f posMouse)
{
    sf::RectangleShape contorno(sf::Vector2f(ancho + 4.f, alto + 4.f));
    contorno.setFillColor(sf::Color::Transparent);
    contorno.setOutlineColor(sf::Color::White);
    contorno.setOutlineThickness(2.f);
    contorno.setPosition(sf::Vector2f(x - 2.f, y - 2.f));
    window.draw(contorno);

    sf::RectangleShape fondo(sf::Vector2f(ancho, alto));
    fondo.setFillColor(sf::Color(200, 200, 200));
    fondo.setPosition(sf::Vector2f(x, y));
    window.draw(fondo);

    float relleno = (stat.valor / 100.f) * ancho;
    sf::RectangleShape barra(sf::Vector2f(relleno, alto));
    barra.setFillColor(colorSegunValor(stat.valor));
    barra.setPosition(sf::Vector2f(x, y));
    window.draw(barra);

    sf::Text etiqueta(fuente, stat.etiqueta, 16);
    etiqueta.setFillColor(sf::Color::Black);
    etiqueta.setPosition(sf::Vector2f(x, y - 20.f));
    window.draw(etiqueta);

    sf::FloatRect areaBarra(sf::Vector2f(x, y), sf::Vector2f(ancho, alto));
    if (areaBarra.contains(posMouse))
    {
        sf::Text porcentaje(fuente, std::to_wstring((int)std::round(stat.valor)) + L"%", 18);
        porcentaje.setFillColor(sf::Color::Black);
        sf::FloatRect boundsPorc = porcentaje.getLocalBounds();
        porcentaje.setPosition(sf::Vector2f(x + ancho / 2.f - boundsPorc.size.x / 2.f, y + 2.f));
        window.draw(porcentaje);
    }
}

void dibujarFlecha(sf::RenderWindow& window, float x, float y, bool apuntaIzquierda)
{
    sf::ConvexShape flecha;
    flecha.setPointCount(3);
    flecha.setFillColor(sf::Color(255, 255, 255, 200));
    flecha.setOutlineColor(sf::Color::Black);
    flecha.setOutlineThickness(2.f);

    if (apuntaIzquierda)
    {
        flecha.setPoint(0, sf::Vector2f(x,        y));
        flecha.setPoint(1, sf::Vector2f(x + 40.f, y - 30.f));
        flecha.setPoint(2, sf::Vector2f(x + 40.f, y + 30.f));
    }
    else
    {
        flecha.setPoint(0, sf::Vector2f(x + 40.f, y));
        flecha.setPoint(1, sf::Vector2f(x,        y - 30.f));
        flecha.setPoint(2, sf::Vector2f(x,        y + 30.f));
    }

    window.draw(flecha);
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({600, 900}),
        "Bilardou",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(60);

    sf::Font fuente;
    fuente.openFromMemory(comicFont, comicFontSize);

    std::vector<sf::String> habitaciones = {L"Dormitorio", L"Vestidor", L"Cocina", L"Sala de juegos", L"Ba\u00f1o"};
    std::vector<sf::Color> coloresFondo = {
        sf::Color(144, 238, 144),
        sf::Color(210, 180, 140),
        sf::Color(135, 206, 235),
        sf::Color(250, 128, 114),
        sf::Color(240, 240, 240)
    };
    int habitacionActual = 0;

Assets assets;
    assets.cargar();

    sf::Text textoHabitacion(fuente, habitaciones[habitacionActual], 24);
    textoHabitacion.setFillColor(sf::Color::White);

    sf::Sprite mascota(assets.mascota[0][0]);
    mascota.setPosition(sf::Vector2f(200.f, 400.f));

    Stat hambre    = {100.f, L"Hambre"};
    Stat felicidad = {100.f, L"Felicidad"};
    Stat sueno     = {100.f, L"Sue\u00f1o"};
    Stat carino    = {100.f, L"Cari\u00f1o"};

    int monedas = 50;
    sf::Text textoMonedas(fuente, L"Monedas: 0", 22);
    textoMonedas.setFillColor(sf::Color(255, 215, 0));
    textoMonedas.setPosition(sf::Vector2f(10.f, 10.f));

    bool mascotaSucia = false;
    float tiempoSuciedad = 0.f;
    const float TIEMPO_ENSUCIARSE = 600.f;

    sf::Sprite objetoLimpieza(assets.esponja);
    sf::Vector2u tamEsponja = assets.esponja.getSize();
    float escalaEsponja = 80.f / tamEsponja.y;
    objetoLimpieza.setScale(sf::Vector2f(escalaEsponja, escalaEsponja));
    float anchoEsponja = tamEsponja.x * escalaEsponja;
    float altoEsponja = tamEsponja.y * escalaEsponja;
    const sf::Vector2f POS_OBJETO(300.f - anchoEsponja / 2.f, 800.f);
    objetoLimpieza.setPosition(POS_OBJETO);

    bool arrastandoObjeto = false;
    sf::Vector2f offsetArrastre;
    float progresoLimpieza = 0.f;
    const float TIEMPO_LIMPIEZA = 3.f;

    sf::RectangleShape fondoLimpieza(sf::Vector2f(200.f, 20.f));
    fondoLimpieza.setFillColor(sf::Color(200, 200, 200));
    fondoLimpieza.setOutlineColor(sf::Color::White);
    fondoLimpieza.setOutlineThickness(2.f);
    fondoLimpieza.setPosition(sf::Vector2f(200.f, 650.f));

    sf::RectangleShape barraLimpieza(sf::Vector2f(0.f, 20.f));
    barraLimpieza.setFillColor(sf::Color(0, 200, 255));
    barraLimpieza.setPosition(sf::Vector2f(200.f, 650.f));

    sf::Text textoLimpiar(fuente, L"Arrastr\u00e1 la esponja", 18);
    textoLimpiar.setFillColor(sf::Color(100, 100, 100));
    textoLimpiar.setPosition(sf::Vector2f(195.f, 675.f));

    bool durmiendo = false;
    sf::Sprite luz(assets.veladorPrendido);
    sf::Vector2u tamLuz = assets.veladorPrendido.getSize();
    float escalaLuz = 120.f / tamLuz.y;
    luz.setScale(sf::Vector2f(escalaLuz, escalaLuz));
    float anchoLuz = tamLuz.x * escalaLuz;
    luz.setPosition(sf::Vector2f(300.f - anchoLuz / 2.f, 760.f));

    sf::RectangleShape overlayOscuro(sf::Vector2f(600.f, 900.f));
    overlayOscuro.setFillColor(sf::Color(0, 0, 0, 180));
    overlayOscuro.setPosition(sf::Vector2f(0.f, 0.f));

    sf::Sprite cama(assets.cama);
    sf::Vector2u tamCama = assets.cama.getSize();
    float escalaCama = 320.f / tamCama.x;
    cama.setScale(sf::Vector2f(escalaCama, escalaCama));
    float anchoCama = tamCama.x * escalaCama;
    float altoCama = tamCama.y * escalaCama;
    cama.setPosition(sf::Vector2f(300.f - anchoCama / 2.f, 760.f - altoCama));

    sf::Text textoZzz(fuente, L"Zzz...", 40);
    textoZzz.setFillColor(sf::Color(200, 200, 255));
    textoZzz.setPosition(sf::Vector2f(270.f, 350.f));

    std::vector<Comida> menu = {
        {L"Comida para perros", 10, 15.f, 0, sf::Color(180, 120, 60), 0.25f, 0.25f},
        {L"Ravioles",           15, 35.f, 0, sf::Color(220, 180, 100), 0.13f, 0.15f},
        {L"Milanesa",           10, 25.f, 0, sf::Color(200, 150, 80),  0.13f, 0.15f},
        {L"Pizza",              15, 20.f, 0, sf::Color(220, 80,  80),  0.17f, 0.15f},
        {L"Asado de C\u00e9sar",20, 50.f, 0, sf::Color(160, 60,  40), 0.15f, 0.15f},
        {L"Helado",             30, 70.f, 0, sf::Color(200, 220, 255), 0.21f, 0.15f},
        {L"Cactus",              5,  5.f, 0, sf::Color(80,  160, 80),  0.28f, 0.15f}
    };

    bool menuAbierto = false;

    sf::Sprite heladera(assets.heladera);
    sf::Vector2u tamHeladera = assets.heladera.getSize();
    float escalaHeladera = 120.f / tamHeladera.y;
    heladera.setScale(sf::Vector2f(escalaHeladera, escalaHeladera));
    float anchoHeladera = tamHeladera.x * escalaHeladera;
    heladera.setPosition(sf::Vector2f(300.f - anchoHeladera / 2.f, 760.f));

    int comidaSeleccionada = -1;
    sf::Vector2f offsetComida;
    sf::Sprite comidaArrastrable(assets.comida[0]);
    sf::Vector2f posComidaArrastrable;
    bool arrastandoComida = false;

    sf::RectangleShape panelMenu(sf::Vector2f(500.f, 700.f));
    panelMenu.setFillColor(sf::Color(245, 245, 220));
    panelMenu.setOutlineColor(sf::Color::Black);
    panelMenu.setOutlineThickness(3.f);
    panelMenu.setPosition(sf::Vector2f(50.f, 100.f));

    sf::Text tituloMenu(fuente, L"Heladera", 28);
    tituloMenu.setFillColor(sf::Color::Black);
    tituloMenu.setPosition(sf::Vector2f(220.f, 115.f));

    sf::Text textoCerrar(fuente, L"X", 28);
    textoCerrar.setFillColor(sf::Color(200, 0, 0));
    textoCerrar.setPosition(sf::Vector2f(510.f, 110.f));

    std::vector<Skin> skins = {
        {L"Bilardo",                  0,  true,  sf::Color(139, 90,  43),  sf::Color(101, 67,  33)},
        {L"Bilardo Super Saiyajin",   50, false, sf::Color(255, 215, 0),   sf::Color(180, 150, 0)},
        {L"Bilardo Pomni",            50, false, sf::Color(100, 180, 255), sf::Color(60,  120, 180)},
        {L"Bilardo Sat\u00e1nico",    50, false, sf::Color(180, 0,   0),   sf::Color(120, 0,   0)},
        {L"Cup Bilardo",              50, false, sf::Color(220, 220, 220), sf::Color(150, 150, 150)},
        {L"Bilardo San",              50, false, sf::Color(255, 255, 200), sf::Color(180, 180, 120)}
    };
    int skinActual = 0;
    bool menuVestuarioAbierto = false;

    sf::Sprite armario(assets.armario);
    sf::Vector2u tamArmario = assets.armario.getSize();
    float escalaArmario = 120.f / tamArmario.y;
    armario.setScale(sf::Vector2f(escalaArmario, escalaArmario));
    float anchoArmario = tamArmario.x * escalaArmario;
    armario.setPosition(sf::Vector2f(300.f - anchoArmario / 2.f, 760.f));

    sf::RectangleShape panelVestuario(sf::Vector2f(500.f, 700.f));
    panelVestuario.setFillColor(sf::Color(245, 245, 220));
    panelVestuario.setOutlineColor(sf::Color::Black);
    panelVestuario.setOutlineThickness(3.f);
    panelVestuario.setPosition(sf::Vector2f(50.f, 100.f));

    sf::Text tituloVestuario(fuente, L"Vestuario", 28);
    tituloVestuario.setFillColor(sf::Color::Black);
    tituloVestuario.setPosition(sf::Vector2f(210.f, 115.f));

    sf::Text textoCerrarVestuario(fuente, L"X", 28);
    textoCerrarVestuario.setFillColor(sf::Color(200, 0, 0));
    textoCerrarVestuario.setPosition(sf::Vector2f(510.f, 110.f));

    // Juegos
    int juegoActivo = 0;
    EstadoJuego1 estadoJ1;
    estadoJ1.activo = false;
    estadoJ1.terminado = false;
    
    EstadoJuego2 estadoJ2;
    estadoJ2.activo = false;
    estadoJ2.terminado = false;
    estadoJ2.record = 0;

    EstadoJuego4 estadoJ4;
    estadoJ4.activo = false;
    estadoJ4.terminado = false;
    estadoJ4.record = 0;

    sf::Sprite cabezaBilardo(assets.cabeza[0]);
    sf::Vector2u tamCabeza = assets.cabeza[0].getSize();
    float escalaCabeza = 80.f / tamCabeza.y;
    cabezaBilardo.setScale(sf::Vector2f(escalaCabeza, escalaCabeza));
    cabezaBilardo.setPosition(sf::Vector2f(270.f, 800.f));

    sf::Clock reloj;

    while (window.isOpen())
    {
        float dt = reloj.restart().asSeconds();
        sf::Vector2f posMouse = sf::Vector2f(sf::Mouse::getPosition(window));
        bool enBano       = (habitacionActual == 4);
        bool enDormitorio = (habitacionActual == 0);
        bool enCocina     = (habitacionActual == 2);
        bool enVestuario  = (habitacionActual == 1);
        bool enSalaJuegos = (habitacionActual == 3);

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::MouseButtonPressed>())
            {
                auto pos = sf::Vector2f(event->getIf<sf::Event::MouseButtonPressed>()->position);

                // Click para volver de un juego terminado
                if (juegoActivo == 1 && estadoJ1.terminado)
                {
                    juegoActivo = 0;
                    estadoJ1.activo = false;
                    estadoJ1.terminado = false;
                    cabezaBilardo.setPosition(sf::Vector2f(270.f, 800.f));
                }

                if (juegoActivo == 2 && estadoJ2.terminado)
                {
                    juegoActivo = 0;
                    estadoJ2.activo = false;
                    estadoJ2.terminado = false;
                }

                if (juegoActivo == 2 && !estadoJ2.terminado)
                    saltar(estadoJ2);
                     {
                        bool saltando = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) ||
                           sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
                           sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
                           sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
                        if (saltando) mantenerSalto(estadoJ2);
        }

                if (juegoActivo == 4 && estadoJ4.terminado)
                {
                    juegoActivo = 0;
                    estadoJ4.activo = false;
                    estadoJ4.terminado = false;
                }

                // Click en sector del arco
                if (juegoActivo == 4)
                    procesarClickJuego4(estadoJ4, pos);

                // Flechas habitacion
                if (!durmiendo && !menuAbierto && !menuVestuarioAbierto && juegoActivo == 0 && pos.x < 40 && pos.y > 420 && pos.y < 480)
                {
                    habitacionActual = (habitacionActual - 1 + habitaciones.size()) % habitaciones.size();
                    textoHabitacion.setString(habitaciones[habitacionActual]);
                    arrastandoObjeto = false;
                    progresoLimpieza = 0.f;
                    objetoLimpieza.setPosition(POS_OBJETO);
                }

                if (!durmiendo && !menuAbierto && !menuVestuarioAbierto && juegoActivo == 0 && pos.x > 560 && pos.y > 420 && pos.y < 480)
                {
                    habitacionActual = (habitacionActual + 1) % habitaciones.size();
                    textoHabitacion.setString(habitaciones[habitacionActual]);
                    arrastandoObjeto = false;
                    progresoLimpieza = 0.f;
                    objetoLimpieza.setPosition(POS_OBJETO);
                }

                // Click en la luz del dormitorio
                if (enDormitorio && luz.getGlobalBounds().contains(pos))
                    durmiendo = !durmiendo;

                // Agarrar objeto de limpieza
                if (enBano && objetoLimpieza.getGlobalBounds().contains(pos))
                {
                    arrastandoObjeto = true;
                    offsetArrastre = objetoLimpieza.getPosition() - pos;
                }

                // Cocina
                if (enCocina)
                {
                    if (!menuAbierto)
                    {
                        if (heladera.getGlobalBounds().contains(pos))
                            menuAbierto = true;

                        float ix = 150.f;
                        float iy = 620.f;
                        int columna = 0;
                        for (int i = 0; i < (int)menu.size(); i++)
                        {
                            if (menu[i].cantidad > 0)
                            {
                                sf::Sprite itemHit(assets.comida[i]);
                                itemHit.setScale(sf::Vector2f(menu[i].escalaInventario, menu[i].escalaInventario));
                                sf::FloatRect bounds = itemHit.getGlobalBounds();
                                itemHit.setPosition(sf::Vector2f(ix, iy - bounds.size.y / 2.f));

                                if (itemHit.getGlobalBounds().contains(pos) && hambre.valor < 100.f)
                                {
                                    arrastandoComida = true;
                                    comidaSeleccionada = i;
                                    comidaArrastrable.setTexture(assets.comida[i], true);
                                    comidaArrastrable.setScale(sf::Vector2f(menu[i].escalaInventario, menu[i].escalaInventario));
                                    posComidaArrastrable = sf::Vector2f(ix, iy - bounds.size.y / 2.f);
                                    offsetComida = itemHit.getPosition() - pos;
                                    menu[i].cantidad--;
                                }

                                columna++;
                                if (columna >= 4)
                                {
                                    columna = 0;
                                    ix = 150.f;
                                    iy += 80.f;
                                }
                                else
                                    ix += bounds.size.x + 15.f;
                            }
                        }
                    }
                    else
                    {
                        if (textoCerrar.getGlobalBounds().contains(pos))
                            menuAbierto = false;

                        for (int i = 0; i < (int)menu.size(); i++)
                        {
                            float by = 180.f + i * 85.f;
                            sf::FloatRect boton(sf::Vector2f(390.f, by + 10.f), sf::Vector2f(100.f, 35.f));
                            if (boton.contains(pos) && monedas >= menu[i].precio)
                            {
                                monedas -= menu[i].precio;
                                menu[i].cantidad++;
                            }
                        }
                    }
                }

                // Vestuario
                if (enVestuario)
                {
                    if (!menuVestuarioAbierto)
                    {
                        if (armario.getGlobalBounds().contains(pos))
                            menuVestuarioAbierto = true;
                    }
                    else
                    {
                        if (textoCerrarVestuario.getGlobalBounds().contains(pos))
                            menuVestuarioAbierto = false;

                        for (int i = 0; i < (int)skins.size(); i++)
                        {
                            float by = 160.f + i * 90.f;
                            sf::FloatRect boton(sf::Vector2f(390.f, by + 25.f), sf::Vector2f(110.f, 35.f));
                            if (boton.contains(pos))
                            {
                                if (!skins[i].comprada && monedas >= skins[i].precio)
                                {
                                    monedas -= skins[i].precio;
                                    skins[i].comprada = true;
                                }
                                if (skins[i].comprada && skinActual != i)
                                {
                                    skinActual = i;
                                    menuVestuarioAbierto = false;
                                }
                            }
                        }
                    }
                }

                // Sala de juegos - menu
                if (enSalaJuegos && juegoActivo == 0)
                {
                    sf::Sprite joystickHit(assets.joystick);
                    sf::Vector2u tamJoystick = assets.joystick.getSize();
                    float escalaJoystick = 100.f / tamJoystick.y;
                    joystickHit.setScale(sf::Vector2f(escalaJoystick, escalaJoystick));
                    float anchoJoystick = tamJoystick.x * escalaJoystick;
                    joystickHit.setPosition(sf::Vector2f(300.f - anchoJoystick / 2.f, 780.f));
                    if (joystickHit.getGlobalBounds().contains(pos))
                        juegoActivo = -1;
                }

                if (juegoActivo == -1)
                {
                    // Cerrar menu
                    sf::FloatRect cerrar(sf::Vector2f(470.f, 160.f), sf::Vector2f(30.f, 30.f));
                    if (cerrar.contains(pos))
                        juegoActivo = 0;

                    // Boton juego 1
                    sf::FloatRect b1(sf::Vector2f(150.f, 230.f), sf::Vector2f(300.f, 60.f));
                    if (b1.contains(pos))
                    {
                        juegoActivo = 1;
                        iniciarJuego1(estadoJ1);
                        cabezaBilardo.setPosition(sf::Vector2f(270.f, 800.f));
                    }
                    sf::FloatRect b2(sf::Vector2f(150.f, 320.f), sf::Vector2f(300.f, 60.f));
                    if (b2.contains(pos))
                    {
                        juegoActivo = 2;
                        iniciarJuego2(estadoJ2);
                    }
                    
                    sf::FloatRect b4(sf::Vector2f(150.f, 500.f), sf::Vector2f(300.f, 60.f));
                    if (b4.contains(pos))
                    {
                        juegoActivo = 4;
                        iniciarJuego4(estadoJ4);
                    }
                }
            }

            // Soltar objeto de limpieza
            if (event->is<sf::Event::MouseButtonReleased>())
            {
                if (arrastandoObjeto)
                {
                    arrastandoObjeto = false;
                    progresoLimpieza = 0.f;
                    objetoLimpieza.setPosition(POS_OBJETO);
                }

                if (arrastandoComida)
                {
                    bool sobreMascota = mascota.getGlobalBounds().contains(posMouse);
                    if (sobreMascota && hambre.valor < 100.f && comidaSeleccionada >= 0)
                        hambre.valor = std::min(100.f, hambre.valor + menu[comidaSeleccionada].hambre);
                    else if (comidaSeleccionada >= 0)
                        menu[comidaSeleccionada].cantidad++;
                    arrastandoComida = false;
                    comidaSeleccionada = -1;
                }
            }

            if (event->is<sf::Event::KeyPressed>())
            {
                auto key = event->getIf<sf::Event::KeyPressed>()->code;
                if (key == sf::Keyboard::Key::Space && juegoActivo == 1 && estadoJ1.terminado)
                {
                    iniciarJuego1(estadoJ1);
                    cabezaBilardo.setPosition(sf::Vector2f(270.f, 800.f));
                }

                if (juegoActivo == 2)
                {
                    if (key == sf::Keyboard::Key::Space ||
                        key == sf::Keyboard::Key::W ||
                        key == sf::Keyboard::Key::Up)
                    {
                        if (estadoJ2.terminado)
                            iniciarJuego2(estadoJ2);
                        else
                            saltar(estadoJ2);
                    }
                }

                if (key == sf::Keyboard::Key::Space && juegoActivo == 4 && estadoJ4.terminado)
                    iniciarJuego4(estadoJ4);
            }
        }

        // Barras bajan lentamente
        float velocidad = durmiendo ? 0.005f : 0.01f;
        hambre.valor    = std::max(0.f, hambre.valor    - dt * velocidad);
        felicidad.valor = std::max(0.f, felicidad.valor - dt * velocidad);
        carino.valor    = std::max(0.f, carino.valor    - dt * velocidad);

        // Acariciar mascota
        bool clickSostenido = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        bool mouseEnMascota = mascota.getGlobalBounds().contains(posMouse);
        if (clickSostenido && mouseEnMascota && !durmiendo && !arrastandoObjeto && !arrastandoComida)
            carino.valor = std::min(100.f, carino.valor + dt * 5.f);

        if (durmiendo)
            sueno.valor = std::min(100.f, sueno.valor + dt * 2.f);
        else
            sueno.valor = std::max(0.f, sueno.valor - dt * 0.01f);

        // Suciedad
        if (!mascotaSucia)
        {
            tiempoSuciedad += dt;
            if (tiempoSuciedad >= TIEMPO_ENSUCIARSE)
            {
                mascotaSucia = true;
                tiempoSuciedad = 0.f;
            }
        }

        if (arrastandoObjeto)
            objetoLimpieza.setPosition(posMouse + offsetArrastre);

        if (arrastandoComida)
            posComidaArrastrable = posMouse + offsetComida;

        if (arrastandoObjeto && mascotaSucia)
        {
            if (mascota.getGlobalBounds().findIntersection(objetoLimpieza.getGlobalBounds()).has_value())
            {
                progresoLimpieza += dt;
                if (progresoLimpieza >= TIEMPO_LIMPIEZA)
                {
                    mascotaSucia = false;
                    progresoLimpieza = 0.f;
                    arrastandoObjeto = false;
                    objetoLimpieza.setPosition(POS_OBJETO);
                    monedas += 5;
                }
            }
        }

        barraLimpieza.setSize(sf::Vector2f((progresoLimpieza / TIEMPO_LIMPIEZA) * 200.f, 20.f));

        sf::FloatRect bounds = textoHabitacion.getLocalBounds();
        textoHabitacion.setPosition(sf::Vector2f(300.f - bounds.size.x / 2.f, 155.f));
        textoMonedas.setString(L"Monedas: " + std::to_wstring(monedas));

        int alphaSucio = (int)(255 * (1.f - progresoLimpieza / TIEMPO_LIMPIEZA));

        // Juego activo - actualizar
        cabezaBilardo.setTexture(assets.cabeza[skinActual]);
        sf::Vector2u tamCab = assets.cabeza[skinActual].getSize();
        float escalaCab = (juegoActivo == 4 ? 120.f : 80.f) / tamCab.y;
        cabezaBilardo.setScale(sf::Vector2f(escalaCab, escalaCab));
        if (juegoActivo == 1)
        {
            float mouseX = posMouse.x - 30.f;
            mouseX = std::max(0.f, std::min(540.f, mouseX));
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                cabezaBilardo.setPosition(sf::Vector2f(mouseX, 800.f));
            actualizarJuego1(estadoJ1, dt, cabezaBilardo, menu, monedas, felicidad, assets.comida);
            dibujarJuego1(window, estadoJ1, cabezaBilardo, fuente);
            window.display();
            continue;
        }
        if (juegoActivo == 2)
        {
            actualizarJuego2(estadoJ2, dt, monedas, felicidad,
                           assets.cactus1, assets.cactus2, assets.cactus3, cabezaBilardo);
            dibujarJuego2(window, estadoJ2, cabezaBilardo, fuente, assets.arenaTex);
            window.display();
            continue;
        }
        if (juegoActivo == 4)
        {
            actualizarJuego4(estadoJ4, dt, monedas, felicidad);
            dibujarJuego4(window, estadoJ4, cabezaBilardo, fuente, assets.arco, assets.pelota);
            window.display();
            continue;
        }

        // ---- DIBUJAR ----
        window.clear(coloresFondo[habitacionActual]);

        dibujarBarra(window, hambre,    20.f,  110.f, 125.f, 25.f, fuente, posMouse);
        dibujarBarra(window, felicidad, 160.f, 110.f, 125.f, 25.f, fuente, posMouse);
        dibujarBarra(window, sueno,     300.f, 110.f, 125.f, 25.f, fuente, posMouse);
        dibujarBarra(window, carino,    440.f, 110.f, 125.f, 25.f, fuente, posMouse);

        window.draw(textoMonedas);
        window.draw(textoHabitacion);

        dibujarFlecha(window,   0.f, 450.f, true);
        dibujarFlecha(window, 560.f, 450.f, false);

        // Determinar estado de la mascota (0=normal, 1=sucio, 2=dormido, 3=sucio+dormido)
        int estadoMascota = 0;
        if (mascotaSucia && durmiendo)       estadoMascota = 3;
        else if (durmiendo)                  estadoMascota = 2;
        else if (mascotaSucia)               estadoMascota = 1;

        mascota.setTexture(assets.mascota[skinActual][estadoMascota]);

        // Escalar manteniendo proporciones, altura maxima 300px
        sf::Vector2u tamTextura = assets.mascota[skinActual][estadoMascota].getSize();
        float escala = 430.f / tamTextura.y;
        mascota.setScale(sf::Vector2f(escala, escala));

        // Centrar horizontalmente
        float anchofinal = tamTextura.x * escala;
        mascota.setPosition(sf::Vector2f(300.f - anchofinal / 2.f, 300.f));

        if (enDormitorio)
            window.draw(cama);
    
        window.draw(mascota);

        if (enBano)
        {
            window.draw(objetoLimpieza);
            if (mascotaSucia)
            {
                window.draw(fondoLimpieza);
                window.draw(barraLimpieza);
                window.draw(textoLimpiar);
            }
        }

        if (enDormitorio)
        {
            luz.setTexture(durmiendo ? assets.veladorApagado : assets.veladorPrendido);
            window.draw(luz);
            if (durmiendo)
            {
                window.draw(overlayOscuro);
                window.draw(textoZzz);
                window.draw(luz);
            }
        }

        if (enVestuario)
        {
            window.draw(armario);
            if (menuVestuarioAbierto)
            {
                window.draw(panelVestuario);
                window.draw(tituloVestuario);
                window.draw(textoCerrarVestuario);

                for (int i = 0; i < (int)skins.size(); i++)
                {
                    float by = 160.f + i * 90.f;

                    // Mostrar bloqueada o normal segun si está comprada
                    sf::Texture& texPreview = (i > 0 && !skins[i].comprada) 
                        ? assets.bloqueada[i - 1] 
                        : assets.mascota[i][0];
                    sf::Sprite preview(texPreview);
                    sf::Vector2u tamPreview = texPreview.getSize();
                    float escalaPreview = 60.f / tamPreview.y;
                    preview.setScale(sf::Vector2f(escalaPreview, escalaPreview));
                    float anchoPreview = tamPreview.x * escalaPreview;
                    preview.setPosition(sf::Vector2f(95.f - anchoPreview / 2.f, by + 10.f));

                    // Borde dorado si está equipada
                    if (skinActual == i)
                    {
                        sf::RectangleShape borde(sf::Vector2f(anchoPreview + 6.f, 60.f + 6.f));
                        borde.setFillColor(sf::Color::Transparent);
                        borde.setOutlineColor(sf::Color(255, 215, 0));
                        borde.setOutlineThickness(3.f);
                        borde.setPosition(sf::Vector2f(95.f - anchoPreview / 2.f - 3.f, by + 7.f));
                        window.draw(borde);
                    }
                    window.draw(preview);

                    sf::Text nombre(fuente, skins[i].nombre, 18);
                    nombre.setFillColor(sf::Color::Black);
                    nombre.setPosition(sf::Vector2f(145.f, by + 10.f));
                    window.draw(nombre);

                    if (skins[i].comprada)
                    {
                        sf::Text textoEstado(fuente, skinActual == i ? L"Equipada" : L"Equipar", 16);
                        textoEstado.setFillColor(skinActual == i ? sf::Color(0, 150, 0) : sf::Color(0, 0, 200));
                        textoEstado.setPosition(sf::Vector2f(145.f, by + 35.f));
                        window.draw(textoEstado);
                    }
                    else
                    {
                        sf::Text textoPrecio(fuente, L"50 monedas", 16);
                        textoPrecio.setFillColor(sf::Color(255, 215, 0));
                        textoPrecio.setPosition(sf::Vector2f(145.f, by + 35.f));
                        window.draw(textoPrecio);
                    }

                    if (i != 0 || skinActual != 0)
                    {
                        sf::RectangleShape boton(sf::Vector2f(110.f, 35.f));
                        bool puedo = skins[i].comprada ? (skinActual != i) : (monedas >= 50);
                        boton.setFillColor(puedo ? sf::Color(0, 180, 0) : sf::Color(150, 150, 150));
                        boton.setOutlineColor(sf::Color::Black);
                        boton.setOutlineThickness(2.f);
                        boton.setPosition(sf::Vector2f(390.f, by + 25.f));
                        window.draw(boton);

                        sf::Text textoBoton(fuente, skins[i].comprada ? L"Equipar" : L"Comprar", 16);
                        textoBoton.setFillColor(sf::Color::White);
                        textoBoton.setPosition(sf::Vector2f(400.f, by + 32.f));
                        window.draw(textoBoton);
                    }
                }
            }
        }

        if (enCocina)
        {
            window.draw(heladera);

            float ix = 150.f;
            float iy = 620.f;
            int columna = 0;
            for (int i = 0; i < (int)menu.size(); i++)
            {
                if (menu[i].cantidad > 0)
                {
                    sf::Sprite item(assets.comida[i]);
                    item.setScale(sf::Vector2f(menu[i].escalaInventario, menu[i].escalaInventario));
                    sf::FloatRect boundsItem = item.getGlobalBounds();
                    item.setPosition(sf::Vector2f(ix, iy - boundsItem.size.y / 2.f));
                    window.draw(item);

                    sf::Text cant(fuente, std::to_wstring(menu[i].cantidad), 16);
                    cant.setFillColor(sf::Color::White);
                    cant.setPosition(sf::Vector2f(ix, iy + 20.f));
                    window.draw(cant);

                    columna++;
                    if (columna >= 4)
                    {
                        columna = 0;
                        ix = 150.f;
                        iy += 80.f;
                    }
                    else
                        ix += boundsItem.size.x + 15.f;
                }
            }

            if (arrastandoComida && comidaSeleccionada >= 0)
            {
                comidaArrastrable.setTexture(assets.comida[comidaSeleccionada], true);
                sf::Vector2u tamArr = assets.comida[comidaSeleccionada].getSize();
                float escalaArr = 45.f / tamArr.y;
                comidaArrastrable.setScale(sf::Vector2f(escalaArr, escalaArr));
                comidaArrastrable.setPosition(posComidaArrastrable);
                window.draw(comidaArrastrable);
            }

            if (menuAbierto)
            {
                window.draw(panelMenu);
                window.draw(tituloMenu);
                window.draw(textoCerrar);

                for (int i = 0; i < (int)menu.size(); i++)
                {
                    float by = 180.f + i * 85.f;

                    sf::Sprite iconoComida(assets.comida[i]);
                    iconoComida.setScale(sf::Vector2f(menu[i].escalaMenu, menu[i].escalaMenu));
                    sf::FloatRect boundsIcono = iconoComida.getGlobalBounds();
                    float xCentradoIcono = 95.f - boundsIcono.size.x / 2.f;
                    float yCentradoIcono = by + 30.f - boundsIcono.size.y / 2.f;
                    iconoComida.setPosition(sf::Vector2f(xCentradoIcono, yCentradoIcono));
                    window.draw(iconoComida);

                    sf::Text nombre(fuente, menu[i].nombre, 18);
                    nombre.setFillColor(sf::Color::Black);
                    nombre.setPosition(sf::Vector2f(145.f, by + 10.f));
                    window.draw(nombre);

                    int porcentaje = (int)menu[i].hambre;
                    sf::Text precio(fuente, std::to_wstring(menu[i].precio) + L" monedas  +" + std::to_wstring(porcentaje) + L"%", 16);
                    precio.setFillColor(sf::Color(255, 215, 0));
                    precio.setPosition(sf::Vector2f(145.f, by + 35.f));
                    window.draw(precio);

                    sf::RectangleShape boton(sf::Vector2f(100.f, 35.f));
                    bool puedoComprar = monedas >= menu[i].precio;
                    boton.setFillColor(puedoComprar ? sf::Color(0, 180, 0) : sf::Color(150, 150, 150));
                    boton.setOutlineColor(sf::Color::Black);
                    boton.setOutlineThickness(2.f);
                    boton.setPosition(sf::Vector2f(390.f, by + 10.f));
                    window.draw(boton);

                    sf::Text textoBoton(fuente, L"Comprar", 16);
                    textoBoton.setFillColor(sf::Color::White);
                    textoBoton.setPosition(sf::Vector2f(398.f, by + 15.f));
                    window.draw(textoBoton);

                    sf::Text cantInv(fuente, L"x" + std::to_wstring(menu[i].cantidad), 16);
                    cantInv.setFillColor(sf::Color::Black);
                    cantInv.setPosition(sf::Vector2f(510.f, by + 15.f));
                    window.draw(cantInv);
                }
            }
        }

        if (enSalaJuegos && juegoActivo <= 0)
        {
            sf::Sprite joystick(assets.joystick);
            sf::Vector2u tamJoystick = assets.joystick.getSize();
            float escalaJoystick = 100.f / tamJoystick.y;
            joystick.setScale(sf::Vector2f(escalaJoystick, escalaJoystick));
            float anchoJoystick = tamJoystick.x * escalaJoystick;
            joystick.setPosition(sf::Vector2f(300.f - anchoJoystick / 2.f, 780.f));
            window.draw(joystick);
        }

        // Menu de juegos
        if (juegoActivo == -1)
        {
            sf::RectangleShape panel(sf::Vector2f(400.f, 500.f));
            panel.setFillColor(sf::Color(245, 245, 220));
            panel.setOutlineColor(sf::Color::Black);
            panel.setOutlineThickness(3.f);
            panel.setPosition(sf::Vector2f(100.f, 150.f));
            window.draw(panel);

            sf::Text titulo(fuente, L"Minijuegos", 28);
            titulo.setFillColor(sf::Color::Black);
            titulo.setPosition(sf::Vector2f(195.f, 165.f));
            window.draw(titulo);

            sf::Text cerrar(fuente, L"X", 28);
            cerrar.setFillColor(sf::Color(200, 0, 0));
            cerrar.setPosition(sf::Vector2f(470.f, 160.f));
            window.draw(cerrar);

            // Nombres y records de cada juego
            std::vector<sf::String> nombresJuegos = {
                L"Atrapar comida",
                L"Sin internet",
                L"Memoria",
                L"Arco de futbol"
            };
            std::vector<int> records = {
                estadoJ1.record, 0, 0, 0
            };

            for (int i = 0; i < 4; i++)
            {
                float by = 230.f + i * 90.f;

                sf::RectangleShape boton(sf::Vector2f(300.f, 60.f));
                boton.setFillColor(sf::Color(100, 100, 220));
                boton.setOutlineColor(sf::Color::Black);
                boton.setOutlineThickness(2.f);
                boton.setPosition(sf::Vector2f(150.f, by));
                window.draw(boton);

                sf::Text textoJuego(fuente, nombresJuegos[i], 20);
                textoJuego.setFillColor(sf::Color::White);
                textoJuego.setPosition(sf::Vector2f(165.f, by + 8.f));
                window.draw(textoJuego);

                sf::Text textoRecord(fuente, L"Record: " + std::to_wstring(records[i]), 16);
                textoRecord.setFillColor(sf::Color(50, 50, 50));
                textoRecord.setPosition(sf::Vector2f(165.f, by + 35.f));
                window.draw(textoRecord);
            }
        }

        window.display();
    }

    return 0;
}
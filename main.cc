#include<cstdlib>
#include<ctime>
#include<iostream>
#include<unistd.h>
#include<SFML/Graphics.hpp>
#include"board.hpp"

sf::Color background_color {127,127,127,255};
sf::Font game_font;

void wait_for_frame(sf::Clock &clock) {
    const float frametime = 1000000.f/60.f;
    float elapsed_time = clock.getElapsedTime().asMicroseconds();
    if (elapsed_time < frametime) {
        sf::sleep(sf::microseconds(frametime-elapsed_time));
    }
    clock.restart();
}

int main() {
    // initialize randomness
    srand(time(0)*getpid());
    Board board {15, 65};

    int width=30+(board.num_cols_*board.TILE_SIZE_PIXELS);
    int height=80+(board.num_rows_*board.TILE_SIZE_PIXELS);
    sf::RenderWindow window(sf::VideoMode(width, height), "Minesweeper");

    if (!game_font.loadFromFile("res/C64_Pro_Mono-STYLE.ttf"))
        return EXIT_FAILURE;

    sf::Clock frame_timer;
    std::cout << "Made it this far\n";

    int i=0;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                continue;
            case sf::Event::MouseButtonReleased:
                board.handle_mouse_click(event.mouseButton);
                break;
            }
        }
        window.clear(background_color);
        board.draw(window);
        window.display();
        wait_for_frame(frame_timer);
    }
}

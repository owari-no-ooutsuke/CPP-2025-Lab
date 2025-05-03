#include "Gems.hpp"

int main() {
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(800, 800), "GEMS");
    Board b(BOARD_SIZE, window);

    sf::Font font;
    font.loadFromFile("C:/Windows/Fonts/Arial.ttf");

    Game game(b, font);

    while (window.isOpen()) {

        sf::Event event = sf::Event();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    game.HandleClick({ event.mouseButton.x, event.mouseButton.y });
                    game.Update();
                }
            }
        }
        game.board.Display();
        game.CheckGameOver();
    }
    return 0;
}
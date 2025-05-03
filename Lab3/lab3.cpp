#include <iostream>
#include <SFML/Graphics.hpp>
#define BOARD_SIZE 10
#define COLOR_COUNT 5

using std::vector;

const vector<sf::Color> colors = { sf::Color::Red, sf::Color::Green, sf::Color::Blue,
sf::Color::Yellow, sf::Color::Magenta };

class Tile {
public:
    Tile();
    sf::Color color;
};

Tile::Tile() {
    int colorNum = rand() % COLOR_COUNT;
    color = colors[colorNum];
}

class Board {
public:
    Board(unsigned int size);
    unsigned int boardSize;
    vector<Tile> tiles;
    void SwapTiles(int index1, int index2);
    void removeMatchingTiles();
    void Gravity();
    void Display(sf::RenderWindow& window);
};

Board::Board(unsigned int size) {
    boardSize = size;
    for (int i = 0; i < size * size; i++) {
        Tile t;
        tiles.push_back(t);
    }
}

void Board::SwapTiles(int index1, int index2) {
    std::swap(tiles[index1], tiles[index2]);
}

void Board::Display(sf::RenderWindow& window) {
    window.clear();
    float tileSize = (float)window.getSize().x / boardSize;
    float x, y;

    for (int i = 0; i < boardSize * boardSize; i++) {
        sf::RectangleShape r({ tileSize, tileSize });
        y = (i / boardSize) * tileSize;
        x = (i % boardSize) * tileSize;
        r.setPosition({ x, y });
        r.setFillColor(tiles[i].color);
        r.setOutlineColor(sf::Color::Black);
        r.setOutlineThickness(-5.f);
        window.draw(r);
    }
    window.display();
}

class Game {
public:
    Game(Board& b, sf::RenderWindow& win) : board(b), window(win), selectedTileIndex(-1) {};
    void HandleClick(const sf::Vector2i pos);
    void Update();
    Board& board;
    sf::RenderWindow& window;
    int selectedTileIndex;
};

void Game::HandleClick(const sf::Vector2i pos) {
    float tileSize = (float)window.getSize().x / board.boardSize;
    int col = pos.x / tileSize;
    int row = pos.y / tileSize;
    int index = row * board.boardSize + col;

    if (selectedTileIndex == -1) {
        selectedTileIndex = index;
    }
    else {
        if (abs(selectedTileIndex - index) == 1 ||
            abs(selectedTileIndex - index) == board.boardSize) {
            board.SwapTiles(selectedTileIndex, index);
        }
        selectedTileIndex = -1;
    }
}

void Game::Update() {
    board.Display(window);
}

int main() {
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Test");
    Board b(BOARD_SIZE);

    Game game(b, window);

    while (window.isOpen()) {

        sf::Event event = sf::Event();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    game.HandleClick({ event.mouseButton.x, event.mouseButton.y });
                }
            }
        }

        game.Update();

    }
    return 0;
}
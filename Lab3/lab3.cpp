#include <SFML/Graphics.hpp>
#include <windows.h>
#define BOARD_SIZE 5
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
    int FindMatch(int startIndex, vector<int>& matchSeq, int prevIndex, int maxIndex);
    int RemoveMatchTiles();
    void Gravity();
    void Display(sf::RenderWindow& window);
};

Board::Board(unsigned int size) {
    boardSize = size;
    vector<int> matchSeq;
    for (int i = 0; i < size * size; i++) {
        matchSeq.clear();
        Tile t;
        tiles.push_back(t);
        FindMatch(i, matchSeq, -1, i);
        while (matchSeq.size() >= 3) {
            tiles.pop_back();
            matchSeq.clear();
            Tile t;
            tiles.push_back(t);
            FindMatch(i, matchSeq, -1, i);
        }
    }
}

void Board::SwapTiles(int index1, int index2) {
    std::swap(tiles[index1], tiles[index2]);
}

int Board::FindMatch(int startIndex, vector<int>& matchSeq, int prevIndex, int maxIndex) {

    int row = startIndex / boardSize;
    int col = startIndex % boardSize;
    int sameColorNeigh = 0;

    matchSeq.push_back(startIndex);

    if (row != 0 && prevIndex != startIndex - boardSize) {
        if (tiles[startIndex].color == tiles[startIndex - boardSize].color) {
            sameColorNeigh++;
            FindMatch(startIndex - boardSize, matchSeq, startIndex, maxIndex);
        }
    }

    if (startIndex + boardSize <= maxIndex && prevIndex != startIndex + boardSize) {
        if (tiles[startIndex].color == tiles[startIndex + boardSize].color) {
            sameColorNeigh++;
            FindMatch(startIndex + boardSize, matchSeq, startIndex, maxIndex);
        }
    }

    if (col != 0 && prevIndex != startIndex - 1) {
        if (tiles[startIndex].color == tiles[startIndex - 1].color) {
            sameColorNeigh++;
            FindMatch(startIndex - 1, matchSeq, startIndex, maxIndex);
        }
    }

    if (startIndex + 1 <= maxIndex && prevIndex != startIndex + 1) {
        if (tiles[startIndex].color == tiles[startIndex + 1].color) {
            sameColorNeigh++;
            FindMatch(startIndex + 1, matchSeq, startIndex, maxIndex);
        }
    }

    return sameColorNeigh;
}

int Board::RemoveMatchTiles() {
    vector<int> matchSeq;
    int tilesRemoved = 0;
    for (int i = 0; i < boardSize * boardSize; i++) {
        matchSeq.clear();
        if (tiles[i].color != sf::Color::Black) {
            FindMatch(i, matchSeq, -1, boardSize * boardSize - 1);
            if (matchSeq.size() >= 3) {
                for (int j = 0; j < matchSeq.size(); j++) {
                    tiles[matchSeq[j]].color = sf::Color::Black;
                    tilesRemoved++;
                }
            }
        }
    }
    return tilesRemoved;
}

void Board::Gravity() {
    for (int col = 0; col < boardSize; col++) {
        vector<Tile> columnTiles;
        for (int row = boardSize - 1; row >= 0; row--) {
            int index = row * boardSize + col;
            if (tiles[index].color != sf::Color::Black) {
                columnTiles.push_back(tiles[index]);
            }
        }

        for (int row = boardSize - 1, i = 0; row >= 0; row--) {
            int index = row * boardSize + col;
            if (i < columnTiles.size()) {
                tiles[index] = columnTiles[i++];
            }
            else {
                tiles[index].color = sf::Color::Black;
            }
        }
    }
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
            if (board.tiles[index].color != sf::Color::Black
                && board.tiles[selectedTileIndex].color != sf::Color::Black) {
                board.SwapTiles(selectedTileIndex, index);
            }
        }
        selectedTileIndex = -1;
    }
}

void Game::Update() {
    board.Display(window);
    Sleep(200);
    int removed = board.RemoveMatchTiles();
    board.Gravity();
    board.Display(window);
    Sleep(200);
    while (removed > 0) {
        removed = board.RemoveMatchTiles();
        board.Gravity();
        board.Display(window);
        Sleep(200);
    }
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
                    game.Update();
                }
            }
        }
        game.board.Display(window);

    }
    return 0;
}
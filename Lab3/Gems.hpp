#include <SFML/Graphics.hpp>
#include <windows.h>
#define BOARD_SIZE 10 //размер игрового поля BOARD_SIZE x BOARD_SIZE
#define COLOR_COUNT 5 //количество возможных цветов квадратов
#define BONUS_PROB 12 //вероятность выпадения конкретного бонуса 1/BONUS_PROB
#define BONUS_RADIUS 3 //радиус окрестности 

using std::vector;

//возможные цвета квадратов
const vector<sf::Color> colors = { sf::Color::Red, sf::Color::Green, sf::Color::Blue,
sf::Color::Yellow, sf::Color::Magenta };

//виды бонусов
enum BonusType {
    NONE = 0,
    COLOR_CHANGE,
    BOMB
};

//квадрат
class Tile {
public:
    Tile();
    sf::Color color;
    BonusType bonus;
};


//игровое поле
class Board {
public:
    Board(unsigned int size, sf::RenderWindow& wind);
    int boardSize; //размер поля
    vector<Tile> tiles; //массив клеток поля 
    sf::RenderWindow& window; //окно для вывода
    void SwapTiles(int index1, int index2);
    int FindMatch(int startIndex, vector<int>& matchSeq, int prevIndex, int maxIndex);
    void DestroyTileSeq(vector<int> indices);
    int DestroyMatchTiles();
    void Gravity();
    bool AreNeighbors(int index1, int index2);
    vector<int> ActiveTiles();
    vector<int> ActiveTilesAround(int fromIndex);
    void ActivateColorChange(int fromIndex, sf::Color color);
    void ActivateBomb(int fromIndex);
    void Display();
};

//класс, управляющий игрой (начало/конец игры, взаимодействие с пользователем, обновление поля)
class Game {
public:
    Game(Board& b, sf::Font f) : board(b), selectedTileIndex(-1), font(f) {};
    void HandleClick(const sf::Vector2i pos);
    void Update();
    Board& board; //игровое поле
    int selectedTileIndex; //индекс выбранного квадрата 
    sf::Font font; //шрифт для вывода текста
    void CheckGameOver();
};
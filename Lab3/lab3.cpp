#include <SFML/Graphics.hpp>
#include <windows.h>
#define BOARD_SIZE 10 //размер игрового поля BOARD_SIZE x BOARD_SIZE
#define COLOR_COUNT 5 //количество возможных цветов квадратов
#define BONUS_PROB 10 //вероятность выпадения конкретного бонуса 1/BONUS_PROB
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

//при создании квадрата задаются рандомный цвет и рандомный вид бонуса
Tile::Tile() {
    int colorNum = rand() % COLOR_COUNT;
    color = colors[colorNum];
    int bonusNum = rand() % BONUS_PROB;
    if (bonusNum == 0) {
        bonus = COLOR_CHANGE;
    }
    else if (bonusNum == 1) {
        bonus = BOMB;
    }
    else {
        bonus = NONE;
    }
}

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

/*
при создании поле заполняется квадратами
(с проверкой, чтобы изначально оно не содержало комбинаций по 3 и более соседних
квадратов одного цвета)
*/
Board::Board(unsigned int size, sf::RenderWindow& wind) : window(wind) {
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

//обмен местами двух квадратов
void Board::SwapTiles(int index1, int index2) {
    std::swap(tiles[index1], tiles[index2]);
}

/* поиск комбинаций по 3 и более соседних квадратов одного цвета
*
* записывает комбинации в массив matchSeq
* startIndex - индекс текущего квадрата в массиве tiles
* prevIndex - индекс квадрата, от которого пришли в текущий (в начале поиска можно
* задать равным любому целому числу, не являющемуся индексом в tiles)
* maxIndex - максимальный индекс, до которого можем идти
*/
int Board::FindMatch(int startIndex, vector<int>& matchSeq, int prevIndex, int maxIndex) {
    if (std::find(matchSeq.begin(), matchSeq.end(), startIndex) != matchSeq.end()) {
        return 0;
    }

    int row = startIndex / boardSize;
    int col = startIndex % boardSize;
    int sameColorNeigh = 0;
    sf::Color targetColor = tiles[startIndex].color;

    matchSeq.push_back(startIndex);

    //направления: вверх, вниз, влево, вправо
    const int directions[4] = { -boardSize, boardSize, -1, 1 };

    for (int dir : directions) {
        int newIndex = startIndex + dir;

        bool isValid = true;
        if (newIndex < 0 || newIndex > maxIndex) isValid = false;
        if (newIndex == prevIndex) isValid = false;

        if (abs(dir) == 1) {
            int newRow = newIndex / boardSize;
            if (newRow != row) isValid = false;
        }

        if (isValid && tiles[newIndex].color == targetColor) {
            sameColorNeigh++;
            FindMatch(newIndex, matchSeq, startIndex, maxIndex);
        }
    }

    return sameColorNeigh;
}

/* уничтожение последовательности квадратов
*
* если тип бонуса у уничтожаемого квадрата не NONE, то активируется соответствующий бонус
*/
void Board::DestroyTileSeq(vector<int> indices) {
    vector<sf::Color> colors;
    for (int i = 0; i < indices.size(); i++) {
        colors.push_back(tiles[indices[i]].color);
        tiles[indices[i]].color = sf::Color::Black;
    }
    Gravity();
    Display();
    for (int i = 0; i < indices.size(); i++) {
        if (tiles[indices[i]].bonus == COLOR_CHANGE) {
            Sleep(200);
            ActivateColorChange(indices[i], colors[i]);
            Display();
        }
        else if (tiles[indices[i]].bonus == BOMB) {
            Sleep(200);
            ActivateBomb(indices[i]);
            Display();
        }
    }
    Gravity();
    Display();
}

/* уничтожение всех комбинаций из 3 и более соседних квадратов одного цвета на поле
*
* возвращает 0, если таких комбинаций нет
*/
int Board::DestroyMatchTiles() {
    vector<int> matchSeq;
    int seqDestroyed = 0;
    for (int i = 0; i < boardSize * boardSize; i++) {
        matchSeq.clear();
        if (tiles[i].color != sf::Color::Black) {
            FindMatch(i, matchSeq, -1, boardSize * boardSize - 1);
            if (matchSeq.size() >= 3) {
                DestroyTileSeq(matchSeq);
                seqDestroyed++;
            }
        }
    }
    return seqDestroyed;
}

//сдвиг квадратов вниз
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

//проверка квадратов на соседство на поле
bool Board::AreNeighbors(int index1, int index2) {
    if (abs(index1 - index2) == 1 || abs(index1 - index2) == boardSize) {
        return true;
    }
    else {
        return false;
    }
}

//возвращает массив индексов всех активных (еще не уничтоженных) квадратов на поле
vector<int> Board::ActiveTiles() {
    vector<int> active;
    for (int i = 0; i < boardSize * boardSize; i++) {
        if (tiles[i].color != sf::Color::Black) {
            active.push_back(i);
        }
    }
    return active;
}

//возвращает массив индексов активных квадратов в окрестности квадрата с индексом fromIndex
vector<int> Board::ActiveTilesAround(int fromIndex) {
    vector<int> active;
    int fromRow = fromIndex / boardSize;
    int fromCol = fromIndex % boardSize;
    int index;

    int minRow, maxRow, minCol, maxCol;
    if (fromRow - BONUS_RADIUS < 0) {
        minRow = 0;
    }
    else {
        minRow = fromRow - BONUS_RADIUS;
    }
    if (fromRow + BONUS_RADIUS >= boardSize) {
        maxRow = boardSize - 1;
    }
    else {
        maxRow = fromRow + BONUS_RADIUS;
    }
    if (fromCol - BONUS_RADIUS < 0) {
        minCol = 0;
    }
    else {
        minCol = fromCol - BONUS_RADIUS;
    }
    if (fromCol + BONUS_RADIUS >= boardSize) {
        maxCol = boardSize - 1;
    }
    else {
        maxCol = fromCol + BONUS_RADIUS;
    }

    for (int row = minRow; row <= maxRow; row++) {
        for (int col = minCol; col <= maxCol; col++) {
            index = row * boardSize + col;
            if (tiles[index].color != sf::Color::Black) {
                active.push_back(index);
            }
        }
    }
    return active;
}

//активация бонуса "перекрашивание"
void Board::ActivateColorChange(int fromIndex, sf::Color color) {
    vector<int> targets;
    vector<int> activeAround = ActiveTilesAround(fromIndex);
    if (activeAround.size() == 0) {
        return;
    }

    int targetIndex = rand() % activeAround.size();
    targets.push_back(activeAround[targetIndex]);

    int neighCount = 0;
    for (int i = 0; i < activeAround.size(); i++) {
        if (AreNeighbors(activeAround[i], targets[0]) == true) {
            neighCount++;
        }
    }
    if (activeAround.size() - neighCount < 3) {
        for (int i = 0; i < activeAround.size(); i++) {
            if (AreNeighbors(activeAround[i], targets[0]) == false) {
                targets.push_back(activeAround[i]);
            }
        }
    }
    else {
        while (targets.size() < 3) {
            targetIndex = rand() % activeAround.size();
            if (AreNeighbors(targetIndex, targets[0]) == false
                && std::find(targets.begin(), targets.end(), targetIndex) == targets.end()) {
                targets.push_back(activeAround[targetIndex]);
            }
        }
    }

    //визуальный эффект 
    for (int i = 0; i < targets.size(); i++) {
        tiles[targets[i]].color = sf::Color::White;
    }
    Display();
    Sleep(500);

    //перекрашивание в нужный цвет
    for (int i = 0; i < targets.size(); i++) {
        tiles[targets[i]].color = color;
    }
    Display();
    Sleep(500);
}

//активация бонуса "бомба"
void Board::ActivateBomb(int fromIndex) {
    vector<int> targets;
    vector<int> activeAround = ActiveTilesAround(fromIndex);

    if (activeAround.size() == 0) {
        return;
    }

    int targetIndex = rand() % activeAround.size();
    targets.push_back(activeAround[targetIndex]);

    vector<int> active = ActiveTiles();

    if (active.size() < 5) {
        targets = active;
    }
    else {
        while (targets.size() < 5) {
            int targetIndex = rand() % (active.size());
            if (std::find(targets.begin(), targets.end(), targetIndex) == targets.end()) {
                targets.push_back(active[targetIndex]);
            }
        }
    }

    //визуальный эффект
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < targets.size(); i++) {
            tiles[targets[i]].color = sf::Color::White;
        }
        Display();
        Sleep(50);
        for (int i = 0; i < targets.size(); i++) {
            tiles[targets[i]].color = sf::Color::Red;
        }
        Display();
        Sleep(50);
    }

    //уничтожение квадратов
    DestroyTileSeq(targets);

}

//вывод игрового поля на экран
void Board::Display() {
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

//обработка клика по квадрату
void Game::HandleClick(const sf::Vector2i pos) {
    float tileSize = (float)board.window.getSize().x / board.boardSize;
    int col = pos.x / tileSize;
    int row = pos.y / tileSize;
    int index = row * board.boardSize + col;

    //первый клик
    if (selectedTileIndex == -1) {
        selectedTileIndex = index;
    }
    else { //второй клик
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

/* обновление поля
*
* запускает уничтожение комбинаций 3 и более соседних квадратов на поле,
сдвиг квадратов вниз, вывод поля на экран
*/
void Game::Update() {
    board.Display();
    Sleep(200);
    int removed = board.DestroyMatchTiles();
    board.Gravity();
    board.Display();
    Sleep(200);
    while (removed > 0) {
        removed = board.DestroyMatchTiles();
        board.Gravity();
        board.Display();
        Sleep(200);
    }
}

/* проверка условий окончания игры
*
* завершает игру в двух случаях:
*
* победа - на поле не осталось ни одного квадрата
*
* поражение - из оставшихся квадратов невозможно составить комбинацию из 3 и более соседних
* одного цвета
*/
void Game::CheckGameOver() {
    vector<int> active = board.ActiveTiles();
    if (active.size() == 0) { //победа
        sf::Text text("Game Over!\n  You Won!", font, 50);
        text.setPosition({ 260, 300 });
        text.setFillColor(sf::Color::White);
        board.window.draw(text);
        board.window.display();
        Sleep(5000);
        board.window.close();
    }
    else { //проверка условий поражения
        int index, row;
        int tileCountByColor[COLOR_COUNT] = { 0 };
        for (int col = 0; col < board.boardSize; col++) {
            row = board.boardSize - 1;
            index = row * board.boardSize + col;
            while (row >= 0 && board.tiles[index].color != sf::Color::Black) {
                for (int i = 0; i < COLOR_COUNT; i++) {
                    if (board.tiles[index].color == colors[i]) {
                        tileCountByColor[i]++;
                    }
                }
                row--;
                index = row * board.boardSize + col;
            }
            index = (board.boardSize - 1) * board.boardSize + col;
            if (board.tiles[index].color == sf::Color::Black || col == board.boardSize - 1) {
                for (int i = 0; i < COLOR_COUNT; i++) {
                    if (tileCountByColor[i] >= 3) {
                        return;
                    }
                }
                for (int i = 0; i < COLOR_COUNT; i++) {
                    tileCountByColor[i] = 0;
                }
            }
        }
        sf::Text text("Game Over!\n You Lost!", font, 50);
        text.setPosition({ 260, 300 });
        text.setFillColor(sf::Color::White);
        board.window.draw(text);
        board.window.display();
        Sleep(5000);
        board.window.close();
    }
}

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
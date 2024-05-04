#include <iostream>
#include <fstream>
using namespace std;

struct Rule
{
    char orientation;   // 'v' или 'h' для вертикального или горизонтального представления
    int sum;            // Сумма строки
    int startY;         // Начальная координата по y
    int startX;         // Начальная координата по x
    int length;         // Длина строки
    Rule* next;         // Указатель на следующий элемент
};

// Содержит односвязанный список правил для решения головоломки
class Rulebook
{
private:
    int numRules;
    Rule* head;

    void addRule(Rule* r) // Добавляет правило в список
    {
        if (head)
        {
            r->next = head;
        }
        head = r;
        numRules++;
    }

public:
    Rulebook()
    {
        head = nullptr; // Устанавливаем указатель на голову списка в значение nullptr, так как список пуст
        numRules = 0; // Устанавливаем количество правил в списке в 0, так как список пуст
    }

    ~Rulebook()
    {
        init(); // Вызываем функцию init() для очистки списка правил перед уничтожением объекта Rulebook
    }

    void addRule(char orient, int sum, int startY, int startX, int len)   // Принимает правило от клиента и добавляет его в список правил
    {
        Rule* r = new Rule{ orient, sum, startY, startX, len, nullptr }; // Создаем новый объект Rule и инициализируем его значениями, переданными от клиента
        addRule(r); // Вызываем вспомогательную функцию addRule() для добавления правила в список
    }

    void init()    // Очищает список правил
    {
        Rule* p = head, * q = nullptr; // Инициализируем указатель p на голову списка и указатель q в nullptr
        while (p) // Пока p не равен nullptr, выполняем цикл
        {
            q = p; // Присваиваем q значение p, чтобы сохранить ссылку на текущий элемент списка
            p = p->next; // Переходим к следующему элементу списка
            delete q; // Освобождаем память, выделенную для элемента списка
        }
        numRules = 0; // Устанавливаем количество правил в списке в 0, так как список пуст
    }

    void printRules(ostream& out)   // Выводит все правила в поток out
    {
        Rule* r = head; // Устанавливаем указатель r на голову списка
        while (r) // Пока r не равен nullptr, выполняем цикл
        {
            out << r->orientation << ' ' << r->sum << ' '
                << r->startY << ' ' << r->startX
                << ' ' << r->length << endl; // Выводим значения полей объекта Rule в поток out
            r = r->next; // Переходим к следующему элементу списка
        }
    }

    Rule& findRule(char orient, int y, int x)  // Ищет правило с заданной ориентацией и начальными координатами
    {
        Rule* p = head; // Устанавливаем указатель p на голову списка
        if (orient == 'h') // Если ориентация равна 'h'
        {
            while (p && !(p->orientation == orient && x >= p->startX
                && x < p->startX + p->length && y == p->startY)) // Пока p не равен nullptr и не найдено правило с заданными координатами и ориентацией
            {
                p = p->next; // Переходим к следующему элементу списка
            }
        }
        else if (orient == 'v') // Если ориентация равна 'v'
        {
            while (p && !(p->orientation == orient && y >= p->startY
                && y < p->startY + p->length && x == p->startX)) // Пока p не равен nullptr и не найдено правило с заданными координатами и ориентацией
            {
                p = p->next; // Переходим к следующему элементу списка
            }
        }
        return *p; // Возвращаем найденное правило
    }
};

// Вспомогательная функция решения головоломки
int tryToPut(int guess, int y, int x, int board[][10], Rulebook& myRules)
{
    Rule xRule, yRule;
    int curSumX = 0, curSumY = 0, i = 0;
    bool uniqX = true, uniqY = true, filled = true;

    if (x + y > 18)
    {
        return 1; // Если сумма координат превышает 18, возвращаем 1, чтобы указать на нарушение
    }

    if (board[y][x] == -1)
    {
        return x <= 9 ? tryToPut(guess, y, x + 1, board, myRules)
            : tryToPut(guess, y + 1, 0, board, myRules);
    }

    board[y][x] = guess;

    // Находим соответствующие правила для x и y
    xRule = myRules.findRule('h', y, x);
    yRule = myRules.findRule('v', y, x);

    // Проверяем на нарушение или повторение в строке
    for (i = xRule.startX, curSumX = 0, uniqX = true;
        i < xRule.startX + xRule.length; i++)
    {
        curSumX += board[y][i];
        if (guess == board[y][i] && i != x)
        {
            uniqX = false;
        }
    }

    // Если нарушение в x, возвращаем нарушение
    if (curSumX > xRule.sum)
    {
        board[y][x] = 0;
        return 2;
    }

    // Проверяем на нарушение или повторение в столбце
    for (i = yRule.startY, curSumY = 0, uniqY = true;
        i < yRule.startY + yRule.length; i++)
    {
        curSumY += board[i][x];
        if (guess == board[i][x] && i != y)
        {
            uniqY = false;
        }
    }

    // Если нарушение в y, возвращаем нарушение
    if (curSumY > yRule.sum)
    {
        board[y][x] = 0;
        return 2;
    }

    // Если x или y не уникальны, возвращаем не нарушение
    if (!(uniqY && uniqX))
    {
        board[y][x] = 0;
        return 3;
    }

    // Проверяем, заполнена ли строка
    filled = x == xRule.startX + xRule.length - 1;

    // Если строка заполнена, убеждаемся, что правило соблюдено
    if (filled && curSumX != xRule.sum)
    {
        board[y][x] = 0;
        return 3;
    }

    // Проверяем, заполнен ли столбец
    filled = y == yRule.startY + yRule.length - 1;

    // Если столбец заполнен, убеждаемся, что правило соблюдено
    if (filled && curSumY != yRule.sum)
    {
        board[y][x] = 0;
        return 3;
    }

    // Пытаемся поместить {1,2,...9} в следующую доступную ячейку и возвращаем результат
    int returnVal = 0, nextX, nextY;

    // Находим следующую позицию
    if (x + 1 > 9) // Если следующая позиция находится за пределами доски, пробуем следующий элемент
    {
        nextY = y + 1;
        nextX = 0;
    }
    else
    {
        nextY = y;
        nextX = x + 1;
    }

    // Устанавливаем i для цикла while
    i = 1;

    while (i <= 9 && returnVal != 1 && returnVal != 2)
    {
        returnVal = tryToPut(i, nextY, nextX, board, myRules);
        i++;
    }

    if (returnVal != 1)
    {
        board[y][x] = 0;
    }

    return returnVal != 2 ? returnVal : 3;
}

// Функция решения головоломки
bool solve(int board[][10], Rulebook& myRules)
{
    int returnVal = 0;
    int i = 1;

    while (i <= 9 && returnVal != 1 && returnVal != 2)
    {
        returnVal = tryToPut(i, 0, 0, board, myRules);
        i++;
    }

    return returnVal == 1;
}

// Функция вывода поля игры
void printBoard(ostream& out, int board[][10])
{
    for (int i = 0; i < 9; i++)//внешний цикл, проходящий через строки
    {
        for (int j = 0; j < 9; j++)//внутрениий цикл, проходящий через столбцы
        {
            if (board[i][j] == -1)
            {
                out << 'x' << '\t';
            }
            else
            {
                out << board[i][j] << '\t';
            }
        }
        out << endl;
    }
    return;
}

int main()
{
    setlocale(LC_ALL, "rus");
    int board[10][10];  // Для хранения доски
    char orient,        // Для поддержания ориентации
        place;         // Для чтения начального состояния доски
    int numRules, tempX, tempY, sum, len;    // Временные переменные
    Rulebook myRules;  // Содержит список правил головоломки
    ifstream fin("board.txt");    // Попытка открыть входной файл

    if (!fin)
    {
        cout << "Не удалось открыть доску." << endl;
        return -1;
    }

    // Инициализируем доску
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            board[i][j] = -1;
        }
    }

    // Заполняем пространства в соответствии с входным файлом
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            fin >> place;
            board[i][j] = place == 'x' ? -1 : 0;
        }
    }

    // Печатаем начальную доску
    cout << "Начальный вид головоломки:\n" << endl;
    printBoard(cout, board);
    cout << endl;

    // Получаем все правила из входного файла
    fin >> numRules;

    // Передаем данные правил в список правил
    for (int i = 0; i < numRules; i++)
    {
        fin >> orient >> sum >> tempY >> tempX >> len;
        myRules.addRule(orient, sum, tempY, tempX, len);
    }

    // Пытаемся решить головоломку
    if (solve(board, myRules))
    {
        cout << "---------------------------------------------------------------------------" << endl;
        cout << "Решённая головоломка:\n" << endl;
        printBoard(cout, board);
    }
    else
    {
        cout << "Нет решения" << endl;
        return 1;
    }

    return 0;
}
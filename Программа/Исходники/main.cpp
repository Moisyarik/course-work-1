#include <iostream>
#include <fstream>
#include <limits>
#include "game.h"

using namespace std;

// Этот класс перехватывает вывод и отправляет его в два места сразу
class TeeBuf : public std::streambuf {
public:
    TeeBuf(std::streambuf* sb1, std::streambuf* sb2) : sb1(sb1), sb2(sb2) {}
protected:
    virtual int overflow(int c) {
        if (c == EOF) {
            return !EOF;
        }
        else {
            int const r1 = sb1->sputc(c);
            int const r2 = sb2->sputc(c);
            return (r1 == EOF || r2 == EOF) ? EOF : c;
        }
    }

    virtual int sync() {
        int const r1 = sb1->pubsync();
        int const r2 = sb2->pubsync();
        return (r1 == 0 && r2 == 0) ? 0 : -1;
    }

private:
    std::streambuf* sb1;
    std::streambuf* sb2;
};

// Вспомогательный класс, чтобы удобно подключать TeeBuf к потоку
class TeeStream : public std::ostream {
public:
    TeeStream(std::ostream& o1, std::ostream& o2)
        : std::ostream(&tbuf), tbuf(o1.rdbuf(), o2.rdbuf()) {}
private:
    TeeBuf tbuf;
};
// ----------------------------------------------

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    // Файловые потоки
    ifstream in_file;
    ofstream out_file;

    // Сохраняем "родной" буфер экрана, чтобы потом восстановить
    streambuf* cout_backup = cout.rdbuf();
    streambuf* cin_backup = cin.rdbuf();

    TeeBuf* tee_buffer = nullptr;

    // --- ЛОГИКА АРГУМЕНТОВ ---

    // Вариант 1: ./main input.txt output.txt (Полная автоматика)
    if (argc >= 3) {
        in_file.open(argv[1]);
        if (in_file.is_open()) cin.rdbuf(in_file.rdbuf());

        out_file.open(argv[2]);
        if (out_file.is_open()) cout.rdbuf(out_file.rdbuf());

        cout << "Режим: Автоматический (файл -> файл)\n";
    }
    // Вариант 2: ./main log.txt (Играем руками, но пишем лог)
    else if (argc == 2) {
        out_file.open(argv[1]);
        if (!out_file.is_open()) {
            cerr << "Ошибка создания лог-файла!\n";
            return 1;
        }

        tee_buffer = new TeeBuf(cout_backup, out_file.rdbuf());


        cout.rdbuf(tee_buffer);

        cout << "Режим: Интерактивный с записью в " << argv[1] << "\n";
    }
    // Вариант 3: Просто запуск (Только экран)
    else {
        cout << "Режим: Только экран (логи не пишутся)\n";
    }

    // --- ИГРОВОЙ ЦИКЛ---

    KalahGame game;
    bool isPlayerTurn = true;
    bool repeatTurn = false;

    cout << "Добро пожаловать в игру Каллах!\n";
    game.printBoard();

    while (!game.isGameOver()) {
        if (isPlayerTurn) {
            int choice;
            cout << "Ваш ход (введите номер лунки 1-6): ";
            while (true) {
                if (cin >> choice) {
                    choice--;
                    if (choice >= 0 && choice <= 5) {
                        if (game.getBoard()[choice] > 0) break;
                        else cout << "Лунка пуста! Выберите другую: ";
                    }
                    else {
                        cout << "Неверный номер (нужно 1-6): ";
                    }
                }
                else {
                    if (cin.eof()) goto EndGame;
                    cout << "Введите число: ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }

            // Если ввод из файла, дублируем его в вывод
            if (argc >= 3) cout << (choice + 1) << endl;

            repeatTurn = game.makeMove(choice, true);
            cout << "Вы выбрали лунку " << (choice + 1) << ".\n";

        }
        else {
            cout << "Компьютер думает...\n";
            int aiMove = getBestMove(game);
            if (aiMove == -1) break;
            repeatTurn = game.makeMove(aiMove, false);
            cout << "Компьютер выбрал лунку " << (aiMove) << " (по индексу 7-12).\n";
        }

        game.printBoard();

        if (repeatTurn) {
            cout << (isPlayerTurn ? "Бонусный ход! Снова вы.\n" : "Бонусный ход ПК.\n");
        }
        else {
            isPlayerTurn = !isPlayerTurn;
        }
    }

EndGame:
    game.finishGame();
    game.printBoard();
    game.announceWinner();

    // Восстанавливаем потоки и чистим память
    cin.rdbuf(cin_backup);
    cout.rdbuf(cout_backup);
    if (tee_buffer) delete tee_buffer;

    // Пауза перед выходом
    if (argc < 3) {
        cout << "Нажмите Enter, чтобы выйти...";
        cin.ignore(); cin.get();
    }

    return 0;
}
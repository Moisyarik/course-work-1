#ifndef GAME_H
#define GAME_H

#include <vector>

// Глобальные константы правил игры
const int PLAYER_PITS = 6;
const int TOTAL_PITS = 14;
const int P1_KALAH = 6; 
const int P2_KALAH = 13;
const int START_STONES = 4;


const int MAX_DEPTH = 10;
const int INF = 1000000;

// Класс, описывающий состояние игры
class KalahGame {
private:
    std::vector<int> board;

public:
    KalahGame();

    // Конструктор для создания копии состояния
    KalahGame(const std::vector<int>& currentBoard);

    // Получить текущее состояние доски
    std::vector<int> getBoard() const;

    // Отображение доски в консоли
    void printBoard() const;

    // Проверка окончания игры
    bool isGameOver() const;

    // Финализация игры (сбор оставшихся камней)
    void finishGame();

    // Объявление победителя
    void announceWinner() const;

    // Логика хода. Возвращает true, если нужен повторный ход.
    bool makeMove(int pitIndex, bool isPlayerTurn);

    // Получить список доступных ходов
    std::vector<int> getValidMoves(bool isPlayerTurn) const;

    // Эвристическая оценка позиции
    int evaluate() const;
};


int getBestMove(KalahGame game);
int alphaBeta(KalahGame game, int depth, int alpha, int beta, bool maximizingPlayer);

#endif // GAME_H
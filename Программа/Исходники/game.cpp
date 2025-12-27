#include "game.h"
#include <iostream>
#include <iomanip>
#include <numeric>
#include <algorithm>

using namespace std;

// --- Реализация методов класса KalahGame ---

KalahGame::KalahGame() {
    board.resize(TOTAL_PITS, START_STONES);
    board[P1_KALAH] = 0;
    board[P2_KALAH] = 0;
}

KalahGame::KalahGame(const vector<int>& currentBoard) {
    board = currentBoard;
}

vector<int> KalahGame::getBoard() const {
    return board;
}

void KalahGame::printBoard() const {
    cout << "\n-----------------------------------------\n";
    cout << "       [12] [11] [10] [ 9] [ 8] [ 7] (Компьютер)\n";
    cout << "      ";
    for (int i = 12; i >= 7; --i) {
        cout << setw(4) << board[i] << " ";
    }
    cout << "\n";

    cout << " (" << board[P2_KALAH] << ")                              (" << board[P1_KALAH] << ")\n";

    cout << "      ";
    for (int i = 0; i <= 5; ++i) {
        cout << setw(4) << board[i] << " ";
    }
    cout << "\n       [ 1] [ 2] [ 3] [ 4] [ 5] [ 6] (Вы)\n";
    cout << "-----------------------------------------\n";
}

bool KalahGame::isGameOver() const {
    int sumP1 = 0, sumP2 = 0;
    for (int i = 0; i < 6; ++i) sumP1 += board[i];
    for (int i = 7; i < 13; ++i) sumP2 += board[i];
    return (sumP1 == 0 || sumP2 == 0);
}

void KalahGame::finishGame() {
    int sumP1 = 0, sumP2 = 0;
    for (int i = 0; i < 6; ++i) {
        sumP1 += board[i];
        board[i] = 0;
    }
    for (int i = 7; i < 13; ++i) {
        sumP2 += board[i];
        board[i] = 0;
    }
    board[P1_KALAH] += sumP1;
    board[P2_KALAH] += sumP2;
}

void KalahGame::announceWinner() const {
    cout << "\n=== ИГРА ОКОНЧЕНА ===\n";
    cout << "Счет игрока: " << board[P1_KALAH] << "\n";
    cout << "Счет компьютера: " << board[P2_KALAH] << "\n";
    if (board[P1_KALAH] > board[P2_KALAH])
        cout << "Поздравляем! Вы победили!\n";
    else if (board[P1_KALAH] < board[P2_KALAH])
        cout << "Победил компьютер.\n";
    else
        cout << "Ничья!\n";
}

bool KalahGame::makeMove(int pitIndex, bool isPlayerTurn) {
    int stones = board[pitIndex];
    board[pitIndex] = 0;
    int currentIndex = pitIndex;

    while (stones > 0) {
        currentIndex = (currentIndex + 1) % TOTAL_PITS;

        // Пропускаем чужой каллах
        if (isPlayerTurn && currentIndex == P2_KALAH) continue;
        if (!isPlayerTurn && currentIndex == P1_KALAH) continue;

        board[currentIndex]++;
        stones--;
    }

    // 1. Попал в свой каллах - повторный ход
    if (isPlayerTurn && currentIndex == P1_KALAH) return true;
    if (!isPlayerTurn && currentIndex == P2_KALAH) return true;

    // 2. Захват камней
    if (board[currentIndex] == 1) {
        bool isMySide = (isPlayerTurn && currentIndex >= 0 && currentIndex <= 5) ||
            (!isPlayerTurn && currentIndex >= 7 && currentIndex <= 12);

        if (isMySide) {
            int oppositeIndex = 12 - currentIndex;
            if (board[oppositeIndex] > 0) {
                int capture = board[currentIndex] + board[oppositeIndex];
                board[currentIndex] = 0;
                board[oppositeIndex] = 0;

                if (isPlayerTurn)
                    board[P1_KALAH] += capture;
                else
                    board[P2_KALAH] += capture;
            }
        }
    }

    return false;
}

vector<int> KalahGame::getValidMoves(bool isPlayerTurn) const {
    vector<int> moves;
    int start = isPlayerTurn ? 0 : 7;
    int end = isPlayerTurn ? 5 : 12;

    for (int i = start; i <= end; ++i) {
        if (board[i] > 0) {
            moves.push_back(i);
        }
    }
    return moves;
}

int KalahGame::evaluate() const {
    return board[P2_KALAH] - board[P1_KALAH];
}



int getBestMove(KalahGame game) {
    int bestVal = -INF;
    int bestMove = -1;
    vector<int> moves = game.getValidMoves(false);

    for (int move : moves) {
        KalahGame newGame = game;
        bool repeatTurn = newGame.makeMove(move, false);

        int moveVal = alphaBeta(newGame, MAX_DEPTH - 1, -INF, INF, repeatTurn ? true : false);

        if (moveVal > bestVal) {
            bestVal = moveVal;
            bestMove = move;
        }
    }
    return bestMove;
}

int alphaBeta(KalahGame game, int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0 || game.isGameOver()) {
        if (game.isGameOver()) game.finishGame();
        return game.evaluate();
    }

    if (maximizingPlayer) { // Компьютер
        int maxEval = -INF;
        vector<int> moves = game.getValidMoves(false);
        if (moves.empty()) return game.evaluate();

        for (int move : moves) {
            KalahGame newGame = game;
            bool repeatTurn = newGame.makeMove(move, false);

            int eval = alphaBeta(newGame, depth - 1, alpha, beta, repeatTurn ? true : false);

            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    }
    else { // Человек
        int minEval = INF;
        vector<int> moves = game.getValidMoves(true);
        if (moves.empty()) return game.evaluate();

        for (int move : moves) {
            KalahGame newGame = game;
            bool repeatTurn = newGame.makeMove(move, true);

            int eval = alphaBeta(newGame, depth - 1, alpha, beta, repeatTurn ? false : true);

            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}
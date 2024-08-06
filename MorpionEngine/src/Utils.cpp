#include "Utils.h"
#include "Position.h"

namespace MorpionEngine {

    std::vector<int> kindaConvertToBase3(int n) {
        std::vector<int> result;

        if (n == 0) {
            result.push_back(0);
        }

        while (n > 0) {
            result.push_back(n % 3);
            n /= 3;
        }

        // for the function it's used in (Evaluate::Init); can be moved if necessary
        for (size_t i = result.size(); i < 9; i++) {
            result.push_back(0);
        }

        return result;
    }

    U16 countSetBits(U16 n) {
        U16 count = 0;
        while (n) {
            count += n & 1;
            n >>= 1;
        }
        return count;
    }


    HashTable* Utils::globalHashTable;

    void Utils::InitGlobalHashTable() {
        globalHashTable = new HashTable();
        globalHashTable->InitHashTable();
    }

    void Utils::UninitGlobalHashTable() {
        delete globalHashTable;
    }

    std::string Utils::MoveToString(U16 move) {
        return BoardString[BOARD(move)] + (std::string)":" + SquareString[SQUARE(SMOVE(move))];
    }

    Square Utils::StringToSquare(std::string inp) {
        return (Square)((inp[0] - 'A') * 3 + (inp[1] - '1'));
    }

    Boards Utils::StringToBoard(std::string inp) {
        return (Boards)((inp[0] - 'X') * 3 + (inp[1] - '1'));
    }

    U16 Utils::StringToMove(std::string move, Position* pos) {
        Boards board = StringToBoard(move.substr(0, 2));
        Square square = StringToSquare(move.substr(3, 2));
        Piece piece = pos->GetTurn();
        BoardState state = pos->GetBoard(board).GetStateAfterMove(FOLDSMOVE(square, piece, 0));
        Boards afterBoard = pos->GetBoard((Boards)square).GetState() == Playing ? (Boards)square : ANY;
        return FOLDBMOVE(FOLDSMOVE(square, piece, state), board, afterBoard);
    }


}
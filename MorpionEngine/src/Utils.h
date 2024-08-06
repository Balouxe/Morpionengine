#pragma once
#include "Types.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace MorpionEngine {

    class HashTable;
    class Position;

	constexpr const char* SquareString[9] = { "A1", "A2", "A3", "B1", "B2", "B3", "C1", "C2", "C3" };
	constexpr const char* BoardString[11] = { "X1", "X2", "X3", "Y1", "Y2", "Y3", "Z1", "Z2", "Z3", "Any" , "None"};
	constexpr int InverseTurn[2] = { 1, 0 };
    constexpr U16 FileBrd[3] = { 73, 146, 292 };
    constexpr U16 RankBrd[3] = { 7, 56, 448 };
    constexpr U16 DiagonalBrd[2] = { 273, 84 };
    constexpr U16 FullBrd = 511;
    constexpr U16 ThreatsBrd[24] = { 9, 72, 18, 144, 36, 288, 65, 130, 260, 3, 6, 24, 48, 192, 384, 5, 40, 320, 17, 272, 257, 80, 20, 68 };
    constexpr U16 ThreatsSquareBrd[24] = { 64, 1, 128, 2, 256, 4, 8, 16, 32, 4, 1, 32, 8, 256, 64, 2, 16, 128, 256, 1, 16, 4, 64, 16 };

    U16 countSetBits(U16 n);

    std::vector<int> kindaConvertToBase3(int n);

    class Utils {
    public:
        static void InitGlobalHashTable();
        static void UninitGlobalHashTable();

        static std::string MoveToString(U16 move);
        static Square StringToSquare(std::string inp);
        static Boards StringToBoard(std::string inp);
        static U16 StringToMove(std::string move, Position* pos);

        static HashTable* globalHashTable;
    };

}
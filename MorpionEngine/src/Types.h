#pragma once
#include <iostream>
#include <assert.h>
#include <inttypes.h>

#define NUMTHREADS 1

#define SPOS_LENGTH 9
#define MAXSMALLPOS 19683
#define MAXDEPTH 128
#define AB_BOUND 999999

#define BIGEVAL 30000
#define MATE 10000
#define BIGONEAWAY 200
#define BIGCENTER 100
#define SMALLWIN 100
#define SMALLONEAWAY 20
#define SMALLCENTER 5

#define SCOREPV 4
#define SCOREANYBOARD -4
#define SCORESMALLWIN 10

#define RAND_64 ( \
	(U64)rand() | \
	(U64)rand() << 15 | \
	(U64)rand() << 30 | \
	(U64)rand() << 45 | \
	((U64)rand() & 0xf) << 60)

typedef unsigned long long U64;
typedef unsigned short U16;

namespace MorpionEngine {

	enum Piece {
		Circle,
		Cross,
		Both,
		PC_NONE
	};

	enum Boards : int {
		X1, X2, X3,
		Y1, Y2, Y3,
		Z1, Z2, Z3,
		ANY,
		BRD_NONE
	};

	enum Square : int {
		A1, A2, A3,
		B1, B2, B3,
		C1, C2, C3,
		SQ_NONE
	};

	enum BoardState : int {
		Playing,
		CircleW,
		CrossW,
		Draw
	};

	enum { HFNONE, HFALPHA, HFBETA, HFEXACT };


#define FOLDSMOVE(sq, pc, st) ( (sq) | ((pc) << 4) | ( (st) << 5 ))
#define SQUARE(m) (Square)((m) & 0xF)
#define PIECE(m) (Piece)(((m) >> 4) & 0x1)
#define STATEAFTER(m) (BoardState)(((m) >> 5) & 0x3)

#define FOLDBMOVE(smove, brd, afterbrd) ( (smove) | ((brd) << 7) | ((afterbrd) << 11) )
#define SMOVE(m) (char)((m) & 0x7F)
#define BOARD(m) (Boards)(((m) >> 7) & 0xF)
#define AFTERBOARD(m) (Boards)(((m) >> 11) & 0xF)

	constexpr char SMOVENULL = -1;
	constexpr char BMOVENULL = -1;

	// SMOVE is 7 bits long
	// BMove is 15 bits long, for practicality it's 16
	// HashEntry is 42 bits long, for practicality it's 64 because nobody likes 3 bits

#define FOLDHASHENTRY(mv, sc, dp, fl) ((sc + BIGEVAL) | ((dp) << 16) | ((fl) << 24) | ((U64)(mv) << 26))
#define EXTRACTSCORE(hash) (int)(((hash) & 0xFFFF) - BIGEVAL)
#define EXTRACTDEPTH(hash) (int)((hash >> 16) & 0xFF)
#define EXTRACTFLAGS(hash) ((hash >> 24) & 0x3)
#define EXTRACTMOVE(hash) ((U16)(hash >> 26))

	struct HashEntry {
		U64 posKey;
		U64 data;
	};

	struct HashTableContainer {
		HashEntry* pTable;
		int newWrite;
		int hit;
		int cut;
		int overWrite;
		int currentAge;
	};

	struct SMoveList {
		// not ideal way to do this but it'll do for now
		char moves[9];
		int count = 0;
	};

	struct BMoveList {
		U16 moves[81];
		int scores[81];
		int count = 0;
	};

	struct SearchInfo {
		bool stop = false;
		bool timeset = false;
		int depth = MAXDEPTH;
		int nodes = 0;
		int threadCount = 1;
	};
	
#define CLRBIT64(bb, sq) ((bb.Get()) &= ClearMask[(sq)])
#define SETBIT64(bb, sq) ((bb.Get()) |= SetMask[(sq)])

}
#pragma once
#include "Bitboard.h"
#include "Types.h"

#include <string>

namespace MorpionEngine {

	class BigMoveGen;
	class HashTable;

	class SingleBoard {
	public:
		SingleBoard();
		~SingleBoard() = default;

		void ResetBoard();
		void SetPosition(std::string pos);

		void MakeMove(char move);
		void TakeMove(char move);

		void CalculateState();
		void SetState(BoardState state);

		BoardState GetStateAfterMove(char move);
		int GetEvalDifferenceAfterMove(char move);

		inline BoardState GetState() { return m_state; }

		void PrintPosition();

	private:
		// This coding style really is beginning to show its limits

		friend class BigMoveGen;
		friend class Position;
		friend class Evaluate;
		friend class Hash;

		Bitboard16 m_pieces[3];
		BoardState m_state;

		U64 m_hashKey;
	};

	class Position {
	public:
		Position();
		Position(const Position& rhs);
		~Position() = default;

		void ResetBoard();
		void ClearForSearch();

		void MakeMove(U16 move);
		void TakeMove(U16 move);

		void PrintPosition();

		void UpdateStates();
		void CalculateState();

		inline Piece GetTurn() const { return m_turn; }
		inline SingleBoard& GetBoard(Boards board) { return m_boards[board]; }

	private:
		friend class BigMoveGen;
		friend class Evaluate;
		friend class Search;
		friend class HashTable;
		friend class Hash;

		SingleBoard m_boards[9];
		Piece m_turn;
		Boards m_currBoard;

		Bitboard16 m_states[3];
		BoardState m_state;

		int m_ply;
		int m_hisPly;

		U64 m_hashKey;
		U16 PvArray[MAXDEPTH];
	};

	class BigMoveGen {
	public:
		BigMoveGen(Position* board);
		~BigMoveGen() = default;

		void GenerateMoves();
		void GenerateQuiescence();
		void PrintMoveList();

		bool MoveExists(U16 move);

		void ScoreMoveList(short PvMove);
		void SortMoveList();

		inline BMoveList* GetMoveList() { return &m_moveList; }

	private:
		BMoveList m_moveList;
		Position* m_board;
	};

	class Hash {
	public:
		
		static void InitHashing();

		static U64 HashSingleBoard(SingleBoard* pos);
		static U64 HashPosition(Position* pos);

	private:
		static U64 m_PieceKeys[2][9];
		static U64 m_SideKey;
		static U64 m_CurrBoardKeys[10];

	};

	class HashTable {
	public:
		HashTable();

		void InitHashTable();
		void ClearHashTable();

		int ProbeHashEntry(Position* board, U16* move, int* score, int alpha, int beta, int depth);

		void StoreHashEntry(Position* board, U16 move, int score, int flags, int depth);

		int GetPvLine(Position* board, const int depth);
		U16 ProbePvMove(Position* board);

	public:
		HashTableContainer table;
		int numEntries;
	};

}
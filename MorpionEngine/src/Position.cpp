#include "Position.h"

#include "Search.h"
#include "Utils.h"

#include <algorithm>

namespace MorpionEngine {

	SingleBoard::SingleBoard() {
		ResetBoard();
	}

	void SingleBoard::ResetBoard() {
		m_pieces[0] = 0ULL;
		m_pieces[1] = 0ULL;
		m_state = Playing;

		m_hashKey = Hash::HashSingleBoard(this);
	}

	void SingleBoard::SetState(BoardState state) {
		m_state = state;
	}

	// Left to right, top to bottom. For example: xox-xo-oxo means :
	// X O X
	//   X O
	// O X O

	void SingleBoard::SetPosition(std::string pos) {
		assert(pos.size() == SPOS_LENGTH);

		for (int i = 0; i < SPOS_LENGTH; i++) {
			if (pos[i] == 'x') {
				m_pieces[Cross].SetBit(i);
				m_pieces[Both].SetBit(i);
			}
			else if (pos[i] == 'o') {
				m_pieces[Cross].SetBit(i);
				m_pieces[Both].SetBit(i);
			}
			else if (pos[i] == '-') {
				m_pieces[Cross].ClearBit(i);
				m_pieces[Circle].SetBit(i);
				m_pieces[Both].ClearBit(i);
			}
			else {
				assert(false);
			}
		}

		m_hashKey = Hash::HashSingleBoard(this);
	}

	void SingleBoard::MakeMove(char move) {
		assert(PIECE(move) != PC_NONE);
		assert(SQUARE(move) != SQ_NONE);
		assert(m_pieces[Both].GetBit(SQUARE(move)) == 0);
		assert(m_state == Playing);
		
		m_pieces[PIECE(move)].SetBit(SQUARE(move));
		m_pieces[Both].SetBit(SQUARE(move));

		//CalculateState();
		m_state = STATEAFTER(move);

		m_hashKey = Hash::HashSingleBoard(this);
	}

	void SingleBoard::TakeMove(char move) {
		assert(PIECE(move) != PC_NONE);
		assert(SQUARE(move) != SQ_NONE);
		assert(m_pieces[Both].GetBit(SQUARE(move)) != 0);

		m_pieces[PIECE(move)].ClearBit(SQUARE(move));
		m_pieces[Both].ClearBit(SQUARE(move));
		
		CalculateState();

		m_hashKey = Hash::HashSingleBoard(this);
	}

	void SingleBoard::PrintPosition() {
		U64 shiftMe = 1ULL;

		int rank = 0;
		int file = 0;
		int square = 0;

		std::cout << "\n";
		for (rank = 0; rank <= 2; ++rank) {
			for (file = 0; file <= 2; ++file) {

				square = rank * 3 + file;

				if ((shiftMe << square) & m_pieces[Circle].Get()) {
					std::cout << "O";
				}
				else if ((shiftMe << square) & m_pieces[Cross].Get()) {
					std::cout << "X";
				}
				else {
					std::cout << "-";
				}
			}
			std::cout << "\n";
		}
		std::cout << "\n";
	}

	void SingleBoard::CalculateState() {
		int i, pieceType;

		for (pieceType = 0; pieceType < 2; pieceType++) {
			for (i = 0; i < 3; i++) {
				if ((m_pieces[pieceType].Get() & RankBrd[i]) == RankBrd[i] || (m_pieces[pieceType].Get() & FileBrd[i]) == FileBrd[i]) {
					m_state = BoardState(CircleW + pieceType);
					return;
				}
			}

				if ((m_pieces[pieceType].Get() & DiagonalBrd[0]) == DiagonalBrd[0]) {
					m_state = BoardState(CircleW + pieceType);
					return;
				}

				if ((m_pieces[pieceType].Get() & DiagonalBrd[1]) == DiagonalBrd[1]) {
					m_state = BoardState(CircleW + pieceType);
					return;
				}
		}

		if ((m_pieces[Both].Get() & FullBrd) == FullBrd) {
			m_state = BoardState::Draw;
			return;
		}

		m_state = BoardState::Playing;
		return;
	}

	BoardState SingleBoard::GetStateAfterMove(char move) {
		BoardState temp = m_state;
		m_pieces[PIECE(move)].SetBit(SQUARE(move));
		m_pieces[Both].SetBit(SQUARE(move));
		CalculateState();
		BoardState ret = m_state;
		m_pieces[PIECE(move)].ClearBit(SQUARE(move));
		m_pieces[Both].ClearBit(SQUARE(move));
		m_state = temp;
		return ret;
	}

	int SingleBoard::GetEvalDifferenceAfterMove(char move) {
		int eval = Evaluate::EvaluateSingleBoard(this);
		MakeMove(move);
		eval = abs(Evaluate::EvaluateSingleBoard(this) - eval);
		TakeMove(move);
		return eval;
	}

	Position::Position() {
		ResetBoard();
	}

	Position::Position(const Position& rhs) {
		for (int i = 0; i < 9; i++) {
			m_boards[i] = rhs.m_boards[i];
		}
		m_turn = rhs.m_turn;
		m_currBoard = rhs.m_currBoard;
		for (int i = 0; i < 3; i++) {
			m_states[i] = rhs.m_states[i];
		}
		m_state = rhs.m_state;

		m_ply = rhs.m_ply;
		m_hisPly = rhs.m_hisPly;

		m_hashKey = rhs.m_hashKey;
		for (int i = 0; i < MAXDEPTH; i++) {
			PvArray[i] = rhs.PvArray[i];
		}
	}

	void Position::ResetBoard() {
		for (int i = 0; i < 9; i++) {
			m_boards[i].ResetBoard();
		}
		for (int i = 0; i < MAXDEPTH; i++) {
			PvArray[i] = SMOVENULL;
		}
		m_currBoard = Y2;
		m_turn = Circle;
		m_ply = 0;
		m_hisPly = 0;
		m_states[0] = 0;
		m_states[1] = 0;
		m_states[2] = 0;
		m_hashKey = Hash::HashPosition(this);
		m_state = Playing;
	}


	void Position::MakeMove(U16 move) {
		assert(PIECE(SMOVE(move)) != PC_NONE);
		assert(SQUARE(SMOVE(move)) != SQ_NONE);
		assert(m_boards[BOARD(move)].m_pieces[Both].GetBit(SQUARE(SMOVE(move))) == 0);
		assert(m_boards[BOARD(move)].m_state == Playing);

		m_boards[BOARD(move)].MakeMove(SMOVE(move));
		m_currBoard = AFTERBOARD(move);
		m_turn = (Piece)InverseTurn[m_turn];
		m_ply++;
		m_hisPly++;

		UpdateStates(); // TODO : Only update what's necessary when necessary
		CalculateState();
		m_hashKey = Hash::HashPosition(this); // same
	}

	void Position::TakeMove(U16 move) {
		assert(PIECE(SMOVE(move)) != PC_NONE);
		assert(SQUARE(SMOVE(move)) != SQ_NONE);
		assert(m_boards[BOARD(move)].m_pieces[Both].GetBit(SQUARE(SMOVE(move))) != 0);

		m_currBoard = BOARD(move);
		m_boards[BOARD(move)].TakeMove(SMOVE(move));
		m_turn = (Piece)InverseTurn[m_turn];
		m_ply--;
		m_hisPly--;

		UpdateStates();
		CalculateState();
		m_hashKey = Hash::HashPosition(this);
	}

	void Position::PrintPosition() {
		for (int i = 0; i < 9; i++) {
			std::cout << BoardString[i] << " : \n";
			m_boards[i].PrintPosition();
		}
		std::cout << std::endl << "Current board : " << BoardString[m_currBoard];
	}

	void Position::UpdateStates() {
		BoardState state;
		for (int i = 0; i < 9; i++) {
			state = m_boards[i].GetState();
			if (state == Playing) {
				m_states[0].ClearBit(i);
				m_states[1].ClearBit(i);
				m_states[2].ClearBit(i);
			}
			else {
				m_states[state - 1].SetBit(i);
			}
		}
	}

	void Position::ClearForSearch() {
		for (int i = 0; i < MAXDEPTH; i++) {
			PvArray[i] = BMOVENULL;
		}
		m_ply = 0;
		m_hisPly = 0;
	}

	void Position::CalculateState() {
		int i, pieceType;

		for (pieceType = 0; pieceType < 2; pieceType++) {
			for (i = 0; i < 3; i++) {
				if ((m_states[pieceType].Get() & RankBrd[i]) == RankBrd[i] || (m_states[pieceType].Get() & FileBrd[i]) == FileBrd[i]) {
					m_state = BoardState(CircleW + pieceType);
					return;
				}
			}

			if ((m_states[pieceType].Get() & DiagonalBrd[0]) == DiagonalBrd[0]) {
				m_state = BoardState(CircleW + pieceType);
				return;
			}

			if ((m_states[pieceType].Get() & DiagonalBrd[1]) == DiagonalBrd[1]) {
				m_state = BoardState(CircleW + pieceType);
				return;
			}
		}

		if ((m_states[Both].Get() & FullBrd) == FullBrd) {
			m_state = BoardState::Draw;
			return;
		}

		m_state = BoardState::Playing;
		return;
	}

	BigMoveGen::BigMoveGen(Position* board) {
		m_board = board;
		m_moveList.count = 0;
	}

	void BigMoveGen::GenerateQuiescence() {
		m_moveList.count = 0;
		if (m_board->m_state != Playing) {
			return;
		}

		int square, piece, board, afterboard, state;
		char smove;

		if (m_board->m_currBoard != ANY && m_board->m_boards[m_board->m_currBoard].GetState() == Playing) {
			SingleBoard& currBoard = m_board->m_boards[m_board->m_currBoard];
			for (int i = 0; i < 9; i++) {
				if (currBoard.m_pieces[Both].GetBit(i) == 0) {
					square = Square(i);
					piece = m_board->m_turn;
					smove = FOLDSMOVE(square, piece, 0);
					state = currBoard.GetStateAfterMove(smove);
					if (state == Playing) {
						continue;
					}
					smove = FOLDSMOVE(square, piece, state);

					board = m_board->m_currBoard;
					if (m_board->m_boards[i].GetState() == BoardState::Playing) {
						afterboard = Boards(i);
					}
					else {
						afterboard = ANY;
					}


					m_moveList.moves[m_moveList.count] = FOLDBMOVE(smove, board, afterboard);
					m_moveList.count++;
				}
			}

		}
		else {
			for (int j = 0; j < 9; j++) {
				SingleBoard& currBoard = m_board->m_boards[j];
				if (currBoard.GetState() != Playing) {
					continue;
				}
				for (int i = 0; i < 9; i++) {
					if (currBoard.m_pieces[Both].GetBit(i) == 0) {
						square = Square(i);
						piece = m_board->m_turn;
						smove = FOLDSMOVE(square, piece, 0);
						state = currBoard.GetStateAfterMove(smove);
						if (state == Playing) {
							continue;
						}
						smove = FOLDSMOVE(square, piece, state);
						board = j;
						if (m_board->m_boards[i].GetState() == BoardState::Playing) {
							afterboard = Boards(i);
						}
						else {
							afterboard = ANY;
						}

						m_moveList.moves[m_moveList.count] = FOLDBMOVE(smove, board, afterboard);
						m_moveList.count++;
					}
				}
			}
		}
	}

	void BigMoveGen::GenerateMoves() {
		m_moveList.count = 0;
		if (m_board->m_state != Playing) {
			return;
		}

		int square, piece, board, afterboard, state;
		char smove;

		if (m_board->m_currBoard != ANY && m_board->m_boards[m_board->m_currBoard].GetState() == Playing) {
			SingleBoard& currBoard = m_board->m_boards[m_board->m_currBoard];
			for (int i = 0; i < 9; i++) {
				if (currBoard.m_pieces[Both].GetBit(i) == 0) {
					square = Square(i);
					piece = m_board->m_turn;
					board = m_board->m_currBoard;
					if (m_board->m_boards[i].GetState() == BoardState::Playing) {
						afterboard = Boards(i);
					}
					else {
						afterboard = ANY;
					}

					smove = FOLDSMOVE(square, piece, 0);
					state = currBoard.GetStateAfterMove(smove);
					smove = FOLDSMOVE(square, piece, state);
					m_moveList.moves[m_moveList.count] = FOLDBMOVE(smove, board, afterboard);
					m_moveList.count++;
				}
			}

		}
		else {
			for (int j = 0; j < 9; j++) {
				SingleBoard& currBoard = m_board->m_boards[j];
				if (currBoard.GetState() != Playing) {
					continue;
				}
				for (int i = 0; i < 9; i++) {
					if (currBoard.m_pieces[Both].GetBit(i) == 0) {
						square = Square(i);
						piece = m_board->m_turn;
						board = j;
						if (m_board->m_boards[i].GetState() == BoardState::Playing) {
							afterboard = Boards(i);
						}
						else {
							afterboard = ANY;
						}

						smove = FOLDSMOVE(square, piece, 0);
						state = currBoard.GetStateAfterMove(smove);
						smove = FOLDSMOVE(square, piece, state);
						m_moveList.moves[m_moveList.count] = FOLDBMOVE(smove, board, afterboard);
						m_moveList.count++;
					}
				}
			}
		}
	}

	bool BigMoveGen::MoveExists(U16 move){
		GenerateMoves();

		int moveNum;
		for (moveNum = 0; moveNum < m_moveList.count; ++moveNum) {
			if (m_moveList.moves[moveNum] == move) {
				return true;
			}
		}

		return false;
	}

	void BigMoveGen::PrintMoveList() {
		for (int i = 0; i < m_moveList.count; i++) {
			std::cout << "Move " << i << ": " << SquareString[int(SQUARE(SMOVE(m_moveList.moves[i])))] << " on board " << BoardString[int(BOARD(m_moveList.moves[i]))] << std::endl;
		}
	}

	void BigMoveGen::ScoreMoveList(short PvMove){
		int score;
		for (int i = 0; i < m_moveList.count; i++) {
			score = 0;
			if (STATEAFTER(m_moveList.moves[i]) == CircleW || STATEAFTER(m_moveList.moves[i]) == CrossW) {
				score += SCORESMALLWIN;
			}

			if (AFTERBOARD(m_moveList.moves[i]) == ANY) {
				score += SCOREANYBOARD;
			}

			if (m_moveList.moves[i] == PvMove) {
				score += SCOREPV;
			}

			m_moveList.scores[i] = score;
		}
	}

	void BigMoveGen::SortMoveList() {
		// Bubble sort lol I'm too lazy to do anything else
		int tempScore;
		U16 tempMove;
		for (int i = 0; i < m_moveList.count; i++) {
			for (int j = i + 1; j < m_moveList.count; j++) {
				if (m_moveList.scores[j] > m_moveList.scores[i]) {
					tempScore = m_moveList.scores[i];
					tempMove = m_moveList.moves[i];

					m_moveList.scores[i] = m_moveList.scores[j];
					m_moveList.scores[j] = tempScore;

					m_moveList.moves[i] = m_moveList.moves[j];
					m_moveList.moves[j] = tempMove;

				}
			}
		}


	}

	U64 Hash::m_PieceKeys[2][9];
	U64 Hash::m_SideKey;
	U64 Hash::m_CurrBoardKeys[10];

	constexpr int HashSize = 0x100000 * 64;

	void Hash::InitHashing() {
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 9; j++) {
				m_PieceKeys[i][j] = RAND_64;
			}
		}
		for (int j = 0; j < 10; j++) {
			m_CurrBoardKeys[j] = RAND_64;
		}
		m_SideKey = RAND_64;
	}

	U64 Hash::HashSingleBoard(SingleBoard* pos) {
		U64 result = 0ULL;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 9; j++) {
				if (pos->m_pieces[i].GetBit(j)) {
					result ^= m_PieceKeys[i][j];
				}
			}
		}
		return result;
	}

	U64 Hash::HashPosition(Position* pos) {
		U64 result = 0ULL;
		for (int i= 0; i < 9; i++) {
			result ^= pos->m_boards[i].m_hashKey;
		}
		if (pos->m_turn == 0) {
			result ^= m_SideKey;
		}
		result ^= m_CurrBoardKeys[pos->m_currBoard];
		return result;
	}

	HashTable::HashTable() {
		table = HashTableContainer();
	}

	void HashTable::InitHashTable() {
		numEntries = HashSize / sizeof(HashEntry);
		numEntries -= 2;
		free(table.pTable);
		table.pTable = (HashEntry*)malloc(numEntries * sizeof(HashEntry));
		ClearHashTable();
	}

	void HashTable::ClearHashTable() {
		HashEntry* entry;

		for (entry = table.pTable; entry < table.pTable + numEntries; entry++) {
			entry->posKey = 0ULL;
			entry->data = 0ULL;
		}

		table.currentAge = 0;
		table.newWrite = 0;
		table.overWrite = 0;
	}

	void HashTable::StoreHashEntry(Position* board, const U16 move, int score, int flags, int depth) {
		int i = board->m_hashKey % numEntries;
		assert(i >= 0 && i <= numEntries - 1);

		bool replace = false;

		if (table.pTable[i].posKey == 0ULL) {
			table.newWrite++;
			replace = true;
		}
		else {
			if (EXTRACTDEPTH(table.pTable[i].data) <= depth) {
				replace = true;
			}
		}

		if (replace == false) return;

		if (score > MATE) score += board->m_ply;
		else if (score < -MATE) score -= board->m_ply;

		table.pTable[i].posKey = board->m_hashKey;
		table.pTable[i].data = FOLDHASHENTRY(move, score, depth, flags);
		/*
		table.pTable[i].move = move;
		table.pTable[i].depth = depth;
		table.pTable[i].flags = flags;
		table.pTable[i].score = score;*/
	}

	int HashTable::ProbeHashEntry(Position* board, U16* move, int* score, int alpha, int beta, int depth) {
		int i = board->m_hashKey % numEntries;
		assert(i >= 0 && i <= numEntries - 1);

		if (table.pTable[i].posKey == board->m_hashKey) {

			*move = EXTRACTMOVE(table.pTable[i].data);

			if (EXTRACTDEPTH(table.pTable[i].data) >= depth) {

				table.hit++;

				*score = EXTRACTSCORE(table.pTable[i].data);

				if (*score > MATE) *score -= board->m_ply;
				else if (*score < -MATE) *score += board->m_ply;

				switch (EXTRACTFLAGS(table.pTable[i].data)) {
				case HFALPHA:
					if (*score <= alpha) {
						*score = alpha;
						return true;
					}
					break;
				case HFBETA:
					if (*score >= beta) {
						*score = beta;
						return true;
					}
					break;
				case HFEXACT:
					return true;
					break;
				default:
					assert(false);
					break;
				}
			}
		}

		return false;
	}

	int HashTable::GetPvLine(Position* board, const int depth) {
		assert(depth < MAXDEPTH);

		U16 move = ProbePvMove(board);
		int count = 0;

		BigMoveGen moveGen(board);

		while (move != BMOVENULL && count < depth) {
			assert(count < MAXDEPTH);

			if (moveGen.MoveExists(move)) {
				board->MakeMove(move);
				board->PvArray[count++] = move;
			}
			else {
				break;
			}
			move = ProbePvMove(board);
		}

		int returnCount = count;

		while (board->m_ply > 0) {
			board->TakeMove(board->PvArray[--count]);
		}

		return returnCount;
	}

	U16 HashTable::ProbePvMove(Position* board) {

		int index = board->m_hashKey % Utils::globalHashTable->numEntries;

		assert(index >= 0 && index <= Utils::globalHashTable->numEntries - 1);

		if (Utils::globalHashTable->table.pTable[index].posKey == board->m_hashKey) {
			return EXTRACTMOVE(Utils::globalHashTable->table.pTable[index].data);
		}

		return BMOVENULL;
	}

}
#include "Search.h"
#include "Position.h"
#include "Utils.h"

#include <vector>

namespace MorpionEngine {

	std::unordered_map<U64, int> Evaluate::m_smallEvals;

	void Evaluate::Init() {

		std::vector<int> base3;
		SingleBoard board;
		int eval;
		U64 key;

		for (int i = 0; i < MAXSMALLPOS; i++) {
			eval = 0;
			board.ResetBoard();

			base3 = kindaConvertToBase3(i);

			for (int j = 0; j < 9; j++) {
				switch (base3[j]) {
					case(0):
						break;
					case(1):
						board.m_pieces[Circle].SetBit(j);
						board.m_pieces[Both].SetBit(j);
						break;
					case(2):
						board.m_pieces[Cross].SetBit(j);
						board.m_pieces[Both].SetBit(j);
						break;
					default:
						std::cout << "wtf big erreur dans Evaluate::Init";

				}
			}

			key = Hash::HashSingleBoard(&board);
			board.CalculateState();

			if (board.GetState() == BoardState::Draw) {
				m_smallEvals[key] = 0;
				continue;
			}
			else if (board.GetState() == BoardState::CircleW) {
				m_smallEvals[key] = SMALLWIN;
				continue;
			}
			else if (board.GetState() == BoardState::CrossW) {
				m_smallEvals[key] = -SMALLWIN;
				continue;
			}

			// Circle

			for (int i = 0; i < 24; i++) {
				if (((board.m_pieces[Circle].Get() & ThreatsBrd[i]) == ThreatsBrd[i]) && !(board.m_pieces[Cross].Get() & ThreatsSquareBrd[i])) {
					eval += SMALLONEAWAY;
				}
			}

			if (board.m_pieces[Circle].Get() & Bitboard16::SetMask[B2]) {
				eval += SMALLCENTER;
			}

			eval += board.m_pieces[Circle].CountBits();

			// Cross

			for (int i = 0; i < 24; i++) {
				if (((board.m_pieces[Cross].Get() & ThreatsBrd[i]) == ThreatsBrd[i]) && !(board.m_pieces[Circle].Get() & ThreatsSquareBrd[i])) {
					eval -= SMALLONEAWAY;
				}
			}

			if (board.m_pieces[Cross].Get() & Bitboard16::SetMask[B2]) {
				eval -= SMALLCENTER;
			}

			eval -= board.m_pieces[Cross].CountBits();

			m_smallEvals[key] = eval;

		}

	}

	void Evaluate::Uninit() {
		// ...
	}

	int Evaluate::EvaluatePosition(Position* pos) {
		int eval = 0;
		int i;

		// Circle
		for (i = 0; i < 24; i++) {
			if (((pos->m_states[Circle].Get() & ThreatsBrd[i]) == ThreatsBrd[i]) && !(pos->m_states[Cross].Get() & ThreatsSquareBrd[i]) && !(pos->m_states[2].Get() & ThreatsSquareBrd[i])) {
				eval += BIGONEAWAY;
			}
		}

		if ((pos->m_states[Circle].GetBit(B2))) {
			eval += BIGCENTER;
		}

		// Cross
		for (i = 0; i < 24; i++) {
			if (((pos->m_states[Cross].Get() & ThreatsBrd[i]) == ThreatsBrd[i]) && !(pos->m_states[Circle].Get() & ThreatsSquareBrd[i]) && !(pos->m_states[2].Get() & ThreatsSquareBrd[i])) {
				eval -= BIGONEAWAY;
			}
		}

		if ((pos->m_states[Cross].GetBit(B2))) {
			eval -= BIGCENTER;
		}

		for (i = 0; i < 9; i++) {
			eval += EvaluateSingleBoard(&pos->m_boards[i]);
		}

		if (pos->m_turn == Circle) {
			return eval;
		}
		else {
			return -eval;
		}


	}

	Search::Search(Position* position) {
		pos = position;
	}

	void Search::ClearForSearch() {
		if (m_searchInfo) {
			m_searchInfo->depth = 0;
			m_searchInfo->nodes = 0;
			m_searchInfo->stop = false;
		}

	}

	void Search::CheckUp() {
		/*if (m_searchInfo->timeset == true && GetTimeMs() > info->stoptime) {
			info->stop = true;
		}

		// ReadInput(info); */

	}

	void Search::IterativeDeepen(int threadNum) {

		int bestScore = -AB_BOUND;
		U16 bestMove = BMOVENULL;
		int currentDepth = 0;
		int pvNum = 0;
		int pvMoves = 0;

		for (currentDepth = 1; currentDepth <= m_searchInfo->depth; ++currentDepth) {

			bestScore = AlphaBeta(-AB_BOUND, AB_BOUND, currentDepth);

			pvMoves = Utils::globalHashTable->GetPvLine(pos, currentDepth);
			bestMove = pos->PvArray[0];

			if (threadNum == 0) {
				printf("info score cp %d depth %d nodes %d", bestScore, currentDepth, m_searchInfo->nodes);

				printf(" pv");
				for (pvNum = 0; pvNum < pvMoves; ++pvNum) {
					std::cout << " " << Utils::MoveToString(pos->PvArray[pvNum]);
				}
				printf("\n");
			}
		}
	}

	int Search::Quiescence(int alpha, int beta) {
		// This version does not improve the engine, so currently unused

		assert(m_board->CheckBoard());

		if ((m_searchInfo->nodes & 2047) == 0) {
			CheckUp();
		}

		m_searchInfo->nodes++;

		if (pos->m_ply > MAXDEPTH - 1) [[unlikely]] {
			return Evaluate::EvaluatePosition(pos);
		}

		int Score = Evaluate::EvaluatePosition(pos);
		int StaticScore = Score;

		if (Score >= beta) {
			return beta;
		}

		if (Score > alpha) {
			alpha = Score;
		}

		BigMoveGen moveGen(pos);
		moveGen.GenerateQuiescence();

		int moveNum = 0;
		int legal = 0;
		int OldAlpha = alpha;
		U16 BestMove = BMOVENULL;
		Score = -AB_BOUND;
		U16 PvMove = BMOVENULL;

		if (Utils::globalHashTable->ProbeHashEntry(pos, &PvMove, &Score, alpha, beta, 0)) {
			Utils::globalHashTable->table.cut++;
			return Score;
		}

		for (moveNum = 0; moveNum < moveGen.GetMoveList()->count; ++moveNum) {

			pos->MakeMove(moveGen.GetMoveList()->moves[moveNum]);

			legal++;
			Score = -Quiescence(-beta, -alpha);
			pos->TakeMove(moveGen.GetMoveList()->moves[moveNum]);

			if (m_searchInfo->stop == true) {
				return 0;
			}

			if (Score > alpha) {
				if (Score >= beta) {
					return beta;
				}
				alpha = Score;
				BestMove = moveGen.GetMoveList()->moves[moveNum];
			}

		}

		return alpha;
	}

	int Search::AlphaBeta(int alpha, int beta, int depth) {
		m_searchInfo->nodes++;

		if (pos->m_state != Playing) {
			if (pos->m_state == Draw) {
				return 0;
			}
			else if (pos->m_state == CircleW) {
				return pos->m_turn == Circle ? -MATE + pos->m_ply : MATE - pos->m_ply;
			}
			else if (pos->m_state == CrossW) {
				return pos->m_turn == Cross ? -MATE + pos->m_ply : MATE - pos->m_ply;
			}
		}

		if (depth <= 0) {
			//return Quiescence(alpha, beta);
			return Evaluate::EvaluatePosition(pos);
		}

		
		if ((m_searchInfo->nodes & 2047) == 0) {
			CheckUp();
		}

		if (pos->m_ply >= MAXDEPTH) [[unlikely]] {
			return Evaluate::EvaluatePosition(pos);
			}

		int Score = -AB_BOUND;

		int moveNum = 0;
		int legal = 0;
		U16 BestMove = BMOVENULL;
		int BestScore = -AB_BOUND;
		Score = -AB_BOUND;
		int OldAlpha = alpha;
		U16 currMove;
		U16 PvMove = BMOVENULL;

		if (Utils::globalHashTable->ProbeHashEntry(pos, &PvMove, &Score, alpha, beta, depth)) {
			return Score;
		}

		BigMoveGen gen(pos);
		gen.GenerateMoves();

		gen.ScoreMoveList(PvMove);
		gen.SortMoveList();

		for (moveNum = 0; moveNum < gen.GetMoveList()->count; ++moveNum) {
			currMove = gen.GetMoveList()->moves[moveNum];

			pos->MakeMove(currMove);
			legal++;

			Score = -AlphaBeta(-beta, -alpha, depth - 1);

			pos->TakeMove(currMove);

			if (m_searchInfo->stop == true) {
				return 0;
			}

			if (Score > BestScore) {
				BestScore = Score;
				BestMove = currMove;
				if (Score > alpha) {
					if (Score >= beta) {

						Utils::globalHashTable->StoreHashEntry(pos, currMove, beta, HFBETA, depth);
						return beta;
					}
					alpha = Score;
				}
			}
		}

		assert(alpha >= OldAlpha);

		if (alpha != OldAlpha) {
			Utils::globalHashTable->StoreHashEntry(pos, BestMove, BestScore, HFEXACT, depth);
		}
		else {
			Utils::globalHashTable->StoreHashEntry(pos, BestMove, alpha, HFALPHA, depth);
		}

		return alpha;
	}

}
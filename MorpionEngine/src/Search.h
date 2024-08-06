#pragma once
#include "Types.h"
#include "Bitboard.h"
#include "Position.h"

#include <unordered_map>

namespace MorpionEngine {

	class Position;
	class SingleBoard;

	class Evaluate {
	public:
		static void Init();
		static void Uninit();

		static int EvaluatePosition(Position* pos);
		inline static int EvaluateSingleBoard(SingleBoard* board) { return m_smallEvals[board->m_hashKey]; }

	private:
		static std::unordered_map<U64, int> m_smallEvals;
	};

	class Search {
	public:
		Search(Position* pos);
		void IterativeDeepen(int threadNum);

		inline void SetSearchInfo(SearchInfo* searchInfo) { m_searchInfo = searchInfo; }
	private:
		void CheckUp();
		void ClearForSearch();

		int AlphaBeta(int alpha, int beta, int depth);
		int Quiescence(int alpha, int beta);

		Position* pos;
		SearchInfo* m_searchInfo;
	};

}
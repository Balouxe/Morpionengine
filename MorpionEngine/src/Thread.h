#pragma once
#include "Types.h"
#include "Position.h"
#include <thread>

namespace MorpionEngine {

	class SearchThread {
	public:
		SearchThread();
		~SearchThread();

		void Init(Position& pos, SearchInfo* si, int threadNum);
		void Uninit();

		void Start();
		void Stop();

		static void Fallback(Position* pos, SearchInfo* si, int threadNum);
	private:
		Position* m_pos;
		SearchInfo m_info;
		int m_threadNum;
		std::jthread thread;
	};

}
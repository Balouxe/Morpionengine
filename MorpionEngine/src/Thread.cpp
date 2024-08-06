#include "Thread.h"
#include "Search.h"

namespace MorpionEngine {

	SearchThread::SearchThread() {

	}

	void SearchThread::Init(Position& pos, SearchInfo* si, int threadNum) {
		Uninit();
		m_pos = new Position(pos);
		m_info = *si;
		m_threadNum = threadNum;
	}

	void SearchThread::Uninit() {
		delete m_pos;
	}

	SearchThread::~SearchThread() {
		Uninit();
	}

	void SearchThread::Start() {
		thread = std::jthread(Fallback, m_pos, &m_info, m_threadNum);
	}

	void SearchThread::Stop() {
		thread.request_stop();
		thread.join();
	}

	void SearchThread::Fallback(Position* pos, SearchInfo* si, int threadNum) {
		Search search(pos);
		search.SetSearchInfo(si);
		search.IterativeDeepen(threadNum);
	}

}
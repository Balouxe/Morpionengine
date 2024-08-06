#pragma once
#include "Position.h"
#include "Thread.h"

#include <string>
#include <vector>

namespace MorpionEngine {

	class Application {
	public:
		void Run();

		void Loop();

		void ParseGo(std::vector<std::string> inp);
		void ParsePosition(std::vector<std::string> inp);

	private:
		Position pos;
		SearchInfo info;
		SearchThread* threads[NUMTHREADS];
	};

}
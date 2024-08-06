#include "Application.h"

#include "Search.h"
#include "Utils.h"

namespace MorpionEngine {

	std::vector<std::string> SplitString(std::string str) {
		std::string temp = "";
		std::vector<std::string> ret;
		for (int i = 0; i < str.length(); i++) {
			if (str[i] == ' ') {
				ret.push_back(temp);
				temp = "";
				continue;
			}

			temp += str[i];
		}

		ret.push_back(temp);
		return ret;
	}

	void Application::Run() {
		Hash::InitHashing();
		Bitboard16::InitMasks();
		Bitboard64::InitMasks();
		Evaluate::Init();
		Utils::InitGlobalHashTable();

		pos = Position();
		info = SearchInfo();
		pos.ResetBoard();

		for (int i = 0; i < NUMTHREADS; i++) {
			threads[i] = new SearchThread();
		}

		Loop();

		for (int i = 0; i < NUMTHREADS; i++) {
			delete threads[i];
		}

		Utils::UninitGlobalHashTable();
	}

	void Application::Loop() {
		std::string input;
		while (true) {
			input = "";
			std::cout << ">> ";
			std::getline(std::cin, input);
			std::cout << std::endl;

			std::vector<std::string> split = SplitString(input);

			if (split[0] == "go") {
				ParseGo(split);
			}
			else if (split[0] == "test") {
				std::cout << "Test !" << std::endl;
			}
			else if (split[0] == "position") {
				ParsePosition(split);
			}
			else if (split[0] == "print") {
				pos.PrintPosition();
			}
		}
	}

	void Application::ParseGo(std::vector<std::string> inp) {
		Utils::globalHashTable->ClearHashTable();
		pos.ClearForSearch();

		if (inp.size() == 1) {
			info.depth = MAXDEPTH;
			info.timeset = false;
		} else if (inp[1] == "infinite") {
			info.depth = MAXDEPTH;
			info.timeset = false;
		}

		for (int i = 0; i < NUMTHREADS; i++) {
			threads[i]->Init(pos, &info, i);
			threads[i]->Start();
		}

		// Search search(&pos);
		// search.SetSearchInfo(&info);
		// search.IterativeDeepen();
	}

	void Application::ParsePosition(std::vector<std::string> inp) {
		if (inp[1] == "startpos") {
			pos.ResetBoard();

			if (inp.size() <= 2) return;

			for (int i = 2; i < inp.size(); i++) {
				pos.MakeMove(Utils::StringToMove(inp[i], &pos));
			}

			pos.PrintPosition();
		}
	}
}
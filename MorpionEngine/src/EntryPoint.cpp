#include "Application.h"

/*void Perft(int depth) {
	BigMoveGen gen(&pos);
	gen.GenerateMoves();
	if (depth == 1) {
		num += gen.GetMoveList()->count;
		return;
	}
	for (int i = 0; i < gen.GetMoveList()->count; i++) {
		pos.MakeMove(gen.GetMoveList()->moves[i]);

		Perft(depth - 1);

		pos.TakeMove(gen.GetMoveList()->moves[i]);
	}
}*/

int main() {

	MorpionEngine::Application app;
	app.Run();

	return 0;
}
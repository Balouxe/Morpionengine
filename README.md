# MorpionEngine

Morpionception engine written in C++ to mercilessly beat my friends.


## Morpionception, the rules

The rules for this game can be found here (french) :
https://adnjeux.wordpress.com/wp-content/uploads/2020/04/morpionception.pdf

Basically, tic-tac-toes inside a big tic-tac-toe.
Circle starts the game.


## Usage and the Morpionception Interface (MCI)

What I pretentiously call MCI is the protocol I made to communicate with the engine. It is very similar to UCI, the Universal Chess Interface.
The main difference is how moves are defined.

The squares inside the big tic-tac-toe board are named X1-Z3 with letters for columns and numbers for rows.
The squares inside the small tic-tac-toe boards are named A1-C3 with again, letters for columns and numbers for rows.

A move is written starting with the name of the board, followed with the square played, separated by a colon (example: Y2:A1).

To set the board of the engine, use position startpos [every move played] (example: position startpos Y2:A1 X1:B2 Y2:C3).


## Building

I don't include CMake/Premake files as this project is nowhere near the size where they are needed. This repo includes a Visual Studio 2022 solution and project files.
However, there aren't any dependencies, and literally the only files you need to compile this project are inside the src folder.

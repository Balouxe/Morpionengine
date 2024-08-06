#include "Bitboard.h"

namespace MorpionEngine {

	U64 Bitboard64::SetMask[64];
	U64 Bitboard64::ClearMask[64];
	U16 Bitboard16::SetMask[16];
	U16 Bitboard16::ClearMask[16];

	Bitboard64::Bitboard64() : bitboard(0ULL) {
	}

	Bitboard64::Bitboard64(U64 b) : bitboard(b) {
	}

	void Bitboard64::InitMasks() {
		int i = 0;

		for (i = 0; i < 64; i++) {
			SetMask[i] = 0ULL;
			ClearMask[i] = 0ULL;
		}

		for (i = 0; i < 64; i++) {
			SetMask[i] |= (1ULL << i);
			ClearMask[i] = ~SetMask[i];
		}
	}

	void Bitboard64::SetBit(uint8_t pos) {
		bitboard |= SetMask[pos];
	}

	void Bitboard64::ClearBit(uint8_t pos) {
		bitboard &= ClearMask[pos];
	}

	bool Bitboard64::GetBit(uint8_t pos) {
		return bitboard & SetMask[pos];
	}

	Bitboard16::Bitboard16() : bitboard(0ULL) {
	}

	Bitboard16::Bitboard16(U16 b) : bitboard(b) {
	}

	void Bitboard16::InitMasks() {
		int i = 0;

		for (i = 0; i < 16; i++) {
			SetMask[i] = 0ULL;
			ClearMask[i] = 0ULL;
		}

		for (i = 0; i < 16; i++) {
			SetMask[i] |= (1ULL << i);
			ClearMask[i] = ~SetMask[i];
		}
	}

	void Bitboard16::SetBit(uint8_t pos) {
		bitboard |= SetMask[pos];
	}

	void Bitboard16::ClearBit(uint8_t pos) {
		bitboard &= ClearMask[pos];
	}

	bool Bitboard16::GetBit(uint8_t pos) {
		return bitboard & SetMask[pos];
	}

	int Bitboard16::CountBits() {
		int count = 0;
		U16 copy = bitboard;

		while (copy > 0) {
			count += copy & 1;
			copy >>= 1;
		}

		return count;
	}

	void Bitboard16::PrintBitboardAs9() {
		U64 shiftMe = 1ULL;

		int rank = 0;
		int file = 0;
		int square = 0;

		std::cout << "\n";
		for (rank = 0; rank <= 2; ++rank) {
			for (file = 0; file <= 2; ++file) {

				square = rank * 3 + file;

				if ((shiftMe << square) & bitboard) {
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

}
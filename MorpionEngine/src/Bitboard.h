#pragma once
#include "Types.h"

namespace MorpionEngine {

	class Bitboard64 {
	public:
		Bitboard64();
		Bitboard64(U64 b);

		inline U64& Get() {
			return bitboard;
		}

		void SetBit(uint8_t pos);
		void ClearBit(uint8_t pos);
		bool GetBit(uint8_t pos);

		static void InitMasks();
		static U64 SetMask[64];
		static U64 ClearMask[64];
		
	private:
		U64 bitboard;
	};

	class Bitboard16 {
	public:
		Bitboard16();
		Bitboard16(U16 b);

		inline U16& Get() {
			return bitboard;
		}

		void SetBit(uint8_t pos);
		void ClearBit(uint8_t pos);
		bool GetBit(uint8_t pos);
		int CountBits();

		void PrintBitboardAs9();

		static void InitMasks();
		static U16 SetMask[16];
		static U16 ClearMask[16];

	private:
		U16 bitboard;
	};

}
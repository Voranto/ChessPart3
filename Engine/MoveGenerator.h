#include "Board.h"
#include "Move.h"
#include <vector>
#include <cstdint>

#pragma once


class MoveGenerator{
	public:
		explicit MoveGenerator(const Board& board);

		void generateLegalMoves(std::vector<Move>& moves) const;
		void generatePseudoLegalMoves(std::vector<Move>& moves) const;
    	void generateCaptures(std::vector<Move>& moves) const;

		//Initializers for lookups and magic
		static void initKnightAttacks();
		static void initKingAttacks();
		static void initSlidingAttacks();
		static void initPawnAttacks();
		

	private:
    		const Board& board;

			static uint64_t knightAttacks[64]; // all squares a knight can jump to
			static uint64_t kingAttacks[64];   // all squares a king can move to
			static uint64_t RookAttackTable[64][4096];
			static uint64_t BishopAttackTable[64][512];
			static uint64_t RookMasks[64];
			static uint64_t BishopMasks[64];
			static uint64_t RookMagics[64];
			static uint64_t BishopMagics[64];
			static int RookRelevantBits[64];
			static int BishopRelevantBits[64];

			static uint64_t WhitePawnPush[64];    // single-square push
			static uint64_t WhitePawnDouble[64];  // double push (only from rank 2)
			static uint64_t WhitePawnAttacks[64]; // diagonal captures

			static uint64_t BlackPawnPush[64];    // single-square push
			static uint64_t BlackPawnDouble[64];  // double push (only from rank 2)
			static uint64_t BlackPawnAttacks[64]; // diagonal captures



    // Piece-specific helpers
    		void generatePawnMoves(std::vector<Move>& moves) const;
    		void generateKnightMoves(std::vector<Move>& moves) const;
			void generateBishopMoves(std::vector<Move>& moves) const;
    		void generateRookMoves(std::vector<Move>& moves) const;
    		void generateQueenMoves(std::vector<Move>& moves) const;
    		void generateKingMoves(std::vector<Move>& moves) const;

			

    		// Legality check
    		bool isLegal(const Move& move) const;

    		// Attack utilities
    		uint64_t attacksFromSquare(int square, PieceType piece) const;

			static uint64_t generateRookMask(int sq);
			static uint64_t generateBishopMask(int sq);
			static  std::vector<uint64_t> generateOccupancies(uint64_t mask);
			static uint64_t rookAttacksOnTheFly(int square, uint64_t blockers);
			static uint64_t bishopAttacksOnTheFly(int square, uint64_t blockers);

			static uint64_t getRookAttacks(int square, uint64_t occupancy);
			static uint64_t getBishopAttacks(int square, uint64_t occupancy);
			static uint64_t getQueenAttacks(int square, uint64_t occupancy);
			uint64_t getPawnAttacks(int square, uint64_t combinedSame, uint64_t combinedOpposite) const;

};

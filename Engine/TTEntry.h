#include "Move.h"
enum TTFlag { EXACT, LOWERBOUND, UPPERBOUND };

struct TTEntry {
    uint64_t key = 0;      // 64-bit Zobrist key
    int16_t depth = -1;     // depth of search (signed to allow -1)
    int16_t score = 0;     // evaluation score
    TTFlag flag;      // EXACT / ALPHA / BETA
    Move bestMove;     // 16–32 bits if you pack it efficiently
};

inline void clear(){
    std::memset(TT, 0, sizeof(TTEntry) * TTSIZE);
}

inline TTEntry* probe(uint64_t key) {
    size_t idx = key & (TTSIZE - 1);
    TTEntry& e = TT[idx];
    return (e.key == key) ? &e : nullptr;
}

inline void store(uint64_t key, int depth, int score, TTFlag flag, Move bestMove) {
    size_t idx = key & (TTSIZE - 1);
    TTEntry& e = TT[idx];
    if (e.key == 0 || depth >= e.depth) {
        e.key = key;
        e.depth = depth;
        e.score = score;
        e.flag = flag;
        e.bestMove = bestMove;
    }
}

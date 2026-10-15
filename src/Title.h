// src/Title.h
#pragma once
#include <cstdint>
#include <string>

struct Title {
    std::string tconst;     // "tt0111161"
    std::string name;       // primaryTitle
    std::string type;       // "movie", "tvSeries", "tvMiniSeries", ...
    int year = -1;          // -1 means missing (\N)
    int runtime = -1;       // minutes, -1 means missing
    uint32_t genreMask = 0; // one bit per genre (see DataLoader)
    float rating = 0.0f;    // averageRating
    int votes = 0;          // numVotes
    double score = 0.0;     // weighted score, filled by computeScores()
};

// The ONE ranking rule both sorts use.
// Higher score first; ties broken by more votes, then by tconst.
// Because no two titles ever tie completely, merge sort and quick sort
// must produce IDENTICAL output, so the correctness check is a simple ==.
inline bool ranksBefore(const Title& a, const Title& b) {
    if (a.score != b.score) return a.score > b.score;
    if (a.votes != b.votes) return a.votes > b.votes;
    return a.tconst < b.tconst;
}
#pragma once
#include <cstdint>
#include <string>

// one movie or show, with everything we need from both imdb files
struct Title {
    std::string tconst;       // imdb's id, like "tt0111161"
    std::string name;         // the title people see
    std::string type;         // "movie", "tvSeries", "tvMiniSeries", etc.
    int year = -1;            // -1 means the file didn't have it
    int runtime = -1;         // in minutes, -1 means the file didn't have it
    uint32_t genreMask = 0;   // each genre is one bit, filled in by the loader
    float rating = 0.0f;      // average imdb rating
    int votes = 0;            // how many people rated it
    double score = 0.0;       // weighted score, filled in by computeScores
};

// the one rule every sort uses. true if a should show up before b.
// higher score wins, then more votes, then the id, so two titles never fully tie
inline bool ranksBefore(const Title& a, const Title& b) {
    if (a.score != b.score) return a.score > b.score;
    if (a.votes != b.votes) return a.votes > b.votes;
    return a.tconst < b.tconst;
}
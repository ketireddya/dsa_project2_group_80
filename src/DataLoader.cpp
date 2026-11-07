#include "DataLoader.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace {   // everything in here is only used inside this file

std::vector<std::string> g_genreNames;              // bit number -> genre name
std::unordered_map<std::string, int> g_genreIndex;  // genre name -> bit number

struct Rating { float rating; int votes; };

// chops a line into pieces wherever delim shows up. "a\tb\tc" -> {"a","b","c"}
std::vector<std::string> splitOn(const std::string& line, char delim) {
    std::vector<std::string> parts;
    size_t start = 0;
    while (true) {
        size_t pos = line.find(delim, start);
        if (pos == std::string::npos) {           // nothing left to split on
            parts.push_back(line.substr(start));  // so grab the last piece
            return parts;
        }
        parts.push_back(line.substr(start, pos - start));
        start = pos + 1;
    }
}

// imdb puts a backslash and an N when something's missing.
// in c++ that has to be typed "\\N" since a backslash needs another backslash in front
bool isMissing(const std::string& s) { return s.empty() || s == "\\N"; }

// turns text into a number, or gives back fallback if it's missing or not a number
int toIntOr(const std::string& s, int fallback) {
    if (isMissing(s)) return fallback;
    try { return std::stoi(s); } catch (...) { return fallback; }
}

// gets rid of the extra \r windows can leave at the end of a line
void stripCR(std::string& line) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
}

// turns "Comedy,Drama" into one number with the comedy and drama bits on
uint32_t maskFromGenres(const std::string& field) {
    if (isMissing(field)) return 0;
    uint32_t mask = 0;
    for (const std::string& g : splitOn(field, ',')) {
        int idx;
        auto it = g_genreIndex.find(g);
        if (it != g_genreIndex.end()) {
            idx = it->second;                  // already know this genre
        } else {
            idx = (int)g_genreNames.size();    // new genre, so it gets the next bit
            if (idx >= 32) continue;           // only 32 bits to go around
            g_genreNames.push_back(g);
            g_genreIndex[g] = idx;
        }
        mask |= (1u << idx);                   // flip that genre's bit on
    }
    return mask;
}

} // namespace

std::vector<Title> loadTitles(const std::string& basicsPath,
                              const std::string& ratingsPath) {
    std::vector<Title> titles;
    std::string line;

    // first, read every rating into a map so we can look one up by id
    std::ifstream rin(ratingsPath);
    if (!rin) { std::cerr << "Can't open " << ratingsPath << "\n"; return titles; }
    std::unordered_map<std::string, Rating> ratings;
    ratings.reserve(2000000);                 // make room up front so it doesn't keep growing
    std::getline(rin, line);                  // first line is just column names
    while (std::getline(rin, line)) {
        stripCR(line);
        std::vector<std::string> f = splitOn(line, '\t');
        if (f.size() != 3) continue;          // skip broken lines
        ratings[f[0]] = { std::stof(f[1]), std::stoi(f[2]) };
    }

    // then go through the big file one line at a time and keep the ones with ratings
    std::ifstream bin(basicsPath);
    if (!bin) { std::cerr << "Can't open " << basicsPath << "\n"; return titles; }
    titles.reserve(ratings.size());
    std::getline(bin, line);                  // first line is just column names
    while (std::getline(bin, line)) {
        stripCR(line);
        std::vector<std::string> f = splitOn(line, '\t');
        if (f.size() != 9) continue;          // skip broken lines
        auto it = ratings.find(f[0]);
        if (it == ratings.end()) continue;    // no rating means we can't rank it
        if (f[4] == "1") continue;            // skip adult titles

        Title t;
        t.tconst    = f[0];
        t.type      = f[1];
        t.name      = f[2];
        t.year      = toIntOr(f[5], -1);
        t.runtime   = toIntOr(f[7], -1);
        t.genreMask = maskFromGenres(f[8]);
        t.rating    = it->second.rating;
        t.votes     = it->second.votes;
        titles.push_back(std::move(t));
    }
    return titles;
}

// mixes each title's rating with the overall average.
// few votes = pulled toward the average, lots of votes = stays near its own rating
void computeScores(std::vector<Title>& titles, int m) {
    if (titles.empty()) return;
    double sum = 0.0;
    for (const Title& t : titles) sum += t.rating;
    double C = sum / titles.size();           // average rating across every title

    for (Title& t : titles) {
        double v = t.votes;                   // a double so the division doesn't round to 0
        t.score = (v / (v + m)) * t.rating + (m / (v + m)) * C;
    }
}

// gives back the bit for one genre name
uint32_t genreBit(const std::string& genre) {
    auto it = g_genreIndex.find(genre);
    return it == g_genreIndex.end() ? 0 : (1u << it->second);
}

// lets other files see the genre list without changing it
const std::vector<std::string>& genreNames() { return g_genreNames; }
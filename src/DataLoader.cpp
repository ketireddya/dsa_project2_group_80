#include "DataLoader.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace {   // everything in here is private to this file

struct Rating { float rating; int votes; };

// Splits "a\tb\tc" into {"a","b","c"}. Keeps empty fields.
std::vector<std::string> splitOn(const std::string& line, char delim) {
    std::vector<std::string> parts;
    size_t start = 0;
    while (true) {
        size_t pos = line.find(delim, start);
        if (pos == std::string::npos) {           // no more delimiters
            parts.push_back(line.substr(start));  // last field
            return parts;
        }
        parts.push_back(line.substr(start, pos - start));
        start = pos + 1;
    }
}

// IMDb writes missing values as backslash + N.
// In C++ source that's written "\\N" (the first backslash escapes the second).
bool isMissing(const std::string& s) { return s.empty() || s == "\\N"; }

int toIntOr(const std::string& s, int fallback) {
    if (isMissing(s)) return fallback;
    try { return std::stoi(s); } catch (...) { return fallback; }
}

// Removes a trailing '\r' from Windows-style line endings.
void stripCR(std::string& line) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
}

} // namespace

std::vector<Title> loadTitles(const std::string& basicsPath,
                              const std::string& ratingsPath) {
    std::vector<Title> titles;
    std::string line;

    // Step A: ratings file -> hash map keyed by tconst
    std::ifstream rin(ratingsPath);
    if (!rin) { std::cerr << "Can't open " << ratingsPath << "\n"; return titles; }
    std::unordered_map<std::string, Rating> ratings;
    ratings.reserve(2000000);                 // avoid rehashing while filling
    std::getline(rin, line);                  // skip the header line
    while (std::getline(rin, line)) {
        stripCR(line);
        std::vector<std::string> f = splitOn(line, '\t');
        if (f.size() != 3) continue;          // skip malformed lines
        ratings[f[0]] = { std::stof(f[1]), std::stoi(f[2]) };
    }

    // Step B: stream basics, keep only titles that have a rating
    std::ifstream bin(basicsPath);
    if (!bin) { std::cerr << "Can't open " << basicsPath << "\n"; return titles; }
    titles.reserve(ratings.size());
    std::getline(bin, line);                  // skip the header line
    while (std::getline(bin, line)) {
        stripCR(line);
        std::vector<std::string> f = splitOn(line, '\t');
        if (f.size() != 9) continue;          // skip malformed lines
        auto it = ratings.find(f[0]);
        if (it == ratings.end()) continue;    // no rating = can't rank it
        if (f[4] == "1") continue;            // skip adult titles (confirm with team)

        Title t;
        t.tconst  = f[0];
        t.type    = f[1];
        t.name    = f[2];
        t.year    = toIntOr(f[5], -1);
        t.runtime = toIntOr(f[7], -1);
        t.rating  = it->second.rating;
        t.votes   = it->second.votes;
        titles.push_back(std::move(t));
    }
    return titles;
}
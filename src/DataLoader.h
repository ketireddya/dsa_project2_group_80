#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "Title.h"

// reads both imdb files, matches them up by id, and keeps only titles that have ratings
std::vector<Title> loadTitles(const std::string& basicsPath,
                              const std::string& ratingsPath);

// gives every title its weighted score. bigger m = more votes needed to trust a rating
void computeScores(std::vector<Title>& titles, int m);

// genre helpers for the filter and the menu
uint32_t genreBit(const std::string& genre);      // gives 0 if we've never seen that genre
const std::vector<std::string>& genreNames();     // all the genres we found, in bit order
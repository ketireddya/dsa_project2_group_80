#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "Title.h"

// reads both TSVs, joins on tconst, returns only titles that have ratings.
std::vector<Title> loadTitles(const std::string& basicsPath,
                              const std::string& ratingsPath);


// gives every title its weighted score. bigger m = more votes needed to trust a rating
void computeScores(std::vector<Title>& titles, int m);

// genre helpers for the filter and menu.
uint32_t genreBit(const std::string& genre);      // 0 if the genre is unknown
const std::vector<std::string>& genreNames();     // every genre seen, in bit order
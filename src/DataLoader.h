#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "Title.h"

// reads both TSVs, joins on tconst, returns only titles that have ratings.
std::vector<Title> loadTitles(const std::string& basicsPath,
                              const std::string& ratingsPath);

// fills t.score for every title using the Bayesian weighted rating.
void computeScores(std::vector<Title>& titles, int m);

// genre helpers for the filter and Bryce's menu.
uint32_t genreBit(const std::string& genre);
const std::vector<std::string>& genreNames();
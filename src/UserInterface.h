#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Filter.h"
#include "Title.h"
#include "Trie.h"

int askForInteger(const std::string& prompt, int minimumAllowed, int maximumAllowed);

std::string chooseTitleType();

uint32_t chooseGenre();

void chooseYearRange(FilterOptions& filters);

void chooseRuntimeRange(FilterOptions& filters);

void chooseMinimumVotes(FilterOptions& filters);

void resetFilters(FilterOptions& filters, std::string& titleSearch);

void printCurrentFilters(const FilterOptions& filters, const std::string& titleSearch);

void searchTitles(std::string& titleSearch);

bool findTopMovies(const std::vector<Title>& allTitles, const FilterOptions& filters, const Trie& titleTrie, const std::string& titleSearch);

void displayMainMenu();
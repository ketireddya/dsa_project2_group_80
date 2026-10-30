#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "Title.h"

// everything the user can filter by. anything left at its default means "no limit"
struct FilterOptions {
    uint32_t genreMask = 0;   // 0 = any genre, otherwise the title needs at least one of these
    int minYear = -1;         // -1 = no limit
    int maxYear = -1;
    int minRuntime = -1;
    int maxRuntime = -1;
    std::string type;         // empty = any type, otherwise has to match, like "movie"
    int minVotes = 0;
};

// copies every title that passes the filters into a new list
std::vector<Title> filterTitles(const std::vector<Title>& all,
                                const FilterOptions& f);
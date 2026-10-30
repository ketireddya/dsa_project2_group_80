#include "Filter.h"

// true if the title passes every filter that's turned on
static bool matches(const Title& t, const FilterOptions& f) {
    if (f.genreMask != 0 && (t.genreMask & f.genreMask) == 0) return false;   // no matching genre
    if (!f.type.empty() && t.type != f.type) return false;
    if (t.votes < f.minVotes) return false;

    // if a limit is set, a title with a missing value (-1) can't pass it
    if (f.minYear != -1 && (t.year == -1 || t.year < f.minYear)) return false;
    if (f.maxYear != -1 && (t.year == -1 || t.year > f.maxYear)) return false;
    if (f.minRuntime != -1 && (t.runtime == -1 || t.runtime < f.minRuntime)) return false;
    if (f.maxRuntime != -1 && (t.runtime == -1 || t.runtime > f.maxRuntime)) return false;
    return true;
}

std::vector<Title> filterTitles(const std::vector<Title>& all,
                                const FilterOptions& f) {
    std::vector<Title> out;
    for (const Title& t : all) {
        if (matches(t, f)) out.push_back(t);
    }
    return out;
}
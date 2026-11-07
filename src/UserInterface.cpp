#include "UserInterface.h"
#include "QuickSort.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>

#include "DataLoader.h"
#include "MergeSort.h"

// Reads an integer within the allowed range.
int askForInteger(const std::string& prompt, int minimumAllowed, int maximumAllowed) {
    while (true) {
        std::cout << prompt;

        std::string input;
        // if input has run out (like ctrl+z), stop instead of asking forever
        if (!std::getline(std::cin, input)) {
            std::cout << "\nNo more input, closing.\n";
            std::exit(0);
        }

        try {
            size_t charactersRead = 0;
            int value = std::stoi(input, &charactersRead);

            // Rejects text following the number.
            if (charactersRead != input.size()) {
                std::cout << "Please enter a whole number.\n";
                continue;
            }

            // Rejects numbers outside the range.
            if (value < minimumAllowed || value > maximumAllowed) {
                std::cout << "Enter a number from " << minimumAllowed << " to " << maximumAllowed << ".\n";
                continue;
            }

            return value;
        }
        catch (...) {
            // Handles text and empty input.
            std::cout << "Please enter a valid whole number.\n";
        }
    }
}


// Makes title types readable.
std::string titleTypeToText(const std::string& type) {
    if (type == "movie") {
        return "Movie";
    }
    else if (type == "tvSeries") {
        return "TV Series";
    }
    else if (type == "tvMiniSeries") {
        return "TV Miniseries";
    }
    else if (type.empty()) {
        return "Any Type";
    }
    else {
        return type;
    }
}

// Converts genre bits into readable names.
std::string genreMaskToText(uint32_t mask) {
    if (mask == 0) {
        return "Any Genre";
    }

    std::string result;
    const std::vector<std::string>& names = genreNames();

    for (size_t bit = 0; bit < names.size() && bit < 32; bit++) {
        uint32_t currentBit = 1u << bit;

        if ((mask & currentBit) != 0) {
            if (!result.empty()) {
                result += ", ";
            }

            result += names[bit];
        }
    }

    if (result.empty()) {
        return "Unknown";
    }
    else {
        return result;
    }
}

// Lets users choose a title type.
std::string chooseTitleType() {
    std::cout << "\nChoose a media type\n";
    std::cout << "1. Movie\n";
    std::cout << "2. TV Series\n";
    std::cout << "3. TV Miniseries\n";
    std::cout << "4. Any Type\n";

    int selection = askForInteger("Selection: ", 1, 4);

    if (selection == 1) {
        return "movie";
    }
    else if (selection == 2) {
        return "tvSeries";
    }
    else if (selection == 3) {
        return "tvMiniSeries";
    }
    else {
        return "";
    }
}

// Lets users choose a genre.
uint32_t chooseGenre() {
    const std::vector<std::string>& names = genreNames();

    std::cout << "\nChoose a genre\n";
    std::cout << "0. Any Genre\n";

    for (size_t i = 0; i < names.size(); i++) {
        std::cout << i + 1 << ". " << names[i] << "\n";
    }

    int selection = askForInteger("Selection: ", 0, static_cast<int>(names.size()));

    if (selection == 0) {
        return 0;
    }
    else {
        size_t bitPosition = static_cast<size_t>(selection - 1);
        return 1u << bitPosition;
    }
}

// Sets optional release year limits.
void chooseYearRange(FilterOptions& filters) {
    while (true) {
        std::cout << "\nEnter -1 for no limit.\n";

        int minimumYear = askForInteger("Minimum year: ", -1, 2100);
        int maximumYear = askForInteger("Maximum year: ", -1, 2100);

        if (minimumYear != -1 && maximumYear != -1 && minimumYear > maximumYear) {
            std::cout << "Minimum year cannot exceed maximum year.\n";
            continue;
        }

        filters.minYear = minimumYear;
        filters.maxYear = maximumYear;
        return;
    }
}

// Sets optional runtime limits.
void chooseRuntimeRange(FilterOptions& filters) {
    while (true) {
        std::cout << "\nEnter -1 for no limit.\n";

        int minimumRuntime = askForInteger("Minimum minutes: ", -1, 1000);
        int maximumRuntime = askForInteger("Maximum minutes: ", -1, 1000);

        if (minimumRuntime != -1 && maximumRuntime != -1 && minimumRuntime > maximumRuntime) {
            std::cout << "Minimum runtime cannot exceed maximum runtime.\n";
            continue;
        }

        filters.minRuntime = minimumRuntime;
        filters.maxRuntime = maximumRuntime;
        return;
    }
}

// Sets the minimum vote count.
void chooseMinimumVotes(FilterOptions& filters) {
    std::cout << "\nEnter -1 for no minimum.\n";

    filters.minVotes = askForInteger("Minimum votes: ", -1, 100000000);
}

// Restores every filter to its default.
void resetFilters(FilterOptions& filters, std::string& titleSearch) {
    filters = FilterOptions{};
    titleSearch.clear();
    std::cout << "\nAll filters have been reset.\n";
}

// Displays the currently selected filters.
void printCurrentFilters(const FilterOptions& filters, const std::string& titleSearch) {
    std::cout << "\nCurrent Filters\n";
    std::cout << "----------------------------------------\n";

    std::cout << "Media Type: " << titleTypeToText(filters.type) << "\n";
    std::cout << "Genre: " << genreMaskToText(filters.genreMask) << "\n";

    std::cout << "Year Range: ";

    if (filters.minYear == -1) {
        std::cout << "Any";
    }
    else {
        std::cout << filters.minYear;
    }

    std::cout << " to ";

    if (filters.maxYear == -1) {
        std::cout << "Any";
    }
    else {
        std::cout << filters.maxYear;
    }

    std::cout << "\nRuntime Range: ";

    if (filters.minRuntime == -1) {
        std::cout << "Any";
    }
    else {
        std::cout << filters.minRuntime;
    }

    std::cout << " to ";

    if (filters.maxRuntime == -1) {
        std::cout << "Any";
    }
    else {
        std::cout << filters.maxRuntime;
    }

    std::cout << " minutes\n";
    std::cout << "Minimum Votes: ";

    if (filters.minVotes <= 0) {
        std::cout << "Any";
    }
    else {
        std::cout << filters.minVotes;
    }

    std::cout << "\nTitle Search: ";
    if (titleSearch.empty()) {
        std::cout << "Any";
    }
    else {
        std::cout << titleSearch;
    }

    std::cout << "\n";

    std::cout << "\n";
    std::cout << "----------------------------------------\n";
}

// Displays up to ten ranked titles.
void printTopResults(const std::vector<Title>& titles) {
    size_t amountToPrint = std::min<size_t>(10, titles.size());

    std::cout << "\n";
    std::cout << std::left << std::setw(5) << "Rank" << std::setw(40) << "Title" << std::setw(8) << "Year" << std::setw(10) << "Rating" << std::setw(12) << "Score" << "Genres\n";
    std::cout << std::string(100, '-') << "\n";

    for (size_t i = 0; i < amountToPrint; i++) {
        const Title& title = titles[i];

        std::string displayedTitle = title.name;

        // Shortens titles exceeding the column.
        if (displayedTitle.size() > 37) {
            displayedTitle = displayedTitle.substr(0, 34) + "...";
        }

        std::string yearText;

        if (title.year == -1) {
            yearText = "Unknown";
        }
        else {
            yearText = std::to_string(title.year);
        }

        std::string genreText;

        if (title.genreMask == 0) {
            genreText = "Unknown";
        }
        else {
            genreText = genreMaskToText(title.genreMask);
        }

        std::cout << std::left << std::setw(5) << i + 1 << std::setw(40) << displayedTitle << std::setw(8) << yearText << std::setw(10) << std::fixed << std::setprecision(1) << title.rating << std::setw(12) << std::setprecision(3) << title.score << genreText << "\n";
    }
}

// Returns to the menu or quits.
bool programCompletionChoice() {
    while (true) {
        std::cout << "\n[M] Return to main menu\n";
        std::cout << "[Q] Quit\n";
        std::cout << "Selection: ";

        std::string input;
        // if input has run out (like ctrl+z), stop instead of asking forever
        if (!std::getline(std::cin, input)) {
            std::cout << "\nNo more input, closing.\n";
            std::exit(0);
        }

        if (input == "M" || input == "m") {
            return true;
        }
        else if (input == "Q" || input == "q") {
            return false;
        }
        else {
            std::cout << "Please enter M or Q.\n";
        }
    }
}

// Sets the title search filter.
void searchTitles(std::string& titleSearch) {
    std::cout << "\nEnter title search: ";
    std::getline(std::cin, titleSearch);
    
    // cut off spaces at the start and end so "  star wars  " still finds "Star Wars"
    size_t first = titleSearch.find_first_not_of(" \t");
    if (first == std::string::npos) {
        titleSearch.clear();                    // it was only spaces, so treat it as empty
    } else {
        size_t last = titleSearch.find_last_not_of(" \t");
        titleSearch = titleSearch.substr(first, last - first + 1);
    }

    if (titleSearch.empty()) {
        std::cout << "Title search removed.\n";
    }
    else {
        std::cout << "Title search set to: " << titleSearch << "\n";
    }
}

// Filters, sorts, times, and displays titles.
bool findTopMovies(const std::vector<Title>& allTitles, const FilterOptions& filters, const Trie& titleTrie, const std::string& titleSearch) {
    std::cout << "\nFiltering titles...\n";

    std::vector<Title> matchingTitles;

    if (titleSearch.empty()) {
        matchingTitles = filterTitles(allTitles, filters);
    }
    else {
        std::vector<int> matchingIndexes = titleTrie.startsWith(titleSearch);
        std::vector<Title> searchedTitles;

        for (int index : matchingIndexes) {
            searchedTitles.push_back(allTitles[index]);
        }

        matchingTitles = filterTitles(searchedTitles, filters);
    }

    std::cout << "Found " << matchingTitles.size() << " matching titles.\n";

    if (matchingTitles.empty()) {
        std::cout << "No titles matched the current filters.\n";
        return programCompletionChoice();
    }

    std::cout << "\nChoose a sorting algorithm\n";
    std::cout << "1. Merge sort\n";
    std::cout << "2. Quick sort\n";

    int sortChoice = askForInteger("Selection: ", 1, 2);

    auto startTime = std::chrono::steady_clock::now();

    if (sortChoice == 1) {
        mergeSort(matchingTitles);
    }
    else {
        quickSort(matchingTitles);
    }

    auto endTime = std::chrono::steady_clock::now();

    double milliseconds =
        std::chrono::duration<double, std::milli>(
            endTime - startTime
        ).count();

    std::cout << "\n========================================\n";
    std::cout << "                RESULTS\n";
    std::cout << "========================================\n";

    printTopResults(matchingTitles);

    std::string algorithmName;
    if (sortChoice == 1) {
        algorithmName = "Merge sort";
    } else {
        algorithmName = "Quick sort";
    }

    std::cout << "\n" << algorithmName << " ranked " << matchingTitles.size() << " titles in " << std::fixed << std::setprecision(3) << milliseconds << " milliseconds.\n";

    return programCompletionChoice();
}

// Displays the main terminal menu.
void displayMainMenu() {
    std::cout << "\n========================================\n";
    std::cout << "         MOVIE NIGHT PICKER\n";
    std::cout << "========================================\n";
    std::cout << "1. Search by title\n";
    std::cout << "2. Choose media type\n";
    std::cout << "3. Choose genre\n";
    std::cout << "4. Choose year range\n";
    std::cout << "5. Choose runtime range\n";
    std::cout << "6. Choose minimum vote count\n";
    std::cout << "7. Find top 10 titles\n";
    std::cout << "8. Reset filters\n";
    std::cout << "9. Run sorting benchmarks\n";
    std::cout << "10. Exit\n";
}

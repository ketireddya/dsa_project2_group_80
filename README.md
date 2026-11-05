# Movie Night Picker

Group 80: Amrita Ketireddy, Sarah Spellman, and Bryce Williams

A C++ program that filters IMDb titles and recommends the top 10. Compare results and sorting times using merge sort and quicksort.

## Setup

Requires a C++17 compiler.

Download `title.basics.tsv.gz` and `title.ratings.tsv.gz` from [IMDb Datasets](https://datasets.imdbws.com/).

Extract them into a `data` folder inside the project:

```text
data/
    title.basics.tsv
    title.ratings.tsv
```

## Build and Run

Open a terminal in the project folder.

### Windows

Use **Developer PowerShell for Visual Studio** with C++ Build Tools installed.

```powershell
cl /std:c++17 /EHsc /O2 /MT src/*.cpp /Fe:movie_picker.exe
.\movie_picker.exe
```

### macOS or Linux

Install a C++17 compiler: Xcode Command Line Tools on macOS or GCC on Linux.

```bash
g++ -std=c++17 -O2 src/*.cpp -o movie_picker
./movie_picker
```

## Usage

Follow the menu to search titles and choose filters.

- **7:** Find the top 10 and choose a sorting algorithm.
- **8:** Reset filters.
- **9:** Run benchmarks.
- **10:** Exit.

Loading the dataset may take a moment.

## Benchmarks

Compares both sorts on random, sorted, and reverse-sorted inputs using 10,000, 100,000, 500,000, and all loaded titles.

Runs three trials per arrangement, displays median times, and checks matching rankings. Requires at least one million loaded titles.

Results overwrite `benchmarks/results.csv`. Memory information describes theoretical extra space, not measured RAM.
# Movie Night Picker

Group 80: Amrita Ketireddy, Sarah Spellman, and Bryce Williams

A C++ program that filters IMDb titles and recommends the top 10 using a weighted score. It compares two sorting algorithms we wrote from scratch, merge sort and quick sort, on speed and correctness.

## Requirements

- A C++17 compiler: **GCC 9 or newer**, Clang, or Visual Studio. GCC 8 on Windows can't compile `<filesystem>`, which the benchmarks use.
- About 2 GB of free memory while the full dataset is loaded.

## Get the data

Download these two files from the [IMDb Non-Commercial Datasets](https://developer.imdb.com/non-commercial-datasets/) page:

- `title.basics.tsv.gz`
- `title.ratings.tsv.gz`

Unzip them into a `data` folder inside the project:

```text
data/
    title.basics.tsv
    title.ratings.tsv
```

In Git Bash, macOS, or Linux, you can unzip with:

```bash
gunzip data/title.basics.tsv.gz data/title.ratings.tsv.gz
```

## Build and run

Open a terminal in the project folder.

### Windows (Git Bash with MinGW-w64 g++)

```bash
g++ -std=c++17 -O2 -static src/*.cpp -o movie_picker
./movie_picker
```

`-static` builds the C++ runtime into the program so it runs without extra DLL files.

### Windows (Developer PowerShell for Visual Studio)

```powershell
cl /std:c++17 /EHsc /O2 /MT src/*.cpp /Fe:movie_picker.exe
.\movie_picker.exe
```

### macOS or Linux

```bash
g++ -std=c++17 -O2 src/*.cpp -o movie_picker
./movie_picker
```

### Using data files somewhere else

By default the program reads from `data/`. To use other locations, pass both paths:

```bash
./movie_picker path/to/title.basics.tsv path/to/title.ratings.tsv
```

## Usage

Loading the dataset takes about 10 to 20 seconds. Then the menu appears:

| Option | What it does |
| --- | --- |
| 1 | Search by title (prefix search, not case sensitive) |
| 2 | Choose media type: movie, TV series, or TV miniseries |
| 3 | Choose a genre |
| 4 | Choose a year range (-1 means no limit) |
| 5 | Choose a runtime range in minutes (-1 means no limit) |
| 6 | Choose a minimum vote count |
| 7 | Find the top 10 titles matching your filters, sorted with merge sort or quick sort |
| 8 | Reset all filters |
| 9 | Run sorting benchmarks |
| 10 | Exit |

The current filters are shown above the menu every time. After results, enter **M** to return to the menu or **Q** to quit.

## Benchmarks

Option 9 times merge sort and quick sort (median-of-3 pivot) on 10,000, 100,000, 500,000, and all loaded titles, each in random, sorted, and reverse-sorted order. Each setting runs 3 times and the median time is shown. Every run also checks that both sorts produce correctly ordered, identical rankings.

- Needs at least 1,000,000 loaded titles.
- Takes a few minutes. **Let it finish**, since results are written as it runs.
- Results are saved to `benchmarks/results.csv`, replacing any earlier results.
- Memory is described theoretically: merge sort uses O(n) extra space, quick sort uses O(log n) stack space.

## Tests

Two test programs check each part separately. Build them from the project folder, then run each one with the full data paths:

```bash
g++ -std=c++17 -O2 -static src/DataLoader.cpp src/Filter.cpp src/MergeSort.cpp tests/test_amrita.cpp -o test_amrita
./test_amrita data/title.basics.tsv data/title.ratings.tsv

g++ -std=c++17 -O2 -static src/DataLoader.cpp src/Filter.cpp src/MergeSort.cpp src/QuickSort.cpp src/TopK.cpp src/Trie.cpp tests/test_sarah.cpp -o test_sarah
./test_sarah data/title.basics.tsv data/title.ratings.tsv
```

On macOS or Linux, leave out `-static`.

- `test_amrita` checks data loading, weighted scores, filters, and merge sort.
- `test_sarah` checks quick sort with all three pivot strategies, top-k selection, and title search.
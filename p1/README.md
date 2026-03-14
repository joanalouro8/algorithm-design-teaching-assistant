# DA - First Project
## Project Structure

```
.
├── input/               # Input CSV datasets
├── output/              # Generated output and risk files
├── build/               # CMake build directory (auto-generated)
├── main.cpp
├── algorithm.cpp        # Ford-Fulkerson, graph building, output logic
├── parser.cpp / .h      # Input file parsing
├── Graph.h 
├── CMakeLists.txt
└── run_all.sh           # Batch runner for all datasets
```

---

## Build & Run

### Build
```bash
mkdir -p build && cd build && cmake .. && cmake --build . && cd ..
```

### Run a single dataset
```bash
./myProg -b sample.csv sample_risk.csv
```

- `sample.csv` — input dataset
- `sample_risk.csv` — risk analysis output file (optional)

### Run all datasets
```bash
chmod +x run_all.sh
./run_all.sh
```

This builds the project and runs every `*.csv` file found in `input/`, writing risk files to `output/<name>_risk.csv`. The assignment output file is controlled by `OutputFileName` inside each dataset — set it to `"output/<name>_out.csv"` for everything to land in the output folder.

---

## Datasets

| File | Scenario |
|---|---|
| `dataset1.csv` | The project example |
| `dataset2.csv` | Small dataset and all the submissions have the minimum reviews needed, with margin. |
| `dataset3.csv` | The same as `dataset2.csv`, but with risk analysis. There is no risky reviewer. |
| `dataset4.csv` | Small dataset and all the submissions have the minimum reviews needed, without any margin. |
| `dataset5.csv` | The same as `dataset4.csv`, but with risk analysis. All reviewers are risky revieweres. |
| `dataset6.csv` | Small dataset where only one submission is missing. |
| `dataset7.csv` | Small dataset where only there are a lot of submissions missing. |
| `dataset8.csv` | ... |
| `dataset9.csv` | The same as `dataset8.csv`, but with risk analysis. Only 2 reviewers are risky reviewers. |

(faltam casos teste para assignmente = 2!)
(fazer programa para o secunddary domain!)
(adaptar para os modos!)
(explicar como usar os meus datasets para corrigir o trabalho dos alunos!)
You need to copy the run_all.sh the input file with the datasets, you can add more datasets and then just build and run. + risk!
avisar da comand line ...
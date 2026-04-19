# DA - First Project

Implementation of the first **DA** project by **Joana Louro**.

This project implements a **scientific conference review assignment tool** using a **maximum flow approach (Ford–Fulkerson)**. The program reads datasets describing submissions, reviewers, and parameters, and generates reviewer assignments while respecting the given constraints.

The program is designed to run **via command line or batch scripts**.

---

## Project Structure

```
.
├── input/               # Input CSV datasets
├── output/              # Generated output files (auto-generated)
├── risk/                # Generated risk files (auto-generated)
├── build/               # CMake build directory (auto-generated)
├── src/
│   ├── main.cpp
│   ├── algorithm.cpp    # Ford-Fulkerson, graph building, output logic
│   ├── parser.cpp / .h  # Input file parsing
│   └── Graph.h          # Given graph structure
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
 ./myProg -b ranges/ranges1.txt registers/registers1.txt output/allocation.txt
```
**Where:**
...

### Run all datasets
```bash
chmod +x run_all.sh
./run_all.sh
```
**The script will:**
1. Build the project
1. Execute the program for every *.csv file inside the input/ folder
1. Save the results automatically

Assignment outputs will be written to the location defined in `OutputFileName` inside each dataset.

For organization, datasets should use:
``` csv
OutputFileName, "output/<dataset_name>_out.csv"
```

Risk analysis files will be written to:
```
risk/<dataset_name>.csv
```

If risk analysis is enabled.

### Reusing the Datasets and Script

This repository can also be used to test or validate other implementations.

To do this:

1. Copy the following items into the root folder of another project:

```
input/
run_all.sh
```

1. Add any additional datasets to the input/ folder if needed.

1. Ensure that each dataset contains an OutputFileName parameter pointing to the output directory:

```
OutputFileName, "output/<dataset_name>.csv"
```

This ensures that all results are stored in the output/ folder for easier comparison between implementations.

### Important Notes

This implementation is designed to be executed via command line or batch scripts.
There is no interactive interface.
Because of this, the parameter `GenerateAssignments` is only supported for values 1 or 2.
The project does not implement mode 0.
---

## Datasets
| File | Scenario |
|---|---|
| `dataset1.csv` | Example dataset provided in the project description. |
| `dataset2.csv` | Primary submission domains only. All submissions receive the required number of reviews with extra reviewer capacity available. |
| `dataset3.csv` | Same as dataset2, with risk analysis enabled. No reviewer is risky. |
| `dataset4.csv` | Primary submission domains only. All submissions receive exactly the required number of reviews (no extra capacity). |
| `dataset5.csv` | Same as dataset4, with risk analysis enabled. All reviewers are risky. |
| `dataset6.csv` | Primary submission domains only. One submission cannot reach the minimum reviews. |
| `dataset7.csv` | Primary submission domains only. Multiple submissions cannot reach the required reviews due to an insufficient number of reviewers across several topics. |
| `dataset8.csv` | Primary submission domains only. All submissions are fully covered with no additional reviewer capacity. |
| `dataset9.csv` | Same as dataset8, with risk analysis enabled. Only reviewers 7 and 8 are risky. |
| `dataset10.csv` | Primary and secondary submission domains enabled. All submissions can be assigned the required reviews with extra capacity. |
| `dataset11.csv` | Same as dataset10, with risk analysis enabled. No reviewers are risky. |
| `dataset12.csv` | Primary and secondary submission domains enabled. The algorithm must choose between primary and secondary domains to achieve a valid assignment. |
| `dataset13.csv` | Same as dataset12, with risk analysis enabled. All reviewers are risky. |
| `dataset14.csv` | Primary and secondary submission domains enabled with risk analysis. Both domains are required to obtain a valid assignment, no risky reviewers. |

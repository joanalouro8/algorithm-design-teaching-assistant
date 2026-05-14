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
For the Basic algorithm:
| Range          | Registers         | Ideal allocation |
| -------------- | ----------------- | ---------------- |
| `ranges1.txt`  | `registers2.txt`  | 2 registers      | 
| `ranges2.txt`  | `registers2.txt`  | 2 registers      | 
| `ranges3.txt`  | `registers2.txt`  | 2 registers      | 
| `ranges4.txt`  | `registers1.txt`  | 1 register       | 
| `ranges5.txt`  | `registers1.txt`  | 1 register       | 
| `ranges6.txt`  | `registers3.txt`  | 3 registers      |  

For the spilling algorithm:
| Range File     | Register File     | Registers Allowed | Expected Spills |
| -------------- | ----------------- | ----------------- | --------------- |
| `ranges7.txt`  | `registers7.txt`  | 2 registers       | 1 spill         |
| `ranges8.txt`  | `registers8.txt`  | 3 registers       | 2 spills        |
| `ranges9.txt`  | `registers7.txt`  | 2 registers       | 1 spill         |
| `ranges10.txt` | `registers9.txt`  | 2 registers       | 2 spills*       |
| `ranges11.txt` | `registers10.txt` | 2 registers       | 2 spills*       |

* The number of spills shown is the ideal result. Depending on the algorithm you may need more spills.

For the splitting algorithm:
| Range           | Registers         | Registers allowed | Split        | 
| --------------  | ----------------- | ----------------  | ---------------- |
| `ranges12.txt`  | `registers10.txt`  | 2 registers       | 1 split |
| `ranges13.txt`  | `registers10.txt`  | 2 registers       | 1 split |
| `ranges14.txt`  | `registers11.txt`  | 3 registers       | 1 split |

For the free algorithm:
| Range          | Registers         | Ideal allocation |
| -------------- | ----------------- | ---------------- |
| `ranges1.txt`  | `registers5.txt`  | 2 registers      | 
| `ranges2.txt`  | `registers5.txt`  | 2 registers      | 
| `ranges3.txt`  | `registers5.txt`  | 2 registers      | 
| `ranges4.txt`  | `registers4.txt`  | 1 register       | 
| `ranges5.txt`  | `registers4.txt`  | 1 register       | 
| `ranges6.txt`  | `registers6.txt`  | 3 registers      |  


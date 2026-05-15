# Description of datasets

## Description of datasets for the Basic algorithm

| Range         | Registers        | Ideal allocation | Allocation        |
| ------------- | ---------------- | ---------------- | ----------------- |
| `ranges1.txt` | `registers2.txt` | 2 registers      | `allocation1.txt` |
| `ranges2.txt` | `registers2.txt` | 2 registers      | `allocation2.txt` |
| `ranges3.txt` | `registers2.txt` | 2 registers      | `allocation3.txt` |
| `ranges4.txt` | `registers1.txt` | 1 register       | `allocation4.txt` |
| `ranges5.txt` | `registers1.txt` | 1 register       | `allocation5.txt` |
| `ranges6.txt` | `registers3.txt` | 3 registers      | `allocation6.txt` |

### Output

The `output` folder contains the solutions produced by the implemented algorithm for each input instance.

It is important to note that:
- The naming of webs in the output files can differ from the ones in your implementation.
- The specific registers assigned to each web may also vary.

These differences are expected, as multiple valid allocations can exist for the same problem, as long as they respect the register constraints and produce a correct allocation.

## Description of datasets for the Free algorithm

| Range          | Registers         | Ideal allocation |
| -------------- | ----------------- | ---------------- |
| `ranges1.txt`  | `registers5.txt`  | 2 registers      | 
| `ranges2.txt`  | `registers5.txt`  | 2 registers      | 
| `ranges3.txt`  | `registers5.txt`  | 2 registers      | 
| `ranges4.txt`  | `registers4.txt`  | 1 register       | 
| `ranges5.txt`  | `registers4.txt`  | 1 register       | 
| `ranges6.txt`  | `registers6.txt`  | 3 registers      | 

**These are the same datasets as the basic algorithm**

## Description of datasets for the Spilling algorithm

| Range File     | Register File     | Registers Allowed | Expected Spills |
| -------------- | ----------------- | ----------------- | --------------- |
| `ranges7.txt`  | `registers7.txt`  | 2 registers       | 1 spill         |
| `ranges8.txt`  | `registers8.txt`  | 3 registers       | 2 spills        |
| `ranges9.txt`  | `registers7.txt`  | 2 registers       | 1 spill         |
| `ranges10.txt` | `registers9.txt`  | 2 registers       | 2 spills*       |
| `ranges11.txt` | `registers10.txt` | 2 registers       | 2 spills*       |

* The number of spills shown is the ideal result. Depending on the algorithm you may need more spills.

## Description of datasets for the Splitting algorithm

| Range File     | Register File     | Registers Allowed | Expected Splits |
| -------------- | ----------------- | ----------------- | --------------- |
| `ranges12.txt` | `registers10.txt` | 2 registers       | 1 split         |
| `ranges13.txt` | `registers10.txt` | 2 registers       | 1 split         |
| `ranges14.txt` | `registers11.txt` | 3 registers       | 1 split         |

## Step 1: Parse the Input & Build Live Ranges

From the input file, you collect the raw live ranges per variable:

**sum:** one range → `7+, 8, 9, 10-`

**i:** four ranges:
- `1+, 2, 3, 4, 5, 6-`
- `9+, 10, 11, 12-`
- `12+, 13, 14-`
- `20+, 11, 12-`

---

## Step 2: Merge Overlapping Ranges → Build Webs

### What `+` and `-` Mean

```
7+   → this line is a DEFINITION (write) of the variable — the range STARTS here
10-  → this line is a USE (read) of the variable — the range ENDS here
```

A line with no marker (just a number) means the variable is live **through** that line — neither defined nor last-used there, just passing through.

---

### How They Affect Merging

The merging rule from your PDF is:

> Two webs that meet at the **same line**, where one **ends with `-`** (a use/read) and the other **starts with `+`** (a definition/write), do **NOT** interfere and do **NOT** need to merge.

This is because at that exact line, the old value is being read and a brand new value is being written — they don't actually coexist. The old one dies, the new one is born. The same register can be reused immediately.

However, two ranges **do** merge if they share a line where **both are alive in the middle** (no `+`/`-` boundary between them), or if one range starts with `-` (a use without its own definition), which means its value must have come from a previous range.

---

### The Key Special Case: A Range Starting With `-`

This is the most important rule for your project. If a range starts with a `-`:

```
i: 9-,10,11,12-
```

Line `9-` is a **use** — but there is **no definition** in this range. That means the value being used at line 9 was **defined somewhere earlier**, in a previous range of the same variable. Therefore this range **must be merged** with whatever range defined that value.

Contrast with:

```
i: 12+,13,14-
```

Line `12+` is a **definition** — this range creates its own new value. It does **not** need to merge with anything unless it shares live points with another range.

---

## The Full Merging Logic With `+`/`-`

Given two ranges R1 and R2 of the same variable, merge them into one web if **any** of these is true:

**Rule 1 — Shared interior point:** They share a line number that is not a `+`/`-` boundary in either range. Both are genuinely live at the same time.

**Rule 2 — Use-start with no definition:** R2 starts with `-` (a use, no def). Its value came from some prior range → merge R2 with that prior range.

**Rule 3 — Shared point that is not a clean def/use boundary:** They share a line, and it is NOT the case that one ends with `-` and the other starts with `+` at exactly that line.

**Do NOT merge** when: R1 ends at line `N-` and R2 starts at line `N+` — clean handoff, same line, old value dies and new value is born. These can safely get different registers (or the same one, which is exactly what the coloring algorithm exploits).

---

## Applied to Your Example

```
i: 1+,2,3,4,5,6-       → Range A, has its own definition at 1
i: 9-,10,11,12-         → Range B, starts with USE at 9, no definition → must merge with A
i: 12+,13,14-           → Range C, has its own definition at 12
i: 20+,11,12-           → Range D, has its own definition at 20
```

Step by step:

- **B starts with `9-`** (use, no def) → merge B into A → **AB** = `{1,2,3,4,5,6,9,10,11,12}`
- **D** has `20+` (own def), check overlap with AB: shares lines 11 and 12 (interior points, not a clean boundary) → merge D into AB → **ABD** = `{1,2,3,4,5,6,9,10,11,12,20}`
- **C** has `12+` (own def), check against ABD: shares line 12, but ABD has `12-` (use) and C has `12+` (def) at the same line → **clean boundary, no merge**
- C has no other overlaps → stays as its own web

Final webs for `i`:
```
web0: 1+,2,3,4,5,6-,9-,10,11,12-,20+   ← A + B + D merged
web1: 12+,13,14-                         ← C alone
```

Which is exactly what the expected output shows. The `+`/`-` markers are what make the difference between merging and not merging at line 12.

## Step 3: Build the Interference Graph

Two webs **interfere** if they are simultaneously live at any program point — with the caveat that a **definition (`+`) and a use (`-`) at the same line do NOT interfere**.

Check each pair:

**web0 vs web1:**
- web0 contains `12-`, web1 contains `12+` → same line, but one is a use and the other is a definition → **no interference**.
- Any other shared points? web0: {1,2,3,4,5,6,9,10,11,12,20}, web1: {12,13,14}. Only 12 is shared, and it's the def/use case → **no edge**.

**web0 vs web2:**
- web0: {1,2,3,4,5,6,9,10,11,12,20}, web2: {7,8,9,10}
- Shared points: **9 and 10**. Are these def/use same-line cases? web0 has `9-` (use) and web2 has 9 as an interior point (neither start nor end at 9). They are both live at line 9 simultaneously → **interference → edge**.

**web1 vs web2:**
- web1: {12,13,14}, web2: {7,8,9,10} → no shared points → **no edge**.

So the interference graph is:

```
web0 ——— web2       web1 (isolated)
```

---

## Step 4: Run the Greedy Graph Coloring Algorithm with N=2

**Simplification phase (build the stack):**

Current degrees: web0=1, web1=0, web2=1. All < N=2.

- All nodes have degree < 2, so no spilling needed.
- Remove in some order, e.g.: remove web1 (degree 0) → push web1. Remove web0 (degree 1) → push web0. Remove web2 (degree 0) → push web2.
- Stack (top→bottom): **web2, web0, web1**

**Coloring phase (pop and assign):**

1. Pop **web2** → no neighbors colored yet → assign **r1**
2. Pop **web0** → neighbor web2 has r1 → assign **r0**
3. Pop **web1** → neighbor web0 has r0, but web1 has no edges in the interference graph → assign **r0** (reuse!)

---

## Step 5: Write the Output

```
webs: 3
web0: 1+,2,3,4,5,6-,9-,10,11,12-,20+
web1: 12+,13,14-
web2: 7+,8,9,10-
registers: 2
r0: web0
r0: web1
r1: web2
```

This matches the expected output exactly. ✓

---

## Summary of the Pipeline

```
Parse ranges → Merge into webs → Build interference graph → Greedy color → Output
     ↑               ↑                    ↑                      ↑
  (ranges       (use-starts          (shared live           (stack-based,
  per var)      merge up,            points = edge,         reuse colors
                overlap merges)      def+use@same           for non-adjacent
                                     line = no edge)        webs)
```

The trickiest parts are: (1) recognizing that a range starting with `-` must be merged with a prior range, and (2) the def/use same-line non-interference rule when building edges.
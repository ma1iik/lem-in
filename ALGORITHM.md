# Lem-in: How It Works

## The Problem

Move N ants from `start` to `end` through a graph of rooms. Rules:
- Each room holds max 1 ant (except start/end)
- Ants move simultaneously through different paths
- Goal: minimize total turns


    ┌─[2]─┐
    │     │
[START]   [END]    2 paths = faster than 1
    │     │
    └─[1]─┘

---

## Our Approach

We run **two methods** and pick whichever gives fewer turns:

              INPUT
                │
       ┌────────┴────────┐
       ▼                 ▼
   BFS disj      DFS All Paths
   (Shortest)        (Least-Conflicts)
       │                 │
       ▼                 ▼
    X turns           Y turns
       │                 │
       └────────┬────────┘
                ▼
          Pick min(X,Y)

---

## Method 1: BFS disj

Find paths that **don't share any nodes**.

1. BFS → find shortest path
2. Mark its nodes as "used"
3. BFS again, skip used nodes
4. Repeat until no path found

Step 1:              Step 2:
  ┌─1─2─┐              ┌─×─×─┐    (1,2 blocked)
START   END          START   END
  └─3─4─┘              └─3─4─┘   → finds path 2

**Good for:** Big maps with separate corridors

---

## Method 2: DFS + Least-Conflicts

Find **all paths**, then pick non-conflicting ones smartly.

1. DFS → find all possible paths
2. Count conflicts (shared nodes) between paths
3. Greedily pick paths with fewest conflicts

```
Paths found:           Selection:
A: START→1→END (1)     ✓ Pick A
B: START→2→END (1)     ✓ Pick B (no conflict with A)
C: START→1→2→END (2)   ✗ Conflicts with A
```

**Good for:** Maps where paths share nodes near start/end

---

## Why Both?

Neither method wins all cases:

| Map Type      | BFS       | DFS       | Winner  |
|---------------|-----------|-----------|---------|
| maps/subject3.map  | 6 turns   | 5 turns   | **DFS** |
| maps/gen_big_1.map | 57 turns  | 423 turns | **BFS** |

BFS fails on subject3 (blocks a useful shared node).
DFS fails on big maps (finds 1 path with 100 conflicts).

---

## Test Results

| Map                | BFS            | DFS            | Winner  |
|--------------------|----------------|----------------|---------|
| gen_big_1          | 57 (9 paths)   | 423 (1 path)   | BFS     |
| gen_big_2          | 57 (9 paths)   | 423 (1 path)   | BFS     |
| gen_big_super_1    | 70 (8 paths)   | 86 (5 paths)   | BFS     |
| gen_big_super_2    | 87 (20 paths)  | 139 (8 paths)  | BFS     |
| gen_flow_one       | 35             | 35             | TIE     |
| gen_flow_ten       | 32             | 32             | TIE     |
| gen_flow_thousand  | 28             | 28             | TIE     |
| subject3           | 6 (1 path)     | 5 (2 paths)    | DFS     |
| others             | same           | same           | TIE     |

**Summary:** BFS wins 4, DFS wins 1, TIE 10. Hybrid needed.

---

## Turn Calculation

Binary search to find minimum turns:

```
For T turns, path of length L can carry (T - L + 1) ants
Sum all path capacities → if ≥ total ants, T works
```

---

## Files

```
src/main.c          - runs both methods, picks best
src/pathfinding.c   - BFS disj paths
src/path_scoring.c  - conflict detection, turn calc
src/parsing.c       - input parsing
```

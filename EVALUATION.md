# Lem-in — Evaluation Preparation Guide

---

## What is lem-in?

Move **N ants** from `##start` to `##end` through a graph in the **minimum number of turns**.

- Each turn, every ant can move one room
- Each room (except start and end) can hold **at most 1 ant at a time**
- All ants move simultaneously in each turn
- Output: print the input map, then one line per turn showing which ants moved where

**Output format:**
```
L<ant_id>-<room_name> L<ant_id>-<room_name> ...
```
Example: `L1-roomA L3-roomB`

---

## Data Structures

### t_room
```c
typedef struct s_room {
    char    *name;
    int     x, y;
    int     is_start, is_end;
    t_list  *connections;   // linked list of t_room* neighbors
    t_room  *parent;        // used during BFS to trace back path
    int     score;          // -1 means "blocked" in disjoint path BFS
    int     visited;        // BFS visited flag
    int     in_queue;       // BFS queue membership flag
} t_room;
```

### t_farm
```c
typedef struct s_farm {
    int     ant_count;
    t_list  *rooms;         // all rooms
    t_room  *start_room;
    t_room  *end_room;
    char    **input_lines;  // original input, printed back at the start
    int     line_count;
} t_farm;
```

### t_path
```c
typedef struct s_path {
    int     len;        // number of edges (rooms - 1)
    int     issues;     // number of conflicting paths
    t_list  *path;      // linked list of t_room* from start to end
} t_path;
```

### t_path_set
```c
typedef struct s_path_set {
    t_path  *paths;     // array of selected paths
    int     count;
} t_path_set;
```

---

## High-Level Flow

```
parse_input()
    ↓
get_disj_paths()   → BFS-based disjoint paths   → disj_arr[]
get_all_paths()    → DFS all paths               → all_arr[]
    ↓
eval_strat(disj_arr, STRATEGY_SHORTEST)       → bfs_set, bfs_t
eval_strat(all_arr,  STRATEGY_LEAST_CONFLICTS) → dfs_set, dfs_t
    ↓
best = (bfs_t <= dfs_t) ? bfs_set : dfs_set
    ↓
dump_input() + run_ants(best)
```

---

## Algorithm 1 — BFS Disjoint Paths (`get_disj_paths`)

**Goal:** Find a set of node-disjoint paths (no shared intermediate rooms).

**Method:** Greedy node-blocking BFS.

1. Run BFS from start to end (finds shortest available path)
2. Mark all intermediate rooms of that path as `score = -1` (blocked)
3. Repeat until BFS finds no more paths

**BFS condition to visit a neighbor:**
```
!visited && !in_queue && (score != -1 || is_start || is_end)
```
Start and end are never blocked.

**Key edge case:** If the path is direct start→end (len=1), no rooms get blocked → infinite loop. Fixed by breaking when `path->len == 1`.

**Path tracing:** Each room stores its `parent` pointer during BFS. After reaching end, walk back through parents to build the path list.

---

## Algorithm 2 — DFS All Paths (`get_all_paths`)

**Goal:** Find up to 100 distinct paths (not necessarily disjoint) for the optimizer to consider.

**Method:** Iterative DFS using an explicit stack. Each stack node carries a **copy of the current path** to track visited rooms per branch (avoids cycles without a global visited array).

**Stack node:**
```c
typedef struct s_stack_node {
    t_room              *room;
    t_list              *path;      // path so far (copied per branch)
    int                 depth;
    struct s_stack_node *next;
} t_stack_node;
```

**Depth limit:** `min(room_count, 500)` — prevents exponential blowup on dense graphs.

**Cap:** MAX_PATHS = 100 paths maximum.

---

## Algorithm 3 — Path Selection (`pick_path_set`)

Given N candidate paths (possibly conflicting), select the best conflict-free subset.

**Two strategies:**

| Strategy | Used with | Picks by |
|----------------------------|--------------------|-----------------------------------------------|
| `STRATEGY_SHORTEST`        | BFS disjoint paths |              fewest edges first               |
| `STRATEGY_LEAST_CONFLICTS` |    DFS all paths   | fewest conflicts first, ties broken by length |

**Steps:**
1. First, greedily add all zero-conflict paths
2. Then repeatedly pick the best remaining available path, dropping all paths that conflict with it
3. Result: a conflict-free set

**Conflict definition:** Two paths conflict if they share any intermediate room (not start/end).

---

## Formula — Minimum Turns Calculation

For a given set of paths sorted by length, and T total turns:

> A path of length L can carry at most `T - L + 1` ants in T turns
> (the first ant enters on turn 1 and exits on turn L, so one more can enter each turn)

**Binary search for minimum T:**
```
lo = 1
hi = ant_count + max_path_len - 1

while lo < hi:
    mid = (lo + hi) / 2
    if fits_turns(paths, ants, mid):
        hi = mid
    else:
        lo = mid + 1
return lo
```

**`fits_turns(paths, n, ants, T)`:**
```
placed = 0
for each path (shortest first):
    cap = T - path.len + 1
    if cap <= 0: skip
    use = min(cap, ants - placed)
    placed += use
    if placed >= ants: return true
return false
```

---

## Ant Distribution (`ants_per_path`)

Once we know the optimal T, assign ants to paths:

```
remaining = total_ants
for each path i (sorted shortest first):
    capacity[i] = T - path[i].len + 1
    assign min(capacity[i], remaining) ants
    remaining -= assigned
```

Shorter paths get more ants because they have higher capacity for a given T.

---

## Simulation (`run_ants`)

Each turn:
1. **Move already-moving ants** (position > 0): advance from furthest to closest to prevent collisions
2. **Launch new ants** (position == 0): send next ant onto path if position 1 is free
3. An ant is **finished** when it reaches `position >= path.len` (end room)

**Collision prevention:** Before launching a new ant, check `room_occupied()` — no two active ants can be in the same non-start/non-end room.

---

## Parsing

**Line types recognized:**
| Type | Example | Condition |
|---|---|---|
| `LINE_ANT_COUNT` | `3` | pure digits, not starting with 0, phase=0 |
| `LINE_COMMAND` | `##start` / `##end` | starts with `##`, phase=1 |
| `LINE_COMMENT` | `# text` | starts with `#` |
| `LINE_ROOM` | `name x y` | 3 space-separated tokens, phase=1 |
| `LINE_LINK` | `room1-room2` | contains `-`, no spaces, phase=1 or 2 |
| `LINE_EMPTY` | `` | → ERROR |
| `LINE_INVALID` | anything else | → ERROR |

**Phase machine:**
- Phase 0: expecting ant count
- Phase 1: expecting rooms/commands
- Phase 2: expecting links (locked after first link seen)

---

## Error Cases → Print `ERROR` and exit

- More than 1 argument to the program
- Ant count = 0 or starts with 0
- Duplicate room names
- Link to unknown room
- Room name starts with `L` or `#`
- No `##start` or `##end`
- Duplicate `##start` or `##end`
- No path from start to end
- Any invalid / empty line

---

## Memory Management

- `free_farm()` — frees all rooms, their connection lists, input lines, and the farm itself
- `free_room()` — frees room name + connection list nodes (does NOT free room pointers stored as content, since those are owned by `farm->rooms`)
- `free_path_list()` — frees list nodes only, NOT the room pointers they point to
- `free_paths_arr()` — frees all path lists inside the array, then the array itself
- After `run_ants`: free `disj_arr`, `all_arr`, `bfs_set.paths`, `dfs_set.paths`, `farm`

**Key pitfall:** `ft_lstclear(list, NULL)` crashes because `ft_lstdelone` unconditionally calls `(*del)(content)` — always use `free_path_list()` for connection lists.

---

## Forbidden Functions

Only allowed: `malloc`, `free`, `read`, `write`, `exit`, `strerror`, `perror` + libft.

- All output uses `ft_printf` (libft), never `printf`
- All input uses `get_next_line` on `STDIN_FILENO`

---

## Visualizer (`visu-hex`)

Built with SDL2. Reads combined lem-in output (map + movement lines).

**Usage:**
```bash
./lem-in < map.map | ./visu-hex
# or
./lem-in < map.map > out.txt && ./visu-hex out.txt
```

**What it does:** Parses rooms/links/turns from the combined output, displays the graph scaled to the window, animates ants with smooth interpolation, color-codes each ant by ID.

**Controls:** `R` restart, `Q`/`ESC` quit.

---

## Key Numbers / Limits

| Limit | Value |
|---|---|
| Max paths from DFS | 100 |
| Max DFS depth | min(room_count, 500) |
| Expected performance | < 1s on large maps (1000+ rooms) |

---

## Things That Could Trip You Up in Eval

1. **Why two algorithms?** BFS finds node-disjoint paths (optimal for simple graphs). DFS finds all paths so the optimizer can pick a better non-disjoint set when that gives fewer turns.

2. **Why break on len==1 in get_disj_paths?** A direct start→end path has no intermediate rooms to block. Without the break, BFS finds the same path infinitely.

3. **Why DFS not BFS for all-paths?** BFS (FIFO) explores level by level — for dense graphs, the queue explodes in memory and time. DFS (LIFO stack) finds paths quickly and respects the depth limit efficiently.

4. **Why sort paths shortest-first in run_ants?** The turns formula requires paths sorted by length to correctly distribute ants — shorter paths get more capacity.

5. **What if start and end are directly connected AND there are longer paths?** BFS disjoint method breaks after the direct path. DFS all-paths finds everything. eval_strat picks the better result.

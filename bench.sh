#!/usr/bin/env bash

LEMIN="./lem-in-bench"
GEN="./generator_osx"
TMPMAP="/tmp/lemin_bench.map"
N=30  # maps per category

bfs=(0 0 0)
dfs=(0 0 0)
tie=(0 0 0)

run_map() {
    local idx="$1"
    local mapfile="$2"
    local result winner who bfs_t dfs_t
    result=$("$LEMIN" < "$mapfile" 2>&1 1>/dev/null)
    winner=$(echo "$result" | grep "^WINNER:" | head -1)
    [[ -z "$winner" ]] && return
    who=${winner#WINNER:}; who=${who%% *}
    bfs_t=$(echo "$winner" | grep -o 'bfs=[0-9]*' | cut -d= -f2)
    dfs_t=$(echo "$winner" | grep -o 'dfs=[0-9]*' | cut -d= -f2)
    if [[ "$who" == "BFS" && "$bfs_t" == "$dfs_t" ]]; then
        tie[$idx]=$(( ${tie[$idx]} + 1 ))
    elif [[ "$who" == "BFS" ]]; then
        bfs[$idx]=$(( ${bfs[$idx]} + 1 ))
    else
        dfs[$idx]=$(( ${dfs[$idx]} + 1 ))
    fi
}

NAMES=("flow-one" "flow-ten" "flow-thousand")
FLAGS=("--flow-one" "--flow-ten" "--flow-thousand")

echo "Generating and running $N maps per category..."
echo ""

for i in 0 1 2; do
    name="${NAMES[$i]}"
    flag="${FLAGS[$i]}"
    echo -n "[$name x$N] "
    for j in $(seq 1 $N); do
        "$GEN" "$flag" > "$TMPMAP" 2>/dev/null
        [[ $? -ne 0 || ! -s "$TMPMAP" ]] && continue
        run_map "$i" "$TMPMAP"
        echo -n "."
    done
    echo ""
done

rm -f "$TMPMAP"

echo ""
echo "============================================================"
printf "%-16s %8s %8s %8s\n" "Category" "BFS" "DFS" "Tie(equal)"
echo "------------------------------------------------------------"

total_bfs=0; total_dfs=0; total_tie=0

for i in 0 1 2; do
    b=${bfs[$i]}; d=${dfs[$i]}; t=${tie[$i]}
    total_bfs=$(( total_bfs + b ))
    total_dfs=$(( total_dfs + d ))
    total_tie=$(( total_tie + t ))
    printf "%-16s %8d %8d %8d\n" "${NAMES[$i]}" "$b" "$d" "$t"
done

echo "------------------------------------------------------------"
printf "%-16s %8d %8d %8d\n" "TOTAL" "$total_bfs" "$total_dfs" "$total_tie"
echo "============================================================"
echo ""

grand=$(( total_bfs + total_dfs + total_tie ))
if [[ $grand -gt 0 ]]; then
    echo "BFS strictly better: $total_bfs / $grand"
    echo "DFS strictly better: $total_dfs / $grand"
    echo "Tied (same turns):   $total_tie / $grand"
    echo ""
    if [[ $total_dfs -eq 0 ]]; then
        echo "=> DFS NEVER wins. You can safely remove it."
    elif [[ $total_bfs -eq 0 ]]; then
        echo "=> BFS NEVER wins. You can safely remove it."
    else
        echo "=> Both algos contribute. Keep both."
    fi
fi

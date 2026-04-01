# lem-in

42 project – ant farm pathfinding. You get a map (rooms + links), a start and an end, and a bunch of ants. Goal is to get all ants from start to end in as few turns as possible. One ant per room (except start/end), one move per ant per turn.

## build

```bash
make
```

Builds lem-in and the visualizer. You need SDL2 for the visualizer (`brew install sdl2` on mac). Libft is in the repo and gets built automatically.

`make re` does a full clean + build. `make fclean` nukes everything including visu-hex.

## run

```bash
./lem-in < maps/subject3.map
```

Reads the map from stdin, prints it back then the solution (one line per turn, `Lant-room` moves).

Maps are in `maps/`. The subject examples are there, plus some generator maps (gen_flow_*, gen_big_*).

## visualizer

There’s a bonus SDL visualizer that draws the farm and animates the ants.

**Linux:** pipe works fine.

```bash
./lem-in < maps/subject3.map | ./visu-hex
```

**Mac:** piping into the visualizer can hang (something to do with the terminal/window server). Use a file instead:

```bash
./lem-in < maps/subject3.map > /tmp/out.txt
./visu-hex /tmp/out.txt
```

Controls: **R** restart, **Q** or **ESC** quit. It auto-plays.

## how it works (short version)

Parse the map, find paths (BFS for disj paths, DFS for all paths), score them and pick a set that minimizes turns for the given number of ants. Then simulate turn by turn and print the moves. See ALGORITHM.md if you care about the details.

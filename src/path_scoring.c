#include "lem_in.h"

int fits_turns(t_path *paths, int n, int ants, int turns) {
	int placed;
	int i;
	int cap;
	int use;

	placed = 0;
	for (i = 0; i < n; i++) {
		cap = turns - paths[i].len + 1;
		if (cap <= 0)
			continue;
		use = (cap > ants - placed) ? ants - placed : cap;
		placed += use;
		if (placed >= ants)
			return (1);
	}
	return (0);
}

int max_path_len(t_path *paths, int n) {
	int max;
	int i;

	max = 0;
	for (i = 0; i < n; i++) {
		if (paths[i].len > max)
			max = paths[i].len;
	}
	return (max);
}

int min_turns(t_path *paths, int n, int ants) {
	int lo;
	int hi;
	int mid;

	lo = 1;
	hi = ants + max_path_len(paths, n) - 1;
	while (lo < hi) {
		mid = (lo + hi) / 2;
		if (fits_turns(paths, n, ants, mid))
			hi = mid;
		else
			lo = mid + 1;
	}
	return (lo);
}


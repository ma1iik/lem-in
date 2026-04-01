#include "lem_in.h"

/* shared room (not start/end) = conflict */
int paths_conflict(t_path *p1, t_path *p2) {
	t_list *a = p1->path;

	while (a) {
		t_room *r1 = (t_room *)a->content;

		if (r1->is_start || r1->is_end) {
			a = a->next;
			continue;
		}
		t_list *b = p2->path;
		while (b) {
			t_room *r2 = (t_room *)b->content;
			if (r1 == r2)
				return (1);
			b = b->next;
		}
		a = a->next;
	}
	return (0);
}

void count_issues(t_path *paths, int n) {
	int i;
	int j;

	for (i = 0; i < n; i++)
		paths[i].issues = 0;
	for (i = 0; i < n; i++) {
		for (j = i + 1; j < n; j++) {
			if (paths_conflict(&paths[i], &paths[j])) {
				paths[i].issues++;
				paths[j].issues++;
			}
		}
	}
}

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

static int pick_best(t_path *paths, int *avail, int n, t_strategy strat)
{
	int best;
	int i;

	best = -1;
	for (i = 0; i < n; i++) {
		if (!avail[i])
			continue;
		if (best == -1) {
			best = i;
			continue;
		}
		if (strat == STRATEGY_SHORTEST) {
			if (paths[i].len < paths[best].len)
				best = i;
		} else if (strat == STRATEGY_LEAST_CONFLICTS) {
			if (paths[i].issues < paths[best].issues)
				best = i;
			else if (paths[i].issues == paths[best].issues
				&& paths[i].len < paths[best].len)
				best = i;
		}
	}
	return (best);
}

static void drop_conflicts(t_path *paths, int *avail, int n, int sel)
{
	int i;

	for (i = 0; i < n; i++) {
		if (avail[i] && i != sel) {
			if (paths_conflict(&paths[sel], &paths[i]))
				avail[i] = 0;
		}
	}
}

void pick_path_set(t_path *paths, int n, t_path_set *out, t_strategy strat)
{
	int		*avail;
	t_path	*sel;
	int		nsel;
	int		i;
	int		best;

	avail = malloc(sizeof(int) * n);
	sel = malloc(sizeof(t_path) * n);
	nsel = 0;

	for (i = 0; i < n; i++)
		avail[i] = 1;

	for (i = 0; i < n; i++) {
		if (paths[i].issues == 0) {
			sel[nsel] = paths[i];
			nsel++;
			avail[i] = 0;
			drop_conflicts(paths, avail, n, i);
		}
	}

	while (1) {
		best = pick_best(paths, avail, n, strat);
		if (best == -1)
			break;
		sel[nsel] = paths[best];
		nsel++;
		avail[best] = 0;
		drop_conflicts(paths, avail, n, best);
	}

	free(avail);
	out->paths = sel;
	out->count = nsel;
}

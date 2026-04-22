#include "lem_in.h"

typedef struct s_ant {
	int		id;
	int		path_idx;
	int		position;
	int		finished;
}	t_ant;

static void	sort_paths(t_path *paths, int count)
{
	int		i;
	int		j;
	t_path	tmp;

	i = 0;
	while (i < count - 1)
	{
		j = i + 1;
		while (j < count)
		{
			if (paths[j].len < paths[i].len)
			{
				tmp = paths[i];
				paths[i] = paths[j];
				paths[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

static int	*ants_per_path(t_path *paths, int path_count, int total_ants,
	int target_turns)
{
	int	*ants_per_path;
	int	i;
	int	capacity;
	int	remaining;

	ants_per_path = malloc(sizeof(int) * path_count);
	if (!ants_per_path)
		return (NULL);
	remaining = total_ants;
	i = 0;
	while (i < path_count)
	{
		capacity = target_turns - paths[i].len + 1;
		if (capacity < 0)
			capacity = 0;
		if (capacity > remaining)
			capacity = remaining;
		ants_per_path[i] = capacity;
		remaining -= capacity;
		i++;
	}
	return (ants_per_path);
}

static char	*room_at(t_path *path, int pos)
{
	t_list	*node;
	int		i;

	node = path->path;
	i = 0;
	while (node && i < pos)
	{
		node = node->next;
		i++;
	}
	if (node)
		return (((t_room *)node->content)->name);
	return (NULL);
}

static t_ant	*setup_ants(int *n_per_path, int path_count, int total_ants)
{
	t_ant	*ants;
	int		ant_id;
	int		i;
	int		j;

	ants = malloc(sizeof(t_ant) * total_ants);
	if (!ants)
		return (NULL);
	ant_id = 1;
	i = 0;
	while (i < path_count)
	{
		j = 0;
		while (j < n_per_path[i])
		{
			ants[ant_id - 1].id = ant_id;
			ants[ant_id - 1].path_idx = i;
			ants[ant_id - 1].position = 0;
			ants[ant_id - 1].finished = 0;
			ant_id++;
			j++;
		}
		i++;
	}
	return (ants);
}

static int	room_occupied(t_ant *ants, int total_ants, t_path *paths,
	int path_idx, int position)
{
	int		i;
	char	*room_name;
	char	*other_room;

	if (position == 0)
		return (0);
	room_name = room_at(&paths[path_idx], position);
	if (!room_name)
		return (0);
	i = 0;
	while (i < total_ants)
	{
		if (!ants[i].finished && ants[i].position > 0
			&& ants[i].position < paths[ants[i].path_idx].len)
		{
			other_room = room_at(&paths[ants[i].path_idx],
					ants[i].position);
			if (other_room && ft_strcmp(room_name, other_room) == 0)
				return (1);
		}
		i++;
	}
	return (0);
}

static void	print_turn(t_ant *ants, int total_ants, t_path *paths,
	int *moved, int turn)
{
	int		i;
	int		first;
	char	*room_name;

	(void)turn;
	first = 1;
	i = 0;
	while (i < total_ants)
	{
		if (moved[i])
		{
			room_name = room_at(&paths[ants[i].path_idx],
					ants[i].position);
			if (!first)
				ft_printf(" ");
			ft_printf("L%d", ants[i].id);
			ft_printf("-%s", room_name);
			first = 0;
		}
		i++;
	}
	if (!first)
		ft_printf("\n");
}

static int	max_pos(t_ant *ants, int total_ants, int path_idx)
{
	int	mp;
	int	i;

	mp = 0;
	i = 0;
	while (i < total_ants)
	{
		if (ants[i].path_idx == path_idx && !ants[i].finished
			&& ants[i].position > mp)
			mp = ants[i].position;
		i++;
	}
	return (mp);
}

static int	move_ants_on_path(t_ant *ants, int total_ants, t_path *path,
	int path_idx, int *moved)
{
	int	pos;
	int	i;
	int	finished;

	finished = 0;
	pos = max_pos(ants, total_ants, path_idx);
	while (pos > 0)
	{
		i = 0;
		while (i < total_ants)
		{
			if (ants[i].path_idx == path_idx && !ants[i].finished
				&& ants[i].position == pos)
			{
				if (pos + 1 >= path->len)
				{
					ants[i].position++;
					ants[i].finished = 1;
					moved[i] = 1;
					finished++;
				}
				else
				{
					ants[i].position++;
					moved[i] = 1;
				}
			}
			i++;
		}
		pos--;
	}
	return (finished);
}

static int	do_turn(t_ant *ants, int total_ants, t_path *paths,
	int path_count, int turn)
{
	int	*moved;
	int	i;
	int	finished;

	(void)turn;
	moved = malloc(sizeof(int) * total_ants);
	if (!moved)
		return (0);
	i = -1;
	while (++i < total_ants)
		moved[i] = 0;
	finished = 0;
	i = 0;
	while (i < path_count)
	{
		finished += move_ants_on_path(ants, total_ants, &paths[i], i, moved);
		i++;
	}
	i = 0;
	while (i < total_ants)
	{
		if (!ants[i].finished && ants[i].position == 0)
		{
			if (max_pos(ants, total_ants, ants[i].path_idx) == 0
				|| !room_occupied(ants, total_ants, paths,
					ants[i].path_idx, 1))
			{
				ants[i].position = 1;
				moved[i] = 1;
				if (ants[i].position >= paths[ants[i].path_idx].len)
				{
					ants[i].finished = 1;
					finished++;
				}
				while (i + 1 < total_ants
					&& ants[i + 1].path_idx == ants[i].path_idx)
					i++;
			}
		}
		i++;
	}
	print_turn(ants, total_ants, paths, moved, turn);
	free(moved);
	return (finished);
}

void	run_ants(t_path_set *set, int ants)
{
	t_path	*paths;
	int		n;
	int		turns;
	int		*per_path;
	t_ant	*ant_arr;
	int		turn;
	int		done;


	paths = set->paths;
	n = set->count;

	sort_paths(paths, n);
	turns = min_turns(paths, n, ants);
	per_path = ants_per_path(paths, n, ants, turns);
	if (!per_path)
		return ;

	ant_arr = setup_ants(per_path, n, ants);
	if (!ant_arr)
	{
		free(per_path);
		return ;
	}

	turn = 1;
	done = 0;
	while (done < ants)
	{
		done += do_turn(ant_arr, ants, paths, n, turn);
		turn++;
	}
	ft_printf("%d\n", turn - 1);

	free(per_path);
	free(ant_arr);
}

#include "lem_in.h"


static t_path	*lst_to_arr(t_list *lst, int *n)
{
	t_path	*arr;
	t_list	*cur;
	int		i;

	*n = ft_lstsize(lst);
	if (*n == 0)
		return (NULL);
	arr = malloc(sizeof(t_path) * *n);
	if (!arr)
		return (NULL);
	cur = lst;
	i = 0;
	while (cur)
	{
		arr[i] = *((t_path *)cur->content);
		cur = cur->next;
		i++;
	}
	return (arr);
}

static int	eval_strat(t_path *paths, int n, int ants, t_path_set *out, t_strategy strat)
{
	count_issues(paths, n);
	pick_path_set(paths, n, out, strat);
	return (min_turns(out->paths, out->count, ants));
}

int	main(int argc, char **argv)
{
	t_farm		*farm;
	t_list		*disj_lst;
	/* t_list		*all_lst; */
	t_path		*disj_arr;
	/* t_path		*all_arr; */
	int			paths_num_meth1;
	/* int			paths_num_meth2; */
	t_path_set	bfs_set;
	/* t_path_set	dfs_set; */
	/* int			bfs_t; */
	/* int			dfs_t; */

	bfs_set.paths = NULL;
	bfs_set.count = 0;
	/* dfs_set.paths = NULL; */
	/* dfs_set.count = 0; */
	(void)argv;
	if (argc != 1)
	{
		ft_printf("ERROR\n");
		return (1);
	}
	farm = parse_input();
	if (!farm || !farm->start_room || !farm->end_room)
	{
		ft_printf("ERROR\n");
		exit(1);
	}
	disj_lst = get_disj_paths(farm);
	disj_arr = lst_to_arr(disj_lst, &paths_num_meth1);
	ft_lstclear(&disj_lst, free);
	/* all_lst = get_all_paths(farm); */
	/* all_arr = lst_to_arr(all_lst, &paths_num_meth2); */
	/* ft_lstclear(&all_lst, free); */
	if (!disj_arr || paths_num_meth1 == 0)
	{
		free_paths_arr(disj_arr, paths_num_meth1);
		free_farm(farm);
		ft_printf("ERROR\n");
		exit(1);
	}
	eval_strat(disj_arr, paths_num_meth1, farm->ant_count, &bfs_set, STRATEGY_SHORTEST);
	/* bfs_t = eval_strat(disj_arr, paths_num_meth1, farm->ant_count, &bfs_set, STRATEGY_SHORTEST); */
	/* dfs_t = eval_strat(all_arr, paths_num_meth2, farm->ant_count, &dfs_set, STRATEGY_LEAST_CONFLICTS); */
	/* best = (bfs_t <= dfs_t) ? &bfs_set : &dfs_set; */
	dump_input(farm);
	ft_printf("\n");
	run_ants(&bfs_set, farm->ant_count);
	free_paths_arr(disj_arr, paths_num_meth1);
	/* free_paths_arr(all_arr, paths_num_meth2); */
	free(bfs_set.paths);
	/* free(dfs_set.paths); */
	free_farm(farm);
	return (0);
}
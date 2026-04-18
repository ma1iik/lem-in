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



int	main(int argc, char **argv)
{
	t_farm		*farm;
	t_list		*disj_lst;
	t_path		*disj_arr;
	int			paths_num_meth1;
	t_path_set	bfs_set;

	bfs_set.paths = NULL;
	bfs_set.count = 0;
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
	if (!disj_arr || paths_num_meth1 == 0)
	{
		free_paths_arr(disj_arr, paths_num_meth1);
		free_farm(farm);
		ft_printf("ERROR\n");
		exit(1);
	}
	bfs_set.paths = disj_arr;
	bfs_set.count = paths_num_meth1;
	dump_input(farm);
	ft_printf("\n");
	run_ants(&bfs_set, farm->ant_count);
	free_paths_arr(disj_arr, paths_num_meth1);
	free_farm(farm);
	return (0);
}
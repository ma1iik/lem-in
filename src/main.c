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
	t_list		*paths_lst;
	t_path		*paths_arr;
	int			paths_count;
	t_path_set	solution;

	solution.paths = NULL;
	solution.count = 0;
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
	paths_lst = get_optimal_paths(farm);
	paths_arr = lst_to_arr(paths_lst, &paths_count);
	ft_lstclear(&paths_lst, free);
	if (!paths_arr || paths_count == 0)
	{
		free_paths_arr(paths_arr, paths_count);
		free_farm(farm);
		ft_printf("ERROR\n");
		exit(1);
	}
	solution.paths = paths_arr;
	solution.count = paths_count;
	dump_input(farm);
	ft_printf("\n");
	run_ants(&solution, farm->ant_count);
	free_paths_arr(paths_arr, paths_count);
	free_farm(farm);
	return (0);
}
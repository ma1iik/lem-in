#include "lem_in.h"

void free_room(t_room *room) {
	if (!room)
		return;

	free(room->name);
	free_path_list(room->connections);
	free(room);
}

void free_rooms(t_list *rooms){
	t_list *current = rooms;
	t_list *tmp;

	while (current != NULL) {
		tmp = current;
		current = current->next;
		free_room((t_room *)tmp->content);
		free(tmp);
	}
}

void free_farm(t_farm *farm){
	if (!farm)
		return;

	if (farm->input_lines){
		for (int i = 0; i < farm->line_count; i++)
			free(farm->input_lines[i]);
		free(farm->input_lines);
	}

	free_rooms(farm->rooms);
	free(farm);
}

void save_line(t_farm *farm, char *line){
	char **new_lines = malloc(sizeof(char *) * (farm->line_count + 1));
	if (!new_lines) {
		ft_printf("ERROR\n");
		exit(1);
	}
	
	for (int i = 0; i < farm->line_count; i++) {
		new_lines[i] = farm->input_lines[i];
	}

	new_lines[farm->line_count] = ft_strdup(line);
	free(farm->input_lines);
	farm->input_lines = new_lines;
	farm->line_count++;
}

int is_valid_num(char *str) {
	if (!str || !*str)
		return 0;
	
	int i = 0;
	while (str[i]) {
		if (!ft_isdigit(str[i]))
			return 0;
		i++;
	}
	return 1;
}

char *trim_newline(char *str) {
	int len = ft_strlen(str);
	if (len > 0 && str[len - 1] == '\n')
		str[len - 1] = '\0';
	return str;
}

void dump_input(t_farm *farm) {
	for (int i = 0; i < farm->line_count; i++)
		ft_printf("%s\n", farm->input_lines[i]);
}

void	free_path_list(t_list *lst)
{
	t_list	*tmp;

	while (lst)
	{
		tmp = lst;
		lst = lst->next;
		free(tmp);
	}
}

void	free_paths_arr(t_path *arr, int n)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (i < n)
	{
		free_path_list(arr[i].path);
		i++;
	}
	free(arr);
}

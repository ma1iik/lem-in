#include "lem_in.h"

t_farm *init_farm(void){
	t_farm *farm;
	
	farm = malloc(sizeof(t_farm));
	if (!farm)
		return (NULL);
	
	farm->ant_count = 0;
	farm->rooms = NULL;
	farm->start_room = NULL;
	farm->end_room = NULL;
	farm->input_lines = NULL;
	farm->line_count = 0;
	
	return (farm);
}

int valid_room_name(char *name) {
	if (!name)
		return 0;

	if (name[0] == 'L' || name[0] == '#')
		return 0;

	for (int i = 0; name[i]; i++) {
		if (!ft_isalnum(name[i]) && name[i] != '_' && name[i] != '-')
			return 0;
	}
	return 1;
}

int valid_link(char* line) {
	char **links = ft_split(line, '-');

	if (!links)
		return 0;

	int count = 0;
	while (links[count])
		count++;

	int ok = (count == 2 &&
				valid_room_name(links[0]) &&
				valid_room_name(links[1]));

	for (int i = 0; links[i] != NULL; i++)
		free(links[i]);
	free(links);
	return ok;
}

int valid_room_line(char *line) {
	char **room = ft_split(line, ' ');

	if (!room)
		return 0;

	int count = 0;
	while (room[count])
		count++;

	int ok = (count == 3 &&
				valid_room_name(room[0]) &&
				is_valid_num(room[1]) &&
				is_valid_num(room[2]));

	for (int i = 0; room[i] != NULL; i++)
		free(room[i]);
	free(room);
	return ok;
}

t_line_type line_kind(char *line, int *phase) {
	if (*phase == 0 && is_valid_num(line)) {
		if (line[0] == '0')
			return LINE_INVALID;
		return LINE_ANT_COUNT;
	}
	if (!line || ft_strlen(line) == 0)
		return LINE_EMPTY;
	if (valid_room_line(line) && *phase == 1)
		return LINE_ROOM;
	if (valid_link(line) && (*phase == 1 || *phase == 2)) {
		if (*phase == 1)
			*phase = 2;
		return LINE_LINK;
	}
	if (ft_strncmp(line, "##", 2) == 0 && *phase == 1)
		return LINE_COMMAND;
	if (ft_strncmp(line, "#", 1) == 0)
		return LINE_COMMENT;
	return LINE_INVALID;
}

t_room *room_by_name(t_farm *farm, char *name) {
	t_list *current = farm->rooms;

	while (current) {
		t_room *room = (t_room *)current->content;
		if (ft_strcmp(room->name, name) == 0)
			return room;
		current = current->next;
	}
	return NULL;
}

int already_linked(t_room *r1, t_room *r2) {
	t_list *cur = r1->connections;

	while (cur) {
		if ((t_room *)cur->content == r2)
			return 1;
		cur = cur->next;
	}
	return 0;
}

int add_link(t_farm *farm, char *line){
	char **links = ft_split(line, '-');

	if (!links)
		return 0;
	

	char *name1 = links[0];
	char *name2 = links[1];

	if (ft_strcmp(name1, name2) == 0) {
		for (int i = 0; links[i]; i++)
			free(links[i]);
		free(links);
		return 0;
	}

	t_room *room1 = room_by_name(farm, name1);
	t_room *room2 = room_by_name(farm, name2);

	if ((!room1 || !room2) || already_linked(room1, room2)) {
		for (int i = 0; links[i]; i++)
			free(links[i]);
		free(links);
		return 0;
	}

	t_list *node1 = ft_lstnew(room2);
	t_list *node2 = ft_lstnew(room1);

	if (!node1 || !node2) {
		if (node1) free(node1);
		if (node2) free(node2);
		for (int i = 0; links[i]; i++)
			free(links[i]);
		free(links);
		return 0;
	}

	ft_lstadd_back(&room1->connections, node1);
	ft_lstadd_back(&room2->connections, node2);


	for (int i = 0; links[i]; i++)
		free(links[i]);
	free(links);

	return 1;
}

void parse_cmd(char *line, int *next_start, int *next_end){
	if (ft_strcmp(line, "##start") == 0) {
		*next_start = 1;
		*next_end = 0;
	}

	if (ft_strcmp(line, "##end") == 0) {
		*next_end = 1;
		*next_start = 0;
	}
}

int add_room(t_farm *farm, char* line, int next_start, int next_end) {
	char	**parts;
	t_room	*room;

	if (!line)
		return (0);
	if ((next_start && farm->start_room != NULL) ||
		(next_end && farm->end_room != NULL) ||
		(next_start && next_end))
		return (0);
	parts = ft_split(line, ' ');
	if (!parts)
		return (0);
	if (room_by_name(farm, parts[0])) {
		for (int i = 0; parts[i]; i++)
			free(parts[i]);
		free(parts);
		return (0);
	}
	room = malloc(sizeof(t_room));
	if (!room) {
		for (int i = 0; parts[i]; i++)
			free(parts[i]);
		free(parts);
		return (0);
	}
	room->name = ft_strdup(parts[0]);
	room->x = ft_atoi(parts[1]);
	room->y = ft_atoi(parts[2]);
	room->is_start = 0;
	room->is_end = 0;
	room->connections = NULL;
	room->parent = NULL;
	room->score = 0;
	room->visited = 0;
	room->in_queue = 0;
	if (next_start) {
		room->is_start = 1;
		farm->start_room = room;
	}
	else if (next_end) {
		room->is_end = 1;
		farm->end_room = room;
	}
	ft_lstadd_back(&farm->rooms, ft_lstnew((t_room *)room));
	for (int i = 0; parts[i] != NULL; i++)
		free(parts[i]);
	free(parts);
	return (1);
}

t_farm *parse_input(){
	t_farm *farm;
	farm = init_farm();
	if (!farm)
		return (NULL);

	char *line = NULL;
	int phase = 0;
	int next_start = 0;
	int next_end = 0;
	while ((line = get_next_line(STDIN_FILENO)) != NULL){
		line = trim_newline(line);
		t_line_type kind = line_kind(line, &phase);
		
		save_line(farm, line);
		
		switch (kind) {
			case LINE_ANT_COUNT:
				if (phase == 0) {
					phase++;
					if (ft_strlen(line) > 10)
					{
						free(line);
						free_farm(farm);
						ft_printf("ERROR\n");
						exit(1);
					}
					farm->ant_count = ft_atoi(line);
					if (farm->ant_count <= 0)
					{
						free(line);
						free_farm(farm);
						ft_printf("ERROR\n");
						exit(1);
					}
				}
				else {
					free(line);
					free_farm(farm);
					ft_printf("ERROR\n");
					exit(1);
				}
				break;
			case LINE_COMMAND:
				parse_cmd(line, &next_start, &next_end);
				break;
			case LINE_COMMENT:
				break;
			case LINE_ROOM:
				if (!add_room(farm, line, next_start, next_end)) {
					free(line);
					free_farm(farm);
					ft_printf("ERROR\n");
					exit(1);
				}
				next_start = 0;
				next_end = 0;
				break;
			case LINE_LINK:
				if (!add_link(farm, line)){
					free(line);
					free_farm(farm);
					ft_printf("ERROR\n");
					exit(1);
				}
				break;
			case LINE_EMPTY:
			case LINE_INVALID:
				free(line);
				free_farm(farm);
				ft_printf("ERROR\n");
				exit(1);
			}
			free(line);
	}

	if (!farm->start_room || !farm->end_room) {
	free_farm(farm);
	ft_printf("ERROR\n");
	exit(1);
	}
	return farm;
}
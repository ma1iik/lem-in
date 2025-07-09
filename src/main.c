#include "lem_in.h"
int num = 1;

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

int is_room_name_format(char *name) {
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

int is_link_format(char* line) {
	char **links = ft_split(line, '-');

	if (!links)
		return 0;

	int count = 0;
	while (links[count])
		count++;

	int valid = (count == 2 &&
				is_room_name_format(links[0]) &&
				is_room_name_format(links[1]));

	for (int i = 0; links[i] != NULL; i++)
		free(links[i]);
	free(links);
	return valid;
}

int is_room_format(char *line) {
	char **room = ft_split(line, ' ');

	if (!room)
		return 0;

	int count = 0;
	while (room[count])
		count++;

	int valid = (count == 3 &&
				is_room_name_format(room[0]) &&
				(ft_atoi(room[1]) >= 0) &&
				(ft_atoi(room[2]) >= 0));

	for (int i = 0; room[i] != NULL; i++)
		free(room[i]);
	free(room);
	return valid;
}

t_line_type get_line_type(char *line, int *passing_phase) {
	if (*passing_phase == 0 && ft_atoi(line) > 0)
		return LINE_ANT_COUNT;
	if (!line || ft_strlen(line) == 0)
		return LINE_EMPTY;
	if (is_room_format(line) && *passing_phase == 1)
		return LINE_ROOM;
	if (is_link_format(line) && (*passing_phase == 1 || *passing_phase == 2)) {
		if (*passing_phase == 1)
			*passing_phase = 2;
		return LINE_LINK;
	}
	if (ft_strncmp(line, "##", 2) == 0 && *passing_phase == 1)
        return LINE_COMMAND;
	if (ft_strncmp(line, "#", 1) == 0)
        return LINE_COMMENT;
	return LINE_INVALID;
}

t_room *find_room_by_name(t_farm *farm, char *name) {
	t_list *current = farm->rooms;

	while (current) {
		t_room *room = (t_room *)current->content;
		if (ft_strcmp(room->name, name) == 0)
			return room;
		current = current->next;
	}
	return NULL;
}

int room_already_connected(t_room *room1, t_room *room2) {
	t_list *current = room1->connections;

	while (current) {
		if ((t_room *)current->content == room2)
			return 1;
		current = current->next;
	}
	return 0;
}

int create_link(t_farm *farm, char *line){
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

	t_room *room1 = find_room_by_name(farm, name1);
    t_room *room2 = find_room_by_name(farm, name2);

	if ((!room1 || !room2) || (room_already_connected(room1, room2))) {
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
	ft_lstadd_back(room2->connections, node2);

	for (int i = 0; links[i]; i++)
		free(links[i]);
	free(links);

	return 1;
}

void add_command(char *line, int *next_is_start, int *next_is_end){
	if (ft_strcmp(line, "##start") == 0) {
		*next_is_start = 1;
		*next_is_end = 0;
    }

	if (ft_strcmp(line, "##end") == 0) {
		*next_is_end = 1;
		*next_is_start = 0;
	}
}

void create_room(t_farm *farm, char* line, int next_is_start, int next_is_end) {
	if (!line)
		exit(1);

	if ((next_is_start && farm->start_room != NULL) ||
		(next_is_end && farm->end_room != NULL) ||
		(next_is_start && next_is_end)){
			exit(1);
	}

	char **parts = ft_split(line, ' ');

	t_room *room = malloc(sizeof(t_room) * 1);
	if (!room)
		exit(1);
	room->name = ft_strdup(parts[0]);
	room->x = ft_atoi(parts[1]);
	room->y = ft_atoi(parts[2]);
	room->is_start = 0;
	room->is_end = 0;
	room->connections = NULL;
	if (next_is_start){
		room->is_start = 1;
		farm->start_room = room;
	}
	else if (next_is_end){
		room->is_end = 1;
		farm->end_room = room;
	}

	ft_lstadd_back(&farm->rooms, ft_lstnew(room));

	for (int i = 0; parts[i] != NULL; i++)
		free(parts[i]);
	free(parts);
}

t_farm *parse_input(){
	t_farm *farm;
	farm = init_farm();
	if (!farm)
		return (NULL);

	char *line = NULL;
	int passing_phase = 0;
	int next_is_start = 0;
	int next_is_end = 0;
	while ((line = get_next_line(STDIN_FILENO)) != NULL){
		t_line_type line_type = get_line_type(line, &passing_phase);
		
		store_input_lines(farm, line);
		
		switch (line_type) {
			case LINE_ANT_COUNT:
				if (passing_phase == 0) {
					passing_phase++;
					farm->ant_count = ft_atoi(line);
				}
				else
					exit(1);
				break;
			case LINE_COMMAND:
				add_command(line, &next_is_start, &next_is_end);
				break;
			case LINE_COMMENT:
				break;
			case LINE_ROOM:
				create_room(farm, line, next_is_start, next_is_end);
				break;
			case LINE_LINK:
				if (!create_link(farm, line)){
					free(line);
					free_farm(farm);
					exit(1);
				}
				break;
			case LINE_EMPTY:
			case LINE_INVALID:
				free(line);
				free_farm(farm);
				exit(1);
			}
			free(line);
	}
	for(int i = 0; i < farm->line_count; i++)
		ft_printf("%s", farm->input_lines[i]);
	return farm;
}

int main(int argc, char **argv){
	(void)argv;
	if (argc != 1){
		ft_printf("ERROR: No arguments allowed\n");
		return 1;
	}
	// if (isatty(0)) {
	// 	ft_printf("Usage: %s < input_file.map\n", argv[0]);
	// 	ft_printf("   or: echo \"input\" | %s\n", argv[0]);
	// 	return 1;
	// }
	t_farm *farm = parse_input();
	if (!farm)
		exit(0);
	return 0;
}
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

t_farm *parse_input(){
	t_farm *farm;
	farm = init_farm();
	if (!farm)
		return (NULL);

	int passing_phase = 0;
	int next_is_start = 0;
	int next_is_end = 0;
	// char *line = get_next_line(STDIN_FILENO);
	// if (ft_atoi(line) <= 0){
	// 	free_farm(farm);
	// 	return (NULL);
	// }
	// farm->ant_count = ft_atoi(line);
	// store_input_lines(farm, line);
	// if (!farm->input_lines){
	// 	free_farm(farm);
	// 	return (NULL);
	// }
	// free(line);
	while ((line = get_next_line(STDIN_FILENO)) != NULL){
		t_line_type line_type = get_line_type(line, passing_phase)
		switch (line_type) {
			case LINE_ANT_COUNT:
			case LINE_COMMAND:
			case LINE_COMMENT:
			case LINE_ROOM:
			case LINE_LINK:
			case LINE_EMPTY:
			case LINE_INVALID:
			store_input_lines(farm, line);
			num++;
			free(line);
		}
	}
	for(int i = 0; i < farm->line_count; i++)
		printf("%s", farm->input_lines[i]);
	return 0;
}

int main(int argc, char **argv){
	(void)argv;
	if (argc != 1){
		printf("ERROR: No arguments allowed\n");
		return 1;
	}
	if (isatty(0)) {
		printf("Usage: %s < input_file.map\n", argv[0]);
		printf("   or: echo \"input\" | %s\n", argv[0]);
		return 1;
	}
	t_farm *farm = parse_input();
	if (!farm)
		exit(0);
	return 0;
}
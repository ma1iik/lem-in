#ifndef LEM_IN_H
# define LEM_IN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libft.h"
#include <limits.h>

typedef struct s_room {
    char *name;
    int x, y;
    int is_start;
    int is_end;
    t_list *connections;
    struct s_room *parent;
    int score;
    int issues;
} t_room;

typedef struct s_farm {
    int ant_count;
    t_list *rooms;
    t_room *start_room;
    t_room *end_room;
    char **input_lines;
    int line_count;
} t_farm;

typedef struct s_path {
    int len;
    int score;
    int issues;
    t_list *path;
} t_path;

typedef struct s_path_set {
	t_path *paths;
	int count;
} t_path_set;

typedef enum e_line_type {
    LINE_ANT_COUNT,
    LINE_COMMAND,
    LINE_COMMENT,
    LINE_ROOM,
    LINE_LINK,
    LINE_EMPTY,
    LINE_INVALID
} t_line_type;

void        store_input_lines(t_farm *farm, char *line);
void        free_farm(t_farm *farm);
void        free_rooms(t_list *rooms);
void        free_single_room(t_room *room);
int         is_valid_num(char *str);
char        *trim_newline(char *str);

t_farm		*init_farm(void);
int			is_room_name_format(char *name);
int			is_link_format(char *line);
int			is_room_format(char *line);
t_line_type	get_line_type(char *line, int *passing_phase);
t_room		*find_room_by_name(t_farm *farm, char *name);
int			room_already_connected(t_room *room1, t_room *room2);
int			create_link(t_farm *farm, char *line);
void		add_command(char *line, int *next_is_start, int *next_is_end);
void		create_room(t_farm *farm, char *line, int next_is_start, int next_is_end);
t_farm		*parse_input(void);

int         is_visited(t_room *neighbour, t_list *visited);
void        del_from_visited(t_room *del, t_list **visited);
void        add_path(t_path **paths, t_path add);
t_path      get_path(t_room *room);
void        dfs(t_room *cur, t_farm *farm, t_list *visited, t_path **all_paths);



#endif
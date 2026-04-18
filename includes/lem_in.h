
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
    int score;
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

void        save_line(t_farm *farm, char *line);
void        free_farm(t_farm *farm);
void        dump_input(t_farm *farm);
void        free_rooms(t_list *rooms);
void        free_room(t_room *room);
int         is_valid_num(char *str);
char        *trim_newline(char *str);
void	    free_paths_arr(t_path *arr, int n);
void	    free_path_list(t_list *lst);


t_farm		*init_farm(void);
int			valid_room_name(char *name);
int			valid_link(char *line);
int			valid_room_line(char *line);
t_line_type	line_kind(char *line, int *phase);
t_room		*room_by_name(t_farm *farm, char *name);
int			already_linked(t_room *r1, t_room *r2);
int			add_link(t_farm *farm, char *line);
void		parse_cmd(char *line, int *next_start, int *next_end);
int		    add_room(t_farm *farm, char *line, int next_start, int next_end);
t_farm		*parse_input(void);

t_list		*get_disj_paths(t_farm *farm);


int			fits_turns(t_path *paths, int n, int ants, int turns);
int			max_path_len(t_path *paths, int n);
int			min_turns(t_path *paths, int n, int ants);

void		run_ants(t_path_set *set, int ants);

#endif
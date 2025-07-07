#ifndef LEM_IN_H
# define LEM_IN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libft.h"

typedef struct s_room {
    char *name;
    int x, y;
    int is_start;
    int is_end;
    t_list *connections;
} t_room;

typedef struct s_farm {
    int ant_count;
    t_list *rooms;
    t_room *start_room;
    t_room *end_room;
    char **input_lines;
    int line_count;
} t_farm;

typedef enum e_line_type {
    LINE_ANT_COUNT,
    LINE_COMMAND,
    LINE_COMMENT,
    LINE_ROOM,
    LINE_LINK,
    LINE_EMPTY,
    LINE_INVALID
} t_line_type;

void store_input_lines(t_farm *farm, char *line);
void free_farm(t_farm *farm);
void free_rooms(t_list *rooms);
void free_single_room(t_room *room);


#endif
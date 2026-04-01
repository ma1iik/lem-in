#ifndef VISUALIZER_H
# define VISUALIZER_H

# include <SDL.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <math.h>

/* Window settings */
# define WIN_WIDTH 1200
# define WIN_HEIGHT 800
# define MAX_ROOM_RADIUS 20
# define MIN_ROOM_RADIUS 3
# define PADDING 60

/* Colors */
# define COLOR_BG_R 30
# define COLOR_BG_G 30
# define COLOR_BG_B 40

# define COLOR_ROOM_R 100
# define COLOR_ROOM_G 100
# define COLOR_ROOM_B 120

# define COLOR_START_R 50
# define COLOR_START_G 200
# define COLOR_START_B 100

# define COLOR_END_R 200
# define COLOR_END_G 80
# define COLOR_END_B 80

# define COLOR_LINK_R 60
# define COLOR_LINK_G 60
# define COLOR_LINK_B 80

# define COLOR_TEXT_R 220
# define COLOR_TEXT_G 220
# define COLOR_TEXT_B 220

/* Animation settings */
# define ANIMATION_SPEED 0.5f
# define FPS 60

/* Maximum limits */
# define MAX_ROOMS 10000
# define MAX_LINKS 50000
# define MAX_ANTS 10000
# define MAX_TURNS 10000
# define MAX_MOVES_PER_TURN 10000
# define MAX_NAME_LEN 256
# define MAX_LINE_LEN 4096

/* Room structure */
typedef struct s_vroom {
    char    name[MAX_NAME_LEN];
    int     x;
    int     y;
    float   draw_x;
    float   draw_y;
    int     is_start;
    int     is_end;
}   t_vroom;

/* Link structure */
typedef struct s_vlink {
    int     room1_idx;
    int     room2_idx;
}   t_vlink;

/* Single ant move */
typedef struct s_move {
    int     ant_id;
    int     to_room_idx;
}   t_move;

/* Turn containing multiple moves */
typedef struct s_turn {
    t_move  moves[MAX_MOVES_PER_TURN];
    int     move_count;
}   t_turn;

/* Ant state for animation */
typedef struct s_ant_state {
    int     current_room;
    int     target_room;
    float   x;
    float   y;
    float   progress;
    int     active;
    int     finished;
}   t_ant_state;

/* Main farm data structure */
typedef struct s_vfarm {
    int         ant_count;
    t_vroom     rooms[MAX_ROOMS];
    int         room_count;
    t_vlink     links[MAX_LINKS];
    int         link_count;
    t_turn      turns[MAX_TURNS];
    int         turn_count;
    int         start_idx;
    int         end_idx;
    float       scale;
    float       offset_x;
    float       offset_y;
    int         room_radius;
    int         ant_radius;
}   t_vfarm;

/* Visualizer state */
typedef struct s_visualizer {
    SDL_Window      *window;
    SDL_Renderer    *renderer;
    t_vfarm         *farm;
    t_ant_state     *ants;
    int             current_turn;
    float           turn_progress;
    int             paused;
    int             running;
    float           speed;
}   t_visualizer;

/* Parser functions (parser.c) */
t_vfarm     *parse_input(const char *filename);
int         find_room_index(t_vfarm *farm, const char *name);

/* Render functions (render.c) */
void        calculate_scale(t_vfarm *farm);
void        render_frame(t_visualizer *vis);
void        draw_circle(SDL_Renderer *renderer, int cx, int cy, int radius);
void        draw_filled_circle(SDL_Renderer *renderer, int cx, int cy, int radius);

/* Animation functions (animation.c) */
void        init_ant_states(t_visualizer *vis);
void        update_animation(t_visualizer *vis, float delta_time);
void        apply_turn(t_visualizer *vis);
void        reset_animation(t_visualizer *vis);

#endif

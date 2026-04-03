#include "visualizer.h"

static int init_sdl(t_visualizer *vis)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return (0);
    }

    vis->window = SDL_CreateWindow("Lem-in Visualizer",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   WIN_WIDTH, WIN_HEIGHT,
                                   SDL_WINDOW_SHOWN);
    if (!vis->window)
    {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return (0);
    }

    vis->renderer = SDL_CreateRenderer(vis->window, -1,
                                       SDL_RENDERER_ACCELERATED |
                                       SDL_RENDERER_PRESENTVSYNC);
    if (!vis->renderer)
    {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(vis->window);
        SDL_Quit();
        return (0);
    }

    SDL_RaiseWindow(vis->window);
    return (1);
}

static void handle_keydown(t_visualizer *vis, SDL_Keycode key)
{
    switch (key)
    {
        case SDLK_r:
            reset_animation(vis);
            vis->paused = 0;
            break;
        case SDLK_ESCAPE:
        case SDLK_q:
            vis->running = 0;
            break;
        default:
            break;
    }
}

static void handle_events(t_visualizer *vis)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                vis->running = 0;
                break;
            case SDL_KEYDOWN:
                handle_keydown(vis, event.key.keysym.sym);
                break;
        }
    }
}

static void cleanup(t_visualizer *vis)
{
    if (vis->ants)
        free(vis->ants);
    if (vis->farm)
        free(vis->farm);
    if (vis->renderer)
        SDL_DestroyRenderer(vis->renderer);
    if (vis->window)
        SDL_DestroyWindow(vis->window);
    SDL_Quit();
}

static void print_controls(void)
{
    printf("\n=== Lem-in Visualizer ===\n");
    printf("Controls:\n");
    printf("  R     - Restart\n");
    printf("  Q/ESC - Quit\n");
    printf("========================\n\n");
}

int main(int argc, char **argv)
{
    t_visualizer vis;
    Uint32 last_time;
    Uint32 current_time;
    float delta_time;
    const char *filename;

    if (argc >= 2)
        filename = argv[1];
    else
        filename = NULL;

    memset(&vis, 0, sizeof(t_visualizer));
    vis.speed = ANIMATION_SPEED;
    vis.paused = 0;
    vis.running = 1;

    vis.farm = parse_input(filename);
    if (!vis.farm || vis.farm->room_count == 0)
    {
        fprintf(stderr, "Error: Failed to parse input or no rooms found\n");
        return (1);
    }

    if (vis.farm->ant_count == 0)
    {
        fprintf(stderr, "Error: No ants specified\n");
        free(vis.farm);
        return (1);
    }

    if (vis.farm->start_idx < 0 || vis.farm->end_idx < 0)
    {
        fprintf(stderr, "Error: Start or end room not found\n");
        free(vis.farm);
        return (1);
    }

    if (!init_sdl(&vis))
    {
        free(vis.farm);
        return (1);
    }

    print_controls();

    calculate_scale(vis.farm);

    init_ant_states(&vis);
    if (vis.farm->turn_count > 0)
        apply_turn(&vis);

    last_time = SDL_GetTicks();
    while (vis.running)
    {
        current_time = SDL_GetTicks();
        delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;

        handle_events(&vis);

        update_animation(&vis, delta_time);

        render_frame(&vis);

        SDL_Delay(1000 / FPS);
    }

    cleanup(&vis);
    return (0);
}

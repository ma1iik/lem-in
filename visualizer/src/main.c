#include "visualizer.h"

static float clamp_speed(float v)
{
    if (v < 0.1f)
        return (0.1f);
    if (v > 4.0f)
        return (4.0f);
    return (v);
}

static int init_sdl(t_visualizer *vis)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        ft_putstr_fd("SDL init failed: ", 2);
        ft_putstr_fd((char *)SDL_GetError(), 2);
        ft_putstr_fd("\n", 2);
        return (0);
    }

    vis->window = SDL_CreateWindow("Lem-in Visualizer",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   WIN_WIDTH, WIN_HEIGHT,
                                   SDL_WINDOW_SHOWN);
    if (!vis->window)
    {
        ft_putstr_fd("Window creation failed: ", 2);
        ft_putstr_fd((char *)SDL_GetError(), 2);
        ft_putstr_fd("\n", 2);
        SDL_Quit();
        return (0);
    }

    vis->renderer = SDL_CreateRenderer(vis->window, -1,
                                       SDL_RENDERER_ACCELERATED |
                                       SDL_RENDERER_PRESENTVSYNC);
    if (!vis->renderer)
    {
        ft_putstr_fd("Renderer creation failed: ", 2);
        ft_putstr_fd((char *)SDL_GetError(), 2);
        ft_putstr_fd("\n", 2);
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
        case SDLK_SPACE:
            vis->paused = !vis->paused;
            break;
        case SDLK_n:
            if (vis->paused)
                step_one_turn(vis);
            break;
        case SDLK_EQUALS:
        case SDLK_PLUS:
            vis->speed = clamp_speed(vis->speed + 0.2f);
            break;
        case SDLK_MINUS:
            vis->speed = clamp_speed(vis->speed - 0.2f);
            break;
        case SDLK_0:
            vis->speed = ANIMATION_SPEED;
            break;
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
    ft_printf("\n=== Lem-in Visualizer ===\n");
    ft_printf("Controls:\n");
    ft_printf("  SPACE - Pause/Resume\n");
    ft_printf("  N     - Step one turn (while paused)\n");
    ft_printf("  +/-   - Speed down/up\n");
    ft_printf("  0     - Reset speed\n");
    ft_printf("  R     - Restart\n");
    ft_printf("  Q/ESC - Quit\n");
    ft_printf("========================\n\n");
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

    ft_bzero(&vis, sizeof(t_visualizer));
    vis.speed = ANIMATION_SPEED;
    vis.paused = 0;
    vis.running = 1;

    vis.farm = parse_input(filename);
    if (!vis.farm || vis.farm->room_count == 0)
    {
        ft_putstr_fd("Error: Failed to parse input or no rooms found\n", 2);
        return (1);
    }

    if (vis.farm->ant_count == 0)
    {
        ft_putstr_fd("Error: No ants specified\n", 2);
        free(vis.farm);
        return (1);
    }

    if (vis.farm->start_idx < 0 || vis.farm->end_idx < 0)
    {
        ft_putstr_fd("Error: Start or end room not found\n", 2);
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

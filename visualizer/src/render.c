#include "visualizer.h"

/*
** Draw a circle outline using midpoint circle algorithm
*/
void draw_circle(SDL_Renderer *renderer, int cx, int cy, int radius)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        SDL_RenderDrawPoint(renderer, cx + x, cy + y);
        SDL_RenderDrawPoint(renderer, cx + y, cy + x);
        SDL_RenderDrawPoint(renderer, cx - y, cy + x);
        SDL_RenderDrawPoint(renderer, cx - x, cy + y);
        SDL_RenderDrawPoint(renderer, cx - x, cy - y);
        SDL_RenderDrawPoint(renderer, cx - y, cy - x);
        SDL_RenderDrawPoint(renderer, cx + y, cy - x);
        SDL_RenderDrawPoint(renderer, cx + x, cy - y);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

/*
** Draw a filled circle
*/
void draw_filled_circle(SDL_Renderer *renderer, int cx, int cy, int radius)
{
    int x, y;
    int r2 = radius * radius;

    for (y = -radius; y <= radius; y++)
    {
        for (x = -radius; x <= radius; x++)
        {
            if (x * x + y * y <= r2)
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
        }
    }
}

/*
** Calculate minimum distance between any two connected rooms (in screen coords)
*/
static float calc_min_link_distance(t_vfarm *farm)
{
    float min_dist = 999999.0f;
    int i;
    float dx, dy, dist;
    int r1, r2;

    i = 0;
    while (i < farm->link_count)
    {
        r1 = farm->links[i].room1_idx;
        r2 = farm->links[i].room2_idx;
        dx = farm->rooms[r1].draw_x - farm->rooms[r2].draw_x;
        dy = farm->rooms[r1].draw_y - farm->rooms[r2].draw_y;
        dist = sqrtf(dx * dx + dy * dy);
        if (dist > 0 && dist < min_dist)
            min_dist = dist;
        i++;
    }
    return min_dist;
}

/*
** Calculate scale and offset to fit all rooms in window
*/
void calculate_scale(t_vfarm *farm)
{
    int min_x, max_x, min_y, max_y;
    int i;
    float scale_x, scale_y;
    int range_x, range_y;
    float min_dist;

    if (farm->room_count == 0)
        return;

    /* Find bounding box */
    min_x = max_x = farm->rooms[0].x;
    min_y = max_y = farm->rooms[0].y;

    i = 1;
    while (i < farm->room_count)
    {
        if (farm->rooms[i].x < min_x)
            min_x = farm->rooms[i].x;
        if (farm->rooms[i].x > max_x)
            max_x = farm->rooms[i].x;
        if (farm->rooms[i].y < min_y)
            min_y = farm->rooms[i].y;
        if (farm->rooms[i].y > max_y)
            max_y = farm->rooms[i].y;
        i++;
    }

    /* Calculate scale */
    range_x = max_x - min_x;
    range_y = max_y - min_y;

    if (range_x == 0)
        range_x = 1;
    if (range_y == 0)
        range_y = 1;

    scale_x = (float)(WIN_WIDTH - 2 * PADDING) / range_x;
    scale_y = (float)(WIN_HEIGHT - 2 * PADDING - 80) / range_y;

    farm->scale = (scale_x < scale_y) ? scale_x : scale_y;

    /* Calculate offset to center */
    farm->offset_x = PADDING + ((WIN_WIDTH - 2 * PADDING) - range_x * farm->scale) / 2
                     - min_x * farm->scale;
    farm->offset_y = PADDING + 40 + ((WIN_HEIGHT - 2 * PADDING - 80) - range_y * farm->scale) / 2
                     - min_y * farm->scale;

    /* Calculate draw positions for each room */
    i = 0;
    while (i < farm->room_count)
    {
        farm->rooms[i].draw_x = farm->rooms[i].x * farm->scale + farm->offset_x;
        farm->rooms[i].draw_y = farm->rooms[i].y * farm->scale + farm->offset_y;
        i++;
    }

    /* Calculate adaptive room radius based on minimum link distance */
    min_dist = calc_min_link_distance(farm);
    
    /* Room radius should be less than half the minimum distance between connected rooms */
    farm->room_radius = (int)(min_dist * 0.35f);
    
    /* Clamp to reasonable values */
    if (farm->room_radius > MAX_ROOM_RADIUS)
        farm->room_radius = MAX_ROOM_RADIUS;
    if (farm->room_radius < MIN_ROOM_RADIUS)
        farm->room_radius = MIN_ROOM_RADIUS;
    
    /* Ant radius is proportional to room radius */
    farm->ant_radius = farm->room_radius * 2 / 5;
    if (farm->ant_radius < 2)
        farm->ant_radius = 2;
}

/*
** Draw a thick line between two points
*/
static void draw_thick_line(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int thickness)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrtf(dx * dx + dy * dy);
    float nx, ny;
    int i;

    if (len == 0)
        return;

    /* Normalized perpendicular vector */
    nx = -dy / len;
    ny = dx / len;

    /* Draw multiple parallel lines */
    for (i = -thickness / 2; i <= thickness / 2; i++)
    {
        SDL_RenderDrawLine(renderer,
                          x1 + (int)(nx * i), y1 + (int)(ny * i),
                          x2 + (int)(nx * i), y2 + (int)(ny * i));
    }
}

/*
** Draw all links between rooms
*/
static void draw_links(t_visualizer *vis)
{
    int i;
    t_vfarm *farm;
    int r1, r2;
    int thickness;

    farm = vis->farm;
    
    /* Link thickness based on room density */
    thickness = farm->room_radius / 3;
    if (thickness < 1)
        thickness = 1;
    if (thickness > 3)
        thickness = 3;

    SDL_SetRenderDrawColor(vis->renderer, COLOR_LINK_R, COLOR_LINK_G, 
                           COLOR_LINK_B, 255);

    i = 0;
    while (i < farm->link_count)
    {
        r1 = farm->links[i].room1_idx;
        r2 = farm->links[i].room2_idx;
        if (thickness <= 1)
        {
            SDL_RenderDrawLine(vis->renderer,
                              (int)farm->rooms[r1].draw_x,
                              (int)farm->rooms[r1].draw_y,
                              (int)farm->rooms[r2].draw_x,
                              (int)farm->rooms[r2].draw_y);
        }
        else
        {
            draw_thick_line(vis->renderer,
                           (int)farm->rooms[r1].draw_x,
                           (int)farm->rooms[r1].draw_y,
                           (int)farm->rooms[r2].draw_x,
                           (int)farm->rooms[r2].draw_y,
                           thickness);
        }
        i++;
    }
}

/*
** Draw all rooms
*/
static void draw_rooms(t_visualizer *vis)
{
    int i;
    t_vfarm *farm;
    int radius;
    int special_radius;

    farm = vis->farm;
    radius = farm->room_radius;
    
    /* Start/end rooms should always be visible - minimum 8 pixels */
    special_radius = radius * 2;
    if (special_radius < 8)
        special_radius = 8;
    if (special_radius > 25)
        special_radius = 25;

    /* First pass: draw normal rooms */
    i = 0;
    while (i < farm->room_count)
    {
        if (!farm->rooms[i].is_start && !farm->rooms[i].is_end)
        {
            /* Brighter color for normal rooms */
            SDL_SetRenderDrawColor(vis->renderer, 140, 140, 160, 255);
            draw_filled_circle(vis->renderer,
                              (int)farm->rooms[i].draw_x,
                              (int)farm->rooms[i].draw_y,
                              radius);
        }
        i++;
    }

    /* Second pass: draw start/end rooms on top (so they're always visible) */
    i = 0;
    while (i < farm->room_count)
    {
        if (farm->rooms[i].is_start)
        {
            /* Bright green for start */
            SDL_SetRenderDrawColor(vis->renderer, 50, 255, 100, 255);
            draw_filled_circle(vis->renderer,
                              (int)farm->rooms[i].draw_x,
                              (int)farm->rooms[i].draw_y,
                              special_radius);
            /* White outline */
            SDL_SetRenderDrawColor(vis->renderer, 255, 255, 255, 255);
            draw_circle(vis->renderer,
                       (int)farm->rooms[i].draw_x,
                       (int)farm->rooms[i].draw_y,
                       special_radius);
        }
        else if (farm->rooms[i].is_end)
        {
            /* Bright red for end */
            SDL_SetRenderDrawColor(vis->renderer, 255, 80, 80, 255);
            draw_filled_circle(vis->renderer,
                              (int)farm->rooms[i].draw_x,
                              (int)farm->rooms[i].draw_y,
                              special_radius);
            /* White outline */
            SDL_SetRenderDrawColor(vis->renderer, 255, 255, 255, 255);
            draw_circle(vis->renderer,
                       (int)farm->rooms[i].draw_x,
                       (int)farm->rooms[i].draw_y,
                       special_radius);
        }
        i++;
    }
}

/*
** Generate a color for an ant based on its ID
*/
static void get_ant_color(int ant_id, Uint8 *r, Uint8 *g, Uint8 *b)
{
    /* Generate distinct colors using golden ratio */
    float hue = (float)((ant_id * 137) % 360) / 360.0f;
    float sat = 0.7f;
    float val = 0.9f;
    
    int h_i = (int)(hue * 6);
    float f = hue * 6 - h_i;
    float p = val * (1 - sat);
    float q = val * (1 - f * sat);
    float t = val * (1 - (1 - f) * sat);

    float r_f, g_f, b_f;
    switch (h_i % 6)
    {
        case 0: r_f = val; g_f = t; b_f = p; break;
        case 1: r_f = q; g_f = val; b_f = p; break;
        case 2: r_f = p; g_f = val; b_f = t; break;
        case 3: r_f = p; g_f = q; b_f = val; break;
        case 4: r_f = t; g_f = p; b_f = val; break;
        default: r_f = val; g_f = p; b_f = q; break;
    }

    *r = (Uint8)(r_f * 255);
    *g = (Uint8)(g_f * 255);
    *b = (Uint8)(b_f * 255);
}

/*
** Draw all ants
*/
static void draw_ants(t_visualizer *vis)
{
    int i;
    Uint8 r, g, b;
    int radius;

    if (!vis->ants)
        return;

    /* Ants should be visible - minimum 4 pixels */
    radius = vis->farm->ant_radius;
    if (radius < 4)
        radius = 4;

    i = 0;
    while (i < vis->farm->ant_count)
    {
        if (vis->ants[i].active && !vis->ants[i].finished)
        {
            get_ant_color(i + 1, &r, &g, &b);
            SDL_SetRenderDrawColor(vis->renderer, r, g, b, 255);
            draw_filled_circle(vis->renderer,
                              (int)vis->ants[i].x,
                              (int)vis->ants[i].y,
                              radius);
            /* Add white outline for visibility */
            SDL_SetRenderDrawColor(vis->renderer, 255, 255, 255, 255);
            draw_circle(vis->renderer,
                       (int)vis->ants[i].x,
                       (int)vis->ants[i].y,
                       radius);
        }
        i++;
    }
}

/*
** Draw UI elements (turn counter, controls info)
*/
static void draw_ui(t_visualizer *vis)
{
    SDL_Rect bar;

    /* Draw top bar background */
    SDL_SetRenderDrawColor(vis->renderer, 40, 40, 50, 255);
    bar.x = 0;
    bar.y = 0;
    bar.w = WIN_WIDTH;
    bar.h = 40;
    SDL_RenderFillRect(vis->renderer, &bar);
}

/*
** Main render function
*/
void render_frame(t_visualizer *vis)
{
    /* Clear screen */
    SDL_SetRenderDrawColor(vis->renderer, COLOR_BG_R, COLOR_BG_G, 
                           COLOR_BG_B, 255);
    SDL_RenderClear(vis->renderer);

    /* Draw scene */
    draw_links(vis);
    draw_rooms(vis);
    draw_ants(vis);
    draw_ui(vis);

    /* Present */
    SDL_RenderPresent(vis->renderer);
}

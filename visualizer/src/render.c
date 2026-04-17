#include "visualizer.h"

static void draw_rect_outline(SDL_Renderer *renderer, SDL_Rect *r)
{
    SDL_RenderDrawLine(renderer, r->x, r->y, r->x + r->w, r->y);
    SDL_RenderDrawLine(renderer, r->x, r->y + r->h, r->x + r->w, r->y + r->h);
    SDL_RenderDrawLine(renderer, r->x, r->y, r->x, r->y + r->h);
    SDL_RenderDrawLine(renderer, r->x + r->w, r->y, r->x + r->w, r->y + r->h);
}

static void draw_seg_digit(SDL_Renderer *renderer, int x, int y, int size, int d)
{
    static const int segmask[10] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 111};
    int t;
    int h;
    int m;
    int s;

    if (d < 0 || d > 9)
        return;
    t = size;
    h = size * 5;
    m = h / 2;
    s = segmask[d];
    if (s & 1)
        SDL_RenderFillRect(renderer, &(SDL_Rect){x, y, size * 3, t});
    if (s & 2)
        SDL_RenderFillRect(renderer, &(SDL_Rect){x + size * 2, y, t, m});
    if (s & 4)
        SDL_RenderFillRect(renderer, &(SDL_Rect){x + size * 2, y + m, t, m});
    if (s & 8)
        SDL_RenderFillRect(renderer, &(SDL_Rect){x, y + h - t, size * 3, t});
    if (s & 16)
        SDL_RenderFillRect(renderer, &(SDL_Rect){x, y + m, t, m});
    if (s & 32)
        SDL_RenderFillRect(renderer, &(SDL_Rect){x, y, t, m});
    if (s & 64)
        SDL_RenderFillRect(renderer, &(SDL_Rect){x, y + m - t / 2, size * 3, t});
}

static void draw_number(SDL_Renderer *renderer, int x, int y, int size, int v)
{
    char buf[32];
    int i;

    snprintf(buf, sizeof(buf), "%d", v);
    i = 0;
    while (buf[i])
    {
        if (buf[i] >= '0' && buf[i] <= '9')
            draw_seg_digit(renderer, x + i * (size * 4), y, size, buf[i] - '0');
        i++;
    }
}

static void draw_stroke_char(SDL_Renderer *renderer, int x, int y, int size, char c)
{
    int w;
    int h;
    int m;

    w = size * 4;
    h = size * 6;
    m = y + h / 2;
    if (c == 'A')
    {
        SDL_RenderDrawLine(renderer, x, y + h, x + w / 2, y);
        SDL_RenderDrawLine(renderer, x + w / 2, y, x + w, y + h);
        SDL_RenderDrawLine(renderer, x + size, m, x + w - size, m);
    }
    else if (c == 'N')
    {
        SDL_RenderDrawLine(renderer, x, y, x, y + h);
        SDL_RenderDrawLine(renderer, x, y, x + w, y + h);
        SDL_RenderDrawLine(renderer, x + w, y, x + w, y + h);
    }
    else if (c == 'T')
    {
        SDL_RenderDrawLine(renderer, x, y, x + w, y);
        SDL_RenderDrawLine(renderer, x + w / 2, y, x + w / 2, y + h);
    }
    else if (c == 'S')
    {
        SDL_RenderDrawLine(renderer, x, y, x + w, y);
        SDL_RenderDrawLine(renderer, x, y, x, m);
        SDL_RenderDrawLine(renderer, x, m, x + w, m);
        SDL_RenderDrawLine(renderer, x + w, m, x + w, y + h);
        SDL_RenderDrawLine(renderer, x, y + h, x + w, y + h);
    }
    else if (c == 'U')
    {
        SDL_RenderDrawLine(renderer, x, y, x, y + h);
        SDL_RenderDrawLine(renderer, x + w, y, x + w, y + h);
        SDL_RenderDrawLine(renderer, x, y + h, x + w, y + h);
    }
    else if (c == 'R')
    {
        SDL_RenderDrawLine(renderer, x, y, x, y + h);
        SDL_RenderDrawLine(renderer, x, y, x + w, y);
        SDL_RenderDrawLine(renderer, x + w, y, x + w, m);
        SDL_RenderDrawLine(renderer, x, m, x + w, m);
        SDL_RenderDrawLine(renderer, x, m, x + w, y + h);
    }
    else if (c == 'P')
    {
        SDL_RenderDrawLine(renderer, x, y, x, y + h);
        SDL_RenderDrawLine(renderer, x, y, x + w, y);
        SDL_RenderDrawLine(renderer, x + w, y, x + w, m);
        SDL_RenderDrawLine(renderer, x, m, x + w, m);
    }
    else if (c == 'H')
    {
        SDL_RenderDrawLine(renderer, x, y, x, y + h);
        SDL_RenderDrawLine(renderer, x + w, y, x + w, y + h);
        SDL_RenderDrawLine(renderer, x, m, x + w, m);
    }
    else if (c == 'V')
    {
        SDL_RenderDrawLine(renderer, x, y, x + w / 2, y + h);
        SDL_RenderDrawLine(renderer, x + w, y, x + w / 2, y + h);
    }
    else if (c == 'G')
    {
        SDL_RenderDrawLine(renderer, x, y, x + w, y);
        SDL_RenderDrawLine(renderer, x, y, x, y + h);
        SDL_RenderDrawLine(renderer, x, y + h, x + w, y + h);
        SDL_RenderDrawLine(renderer, x + w, m, x + w, y + h);
        SDL_RenderDrawLine(renderer, x + w / 2, m, x + w, m);
    }
}

static void draw_word(SDL_Renderer *renderer, int x, int y, int size, const char *word)
{
    int i;

    i = 0;
    while (word[i])
    {
        draw_stroke_char(renderer, x + i * (size * 5), y, size, word[i]);
        i++;
    }
}

static int count_active_ants(t_visualizer *vis)
{
    int i;
    int n;

    n = 0;
    i = 0;
    while (i < vis->farm->ant_count)
    {
        if (vis->ants[i].active && !vis->ants[i].finished)
            n++;
        i++;
    }
    return (n);
}

static int count_finished_ants(t_visualizer *vis)
{
    int i;
    int n;

    n = 0;
    i = 0;
    while (i < vis->farm->ant_count)
    {
        if (vis->ants[i].finished)
            n++;
        i++;
    }
    return (n);
}

static int link_is_active(t_visualizer *vis, int r1, int r2)
{
    int i;
    int a;
    int b;

    i = 0;
    while (i < vis->farm->ant_count)
    {
        if (vis->ants[i].active && !vis->ants[i].finished && vis->ants[i].progress < 1.0f)
        {
            a = vis->ants[i].current_room;
            b = vis->ants[i].target_room;
            if ((a == r1 && b == r2) || (a == r2 && b == r1))
                return (1);
        }
        i++;
    }
    return (0);
}

static int count_active_links(t_visualizer *vis)
{
    int i;
    int n;
    int r1;
    int r2;

    n = 0;
    i = 0;
    while (i < vis->farm->link_count)
    {
        r1 = vis->farm->links[i].room1_idx;
        r2 = vis->farm->links[i].room2_idx;
        if (link_is_active(vis, r1, r2))
            n++;
        i++;
    }
    return (n);
}

static int count_paths_used(t_vfarm *farm)
{
    int ant_room[MAX_ANTS];
    unsigned char seen[MAX_ROOMS];
    int i;
    int t;
    int m;
    int ant;
    int to;
    int from;
    int n;

    if (farm->ant_count > MAX_ANTS || farm->room_count > MAX_ROOMS)
        return (0);
    memset(seen, 0, sizeof(unsigned char) * farm->room_count);
    i = 0;
    while (i < farm->ant_count)
    {
        ant_room[i] = farm->start_idx;
        i++;
    }
    n = 0;
    t = 0;
    while (t < farm->turn_count)
    {
        m = 0;
        while (m < farm->turns[t].move_count)
        {
            ant = farm->turns[t].moves[m].ant_id - 1;
            to = farm->turns[t].moves[m].to_room_idx;
            if (ant >= 0 && ant < farm->ant_count)
            {
                from = ant_room[ant];
                if (from == farm->start_idx && to >= 0 && to < farm->room_count && !seen[to])
                {
                    seen[to] = 1;
                    n++;
                }
                ant_room[ant] = to;
            }
            m++;
        }
        t++;
    }
    return (n);
}

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

void calculate_scale(t_vfarm *farm)
{
    int min_x, max_x, min_y, max_y;
    int i;
    float scale_x, scale_y;
    int range_x, range_y;
    float min_dist;

    if (farm->room_count == 0)
        return;

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

    range_x = max_x - min_x;
    range_y = max_y - min_y;

    if (range_x == 0)
        range_x = 1;
    if (range_y == 0)
        range_y = 1;

    scale_x = (float)(WIN_WIDTH - 2 * PADDING) / range_x;
    scale_y = (float)(WIN_HEIGHT - 2 * PADDING - 80) / range_y;

    farm->scale = (scale_x < scale_y) ? scale_x : scale_y;

    farm->offset_x = PADDING + ((WIN_WIDTH - 2 * PADDING) - range_x * farm->scale) / 2
                     - min_x * farm->scale;
    farm->offset_y = PADDING + 40 + ((WIN_HEIGHT - 2 * PADDING - 80) - range_y * farm->scale) / 2
                     - min_y * farm->scale;

    i = 0;
    while (i < farm->room_count)
    {
        farm->rooms[i].draw_x = farm->rooms[i].x * farm->scale + farm->offset_x;
        farm->rooms[i].draw_y = farm->rooms[i].y * farm->scale + farm->offset_y;
        i++;
    }

    min_dist = calc_min_link_distance(farm);
    
    farm->room_radius = (int)(min_dist * 0.35f);
    
    if (farm->room_radius > MAX_ROOM_RADIUS)
        farm->room_radius = MAX_ROOM_RADIUS;
    if (farm->room_radius < MIN_ROOM_RADIUS)
        farm->room_radius = MIN_ROOM_RADIUS;
    
    farm->ant_radius = farm->room_radius * 2 / 5;
    if (farm->ant_radius < 2)
        farm->ant_radius = 2;
}

static void draw_thick_line(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int thickness)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrtf(dx * dx + dy * dy);
    float nx, ny;
    int i;

    if (len == 0)
        return;

    nx = -dy / len;
    ny = dx / len;

    for (i = -thickness / 2; i <= thickness / 2; i++)
    {
        SDL_RenderDrawLine(renderer,
                          x1 + (int)(nx * i), y1 + (int)(ny * i),
                          x2 + (int)(nx * i), y2 + (int)(ny * i));
    }
}

static void draw_links(t_visualizer *vis)
{
    int i;
    t_vfarm *farm;
    int r1, r2;
    int thickness;
    int active;

    farm = vis->farm;
    
    thickness = farm->room_radius / 3;
    if (thickness < 1)
        thickness = 1;
    if (thickness > 3)
        thickness = 3;

    i = 0;
    while (i < farm->link_count)
    {
        r1 = farm->links[i].room1_idx;
        r2 = farm->links[i].room2_idx;
        active = link_is_active(vis, r1, r2);
        if (active)
            SDL_SetRenderDrawColor(vis->renderer, 250, 190, 60, 255);
        else
            SDL_SetRenderDrawColor(vis->renderer, COLOR_LINK_R, COLOR_LINK_G,
                                   COLOR_LINK_B, 255);
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
                           thickness + active);
        }
        i++;
    }
}

static void draw_rooms(t_visualizer *vis)
{
    int i;
    t_vfarm *farm;
    int radius;
    int special_radius;

    farm = vis->farm;
    radius = farm->room_radius;
    
    special_radius = radius * 2;
    if (special_radius < 8)
        special_radius = 8;
    if (special_radius > 25)
        special_radius = 25;

    i = 0;
    while (i < farm->room_count)
    {
        if (!farm->rooms[i].is_start && !farm->rooms[i].is_end)
        {
            SDL_SetRenderDrawColor(vis->renderer, 140, 140, 160, 255);
            draw_filled_circle(vis->renderer,
                              (int)farm->rooms[i].draw_x,
                              (int)farm->rooms[i].draw_y,
                              radius);
        }
        i++;
    }

    i = 0;
    while (i < farm->room_count)
    {
        if (farm->rooms[i].is_start)
        {
            SDL_SetRenderDrawColor(vis->renderer, 50, 255, 100, 255);
            draw_filled_circle(vis->renderer,
                              (int)farm->rooms[i].draw_x,
                              (int)farm->rooms[i].draw_y,
                              special_radius);
            SDL_SetRenderDrawColor(vis->renderer, 255, 255, 255, 255);
            draw_circle(vis->renderer,
                       (int)farm->rooms[i].draw_x,
                       (int)farm->rooms[i].draw_y,
                       special_radius);
        }
        else if (farm->rooms[i].is_end)
        {
            SDL_SetRenderDrawColor(vis->renderer, 255, 80, 80, 255);
            draw_filled_circle(vis->renderer,
                              (int)farm->rooms[i].draw_x,
                              (int)farm->rooms[i].draw_y,
                              special_radius);
            SDL_SetRenderDrawColor(vis->renderer, 255, 255, 255, 255);
            draw_circle(vis->renderer,
                       (int)farm->rooms[i].draw_x,
                       (int)farm->rooms[i].draw_y,
                       special_radius);
        }
        i++;
    }
}

static void get_ant_color(int ant_id, Uint8 *r, Uint8 *g, Uint8 *b)
{
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

static void draw_ants(t_visualizer *vis)
{
    int i;
    Uint8 r, g, b;
    int radius;

    if (!vis->ants)
        return;

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
            SDL_SetRenderDrawColor(vis->renderer, 255, 255, 255, 255);
            draw_circle(vis->renderer,
                       (int)vis->ants[i].x,
                       (int)vis->ants[i].y,
                       radius);
        }
        i++;
    }
}

static void draw_ui(t_visualizer *vis)
{
    SDL_Rect bar;
    SDL_Rect progress;
    int active_ants;
    int active_links;
    int finished_ants;
    float total;
    float done;

    SDL_SetRenderDrawColor(vis->renderer, 40, 40, 50, 255);
    bar.x = 0;
    bar.y = 0;
    bar.w = WIN_WIDTH;
    bar.h = 40;
    SDL_RenderFillRect(vis->renderer, &bar);

    total = (float)vis->farm->turn_count;
    if (total < 1.0f)
        total = 1.0f;
    done = (float)vis->current_turn + vis->turn_progress;
    if (done < 0.0f)
        done = 0.0f;
    if (done > total)
        done = total;

    SDL_SetRenderDrawColor(vis->renderer, 70, 70, 90, 255);
    progress.x = 250;
    progress.y = 12;
    progress.w = WIN_WIDTH - 270;
    progress.h = 14;
    SDL_RenderFillRect(vis->renderer, &progress);

    SDL_SetRenderDrawColor(vis->renderer, 90, 200, 255, 255);
    progress.w = (int)((WIN_WIDTH - 270) * (done / total));
    SDL_RenderFillRect(vis->renderer, &progress);

    active_ants = count_active_ants(vis);
    active_links = count_active_links(vis);
    finished_ants = count_finished_ants(vis);

    SDL_SetRenderDrawColor(vis->renderer, 220, 220, 220, 255);
    draw_number(vis->renderer, 16, 8, 2, vis->current_turn);
    draw_number(vis->renderer, 82, 8, 2, active_ants);
    draw_number(vis->renderer, 148, 8, 2, active_links);
    draw_number(vis->renderer, 214, 8, 2, finished_ants);
}

static void draw_summary_panel(t_visualizer *vis)
{
    SDL_Rect box;
    SDL_Rect row;
    SDL_Rect dot;
    int total_ants;
    int total_turns;
    int paths_used;
    int avg_x10;
    int label_x;
    int value_x;

    if (vis->current_turn < vis->farm->turn_count)
        return;

    total_ants = vis->farm->ant_count;
    total_turns = vis->farm->turn_count;
    paths_used = count_paths_used(vis->farm);
    if (total_turns > 0)
        avg_x10 = (total_ants * 10) / total_turns;
    else
        avg_x10 = 0;

    box.x = WIN_WIDTH / 2 - 190;
    box.y = WIN_HEIGHT / 2 - 130;
    box.w = 380;
    box.h = 260;
    label_x = box.x + 50;
    value_x = box.x + 250;

    SDL_SetRenderDrawColor(vis->renderer, 18, 18, 24, 230);
    SDL_RenderFillRect(vis->renderer, &box);
    SDL_SetRenderDrawColor(vis->renderer, 95, 95, 120, 255);
    draw_rect_outline(vis->renderer, &box);

    row = (SDL_Rect){box.x + 28, box.y + 36, 12, 12};
    SDL_SetRenderDrawColor(vis->renderer, 50, 255, 100, 255);
    SDL_RenderFillRect(vis->renderer, &row);
    SDL_SetRenderDrawColor(vis->renderer, 220, 220, 220, 255);
    draw_word(vis->renderer, label_x, box.y + 28, 2, "ANTS");
    draw_number(vis->renderer, value_x, box.y + 24, 3, total_ants);

    row = (SDL_Rect){box.x + 28, box.y + 86, 12, 12};
    SDL_SetRenderDrawColor(vis->renderer, 90, 200, 255, 255);
    SDL_RenderFillRect(vis->renderer, &row);
    SDL_SetRenderDrawColor(vis->renderer, 220, 220, 220, 255);
    draw_word(vis->renderer, label_x, box.y + 78, 2, "TURNS");
    draw_number(vis->renderer, value_x, box.y + 74, 3, total_turns);

    row = (SDL_Rect){box.x + 28, box.y + 136, 12, 12};
    SDL_SetRenderDrawColor(vis->renderer, 250, 190, 60, 255);
    SDL_RenderFillRect(vis->renderer, &row);
    SDL_SetRenderDrawColor(vis->renderer, 220, 220, 220, 255);
    draw_word(vis->renderer, label_x, box.y + 128, 2, "PATHS");
    draw_number(vis->renderer, value_x, box.y + 124, 3, paths_used);

    row = (SDL_Rect){box.x + 28, box.y + 186, 12, 12};
    SDL_SetRenderDrawColor(vis->renderer, 220, 120, 255, 255);
    SDL_RenderFillRect(vis->renderer, &row);
    SDL_SetRenderDrawColor(vis->renderer, 220, 220, 220, 255);
    draw_word(vis->renderer, label_x, box.y + 178, 2, "AVG");
    draw_number(vis->renderer, value_x, box.y + 174, 3, avg_x10 / 10);
    dot = (SDL_Rect){value_x + 16, box.y + 186, 4, 4};
    SDL_RenderFillRect(vis->renderer, &dot);
    draw_number(vis->renderer, value_x + 24, box.y + 174, 3, avg_x10 % 10);
}

void render_frame(t_visualizer *vis)
{
    SDL_SetRenderDrawColor(vis->renderer, COLOR_BG_R, COLOR_BG_G, 
                           COLOR_BG_B, 255);
    SDL_RenderClear(vis->renderer);

    draw_links(vis);
    draw_rooms(vis);
    draw_ants(vis);
    draw_ui(vis);
    draw_summary_panel(vis);

    SDL_RenderPresent(vis->renderer);
}

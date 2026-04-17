#include "visualizer.h"

void init_ant_states(t_visualizer *vis)
{
    int i;
    t_vfarm *farm;

    farm = vis->farm;
    vis->ants = calloc(farm->ant_count, sizeof(t_ant_state));
    if (!vis->ants)
        return;

    i = 0;
    while (i < farm->ant_count)
    {
        vis->ants[i].current_room = farm->start_idx;
        vis->ants[i].target_room = farm->start_idx;
        vis->ants[i].x = farm->rooms[farm->start_idx].draw_x;
        vis->ants[i].y = farm->rooms[farm->start_idx].draw_y;
        vis->ants[i].progress = 1.0f;
        vis->ants[i].active = 0;
        vis->ants[i].finished = 0;
        i++;
    }
}

void apply_turn(t_visualizer *vis)
{
    t_turn *turn;
    int i;
    int ant_idx;
    int target_room;

    if (vis->current_turn >= vis->farm->turn_count)
        return;

    turn = &vis->farm->turns[vis->current_turn];

    i = 0;
    while (i < turn->move_count)
    {
        ant_idx = turn->moves[i].ant_id - 1;
        target_room = turn->moves[i].to_room_idx;

        if (ant_idx >= 0 && ant_idx < vis->farm->ant_count)
        {
            vis->ants[ant_idx].current_room = vis->ants[ant_idx].target_room;
            vis->ants[ant_idx].target_room = target_room;
            vis->ants[ant_idx].progress = 0.0f;
            vis->ants[ant_idx].active = 1;
        }
        i++;
    }
}

static float ease_in_out(float t)
{
    if (t < 0.5f)
        return 2.0f * t * t;
    return 1.0f - (-2.0f * t + 2.0f) * (-2.0f * t + 2.0f) / 2.0f;
}

void update_animation(t_visualizer *vis, float delta_time)
{
    int i;
    float eased;
    t_vfarm *farm;
    float from_x, from_y, to_x, to_y;

    if (vis->paused)
        return;

    farm = vis->farm;

    vis->turn_progress += delta_time * vis->speed;

    if (vis->turn_progress >= 1.0f)
    {
        vis->turn_progress = 0.0f;
        vis->current_turn++;

        i = 0;
        while (i < farm->ant_count)
        {
            if (vis->ants[i].active)
            {
                vis->ants[i].current_room = vis->ants[i].target_room;
                vis->ants[i].x = farm->rooms[vis->ants[i].current_room].draw_x;
                vis->ants[i].y = farm->rooms[vis->ants[i].current_room].draw_y;
                vis->ants[i].progress = 1.0f;
                if (vis->ants[i].target_room == farm->end_idx)
                    vis->ants[i].finished = 1;
            }
            i++;
        }

        if (vis->current_turn < farm->turn_count)
            apply_turn(vis);
        else
        {
            vis->current_turn = farm->turn_count;
        }
        return;
    }

    eased = ease_in_out(vis->turn_progress);

    i = 0;
    while (i < farm->ant_count)
    {
        if (vis->ants[i].active && vis->ants[i].progress < 1.0f)
        {
            from_x = farm->rooms[vis->ants[i].current_room].draw_x;
            from_y = farm->rooms[vis->ants[i].current_room].draw_y;
            to_x = farm->rooms[vis->ants[i].target_room].draw_x;
            to_y = farm->rooms[vis->ants[i].target_room].draw_y;

            vis->ants[i].x = from_x + (to_x - from_x) * eased;
            vis->ants[i].y = from_y + (to_y - from_y) * eased;
            vis->ants[i].progress = eased;
        }
        i++;
    }
}

void reset_animation(t_visualizer *vis)
{
    vis->current_turn = 0;
    vis->turn_progress = 0.0f;

    if (vis->ants)
        free(vis->ants);
    init_ant_states(vis);

    if (vis->farm->turn_count > 0)
        apply_turn(vis);
}

void step_one_turn(t_visualizer *vis)
{
    int i;
    t_vfarm *farm;

    farm = vis->farm;
    if (vis->current_turn >= farm->turn_count)
        return;
    i = 0;
    while (i < farm->ant_count)
    {
        if (vis->ants[i].active)
        {
            vis->ants[i].current_room = vis->ants[i].target_room;
            vis->ants[i].x = farm->rooms[vis->ants[i].current_room].draw_x;
            vis->ants[i].y = farm->rooms[vis->ants[i].current_room].draw_y;
            vis->ants[i].progress = 1.0f;
            if (vis->ants[i].target_room == farm->end_idx)
                vis->ants[i].finished = 1;
        }
        i++;
    }
    vis->turn_progress = 0.0f;
    vis->current_turn++;
    if (vis->current_turn < farm->turn_count)
        apply_turn(vis);
}

#include "visualizer.h"

int find_room_index(t_vfarm *farm, const char *name)
{
    int i;

    i = 0;
    while (i < farm->room_count)
    {
        if (ft_strcmp(farm->rooms[i].name, name) == 0)
            return (i);
        i++;
    }
    return (-1);
}

static int is_room_line(const char *line)
{
    int spaces;
    int i;

    if (!line || line[0] == '#' || line[0] == 'L' || line[0] == '\0')
        return (0);
    spaces = 0;
    i = 0;
    while (line[i])
    {
        if (line[i] == ' ')
            spaces++;
        if (line[i] == '-' && i > 0 && line[i - 1] != ' ')
        {
            int has_space = 0;
            int j = 0;
            while (line[j])
            {
                if (line[j] == ' ')
                    has_space = 1;
                j++;
            }
            if (!has_space)
                return (0);
        }
        i++;
    }
    return (spaces == 2);
}

static int is_link_line(const char *line)
{
    int i;
    int has_dash;
    int has_space;

    if (!line || line[0] == '#' || line[0] == 'L' || line[0] == '\0')
        return (0);
    has_dash = 0;
    has_space = 0;
    i = 0;
    while (line[i])
    {
        if (line[i] == '-')
            has_dash = 1;
        if (line[i] == ' ')
            has_space = 1;
        i++;
    }
    return (has_dash && !has_space);
}

static int is_move_line(const char *line)
{
    if (!line || line[0] != 'L')
        return (0);
    return (ft_strchr(line, '-') != NULL);
}

static void parse_room(t_vfarm *farm, const char *line, int is_start, int is_end)
{
    char name[MAX_NAME_LEN];
    int x, y;
    int i, j;

    if (farm->room_count >= MAX_ROOMS)
        return;

    i = 0;
    j = 0;
    while (line[i] && line[i] != ' ' && j < MAX_NAME_LEN - 1)
        name[j++] = line[i++];
    name[j] = '\0';

    while (line[i] == ' ')
        i++;
    x = ft_atoi(&line[i]);

    while (line[i] && line[i] != ' ')
        i++;
    while (line[i] == ' ')
        i++;
    y = ft_atoi(&line[i]);

    ft_strlcpy(farm->rooms[farm->room_count].name, name, MAX_NAME_LEN);
    farm->rooms[farm->room_count].x = x;
    farm->rooms[farm->room_count].y = y;
    farm->rooms[farm->room_count].is_start = is_start;
    farm->rooms[farm->room_count].is_end = is_end;

    if (is_start)
        farm->start_idx = farm->room_count;
    if (is_end)
        farm->end_idx = farm->room_count;

    farm->room_count++;
}

static void parse_link(t_vfarm *farm, const char *line)
{
    char room1[MAX_NAME_LEN];
    char room2[MAX_NAME_LEN];
    int i, j;
    int idx1, idx2;

    if (farm->link_count >= MAX_LINKS)
        return;

    i = 0;
    j = 0;
    while (line[i] && line[i] != '-' && j < MAX_NAME_LEN - 1)
        room1[j++] = line[i++];
    room1[j] = '\0';

    if (line[i] == '-')
        i++;

    j = 0;
    while (line[i] && line[i] != '\n' && line[i] != '\r' && j < MAX_NAME_LEN - 1)
        room2[j++] = line[i++];
    room2[j] = '\0';

    idx1 = find_room_index(farm, room1);
    idx2 = find_room_index(farm, room2);

    if (idx1 >= 0 && idx2 >= 0)
    {
        farm->links[farm->link_count].room1_idx = idx1;
        farm->links[farm->link_count].room2_idx = idx2;
        farm->link_count++;
    }
}

static void parse_single_move(t_vfarm *farm, const char *move_str)
{
    int ant_id;
    char room_name[MAX_NAME_LEN];
    int i, j;
    int room_idx;
    t_turn *turn;

    if (farm->turn_count == 0 || move_str[0] != 'L')
        return;

    turn = &farm->turns[farm->turn_count - 1];
    if (turn->move_count >= MAX_MOVES_PER_TURN)
        return;

    i = 1;
    ant_id = ft_atoi(&move_str[i]);

    while (move_str[i] && move_str[i] != '-')
        i++;
    if (move_str[i] == '-')
        i++;

    j = 0;
    while (move_str[i] && move_str[i] != ' ' && move_str[i] != '\n' 
           && move_str[i] != '\r' && j < MAX_NAME_LEN - 1)
        room_name[j++] = move_str[i++];
    room_name[j] = '\0';

    room_idx = find_room_index(farm, room_name);
    if (room_idx >= 0 && ant_id > 0)
    {
        turn->moves[turn->move_count].ant_id = ant_id;
        turn->moves[turn->move_count].to_room_idx = room_idx;
        turn->move_count++;
    }
}

static void parse_move_line(t_vfarm *farm, const char *line)
{
    char move[MAX_NAME_LEN];
    int i, j;

    if (farm->turn_count >= MAX_TURNS)
        return;

    farm->turns[farm->turn_count].move_count = 0;
    farm->turn_count++;

    i = 0;
    while (line[i])
    {
        while (line[i] == ' ')
            i++;

        j = 0;
        while (line[i] && line[i] != ' ' && line[i] != '\n' 
               && line[i] != '\r' && j < MAX_NAME_LEN - 1)
            move[j++] = line[i++];
        move[j] = '\0';

        if (move[0] == 'L')
            parse_single_move(farm, move);
    }
}

static void strip_line(char *line)
{
    int len;

    len = ft_strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r' 
                       || line[len - 1] == ' '))
    {
        line[len - 1] = '\0';
        len--;
    }
}

t_vfarm *parse_input(const char *filename)
{
    t_vfarm *farm;
    char line[MAX_LINE_LEN];
    int next_is_start;
    int next_is_end;
    int got_ants;
    FILE *input;

    farm = ft_calloc(1, sizeof(t_vfarm));
    if (!farm)
        return (NULL);

    if (filename)
    {
        input = fopen(filename, "r");
        if (!input)
        {
            ft_putstr_fd("Error: Cannot open file ", 2);
            ft_putstr_fd((char *)filename, 2);
            ft_putstr_fd("\n", 2);
            free(farm);
            return (NULL);
        }
    }
    else
        input = stdin;

    farm->start_idx = -1;
    farm->end_idx = -1;
    next_is_start = 0;
    next_is_end = 0;
    got_ants = 0;

    while (fgets(line, MAX_LINE_LEN, input))
    {
        strip_line(line);

        if (line[0] == '\0')
            continue;

        if (ft_strcmp(line, "##start") == 0)
        {
            next_is_start = 1;
            continue;
        }
        if (ft_strcmp(line, "##end") == 0)
        {
            next_is_end = 1;
            continue;
        }

        if (line[0] == '#')
            continue;

        if (!got_ants && line[0] >= '0' && line[0] <= '9')
        {
            int num = ft_atoi(line);
            int is_just_num = 1;
            int k = 0;
            while (line[k])
            {
                if (line[k] < '0' || line[k] > '9')
                {
                    is_just_num = 0;
                    break;
                }
                k++;
            }
            if (is_just_num && num > 0)
            {
                farm->ant_count = num;
                got_ants = 1;
                continue;
            }
        }

        if (is_room_line(line))
        {
            parse_room(farm, line, next_is_start, next_is_end);
            next_is_start = 0;
            next_is_end = 0;
            continue;
        }

        if (is_link_line(line))
        {
            parse_link(farm, line);
            continue;
        }

        if (is_move_line(line))
        {
            parse_move_line(farm, line);
            continue;
        }
    }

    if (filename && input)
        fclose(input);

    return (farm);
}

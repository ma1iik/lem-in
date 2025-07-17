#include "lem_in.h"

int paths_conflict(t_path path1, t_path path2) {
	t_list *p1 = path1.path;
	
	while (p1) {
		t_room *room1 = (t_room *)p1->content;
		
		if (room1->is_start || room1->is_end) {
			p1 = p1->next;
			continue;
		}
		
		t_list *p2 = path2.path;
		while (p2) {
			t_room *room2 = (t_room *)p2->content;
			if (room1 == room2) return 1;
			p2 = p2->next;
		}
		p1 = p1->next;
	}
	return 0;
}

int can_finish_in_turns(t_path *paths, int path_count, int total_ants, int target_turns) {
	int ants_placed = 0;
	
	for (int i = 0; i < path_count; i++) {
		int max_ants_on_path = target_turns - paths[i].len + 1;
		
		if (max_ants_on_path <= 0) continue;
		
		int ants_to_use = (max_ants_on_path > total_ants - ants_placed) ? total_ants - ants_placed : max_ants_on_path;
		ants_placed += ants_to_use;
		
		if (ants_placed >= total_ants) return 1;
	}
	return 0;
}

int longest_path_length(t_path *paths, int path_count) {
    int longest = 0;
    for (int i = 0; i < path_count; i++) {
        if (paths[i].len > longest)
            longest = paths[i].len;
    }
    return longest;
}

void add_path_set(t_path_set **valid_sets, t_path_set candidate) {
    static int set_count = 0;
    static int set_capacity = 0;
    
    if (!*valid_sets) {
        set_capacity = 2;
        set_count = 0;
        *valid_sets = malloc(sizeof(t_path_set) * set_capacity);
    }
    else if (set_count + 1 >= set_capacity) {
        set_capacity *= 2;
        t_path_set *new_sets = malloc(sizeof(t_path_set) * set_capacity);
        for (int i = 0; i < set_count; i++) {
            new_sets[i] = (*valid_sets)[i];
        }
        free(*valid_sets);
        *valid_sets = new_sets;
    }
    
    (*valid_sets)[set_count] = candidate;
    set_count++;
    (*valid_sets)[set_count] = (t_path_set){NULL, 0};
}

int calc_least_turns(t_path *paths, int path_count, int total_ants) {
	int min_turns = 1;
	int max_turns = total_ants + longest_path_length(paths, path_count) - 1;
	
	while (min_turns < max_turns) {
		int mid = (min_turns + max_turns) / 2;
		
		if (can_finish_in_turns(paths, path_count, total_ants, mid)) {
			max_turns = mid;
		} else {
			min_turns = mid + 1;
		}
	}
	return min_turns;
}


void generate_path_sets(t_path *all_paths, t_path_set **valid_sets) {
	int path_count = 0;
	while (all_paths[path_count].len != 0) path_count++;
	
	for (int mask = 1; mask < (1 << path_count); mask++) {
		t_path_set candidate;
		candidate.paths = malloc(sizeof(t_path) * path_count);
		candidate.count = 0;
		
		for (int i = 0; i < path_count; i++) {
			if (mask & (1 << i)) {
				candidate.paths[candidate.count] = all_paths[i];
				candidate.count++;
			}
		}
		
		int valid = 1;
		for (int i = 0; i < candidate.count && valid; i++) {
			for (int j = i + 1; j < candidate.count && valid; j++) {
				if (paths_conflict(candidate.paths[i], candidate.paths[j])) {
					valid = 0;
				}
			}
		}
		
		if (valid) {
			add_path_set(valid_sets, candidate);
		} else {
			free(candidate.paths);
		}
	}
}

void find_best_solution(t_farm *farm, t_path_set *valid_sets) {
	int best_turns = INT_MAX;
	t_path_set *best_set = NULL;
	
	for (int i = 0; valid_sets[i].count != 0; i++) {
		int turns = calc_least_turns(valid_sets[i].paths, valid_sets[i].count, farm->ant_count);
										   
		if (turns < best_turns) {
			best_turns = turns;
			best_set = &valid_sets[i];
		}
	}
	
	printf("Best solution: %d turns using %d paths\n", best_turns, best_set->count);
}

void part1(t_farm *farm) {
	t_room *start = farm->start_room;
	t_list *queue = ft_lstnew((t_room *)start);
	t_list *visited = ft_lstnew((t_room *)start);
	t_path *all_paths = NULL;
	t_path_set *valid_sets = NULL;
	dfs(start, farm, visited, &all_paths);
	generate_path_sets(all_paths, &valid_sets);
	find_best_solution(farm, valid_sets);
	// bfs(farm);

	
}

int main(int argc, char **argv){
	(void)argv;
	if (argc != 1){
		ft_printf("ERROR: No arguments allowed\n");
		return 1;
	}
	// if (isatty(0)) {
	// 	ft_printf("Usage: %s < input_file.map\n", argv[0]);
	// 	ft_printf("   or: echo \"input\" | %s\n", argv[0]);
	// 	return 1;
	// }
	t_farm *farm = parse_input();
	if (!farm)
		exit(0);
	part1(farm);
	return 0;
}



// void bfs(t_farm *farm) {
// 	t_room *start = farm->start_room;
// 	t_list *queue = ft_lstnew((t_room *)start);
// 	t_list *visited = ft_lstnew((t_room *)start);
// 	t_path **paths = NULL;
	
// 	while (queue != NULL) {			
// 		t_room *current = queue->content;
// 		t_list *tmp = NULL;
// 		if (queue->next != NULL)
// 			tmp = queue->next;
		
// 		// free(queue->content);
// 		// free(queue->next);
// 		free(queue);

// 		queue = tmp;

// 		t_list *neighbours = current->connections;
// 		for(t_list *neighbour = neighbours; neighbour != NULL; neighbour = neighbour->next){
// 			t_room *neighbour_r = neighbour->content;
// 			if (!is_visited(neighbour_r, visited)) {
// 				neighbour_r->parent = current;
// 				ft_lstadd_back(&visited, ft_lstnew(neighbour_r));

// 				if (farm->end_room->name == neighbour_r->name) {
// 					printf("End is %s\n", farm->end_room->name);
// 					t_path *path = get_path(neighbour_r);
// 					paths = add_path(paths, path);

// 				}
// 				else
// 					ft_lstadd_back(&queue, ft_lstnew(neighbour_r));
					
// 			}
// 		}
// 	}
// }
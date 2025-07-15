#include "lem_in.h"

int is_visited(t_room *neighbour, t_list *visited) {
	if (!visited)
		return 0;

	for (t_list *cur = visited; cur != NULL; cur = cur->next) {
		if (cur->content == neighbour)
			return 1;
	}
	return 0;
}

void del_from_visited(t_room *del, t_list **visited) {
	if (!visited || !*visited)
		return;

	if ((*visited)->content == del) {
		t_list *to_delete = *visited;
		*visited = (*visited)->next;
		free(to_delete);
		return;
	}

	t_list *prev = *visited;
	t_list *cur = (*visited)->next;

	while (cur) {
		if (cur->content == del) {
			prev->next = cur->next;
			free(cur);
			return;
		}
		prev = cur;
		cur = cur->next;
	}
}


void add_path(t_path **paths, t_path add) {
   static int capacity = 0;
   static int count = 0;
   
   if (!*paths) {
   	capacity = 2;
   	count = 0;
   	*paths = malloc(sizeof(t_path) * capacity);
   	(*paths)[count] = add;
   	count++;
   	(*paths)[count] = (t_path){0};
   }
   else {
   	// Check if we need more space
   	if (count + 1 >= capacity) {
   		capacity *= 2;
   		t_path *new_paths = malloc(sizeof(t_path) * capacity);
   		
   		for (int i = 0; i < count; i++) {
   			new_paths[i] = (*paths)[i];
   		}
   		
   		free(*paths);
   		*paths = new_paths;
   	}
   	
   	(*paths)[count] = add;
   	count++;
   	(*paths)[count] = (t_path){0};
   }
}

t_path get_path(t_room *room) {
	int len = 0;
	t_path path_struc;

	t_list *path = ft_lstnew((t_room *)room);
	for (t_room *cur = room->parent; cur != NULL; cur = cur->parent){
		ft_lstadd_front(&path, ft_lstnew(cur));
	}

	for (t_list *cur = path; cur != NULL; cur = cur->next){
		printf("path way: %s\n", ((t_room*)cur->content)->name);
		len++;
	}
	printf("\n");

	path_struc.len = len;
	path_struc.path = path;
	return path_struc;
}




void dfs(t_room *cur, t_farm *farm, t_list *visited, t_path **all_paths) {
	t_list *neighbour = cur->connections;

	if (cur->name == farm->end_room->name){
		t_path path = get_path(cur);
		add_path(all_paths, path);
		return;
	}
	
	while (neighbour) {
		t_room *room = (t_room *)neighbour->content;
		if (!is_visited(room, visited)) {
			room->parent = cur;
			ft_lstadd_back(&visited, ft_lstnew(room));
			dfs(room, farm, visited, all_paths);
			del_from_visited(room, &visited);
		}
		neighbour = neighbour->next;
	}
}

void part1(t_farm *farm) {
	t_room *start = farm->start_room;
	t_list *queue = ft_lstnew((t_room *)start);
	t_list *visited = ft_lstnew((t_room *)start);
	t_path *all_paths = NULL;
	dfs(start, farm, visited, &all_paths);
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
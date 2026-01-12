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
		printf("%s", ((t_room*)cur->content)->name);
		len++;
	}
	path_struc.len = len - 1;
	path_struc.path = path;
	path_struc.issues = 0;
	path_struc.score = 0;
	printf(" path len: %d\n", path_struc.len);
	return path_struc;
}

void dfs(t_room *cur, t_farm *farm, t_list *visited, t_path **all_paths) {
	t_list *neighbour = cur->connections;

	if (ft_strcmp(cur->name, farm->end_room->name) == 0){
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
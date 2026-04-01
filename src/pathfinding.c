#include "lem_in.h"

#define MAX_PATHS 100

/* BFS for disj paths, then BFS-ordered all paths */

typedef struct s_bfs_node
{
	t_room				*room;
	struct s_bfs_node	*next;
}	t_bfs_node;

static void	bfs_enqueue(t_bfs_node **front, t_bfs_node **rear, t_room *room)
{
	t_bfs_node	*node;

	node = malloc(sizeof(t_bfs_node));
	if (!node)
		return ;
	node->room = room;
	node->next = NULL;
	if (*rear)
		(*rear)->next = node;
	else
		*front = node;
	*rear = node;
}

static t_room	*bfs_dequeue(t_bfs_node **front, t_bfs_node **rear)
{
	t_bfs_node	*node;
	t_room		*room;

	if (!*front)
		return (NULL);
	node = *front;
	room = node->room;
	*front = node->next;
	if (!*front)
		*rear = NULL;
	free(node);
	return (room);
}

static void	bfs_clear_queue(t_bfs_node **front)
{
	t_bfs_node	*tmp;

	while (*front)
	{
		tmp = *front;
		*front = (*front)->next;
		free(tmp);
	}
}

static void	reset_bfs(t_farm *farm)
{
	t_list	*cur;
	t_room	*room;

	cur = farm->rooms;
	while (cur)
	{
		room = (t_room *)cur->content;
		room->visited = 0;
		room->in_queue = 0;
		room->parent = NULL;
		cur = cur->next;
	}
}

static t_path	*trace_path(t_room *end_room)
{
	t_path	*path;
	t_list	*path_list;
	t_room	*cur;
	int		len;

	path = malloc(sizeof(t_path));
	if (!path)
		return (NULL);
	path_list = NULL;
	cur = end_room;
	len = 0;
	while (cur)
	{
		ft_lstadd_front(&path_list, ft_lstnew(cur));
		len++;
		cur = cur->parent;
	}
	path->path = path_list;
	path->len = len - 1;
	path->issues = 0;
	return (path);
}

static t_path	*bfs_find_path(t_farm *farm)
{
	t_bfs_node	*front;
	t_bfs_node	*rear;
	t_room		*current;
	t_room		*neighbor;
	t_list		*conn;
	t_path		*result;

	reset_bfs(farm);
	front = NULL;
	rear = NULL;
	farm->start_room->in_queue = 1;
	bfs_enqueue(&front, &rear, farm->start_room);
	result = NULL;
	while (front)
	{
		current = bfs_dequeue(&front, &rear);
		if (current->is_end)
		{
			result = trace_path(current);
			bfs_clear_queue(&front);
			return (result);
		}
		current->visited = 1;
		conn = current->connections;
		while (conn)
		{
			neighbor = (t_room *)conn->content;
			if (!neighbor->visited && !neighbor->in_queue
				&& (neighbor->score != -1 || neighbor->is_start || neighbor->is_end))
			{
				neighbor->parent = current;
				neighbor->in_queue = 1;
				bfs_enqueue(&front, &rear, neighbor);
			}
			conn = conn->next;
		}
	}
	return (NULL);
}

static void	mark_used(t_path *path)
{
	t_list	*cur;
	t_room	*room;

	if (!path || !path->path)
		return ;
	cur = path->path;
	while (cur)
	{
		room = (t_room *)cur->content;
		if (!room->is_start && !room->is_end)
			room->score = -1;
		cur = cur->next;
	}
}

static void	reset_used(t_farm *farm)
{
	t_list	*cur;
	t_room	*room;

	cur = farm->rooms;
	while (cur)
	{
		room = (t_room *)cur->content;
		room->score = 0;
		cur = cur->next;
	}
}

t_list	*get_disj_paths(t_farm *farm)
{
	t_list	*paths;
	t_path	*path;

	if (!farm || !farm->start_room || !farm->end_room)
		return (NULL);
	reset_used(farm);
	paths = NULL;
	while (1)
	{
		path = bfs_find_path(farm);
		if (!path)
			break ;
		ft_lstadd_back(&paths, ft_lstnew(path));
		mark_used(path);
		if (path->len == 1)
			break ;
	}
	reset_used(farm);
	return (paths);
}

typedef struct s_stack_node
{
	t_room				*room;
	t_list				*path;
	int					depth;
	struct s_stack_node	*next;
}	t_stack_node;

static t_stack_node	*stk_new(t_room *room, t_list *current_path, int depth)
{
	t_stack_node	*node;
	t_list			*path_copy;
	t_list			*cur;

	node = malloc(sizeof(t_stack_node));
	if (!node)
		return (NULL);
	path_copy = NULL;
	cur = current_path;
	while (cur)
	{
		ft_lstadd_back(&path_copy, ft_lstnew(cur->content));
		cur = cur->next;
	}
	ft_lstadd_back(&path_copy, ft_lstnew(room));
	node->room = room;
	node->path = path_copy;
	node->depth = depth;
	node->next = NULL;
	return (node);
}

static int	in_path(t_list *path, t_room *room)
{
	while (path)
	{
		if ((t_room *)path->content == room)
			return (1);
		path = path->next;
	}
	return (0);
}

static void	free_stack_node(t_stack_node *node)
{
	if (!node)
		return ;
	free_path_list(node->path);
	free(node);
}

static t_path	*path_from_lst(t_list *path)
{
	t_path	*result;
	t_list	*copy;
	t_list	*cur;
	int		len;

	result = malloc(sizeof(t_path));
	if (!result)
		return (NULL);
	copy = NULL;
	cur = path;
	len = 0;
	while (cur)
	{
		ft_lstadd_back(&copy, ft_lstnew(cur->content));
		len++;
		cur = cur->next;
	}
	result->path = copy;
	result->len = len - 1;
	result->issues = 0;
	return (result);
}

t_list	*get_all_paths(t_farm *farm)
{
	t_stack_node	*stack;
	t_stack_node	*cur;
	t_stack_node	*node;
	t_list			*paths;
	t_list			*neighbors;
	t_room			*neighbor;
	t_path			*new_path;
	int				n;
	int				max_len;

	if (!farm || !farm->start_room || !farm->end_room)
		return (NULL);
	paths = NULL;
	stack = NULL;
	n = 0;
	max_len = ft_lstsize(farm->rooms);
	if (max_len > 500)
		max_len = 500;
	node = stk_new(farm->start_room, NULL, 1);
	if (node)
		stack = node;
	while (stack && n < MAX_PATHS)
	{
		cur = stack;
		stack = stack->next;
		cur->next = NULL;
		if (cur->depth > max_len)
		{
			free_stack_node(cur);
			continue ;
		}
		if (cur->room->is_end)
		{
			new_path = path_from_lst(cur->path);
			if (new_path)
			{
				ft_lstadd_back(&paths, ft_lstnew(new_path));
				n++;
			}
			free_stack_node(cur);
			continue ;
		}
		neighbors = cur->room->connections;
		while (neighbors)
		{
			neighbor = (t_room *)neighbors->content;
			if (!in_path(cur->path, neighbor))
			{
				node = stk_new(neighbor, cur->path, cur->depth + 1);
				if (node)
				{
					node->next = stack;
					stack = node;
				}
			}
			neighbors = neighbors->next;
		}
		free_stack_node(cur);
	}
	while (stack)
	{
		cur = stack;
		stack = stack->next;
		free_stack_node(cur);
	}
	return (paths);
}

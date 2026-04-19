#include "lem_in.h"

#define MAX_PATHS 100

/*
** Edmonds-Karp max flow with node splitting for vertex-disjoint paths.
** Rooms have capacity 1 (split into in/out nodes).
** Stop augmenting when the total turn count stops improving.
*/

typedef struct s_edge
{
	int	to;
	int	cap;
	int	base_cap;
	int	rev_idx;
	int	visited;
}	t_edge;

typedef struct s_graph
{
	t_edge	**adj;
	int		*size;
	int		*alloc;
	int		node_count;
}	t_graph;

static t_graph	*graph_new(int node_count)
{
	t_graph	*g;

	g = malloc(sizeof(t_graph));
	if (!g)
		return (NULL);
	g->node_count = node_count;
	g->adj = ft_calloc(node_count, sizeof(t_edge *));
	g->size = ft_calloc(node_count, sizeof(int));
	g->alloc = ft_calloc(node_count, sizeof(int));
	if (!g->adj || !g->size || !g->alloc)
	{
		free(g->adj);
		free(g->size);
		free(g->alloc);
		free(g);
		return (NULL);
	}
	return (g);
}

static void	graph_free(t_graph *g)
{
	int	i;

	if (!g)
		return ;
	i = 0;
	while (i < g->node_count)
		free(g->adj[i++]);
	free(g->adj);
	free(g->size);
	free(g->alloc);
	free(g);
}

static void	*custom_realloc_edge(void *old_ptr, int old_size, int new_size)
{
	void	*new_ptr;
	int		i;
	char	*src;
	char	*dst;

	new_ptr = malloc(new_size);
	if (!new_ptr)
		return (NULL);
	if (old_ptr)
	{
		src = (char *)old_ptr;
		dst = (char *)new_ptr;
		i = 0;
		while (i < old_size)
		{
			dst[i] = src[i];
			i++;
		}
		free(old_ptr);
	}
	return (new_ptr);
}

static int	graph_grow(t_graph *g, int u)
{
	int		new_size;
	t_edge	*tmp;

	if (g->size[u] < g->alloc[u])
		return (1);
	new_size = g->alloc[u] ? g->alloc[u] * 2 : 4;
	tmp = custom_realloc_edge(g->adj[u], sizeof(t_edge) * g->alloc[u], sizeof(t_edge) * new_size);
	if (!tmp)
		return (0);
	g->adj[u] = tmp;
	g->alloc[u] = new_size;
	return (1);
}

static int	graph_add_edge(t_graph *g, int u, int v, int cap)
{
	int	fwd;
	int	rev;

	if (!graph_grow(g, u) || !graph_grow(g, v))
		return (0);
	fwd = g->size[u];
	rev = g->size[v];
	g->adj[u][fwd].to = v;
	g->adj[u][fwd].cap = cap;
	g->adj[u][fwd].base_cap = cap;
	g->adj[u][fwd].rev_idx = rev;
	g->adj[u][fwd].visited = 0;
	g->adj[v][rev].to = u;
	g->adj[v][rev].cap = 0;
	g->adj[v][rev].base_cap = 0;
	g->adj[v][rev].rev_idx = fwd;
	g->adj[v][rev].visited = 0;
	g->size[u]++;
	g->size[v]++;
	return (1);
}

static void	graph_clear_visited(t_graph *g)
{
	int	i;
	int	j;

	i = 0;
	while (i < g->node_count)
	{
		j = 0;
		while (j < g->size[i])
		{
			g->adj[i][j].visited = 0;
			j++;
		}
		i++;
	}
}

static int	bfs_find_path(t_graph *g, int src, int sink,
	int *parent, int *edge_idx)
{
	int	*visited;
	int	*queue;
	int	head;
	int	tail;
	int	u;
	int	i;

	visited = ft_calloc(g->node_count, sizeof(int));
	queue = malloc(g->node_count * sizeof(int));
	if (!visited || !queue)
	{
		free(visited);
		free(queue);
		return (0);
	}
	head = 0;
	tail = 0;
	visited[src] = 1;
	queue[tail++] = src;
	while (head < tail)
	{
		u = queue[head++];
		if (u == sink)
		{
			free(visited);
			free(queue);
			return (1);
		}
		i = 0;
		while (i < g->size[u])
		{
			if (!visited[g->adj[u][i].to] && g->adj[u][i].cap > 0)
			{
				visited[g->adj[u][i].to] = 1;
				parent[g->adj[u][i].to] = u;
				edge_idx[g->adj[u][i].to] = i;
				queue[tail++] = g->adj[u][i].to;
			}
			i++;
		}
	}
	free(visited);
	free(queue);
	return (0);
}

/* Finds one augmenting path via BFS and pushes flow through it.
** Returns 0 when no path to the sink exists anymore. */
static int	augment_flow(t_graph *g, int src, int sink,
	int *parent, int *edge_idx)
{
	int	v;
	int	e;
	int	flow;

	if (!bfs_find_path(g, src, sink, parent, edge_idx))
		return (0);
	flow = INT_MAX;
	v = sink;
	while (v != src)
	{
		if (g->adj[parent[v]][edge_idx[v]].cap < flow)
			flow = g->adj[parent[v]][edge_idx[v]].cap;
		v = parent[v];
	}
	v = sink;
	while (v != src)
	{
		e = edge_idx[v];
		g->adj[parent[v]][e].cap -= flow;
		g->adj[g->adj[parent[v]][e].to][g->adj[parent[v]][e].rev_idx].cap += flow;
		v = parent[v];
	}
	return (1);
}

/* Traces one path from src to sink through edges that were actually used
** (had capacity and got saturated). Marks edges as visited so the next
** call picks a different path. */
static int	*extract_path(t_graph *g, int src, int sink, int *out_len)
{
	int	*stack;
	int	*parent;
	int	*edge_idx;
	int	*visited;
	int	top;
	int	u;
	int	i;
	int	found;
	int	*path;
	int	path_len;
	int	v;

	stack = malloc(g->node_count * sizeof(int));
	parent = malloc(g->node_count * sizeof(int));
	edge_idx = malloc(g->node_count * sizeof(int));
	visited = ft_calloc(g->node_count, sizeof(int));
	if (!stack || !parent || !edge_idx || !visited)
	{
		free(stack);
		free(parent);
		free(edge_idx);
		free(visited);
		*out_len = 0;
		return (NULL);
	}
	i = 0;
	while (i < g->node_count)
		parent[i++] = -1;
	top = 0;
	stack[0] = src;
	visited[src] = 1;
	while (top >= 0)
	{
		u = stack[top];
		if (u == sink)
			break ;
		found = 0;
		i = 0;
		while (i < g->size[u])
		{
			if (!visited[g->adj[u][i].to]
				&& g->adj[u][i].base_cap > 0
				&& g->adj[u][i].cap == 0
				&& !g->adj[u][i].visited)
			{
				visited[g->adj[u][i].to] = 1;
				parent[g->adj[u][i].to] = u;
				edge_idx[g->adj[u][i].to] = i;
				top++;
				stack[top] = g->adj[u][i].to;
				found = 1;
				break ;
			}
			i++;
		}
		if (!found)
		{
			visited[u] = 0;
			top--;
		}
	}
	if (top < 0)
	{
		free(stack);
		free(parent);
		free(edge_idx);
		free(visited);
		*out_len = 0;
		return (NULL);
	}
	path_len = 0;
	v = sink;
	while (v != -1)
	{
		path_len++;
		v = parent[v];
	}
	path = malloc(path_len * sizeof(int));
	if (!path)
	{
		free(stack);
		free(parent);
		free(edge_idx);
		free(visited);
		*out_len = 0;
		return (NULL);
	}
	v = sink;
	i = path_len - 1;
	while (i >= 0)
	{
		path[i] = v;
		if (parent[v] != -1)
			g->adj[parent[v]][edge_idx[v]].visited = 1;
		v = parent[v];
		i--;
	}
	*out_len = path_len;
	free(stack);
	free(parent);
	free(edge_idx);
	free(visited);
	return (path);
}

/* Turns a raw array of node indices into a t_path with actual room pointers.
** We skip out-nodes (index >= n_rooms) since they're just the split-graph
** artifact — only in-nodes map to real rooms. */
static t_path	*nodes_to_path(int *nodes, int n_nodes,
	t_room **rooms, int n_rooms)
{
	t_path	*p;
	t_list	*lst;
	t_list	*node;
	int		i;

	p = malloc(sizeof(t_path));
	if (!p)
		return (NULL);
	lst = NULL;
	node = ft_lstnew(rooms[nodes[0] - n_rooms]);
	if (!node)
	{
		free(p);
		return (NULL);
	}
	ft_lstadd_back(&lst, node);
	i = 1;
	while (i < n_nodes)
	{
		if (nodes[i] < n_rooms)
		{
			node = ft_lstnew(rooms[nodes[i]]);
			if (!node)
			{
				free_path_list(lst);
				free(p);
				return (NULL);
			}
			ft_lstadd_back(&lst, node);
		}
		i++;
	}
	p->path = lst;
	p->len = ft_lstsize(lst) - 1;
	return (p);
}

static t_list	*extract_paths(t_graph *g, int src, int sink,
	t_room **rooms, int n_rooms)
{
	t_list	*paths;
	int		*nodes;
	int		n_nodes;
	t_path	*p;
	t_list	*entry;

	paths = NULL;
	while (1)
	{
		nodes = extract_path(g, src, sink, &n_nodes);
		if (!nodes)
			break ;
		p = nodes_to_path(nodes, n_nodes, rooms, n_rooms);
		free(nodes);
		if (!p)
			break ;
		entry = ft_lstnew(p);
		if (!entry)
		{
			free_path_list(p->path);
			free(p);
			break ;
		}
		ft_lstadd_back(&paths, entry);
	}
	return (paths);
}

static void	free_tpath(void *content)
{
	t_path	*p;

	p = (t_path *)content;
	free_path_list(p->path);
	free(p);
}

static void	reset_scores(t_farm *farm)
{
	t_list	*cur;

	cur = farm->rooms;
	while (cur)
	{
		((t_room *)cur->content)->score = 0;
		cur = cur->next;
	}
}

t_list	*get_optimal_paths(t_farm *farm)
{
	int			n_rooms;
	t_room		**rooms;
	t_graph		*g;
	t_list		*cur;
	t_room		*room;
	t_list		*conn;
	int			u;
	int			v;
	int			cap;
	int			src;
	int			sink;
	int			*parent;
	int			*edge_idx;
	t_list		*best_paths;
	int			best_turns;
	t_list		*cur_paths;
	int			cur_count;
	t_path		*cur_arr;
	int			cur_turns;
	t_list		*tmp;
	int			i;

	if (!farm || !farm->start_room || !farm->end_room)
		return (NULL);
	n_rooms = 0;
	cur = farm->rooms;
	while (cur)
	{
		((t_room *)cur->content)->score = n_rooms++;
		cur = cur->next;
	}
	rooms = malloc(n_rooms * sizeof(t_room *));
	if (!rooms)
	{
		reset_scores(farm);
		return (NULL);
	}
	cur = farm->rooms;
	while (cur)
	{
		room = (t_room *)cur->content;
		rooms[room->score] = room;
		cur = cur->next;
	}
	g = graph_new(2 * n_rooms);
	if (!g)
	{
		free(rooms);
		reset_scores(farm);
		return (NULL);
	}
	u = 0;
	while (u < n_rooms)
	{
		cap = (rooms[u]->is_start || rooms[u]->is_end) ? n_rooms : 1;
		graph_add_edge(g, u, u + n_rooms, cap);
		u++;
	}
	cur = farm->rooms;
	while (cur)
	{
		room = (t_room *)cur->content;
		u = room->score;
		conn = room->connections;
		while (conn)
		{
			v = ((t_room *)conn->content)->score;
			if (u < v)
			{
				graph_add_edge(g, u + n_rooms, v, 1);
				graph_add_edge(g, v + n_rooms, u, 1);
			}
			conn = conn->next;
		}
		cur = cur->next;
	}
	src = farm->start_room->score + n_rooms;
	sink = farm->end_room->score;
	parent = malloc(2 * n_rooms * sizeof(int));
	edge_idx = malloc(2 * n_rooms * sizeof(int));
	if (!parent || !edge_idx)
	{
		free(parent);
		free(edge_idx);
		graph_free(g);
		free(rooms);
		reset_scores(farm);
		return (NULL);
	}
	best_paths = NULL;
	best_turns = INT_MAX;
	while (augment_flow(g, src, sink, parent, edge_idx))
	{
		graph_clear_visited(g);
		cur_paths = extract_paths(g, src, sink, rooms, n_rooms);
		cur_count = ft_lstsize(cur_paths);
		cur_turns = INT_MAX;
		if (cur_count > 0)
		{
			cur_arr = malloc(sizeof(t_path) * cur_count);
			if (cur_arr)
			{
				tmp = cur_paths;
				i = 0;
				while (tmp)
				{
					cur_arr[i] = *((t_path *)tmp->content);
					tmp = tmp->next;
					i++;
				}
				cur_turns = min_turns(cur_arr, cur_count, farm->ant_count);
				free(cur_arr);
			}
		}
		if (cur_turns < best_turns)
		{
			if (best_paths)
				ft_lstclear(&best_paths, free_tpath);
			best_paths = cur_paths;
			best_turns = cur_turns;
		}
		else
			ft_lstclear(&cur_paths, free_tpath);
	}
	free(parent);
	free(edge_idx);
	graph_free(g);
	free(rooms);
	reset_scores(farm);
	return (best_paths);
}



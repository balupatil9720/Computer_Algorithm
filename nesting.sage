from sage.all import *

def can_fit(box1, box2):
    return all(a < b for a, b in zip(box1, box2))

def longest_path_in_dag(graph):
    memo_length = {}
    memo_path = {}

    def dfs(u):
        if u in memo_length:
            return memo_length[u], memo_path[u]
        max_len = 1
        max_path = [u]
        for v in graph.neighbors_out(u):
            length, path = dfs(v)
            if 1 + length > max_len:
                max_len = 1 + length
                max_path = [u] + path
        memo_length[u] = max_len
        memo_path[u] = max_path
        return max_len, max_path

    max_overall_len = 0
    max_overall_path = []
    for v in graph.vertices():
        length, path = dfs(v)
        if length > max_overall_len:
            max_overall_len = length
            max_overall_path = path
    return max_overall_len, max_overall_path

def nested_boxes_graph(raw_boxes):
    boxes = [tuple(sorted(box)) for box in raw_boxes]
    n = len(boxes)
    G = DiGraph()
    G.add_vertices(range(n))

    for i in range(n):
        for j in range(n):
            if i != j and can_fit(boxes[i], boxes[j]):
                G.add_edge(i, j)

    length, path = longest_path_in_dag(G)

    print(f"Longest nesting chain length: {length}")
    print("Boxes in longest chain (Index: Dimensions):")
    for idx in path:
        print(f" Box {idx}: {boxes[idx]}")

    labels = {i: str(boxes[i]) for i in range(n)}
    path_edges = [(path[i], path[i+1]) for i in range(len(path)-1)]
    plot_options = {
        'edge_colors': ['red' if (u, v) in path_edges else 'gray' for u, v, _ in G.edges()],
        'vertex_size': 1000,
        'vertex_labels': labels,
        'edge_labels': False,
        'figsize': 10,
        'title': f"Nesting Graph: Max Chain Length {length}"
    }

    # Save the graph as an image file in the same directory
    G.plot(**plot_options).save("nesting_boxes.png")
    print("Graph saved as 'nesting_boxes.png' in the current directory")

    return length, path

# ---- New input boxes ----
raw_boxes = [
    (2, 2, 2),
    (3, 5, 4),
    (1, 1, 3),
    (6, 7, 8),
    (4, 4, 5),
    (1, 2, 1)
]

nested_boxes_graph(raw_boxes)

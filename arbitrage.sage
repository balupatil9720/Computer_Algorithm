import math
from sage.graphs.digraph import DiGraph

# 7 currencies
currencies = ["USD", "EUR", "JPY", "GBP", "CAD", "AUD", "CHF"]

# 11 exchange rates
rates = {
    ("USD", "EUR"): 0.92,
    ("EUR", "USD"): 1.09,
    ("USD", "JPY"): 147.0,
    ("JPY", "USD"): 0.0068,
    ("EUR", "GBP"): 0.86,
    ("GBP", "EUR"): 1.16,
    ("USD", "CAD"): 1.35,
    ("CAD", "USD"): 0.74,
    ("AUD", "CHF"): 0.64,
    ("CHF", "AUD"): 1.56,
    ("GBP", "AUD"): 1.82
}

# Build edges with -log(rate)
edges = []
for (u, v), rate in rates.items():
    edges.append((u, v, -math.log(rate)))

G = DiGraph(edges, weighted=True)

# Arbitrage detection using Bellman-Ford
def detect_arbitrage(vertices, edges):
    distance = {v: float('inf') for v in vertices}
    predecessor = {v: None for v in vertices}
    start = vertices[0]
    distance[start] = 0

    # Relax edges
    for _ in range(len(vertices)-1):
        for u, v, w in edges:
            if distance[u] + w < distance[v]:
                distance[v] = distance[u] + w
                predecessor[v] = u

    # Check for negative cycles
    for u, v, w in edges:
        if distance[u] + w < distance[v]:
            # Negative cycle detected
            cycle = [v]
            for _ in range(len(vertices)):
                v = predecessor[v]
            start_cycle = v
            cycle_list = [start_cycle]
            v = predecessor[start_cycle]
            while v != start_cycle:
                cycle_list.append(v)
                v = predecessor[v]
            cycle_list.append(start_cycle)
            cycle_list.reverse()
            return True, cycle_list
    return False, []

# Detect arbitrage
has_cycle, cycle = detect_arbitrage(currencies, edges)

# Print results
print("Currencies:", currencies)
print("\nExchange Rates:")
for (u,v), r in rates.items():
    print(f" {u} -> {v} : {r}")
print("\nArbitrage possible?", has_cycle)
if has_cycle:
    print("Arbitrage cycle found:", cycle)

# Plot full graph and save as PNG
edge_labels = {(u, v): f"{rates[(u,v)]:.4f}" for (u,v) in rates}
G.plot(edge_labels=edge_labels, layout="circular",
       vertex_size=500, vertex_colors="lightblue").save("full_graph.png")

# Highlight arbitrage cycle and save as PNG
if has_cycle:
    G.subgraph(cycle).plot(edge_labels=edge_labels, layout="circular",
                            edge_colors="red", vertex_colors="lightgreen").save("arbitrage_cycle.png")

print("\nGraphs saved as 'full_graph.png' and 'arbitrage_cycle.png' in the current folder.")

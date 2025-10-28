# import matplotlib.pyplot as plt
# import re

# # Path to gprof analysis file
# filename = "analysis.txt"

# functions = []
# times = []

# with open(filename, 'r') as f:
#     lines = f.readlines()

# # Start reading after "Flat profile:"
# start = False
# for line in lines:                                                                
#     if "Flat profile:" in line:
#         start = True
#         continue
#     if start:                    
#         # Skip empty lines
#         if line.strip() == "":
#             continue
#         # Match lines containing % time and function name
#         match = re.match(r"\s*(\d+\.\d+)\s+\S+\s+\S+\s+\S*\s*\S*\s*\S*\s*(\S+)", line)
#         if match:
#             percent = float(match.group(1))
#             func_name = match.group(2)
#             # Only include functions with non-zero % time
#             if percent > 0.0:
#                 functions.append(func_name)
#                 times.append(percent)
#         # Stop when we reach the end of the flat profile table
#         if line.strip().startswith("%") and "time" in line:
#             continue

# if not functions:
#     print("No profiling data found. Check analysis.txt.")
#     exit()

# # Plot pie chart
# plt.figure(figsize=(8,8))
# plt.pie(times, labels=functions, autopct='%1.1f%%', startangle=140)
# plt.title("QuickSort Profiling Time Distribution")
# plt.show()


import re
import networkx as nx
import matplotlib.pyplot as plt

filename = "analysis.txt"

G = nx.DiGraph()

with open(filename, 'r') as f:
    lines = f.readlines()

in_call_graph = False
current_func = None
pattern_func_header = re.compile(r'^\[(\d+)\]\s+[\d\.]+\s+\S+\s+\S+\s+[\d\+\/]*\s*(\w+)')
pattern_call = re.compile(r'\s+[\d\.]*\s*[\d\.]*\s*[\d\+\/]*\s*(\w+)\s*\[\d+\]')

for line in lines:
    # Detect start of Call graph
    if "Call graph" in line:
        in_call_graph = True
        continue
    if not in_call_graph:
        continue

    # Match function headers like: [2]  88.2  0.01  0.14  ...  quickSort [2]
    func_header = pattern_func_header.match(line)
    if func_header:
        current_func = func_header.group(2)
        if current_func not in G:
            G.add_node(current_func)
        continue

    # Match child functions called by the current function
    if current_func:
        child_match = pattern_call.match(line)
        if child_match:
            child_func = child_match.group(1)
            if child_func not in G:
                G.add_node(child_func)
            G.add_edge(current_func, child_func)

# Plot
plt.figure(figsize=(10, 8))
pos = nx.spring_layout(G, k=0.5, iterations=50)
nx.draw(
    G, pos, with_labels=True, node_color='skyblue', node_size=2500,
    font_size=10, font_weight='bold', arrowsize=20, edge_color='gray'
)
plt.title("Call Graph from gprof Analysis", fontsize=14)
plt.show()


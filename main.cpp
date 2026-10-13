#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <queue>
#include <algorithm>
#include <set>
#include <stack>

using namespace std;

// Define infinity for unreachable edges
const double INF = numeric_limits<double>::infinity();

// Function declarations for clarity
void bellman_ford(int V, vector<vector<double>>& adj_matrix, int source,
                  vector<double>& distance, vector<int>& predecessor,
                  vector<bool>& negative_cycle_nodes, int& negative_cycle_count);
vector<int> reconstruct_path(vector<int>& predecessor, int vertex);

int main() {
    // Read the number of vertices in the graph
    int V;
    cout << "Enter the number of vertices:" << endl;
    cin >> V;
    cin.ignore();

    // Initialize adjacency matrix for input
    vector<vector<double>> adj_matrix(V, vector<double>(V));
    cout << "Enter the adjacency matrix (" << V << "x" << V << "):" << endl;

    // Read the adjacency matrix
    for (int i = 0; i < V; ++i) {
        string line;
        getline(cin, line);
        istringstream iss(line);
        string val;
        int j = 0;
        while (iss >> val && j < V) {
            if (val == "INF" || val == "inf" || val == "Inf") {
                adj_matrix[i][j] = INF; // Represent no direct edge as infinity
            } else {
                adj_matrix[i][j] = stod(val); // Convert input to double
            }
            j++;
        }
        if (j < V) {
            cerr << "Insufficient values in row " << i << endl;
            return 1;
        }
    }

    // Read the source vertex
    int source;
    cout << "Enter the source vertex:" << endl;
    cin >> source;

    // Initialize variables for Bellman-Ford
    vector<double> distance(V, INF); // Stores shortest distances from the source
    vector<int> predecessor(V, -1); // Stores predecessor for each vertex
    vector<bool> negative_cycle_nodes(V, false); // Flags vertices in negative cycles
    int negative_cycle_count = 0; // Count of negative weight cycles

    // Run the Bellman-Ford algorithm
    bellman_ford(V, adj_matrix, source, distance, predecessor, negative_cycle_nodes, negative_cycle_count);

    // Output the results
    cout << "\nShortest paths from source vertex " << source << ":" << endl;
    for (int vertex = 0; vertex < V; ++vertex) {
        if (distance[vertex] == INF) {
            // Vertex is unreachable from the source
            cout << "Vertex " << vertex << " is unreachable from the source." << endl;
        } else if (negative_cycle_nodes[vertex]) {
            // Vertex is part of a negative weight cycle
            cout << "Vertex " << vertex << " is unreachable due to a negative weight cycle." << endl;
        } else {
            // Reconstruct and display the shortest path
            vector<int> path = reconstruct_path(predecessor, vertex);
            cout << "Shortest path to vertex " << vertex << ": ";
            for (size_t i = 0; i < path.size(); ++i) {
                cout << path[i];
                if (i != path.size() - 1) {
                    cout << " -> ";
                }
            }
            cout << " with cost " << distance[vertex] << endl;
        }
    }

    // Display the total count of negative weight cycles
    cout << "\nTotal number of negative weight cycles in the graph: " << negative_cycle_count << endl;

    return 0;
}

// Bellman-Ford algorithm implementation
void bellman_ford(int V, vector<vector<double>>& adj_matrix, int source,
                  vector<double>& distance, vector<int>& predecessor,
                  vector<bool>& negative_cycle_nodes, int& negative_cycle_count) {
    vector<tuple<int, int, double>> edges;

    // Convert adjacency matrix to edge list
    for (int u = 0; u < V; ++u) {
        for (int v = 0; v < V; ++v) {
            double weight = adj_matrix[u][v];
            if (weight != INF) {
                edges.push_back(make_tuple(u, v, weight));
            }
        }
    }

    // Initialize distances and predecessors
    distance[source] = 0;

    // Relax edges V-1 times
    for (int i = 0; i < V - 1; ++i) {
        for (auto& edge : edges) {
            int u, v;
            double weight;
            tie(u, v, weight) = edge;
            if (distance[u] != INF && distance[u] + weight < distance[v]) {
                distance[v] = distance[u] + weight;
                predecessor[v] = u;
            }
        }
    }

    // Detect negative weight cycles
    vector<int> affected_nodes;
    for (auto& edge : edges) {
        int u, v;
        double weight;
        tie(u, v, weight) = edge;
        if (distance[u] != INF && distance[u] + weight < distance[v]) {
            negative_cycle_nodes[v] = true;
            affected_nodes.push_back(v);
        }
    }

    // Count distinct negative cycles using DFS
    vector<bool> visited(V, false);
    negative_cycle_count = 0;

    for (int node : affected_nodes) {
        if (!visited[node]) {
            // Found a new negative weight cycle
            negative_cycle_count++;

            // Mark all vertices in the cycle
            stack<int> s;
            s.push(node);
            while (!s.empty()) {
                int current = s.top();
                s.pop();
                if (!visited[current]) {
                    visited[current] = true;
                    negative_cycle_nodes[current] = true;
                    for (auto& edge : edges) {
                        int u, v;
                        double weight;
                        tie(u, v, weight) = edge;
                        if (u == current && !visited[v]) {
                            s.push(v);
                        }
                    }
                }
            }
        }
    }

    // Mark distances of nodes in negative cycles as negative infinity
    for (int i = 0; i < V; ++i) {
        if (negative_cycle_nodes[i]) {
            distance[i] = -INF;
        }
    }
}

// Function to reconstruct the shortest path using predecessor array
vector<int> reconstruct_path(vector<int>& predecessor, int vertex) {
    vector<int> path;
    if (predecessor[vertex] == -1 && vertex != predecessor[vertex]) {
        // No path exists
        path.push_back(vertex);
        return path;
    }
    while (vertex != -1) {
        path.push_back(vertex);
        vertex = predecessor[vertex];
    }
    reverse(path.begin(), path.end());
    return path;
}
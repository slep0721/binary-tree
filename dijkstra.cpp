#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

using namespace std;

struct Edge {
    int u;
    int v;
    int weight;
};

using Graph = vector<vector<pair<int, int>>>; // {neighbor, weight}

void addUndirectedEdge(Graph& graph, int u, int v, int weight) {
    graph[u].push_back({v, weight});
    graph[v].push_back({u, weight});
}

void dijkstra(const Graph& graph, int source, vector<int>& dist,
              vector<int>& parent) {
    const int infinity = numeric_limits<int>::max();
    dist.assign(graph.size(), infinity);
    parent.assign(graph.size(), -1);

    // Each heap item is {distance, node}; greater<> makes the top the minimum.
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> minHeap;
    dist[source] = 0;
    minHeap.push({0, source});

    while (!minHeap.empty()) {
        const auto [currentDist, node] = minHeap.top();
        minHeap.pop();

        if (currentDist != dist[node]) continue; // 過期條目：目前距離已更短，跳過（lazy deletion）。

        for (const auto& [neighbor, weight] : graph[node]) {
            const int candidate = currentDist + weight;
            if (candidate < dist[neighbor]) {
                dist[neighbor] = candidate;
                parent[neighbor] = node;
                minHeap.push({candidate, neighbor});
            }
        }
    }
}

void printPath(const vector<int>& parent, int source, int target) {
    vector<int> path;
    for (int node = target; node != -1; node = parent[node]) {
        path.push_back(node);
        if (node == source) break;
    }

    if (path.empty() || path.back() != source) {
        cout << "No path from " << source << " to " << target << '\n';
        return;
    }

    reverse(path.begin(), path.end());
    cout << "path ";
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) cout << " -> ";
        cout << path[i];
    }
    cout << '\n';
}

int main() {
    constexpr int nodeCount = 6;
    constexpr int source = 0;
    constexpr int target = 5;

    const vector<Edge> edges = {
        {0, 1, 2}, {0, 2, 5}, {1, 2, 1}, {1, 3, 4},
        {2, 4, 2}, {4, 3, 1}, {3, 5, 3}, {4, 5, 6},
    };

    Graph graph(nodeCount);
    for (const Edge& edge : edges) {
        addUndirectedEdge(graph, edge.u, edge.v, edge.weight);
    }

    vector<int> dist;
    vector<int> parent;
    dijkstra(graph, source, dist, parent);

    cout << "dist[" << target << "] = " << dist[target] << '\n';
    printPath(parent, source, target);
}

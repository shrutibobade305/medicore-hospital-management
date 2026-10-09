#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <limits>
#include "../third_party/json.hpp"

namespace medicore {

struct GraphVertex {
    std::string id;
    std::string label;
    std::string floor;
    std::string description;
    double x = 0.0;
    double y = 0.0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(GraphVertex, id, label, floor, description, x, y)
};

struct GraphEdge {
    std::string to;
    double weight;          // In meters or travel seconds
    bool isClosed = false;  // Simulates corridor/hallway maintenance closure
};

struct DijkstraResult {
    bool reachable = false;
    double totalCost = 0.0;
    std::vector<std::string> path;
    std::vector<std::string> exploredOrder;
    std::vector<std::string> instructions;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(DijkstraResult, reachable, totalCost, path, exploredOrder, instructions)
};

struct TraversalResult {
    std::string algorithm;  // "BFS" or "DFS"
    std::string startId;
    std::vector<std::string> visitOrder;
    std::vector<std::pair<std::string, std::string>> treeEdges;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(TraversalResult, algorithm, startId, visitOrder, treeEdges)
};

/**
 * Custom Weighted Graph with Adjacency List
 *
 * Implements:
 *   - Adjacency List Representation
 *   - Dijkstra's Shortest Path Algorithm: O((V + E) log V)
 *   - Breadth-First Search (BFS): O(V + E)
 *   - Depth-First Search (DFS): O(V + E)
 *   - Dynamic Corridor Closure & Route Recalculation
 */
class HospitalGraph {
public:
    HospitalGraph();
    ~HospitalGraph() = default;

    void addVertex(const GraphVertex& vertex);
    void addEdge(const std::string& u, const std::string& v, double weight, bool bidirectional = true);
    bool setEdgeClosure(const std::string& u, const std::string& v, bool closed);

    DijkstraResult findShortestPath(const std::string& startId, const std::string& endId) const;
    TraversalResult bfs(const std::string& startId) const;
    TraversalResult dfs(const std::string& startId) const;

    const GraphVertex* getVertex(const std::string& id) const;
    std::vector<GraphVertex> getAllVertices() const;
    nlohmann::json getGraphData() const;

    void initializeDefaultHospitalMap();

private:
    void dfsUtil(const std::string& current,
                 std::unordered_map<std::string, bool>& visited,
                 std::vector<std::string>& visitOrder,
                 std::vector<std::pair<std::string, std::string>>& treeEdges) const;

    std::unordered_map<std::string, GraphVertex> vertices_;
    std::vector<std::string> vertexOrder_;
    std::unordered_map<std::string, std::vector<GraphEdge>> adjList_;
};

} // namespace medicore

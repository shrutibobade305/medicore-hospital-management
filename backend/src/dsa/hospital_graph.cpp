#include "../../include/dsa/hospital_graph.hpp"
#include <queue>
#include <stack>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace medicore {

HospitalGraph::HospitalGraph() {
    initializeDefaultHospitalMap();
}

void HospitalGraph::addVertex(const GraphVertex& vertex) {
    if (vertices_.find(vertex.id) == vertices_.end()) {
        vertexOrder_.push_back(vertex.id);
    }
    vertices_[vertex.id] = vertex;
    if (adjList_.find(vertex.id) == adjList_.end()) {
        adjList_[vertex.id] = {};
    }
}

void HospitalGraph::addEdge(const std::string& u, const std::string& v, double weight, bool bidirectional) {
    if (vertices_.find(u) == vertices_.end() || vertices_.find(v) == vertices_.end()) return;

    adjList_[u].push_back({v, weight, false});
    if (bidirectional) {
        adjList_[v].push_back({u, weight, false});
    }
}

bool HospitalGraph::setEdgeClosure(const std::string& u, const std::string& v, bool closed) {
    bool found = false;
    if (adjList_.find(u) != adjList_.end()) {
        for (auto& edge : adjList_[u]) {
            if (edge.to == v) {
                edge.isClosed = closed;
                found = true;
            }
        }
    }
    if (adjList_.find(v) != adjList_.end()) {
        for (auto& edge : adjList_[v]) {
            if (edge.to == u) {
                edge.isClosed = closed;
                found = true;
            }
        }
    }
    return found;
}

const GraphVertex* HospitalGraph::getVertex(const std::string& id) const {
    auto it = vertices_.find(id);
    return (it != vertices_.end()) ? &it->second : nullptr;
}

std::vector<GraphVertex> HospitalGraph::getAllVertices() const {
    std::vector<GraphVertex> list;
    for (const auto& id : vertexOrder_) {
        list.push_back(vertices_.at(id));
    }
    return list;
}

DijkstraResult HospitalGraph::findShortestPath(const std::string& startId, const std::string& endId) const {
    DijkstraResult result;
    result.reachable = false;
    result.totalCost = 0.0;

    if (vertices_.find(startId) == vertices_.end() || vertices_.find(endId) == vertices_.end()) {
        return result;
    }

    if (startId == endId) {
        result.reachable = true;
        result.totalCost = 0.0;
        result.path = {startId};
        result.exploredOrder = {startId};
        result.instructions = {"You are already at the destination: " + vertices_.at(startId).label};
        return result;
    }

    const double INF = std::numeric_limits<double>::infinity();
    std::unordered_map<std::string, double> dist;
    std::unordered_map<std::string, std::string> parent;
    std::unordered_map<std::string, bool> visited;

    for (const auto& [id, _] : vertices_) {
        dist[id] = INF;
        visited[id] = false;
    }

    dist[startId] = 0.0;

    // Min-priority queue: pair of (distance, vertexId)
    using PQElement = std::pair<double, std::string>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;
    pq.push({0.0, startId});

    while (!pq.empty()) {
        auto [currentDist, u] = pq.top();
        pq.pop();

        if (visited[u]) continue;
        visited[u] = true;
        result.exploredOrder.push_back(u);

        if (u == endId) break;

        auto adjIt = adjList_.find(u);
        if (adjIt == adjList_.end()) continue;

        for (const auto& edge : adjIt->second) {
            if (edge.isClosed) continue; // Skip closed hallways!

            const std::string& v = edge.to;
            double weight = edge.weight;

            if (!visited[v] && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (dist[endId] == INF) {
        // Destination unreachable
        result.reachable = false;
        result.instructions = {"No available open route between " + vertices_.at(startId).label +
                               " and " + vertices_.at(endId).label + ". Corridors may be closed for maintenance."};
        return result;
    }

    // Reconstruct path
    result.reachable = true;
    result.totalCost = dist[endId];

    std::vector<std::string> pathReversed;
    std::string curr = endId;
    while (curr != startId) {
        pathReversed.push_back(curr);
        curr = parent[curr];
    }
    pathReversed.push_back(startId);
    std::reverse(pathReversed.begin(), pathReversed.end());
    result.path = pathReversed;

    // Build human-friendly step instructions
    for (size_t i = 0; i < result.path.size() - 1; ++i) {
        std::string fromId = result.path[i];
        std::string toId = result.path[i + 1];
        const auto& fromV = vertices_.at(fromId);
        const auto& toV = vertices_.at(toId);

        // Find edge weight
        double segCost = 0;
        for (const auto& e : adjList_.at(fromId)) {
            if (e.to == toId) {
                segCost = e.weight;
                break;
            }
        }

        std::ostringstream oss;
        oss << "Step " << (i + 1) << ": Move from " << fromV.label << " (" << fromV.floor << ") to "
            << toV.label << " (" << toV.floor << ") — approx " << std::fixed << std::setprecision(0) << segCost << " meters";
        result.instructions.push_back(oss.str());
    }

    return result;
}

TraversalResult HospitalGraph::bfs(const std::string& startId) const {
    TraversalResult result;
    result.algorithm = "BFS";
    result.startId = startId;

    if (vertices_.find(startId) == vertices_.end()) return result;

    std::unordered_map<std::string, bool> visited;
    for (const auto& [id, _] : vertices_) {
        visited[id] = false;
    }

    std::queue<std::string> q;
    q.push(startId);
    visited[startId] = true;

    while (!q.empty()) {
        std::string u = q.front();
        q.pop();
        result.visitOrder.push_back(u);

        auto it = adjList_.find(u);
        if (it != adjList_.end()) {
            for (const auto& edge : it->second) {
                if (!edge.isClosed && !visited[edge.to]) {
                    visited[edge.to] = true;
                    q.push(edge.to);
                    result.treeEdges.push_back({u, edge.to});
                }
            }
        }
    }
    return result;
}

TraversalResult HospitalGraph::dfs(const std::string& startId) const {
    TraversalResult result;
    result.algorithm = "DFS";
    result.startId = startId;

    if (vertices_.find(startId) == vertices_.end()) return result;

    std::unordered_map<std::string, bool> visited;
    for (const auto& [id, _] : vertices_) {
        visited[id] = false;
    }

    dfsUtil(startId, visited, result.visitOrder, result.treeEdges);
    return result;
}

void HospitalGraph::dfsUtil(const std::string& current,
                            std::unordered_map<std::string, bool>& visited,
                            std::vector<std::string>& visitOrder,
                            std::vector<std::pair<std::string, std::string>>& treeEdges) const {
    visited[current] = true;
    visitOrder.push_back(current);

    auto it = adjList_.find(current);
    if (it != adjList_.end()) {
        for (const auto& edge : it->second) {
            if (!edge.isClosed && !visited[edge.to]) {
                treeEdges.push_back({current, edge.to});
                dfsUtil(edge.to, visited, visitOrder, treeEdges);
            }
        }
    }
}

void HospitalGraph::initializeDefaultHospitalMap() {
    vertices_.clear();
    vertexOrder_.clear();
    adjList_.clear();

    // 10 Key Hospital Departments with 2D Floor Layout Coordinates
    addVertex({"ENTRANCE", "Main Entrance", "Ground Floor", "Hospital Arrival & Triage Gate", 80.0, 320.0});
    addVertex({"RECEPTION", "Reception & Information", "Ground Floor", "Registration & Central Helpdesk", 240.0, 320.0});
    addVertex({"EMERGENCY", "Emergency Department", "Ground Floor", "Trauma Care, Resuscitation & Triage", 240.0, 140.0});
    addVertex({"RADIOLOGY", "Radiology & Imaging", "Ground Floor", "X-Ray, CT Scan, MRI & Ultrasound", 440.0, 140.0});
    addVertex({"PHARMACY", "Central Pharmacy", "Ground Floor", "24/7 Dispensing & Medications", 440.0, 320.0});
    addVertex({"LABORATORY", "Pathology & Blood Lab", "1st Floor", "Clinical Diagnostics & Phlebotomy", 240.0, 500.0});
    addVertex({"OPD", "Outpatient Clinics (OPD)", "1st Floor", "Consultation Rooms (101-115)", 440.0, 500.0});
    addVertex({"WARD", "General Inpatient Ward", "1st Floor", "Medical/Surgical Inpatient Beds", 650.0, 320.0});
    addVertex({"ICU", "Intensive Care Unit (ICU)", "2nd Floor", "Critical Care & Life Support", 650.0, 140.0});
    addVertex({"OT", "Operation Theatre Complex", "2nd Floor", "Surgical Theatres & Recovery Suites", 820.0, 230.0});

    // Edges with realistic distances (meters)
    addEdge("ENTRANCE", "RECEPTION", 25.0);
    addEdge("RECEPTION", "EMERGENCY", 35.0);
    addEdge("RECEPTION", "PHARMACY", 30.0);
    addEdge("RECEPTION", "LABORATORY", 40.0);

    addEdge("EMERGENCY", "RADIOLOGY", 25.0);
    addEdge("EMERGENCY", "ICU", 45.0);
    addEdge("RADIOLOGY", "PHARMACY", 30.0);
    addEdge("RADIOLOGY", "OT", 60.0);

    addEdge("PHARMACY", "WARD", 40.0);
    addEdge("PHARMACY", "OPD", 35.0);

    addEdge("LABORATORY", "OPD", 30.0);
    addEdge("OPD", "WARD", 50.0);

    addEdge("WARD", "ICU", 35.0);
    addEdge("WARD", "OT", 45.0);
    addEdge("ICU", "OT", 20.0);
}

nlohmann::json HospitalGraph::getGraphData() const {
    nlohmann::json j;
    nlohmann::json vList = nlohmann::json::array();
    for (const auto& id : vertexOrder_) {
        vList.push_back(vertices_.at(id));
    }
    j["vertices"] = vList;

    nlohmann::json eList = nlohmann::json::array();
    // Unique undirected edges
    std::unordered_map<std::string, bool> seenEdge;
    for (const auto& [u, edges] : adjList_) {
        for (const auto& e : edges) {
            std::string key = (u < e.to) ? (u + "--" + e.to) : (e.to + "--" + u);
            if (!seenEdge[key]) {
                seenEdge[key] = true;
                nlohmann::json edgeObj;
                edgeObj["from"] = u;
                edgeObj["to"] = e.to;
                edgeObj["weight"] = e.weight;
                edgeObj["isClosed"] = e.isClosed;
                eList.push_back(edgeObj);
            }
        }
    }
    j["edges"] = eList;
    return j;
}

} // namespace medicore

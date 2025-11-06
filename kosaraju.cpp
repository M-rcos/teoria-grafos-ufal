#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <functional>
using namespace std;

class Graph {
private:
    int V;
    vector<vector<int>> adj;
    vector<vector<int>> adjTranspose;

public:
    Graph(int vertices) : V(vertices) {
        adj.resize(vertices);
        adjTranspose.resize(vertices);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adjTranspose[v].push_back(u);
    }

    void DFS(int v, vector<bool>& visited, stack<int>& stack, const vector<vector<int>>& graph) {
        visited[v] = true;
        for (int neighbor : graph[v]) {
            if (!visited[neighbor]) {
                DFS(neighbor, visited, stack, graph);
            }
        }
        stack.push(v);
    }

    void DFSCollect(int v, vector<bool>& visited, vector<int>& component, const vector<vector<int>>& graph) {
        visited[v] = true;
        component.push_back(v);
        for (int neighbor : graph[v]) {
            if (!visited[neighbor]) {
                DFSCollect(neighbor, visited, component, graph);
            }
        }
    }

    vector<vector<int>> kosaraju() {
        vector<bool> visited(V, false);
        stack<int> stack;

        // Primeira DFS (grafo original)
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                DFS(i, visited, stack, adj);
            }
        }

        // Reset visited
        fill(visited.begin(), visited.end(), false);

        // Segunda DFS (grafo transposto)
        vector<vector<int>> scc;
        while (!stack.empty()) {
            int v = stack.top();
            stack.pop();

            if (!visited[v]) {
                vector<int> component;
                DFSCollect(v, visited, component, adjTranspose);
                scc.push_back(component);
            }
        }

        return scc;
    }

    int getVertices() const { return V; }
};

Graph readSCCFormat(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro ao abrir arquivo: " << filename << endl;
        exit(1);
    }

    string line;
    int vertices, edges;

    // Ler primeira linha: vértices arestas
    getline(file, line);
    stringstream ss(line);
    ss >> vertices >> edges;

    Graph g(vertices);

    // Ler arestas
    int edges_read = 0;
    while (edges_read < edges && getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        int u, v;

        if (ss >> u >> v) {
            g.addEdge(u - 1, v - 1); // Converter 1-based para 0-based
            edges_read++;
        }
    }

    file.close();
    return g;
}

void printSCCResults(const vector<vector<int>>& scc) {
    // Converter componentes para formato de saída
    vector<vector<int>> sortedComponents;

    for (auto& component : scc) {
        vector<int> sortedComponent = component;
        sort(sortedComponent.begin(), sortedComponent.end());
        sortedComponents.push_back(sortedComponent);
    }

    // Ordenar componentes pelo menor vértice
    sort(sortedComponents.begin(), sortedComponents.end(),
         [](const vector<int>& a, const vector<int>& b) {
             return a[0] < b[0];
         });

    // Imprimir no formato esperado: uma linha por componente, vértices separados por espaço
    for (size_t i = 0; i < sortedComponents.size(); i++) {
        for (size_t j = 0; j < sortedComponents[i].size(); j++) {
            if (j > 0) cout << " ";
            cout << (sortedComponents[i][j] + 1); // Converter para 1-based
        }
        if (i < sortedComponents.size() - 1) {
            cout << endl; // Nova linha para cada componente
        }
    }
}

int main(int argc, char* argv[]) {
    string filename;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-f" && i + 1 < argc) {
            filename = argv[++i];
        }
    }

    if (filename.empty()) {
        cerr << "Uso: " << argv[0] << " -f <arquivo>" << endl;
        return 1;
    }

    Graph g = readSCCFormat(filename);
    vector<vector<int>> scc = g.kosaraju();
    printSCCResults(scc);

    return 0;
}

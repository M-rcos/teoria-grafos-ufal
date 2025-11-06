#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <limits>
#include <functional>
#include <algorithm>

using namespace std;

const int INF = numeric_limits<int>::max();

struct Aresta {
    int v, peso;
    Aresta(int v, int peso) : v(v), peso(peso) {}
};

class Grafo {
public:
    int n, m;
    vector<vector<Aresta>> adj;

    Grafo(int n) : n(n), m(0), adj(n) {}

    void adicionaAresta(int u, int v, int peso) {
        if (u < 0 || v < 0 || u >= n || v >= n) return;
        adj[u].emplace_back(v, peso);
        adj[v].emplace_back(u, peso);
        m++;
    }
};

pair<long long, vector<pair<int, int>>> prim(Grafo &g, int inicio) {
    int n = g.n;
    vector<int> dist(n, INF);
    vector<int> pai(n, -1);
    vector<bool> visitado(n, false);

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    long long custoTotal = 0;
    vector<pair<int, int>> arestasAGM;

    dist[inicio] = 0;
    pq.push({0, inicio});

    while (!pq.empty()) {

        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (visitado[u]) continue;

        visitado[u] = true;
        custoTotal += d;

        for (auto &aresta : g.adj[u]) {
            int v = aresta.v;
            int peso = aresta.peso;

            if (!visitado[v] && peso < dist[v]) {
                dist[v] = peso;
                pai[v] = u;
                pq.push({peso, v});
            }
        }
    }

    // Coletar arestas da AGM
    for (int i = 0; i < n; i++) {
        if (pai[i] != -1) {
            int u = min(pai[i], i);
            int v = max(pai[i], i);
            arestasAGM.emplace_back(u, v);
        }
    }

    // Ordenar arestas para consistência
    sort(arestasAGM.begin(), arestasAGM.end());

    return {custoTotal, arestasAGM};
}

Grafo lerGrafo(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir arquivo: " << nomeArquivo << endl;
        exit(1);
    }

    string linha;
    int numVertices, numArestas;

    while (getline(arquivo, linha)) {
        if (linha.empty() || linha[0] == '%') continue;

        stringstream ss(linha);
        if (ss >> numVertices >> numArestas) {
            if (numVertices > 0 && numArestas > 0) break;
        }

        stringstream ss2(linha);
        int linhas, colunas;
        if (ss2 >> linhas >> colunas >> numArestas) {
            numVertices = linhas;
            break;
        }
    }

    Grafo g(numVertices);

    int arestasLidas = 0;
    while (arestasLidas < numArestas && getline(arquivo, linha)) {
        if (linha.empty() || linha[0] == '%') continue;

        stringstream ss(linha);
        int u, v, peso = 1;

        if (ss >> u >> v) {
            ss >> peso;
            if (u >= 1 && u <= numVertices && v >= 1 && v <= numVertices) {
                g.adicionaAresta(u - 1, v - 1, peso);
                arestasLidas++;
            }
        }
    }

    arquivo.close();
    return g;
}

void mostrarHelp(const string& nomePrograma) {
    cout << "Uso: " << nomePrograma << " -f <arquivo> [-o <arquivo_saida>] [-i <vertice_inicial>] [-s]" << endl;
    cout << "Opções:" << endl;
    cout << "  -h              : Mostra este help" << endl;
    cout << "  -f <arquivo>    : Arquivo de entrada com o grafo" << endl;
    cout << "  -o <arquivo>    : Redireciona a saída para o arquivo" << endl;
    cout << "  -i <vertice>    : Vértice inicial (padrão: 1)" << endl;
    cout << "  -s              : Mostra a solução (arestas da AGM)" << endl;
}

int main(int argc, char *argv[]) {
    string nomeArquivo, arquivoSaida;
    bool imprimirSubgrafo = false;
    int inicio = 0; // 0-based, padrão vértice 1

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-h") {
            mostrarHelp(argv[0]);
            return 0;
        } else if (arg == "-f" && i + 1 < argc) {
            nomeArquivo = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            arquivoSaida = argv[++i];
        } else if (arg == "-s") {
            imprimirSubgrafo = true;
        } else if (arg == "-i" && i + 1 < argc) {
            inicio = stoi(argv[++i]) - 1; // Converter para 0-based
        }
    }

    if (nomeArquivo.empty()) {
        cerr << "Erro: Arquivo de entrada não especificado." << endl;
        mostrarHelp(argv[0]);
        return 1;
    }

    Grafo g = lerGrafo(nomeArquivo);
    pair<long long, vector<pair<int, int>>> resultado = prim(g, inicio);
    long long custoAGM = resultado.first;
    vector<pair<int, int>> arestasAGM = resultado.second;

    // Output
    ostream* output = &cout;
    ofstream outFile;

    if (!arquivoSaida.empty()) {
        outFile.open(arquivoSaida);
        if (outFile.is_open()) {
            output = &outFile;
        } else {
            cerr << "Erro ao abrir arquivo de saída: " << arquivoSaida << endl;
            return 1;
        }
    }

    if (!imprimirSubgrafo) {
        *output << custoAGM << endl;
    } else {
        for (size_t i = 0; i < arestasAGM.size(); i++) {
            if (i > 0) *output << " ";
            *output << "(" << (arestasAGM[i].first + 1) << "," << (arestasAGM[i].second + 1) << ")";
        }
        *output << endl;
    }

    if (outFile.is_open()) {
        outFile.close();
    }

    return 0;
}

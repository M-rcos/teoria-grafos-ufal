#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>
#include <string>
#include <climits>
#include <algorithm>
using namespace std;

typedef pair<int, int> par_int;

class Grafo {
private:
    int numVertices;
    vector<vector<par_int>> listaAdj;

public:
    Grafo(int vertices) : numVertices(vertices) {
        listaAdj.resize(vertices);
    }

    void adicionarAresta(int u, int v, int peso) {
        listaAdj[u].push_back({v, peso});
        listaAdj[v].push_back({u, peso});
    }

    vector<int> dijkstra(int inicio) {
        vector<int> distancias(numVertices, INT_MAX);
        distancias[inicio] = 0;

        priority_queue<par_int, vector<par_int>, greater<par_int>> fila_prioridade;
        fila_prioridade.push({0, inicio});

        while (!fila_prioridade.empty()) {
            int u = fila_prioridade.top().second;
            int distancia_atual = fila_prioridade.top().first;
            fila_prioridade.pop();

            if (distancia_atual > distancias[u]) continue;

            for (auto &aresta : listaAdj[u]) {
                int v = aresta.first;
                int peso = aresta.second;

                if (distancias[u] != INT_MAX) {
                    int nova_distancia = distancias[u] + peso;
                    if (nova_distancia < distancias[v]) {
                        distancias[v] = nova_distancia;
                        fila_prioridade.push({nova_distancia, v});
                    }
                }
            }
        }

        return distancias;
    }
};

Grafo lerFormatoSimples(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir arquivo: " << nomeArquivo << endl;
        exit(1);
    }

    string linha;
    int vertices, arestas;

    getline(arquivo, linha);
    stringstream ss(linha);
    ss >> vertices >> arestas;

    Grafo g(vertices);

    int arestas_lidas = 0;
    while (arestas_lidas < arestas && getline(arquivo, linha)) {
        if (linha.empty()) continue;

        stringstream ss(linha);
        int u, v, peso;

        if (ss >> u >> v >> peso) {
            g.adicionarAresta(u - 1, v - 1, peso);
            arestas_lidas++;
        }
    }

    arquivo.close();
    return g;
}

void imprimirResultados(const vector<int>& distancias, int inicio, ostream& out = cout) {
    if (distancias.empty()) return;

    vector<pair<int, int>> resultados;

    for (int i = 0; i < distancias.size(); i++) {
        resultados.push_back({i, distancias[i]});
    }

    sort(resultados.begin(), resultados.end());

    for (size_t i = 0; i < resultados.size(); i++) {
        if (i > 0) out << " ";
        int valor = (resultados[i].second == INT_MAX) ? -1 : resultados[i].second;
        out << (resultados[i].first + 1) << ":" << valor;
    }
    out << endl;
}

void mostrarHelp(const string& nomePrograma) {
    cout << "Uso: " << nomePrograma << " -f <arquivo> [-o <arquivo_saida>] [-i <vertice_inicial>]" << endl;
    cout << "Opções:" << endl;
    cout << "  -h              : Mostra este help" << endl;
    cout << "  -f <arquivo>    : Arquivo de entrada com o grafo" << endl;
    cout << "  -o <arquivo>    : Redireciona a saída para o arquivo" << endl;
    cout << "  -i <vertice>    : Vértice inicial (padrão: 1)" << endl;
}

int main(int argc, char* argv[]) {
    string nomeArquivo, arquivoSaida;
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
        } else if (arg == "-i" && i + 1 < argc) {
            inicio = stoi(argv[++i]) - 1; // Converter para 0-based
        }
    }

    if (nomeArquivo.empty()) {
        cerr << "Erro: Arquivo de entrada não especificado." << endl;
        mostrarHelp(argv[0]);
        return 1;
    }

    Grafo g = lerFormatoSimples(nomeArquivo);
    vector<int> distancias = g.dijkstra(inicio);

    // Output
    if (!arquivoSaida.empty()) {
        ofstream outFile(arquivoSaida);
        if (outFile.is_open()) {
            imprimirResultados(distancias, inicio, outFile);
            outFile.close();
        } else {
            cerr << "Erro ao abrir arquivo de saída: " << arquivoSaida << endl;
            return 1;
        }
    } else {
        imprimirResultados(distancias, inicio);
    }

    return 0;
}

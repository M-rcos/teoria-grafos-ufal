#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <numeric>
using namespace std;

struct Aresta {
    int u, v, peso;
    Aresta(int u, int v, int peso) : u(u), v(v), peso(peso) {}

    bool operator<(const Aresta& outra) const {
        return peso < outra.peso;
    }
};

class UniaoBusca {
private:
    vector<int> pai, rank;

public:
    UniaoBusca(int n) {
        pai.resize(n);
        rank.resize(n, 0);
        iota(pai.begin(), pai.end(), 0);
    }

    int encontrar(int x) {
        if (pai[x] != x) {
            pai[x] = encontrar(pai[x]);
        }
        return pai[x];
    }

    void unir(int x, int y) {
        int raizX = encontrar(x);
        int raizY = encontrar(y);

        if (raizX != raizY) {
            if (rank[raizX] < rank[raizY]) {
                pai[raizX] = raizY;
            } else if (rank[raizX] > rank[raizY]) {
                pai[raizY] = raizX;
            } else {
                pai[raizY] = raizX;
                rank[raizX]++;
            }
        }
    }
};

class Grafo {
private:
    int numVertices;
    vector<Aresta> arestas;

public:
    Grafo(int vertices) : numVertices(vertices) {}

    void adicionarAresta(int u, int v, int peso) {
        arestas.emplace_back(u, v, peso);
    }

    pair<int, vector<Aresta>> kruskalAGM() {
        sort(arestas.begin(), arestas.end());

        UniaoBusca ub(numVertices);
        int custoAGM = 0;
        vector<Aresta> agmArestas;

        for (const Aresta& aresta : arestas) {
            if (ub.encontrar(aresta.u) != ub.encontrar(aresta.v)) {
                ub.unir(aresta.u, aresta.v);
                custoAGM += aresta.peso;
                agmArestas.push_back(aresta);

                if (agmArestas.size() == numVertices - 1) {
                    break;
                }
            }
        }

        return {custoAGM, agmArestas};
    }
};

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
                g.adicionarAresta(u - 1, v - 1, peso);
                arestasLidas++;
            }
        }
    }

    arquivo.close();
    return g;
}

void mostrarHelp(const string& nomePrograma) {
    cout << "Uso: " << nomePrograma << " -f <arquivo> [-o <arquivo_saida>] [-s]" << endl;
    cout << "Opções:" << endl;
    cout << "  -h              : Mostra este help" << endl;
    cout << "  -f <arquivo>    : Arquivo de entrada com o grafo" << endl;
    cout << "  -o <arquivo>    : Redireciona a saída para o arquivo" << endl;
    cout << "  -s              : Mostra a solução (arestas da AGM)" << endl;
}

int main(int argc, char* argv[]) {
    string nomeArquivo, arquivoSaida;
    bool imprimirSubgrafo = false;

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
        }
    }

    if (nomeArquivo.empty()) {
        cerr << "Erro: Arquivo de entrada não especificado." << endl;
        mostrarHelp(argv[0]);
        return 1;
    }

    Grafo g = lerGrafo(nomeArquivo);
    pair<int, vector<Aresta>> resultado = g.kruskalAGM();
    int custoAGM = resultado.first;
    vector<Aresta> arestasAGM = resultado.second;

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
            *output << "(" << (arestasAGM[i].u + 1) << "," << (arestasAGM[i].v + 1) << ")";
        }
        *output << endl;
    }

    if (outFile.is_open()) {
        outFile.close();
    }

    return 0;
}

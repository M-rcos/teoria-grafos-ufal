#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <functional>
using namespace std;

class Grafo {
private:
    int numVertices;
    vector<vector<int>> listaAdj;
    vector<vector<int>> listaAdjTransposta;

public:
    Grafo(int vertices) : numVertices(vertices) {
        listaAdj.resize(vertices);
        listaAdjTransposta.resize(vertices);
    }

    void adicionarAresta(int u, int v) {
        listaAdj[u].push_back(v);
        listaAdjTransposta[v].push_back(u);
    }

    void DFS(int v, vector<bool>& visitado, stack<int>& pilha, const vector<vector<int>>& grafo) {
        visitado[v] = true;
        for (int vizinho : grafo[v]) {
            if (!visitado[vizinho]) {
                DFS(vizinho, visitado, pilha, grafo);
            }
        }
        pilha.push(v);
    }

    void DFSColetar(int v, vector<bool>& visitado, vector<int>& componente, const vector<vector<int>>& grafo) {
        visitado[v] = true;
        componente.push_back(v);
        for (int vizinho : grafo[v]) {
            if (!visitado[vizinho]) {
                DFSColetar(vizinho, visitado, componente, grafo);
            }
        }
    }

    vector<vector<int>> kosaraju() {
        vector<bool> visitado(numVertices, false);
        stack<int> pilha;

        // Primeira DFS (grafo original)
        for (int i = 0; i < numVertices; i++) {
            if (!visitado[i]) {
                DFS(i, visitado, pilha, listaAdj);
            }
        }

        // Resetar visitados
        fill(visitado.begin(), visitado.end(), false);

        // Segunda DFS (grafo transposto)
        vector<vector<int>> componentesFortementeConexas;
        while (!pilha.empty()) {
            int v = pilha.top();
            pilha.pop();

            if (!visitado[v]) {
                vector<int> componente;
                DFSColetar(v, visitado, componente, listaAdjTransposta);
                componentesFortementeConexas.push_back(componente);
            }
        }

        return componentesFortementeConexas;
    }

    int obterNumVertices() const { return numVertices; }
};

Grafo lerFormatoSCC(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir arquivo: " << nomeArquivo << endl;
        exit(1);
    }

    string linha;
    int vertices, arestas;

    // Ler primeira linha: vértices arestas
    getline(arquivo, linha);
    stringstream ss(linha);
    ss >> vertices >> arestas;

    Grafo g(vertices);

    // Ler arestas
    int arestasLidas = 0;
    while (arestasLidas < arestas && getline(arquivo, linha)) {
        if (linha.empty()) continue;

        stringstream ss(linha);
        int u, v;

        if (ss >> u >> v) {
            g.adicionarAresta(u - 1, v - 1); // Converter 1-based para 0-based
            arestasLidas++;
        }
    }

    arquivo.close();
    return g;
}

void imprimirResultadosSCC(const vector<vector<int>>& componentesFortementeConexas, ostream& saida = cout) {
    // Converter componentes para formato de saída
    vector<vector<int>> componentesOrdenados;

    for (auto& componente : componentesFortementeConexas) {
        vector<int> componenteOrdenado = componente;
        sort(componenteOrdenado.begin(), componenteOrdenado.end());
        componentesOrdenados.push_back(componenteOrdenado);
    }

    // Ordenar componentes pelo menor vértice
    sort(componentesOrdenados.begin(), componentesOrdenados.end(),
         [](const vector<int>& a, const vector<int>& b) {
             return a[0] < b[0];
         });

    // Imprimir no formato esperado: uma linha por componente, vértices separados por espaço
    for (size_t i = 0; i < componentesOrdenados.size(); i++) {
        for (size_t j = 0; j < componentesOrdenados[i].size(); j++) {
            if (j > 0) saida << " ";
            saida << (componentesOrdenados[i][j] + 1); // Converter para 1-based
        }
        if (i < componentesOrdenados.size() - 1) {
            saida << endl; // Nova linha para cada componente
        }
    }
}

void mostrarAjuda(const string& nomePrograma) {
    cout << "Uso: " << nomePrograma << " -f <arquivo> [-o <arquivo_saida>]" << endl;
    cout << "Opções:" << endl;
    cout << "  -h              : Mostra este help" << endl;
    cout << "  -f <arquivo>    : Arquivo de entrada com o grafo" << endl;
    cout << "  -o <arquivo>    : Redireciona a saída para o arquivo" << endl;
}

int main(int argc, char* argv[]) {
    string nomeArquivo, arquivoSaida;

    // Processar argumentos
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-h") {
            mostrarAjuda(argv[0]);
            return 0;
        } else if (arg == "-f" && i + 1 < argc) {
            nomeArquivo = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            arquivoSaida = argv[++i];
        }
    }

    if (nomeArquivo.empty()) {
        cerr << "Erro: Arquivo de entrada não especificado." << endl;
        mostrarAjuda(argv[0]);
        return 1;
    }

    Grafo g = lerFormatoSCC(nomeArquivo);
    vector<vector<int>> componentesFortementeConexas = g.kosaraju();

    // Saída
    if (!arquivoSaida.empty()) {
        ofstream arquivoSaidaStream(arquivoSaida);
        if (arquivoSaidaStream.is_open()) {
            imprimirResultadosSCC(componentesFortementeConexas, arquivoSaidaStream);
            arquivoSaidaStream.close();
        } else {
            cerr << "Erro ao abrir arquivo de saída: " << arquivoSaida << endl;
            return 1;
        }
    } else {
        imprimirResultadosSCC(componentesFortementeConexas);
    }

    return 0;
}

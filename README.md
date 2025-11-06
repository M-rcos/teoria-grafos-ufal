Aluno: Marcos Vinícius da Silva

# Trabalho de Grafos - Implementações

Implementação dos quatro algoritmos clássicos de Grafos solicitados na disciplina de Teoria dos Grafos.

---

## algoritmos

Cada algoritmo está contido em sua própria pasta, com código-fonte e Makefile independente:

Trabalho_Grafos.zip
└── Trabalho_Grafos/
    ├── kosaraju/ (código + Makefile)
    ├── prim/ (código + Makefile)
    ├── kruskal/ (código + Makefile)
    ├── dijkstra/ (código + Makefile)

---

## Compilação

Cada pasta possui seu próprio **Makefile**, permitindo compilar individualmente:

bash
make -C prim
make -C kruskal
make -C dijkstra
make -C kosaraju


Use -h para ajuda de uso nos algoritmos.
(ex: ./kruskal.bin -h)

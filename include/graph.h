#ifndef GPU_MST_INCLUDE_GRAPH_H_
#define GPU_MST_INCLUDE_GRAPH_H_

#include <vector>
#include <stack>
#include <set>

struct Edge {
  int vertex1, vertex2, component1, component2, weight;
  bool included;
};

struct Graph {
  int V, E;
  std::vector<Edge> edges;
};

Graph* createBidirectionGraph(Graph* g);
std::vector<Edge> sequentialBoruvka(Graph *g);
int createComponents(std::vector<Edge> &tree, Edge *edges, int edgesNum, std::vector<Edge>& result);
std::set<int> dfs(std::vector<Edge>& edges, int start);
void findMinimumEdges(struct Edge *input, struct Edge *output, int global_componentNum, int edgesNum);

#endif //GPU_MST_INCLUDE_GRAPH_H_

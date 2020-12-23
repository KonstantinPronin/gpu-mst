#include <graph.h>

Graph *createBidirectionGraph(Graph *g) {
  auto *bg = new Graph;
  bg->V = g->V;
  bg->E = 2 * g->E;

  for (auto e : g->edges) {
    bg->edges.push_back({e.vertex1, e.vertex2, e.component1, e.component2, e.weight, false});
    bg->edges.push_back({e.vertex2, e.vertex1, e.component1, e.component2, e.weight, false});
  }

  return bg;
}

///Sequential boruvka
std::vector<Edge> sequentialBoruvka(Graph *g) {
  std::vector<Edge> result = std::vector<Edge>();
  int edgesNum = g->E;
  int componentsNum = g->V;

  Edge *edges = new Edge[edgesNum];
  for (int i = 0; i < edgesNum; ++i) {
    edges[i].vertex1 = g->edges[i].vertex1;
    edges[i].vertex2 = g->edges[i].vertex2;
    edges[i].component1 = g->edges[i].vertex1;
    edges[i].component2 = g->edges[i].vertex2;
    edges[i].weight = g->edges[i].weight;
  }

  while (result.size() < g->V - 1 && edgesNum > 0) {
    auto *minEdges = new Edge[componentsNum];

//    runKernel(clConfig, edges, minEdges, componentsNum, edgesNum);
    findMinimumEdges(edges, minEdges, componentsNum, edgesNum);

    for (int i = 0; i < componentsNum; ++i) {
      if (minEdges[i].included) {
        continue;
      }

      int dest = minEdges[i].component2;
      if (minEdges[dest].component2 == i) {
        minEdges[dest].included = true;
      }

      result.push_back(minEdges[i]);
    }

    std::vector<Edge> newEdges;
    componentsNum = createComponents(result, edges, edgesNum, newEdges);
    edgesNum = newEdges.size();

    delete[] edges;

    edges = new Edge[edgesNum];
    for (int i = 0; i < edgesNum; ++i) {
      edges[i] = newEdges[i];
    }

    delete[] minEdges;
  }

  delete[] edges;
  return result;
}

///Sequential search
void findMinimumEdges(struct Edge *input, struct Edge *output, int global_componentNum, int edgesNum) {
  for (int global_i = 0; global_i < global_componentNum; ++global_i) {
    int min = INT_MAX;
    int minIndex = -1;

    for (int j = 0; j < edgesNum; ++j) {
      if (input[j].component1 == global_i && input[j].weight < min) {
        min = input[j].weight;
        minIndex = j;
      }
    }

    if (minIndex != -1) {
      output[global_i].vertex1 = input[minIndex].vertex1;
      output[global_i].vertex2 = input[minIndex].vertex2;
      output[global_i].component1 = input[minIndex].component1;
      output[global_i].component2 = input[minIndex].component2;
      output[global_i].weight = input[minIndex].weight;
    }
  }
}

int createComponents(std::vector<Edge> &tree, Edge *edges, int edgesNum, std::vector<Edge>& result) {
  std::vector<std::set<int>> components = std::vector<std::set<int>>();

  //Поиск компонент
  for (auto &edge : tree) {
    edge.component1 = -1;
  }

  for (Edge edge : tree) {
    if (edge.component1 == -1) {
      components.push_back(dfs(tree, edge.vertex1));
    }
  }

  //Удаление уже добавленных ребер
  for (int i = 0; i < edgesNum; ++i) {
    bool exist = false;

    for (Edge existing : tree) {
      if ((edges[i].vertex1 == existing.vertex1 && edges[i].vertex2 == existing.vertex2)
          || (edges[i].vertex1 == existing.vertex2 && edges[i].vertex2 == existing.vertex1)) {
        exist = true;
        break;
      }
    }

    if (!exist) {
      //Присваивание номеров компонент
      for (size_t j = 0; j < components.size(); ++j) {
        if (components[j].find(edges[i].vertex1) != components[j].end()) {
          edges[i].component1 = j;
        }
        if (components[j].find(edges[i].vertex2) != components[j].end()) {
          edges[i].component2 = j;
        }
      }

      //Если ребра из разных компонент, то добавляем
      if (edges[i].component1 != edges[i].component2) {
        result.push_back(edges[i]);
      }
    }
  }

  return components.size();
}

std::set<int> dfs(std::vector<Edge> &edges, int start) {
  std::set<int> component;
  std::stack<int> s;

  component.insert(start);
  s.push(start);

  while (!s.empty()) {
    int v = s.top();
    s.pop();

    for (auto & edge : edges) {
      if (edge.vertex1 == v && component.find(edge.vertex2) == component.end()) {
        s.push(edge.vertex2);
        component.insert(edge.vertex2);

        edge.component1 = -2;
      } else if (edge.vertex2 == v && component.find(edge.vertex1) == component.end()) {
        s.push(edge.vertex1);
        component.insert(edge.vertex1);

        edge.component1 = -2;
      }
    }
  }

  return component;
}
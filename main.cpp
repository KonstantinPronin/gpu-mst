#include <iostream>
#include <parallel.h>

int main() {
  auto* clConfig = initOpenCl("./kernel.cl");

  Graph g{};
  std::cin >> g.V >> g.E;
  for (int i = 0; i < g.E; ++i) {
    Edge e{};
    std::cin >> e.vertex1 >> e.vertex2 >> e.weight;
    g.edges.push_back(e);
  }

  auto bg = createBidirectionGraph(&g);
  auto result = parallelBoruvka(bg, clConfig);

  std::cout << "RESULT:" << std::endl;
  for (auto e : result) {
    std::cout << e.vertex1 << ' ' << e.vertex2 << ' ' << e.weight << std::endl;
  }

  delete bg;
  return 0;
}
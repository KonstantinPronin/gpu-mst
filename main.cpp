#include <iostream>
#include <parallel.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "not enough arguments";
    return -1;
  }

  auto *clConfig = initOpenCl(argv[1]);
  cl_mem memObjects[3] = {0, 0, 0};

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

  Cleanup(clConfig->context, clConfig->queue, clConfig->program, clConfig->kernel, memObjects);
  delete clConfig;
  delete bg;

  return 0;
}
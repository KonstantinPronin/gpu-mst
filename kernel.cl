struct Edge {
  int vertex1, vertex2, component1, component2, weight;
  bool included;
};

__kernel void findMinimumEdges(__global struct Edge *input, __global struct Edge *output, __global int *edgesNum) {
  int min = INT_MAX;
  int minIndex = -1;
  int global_i = get_global_id(0);

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
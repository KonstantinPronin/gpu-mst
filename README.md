# gpu-mst

### Description
Parallel boruvka algorithm for finding the minimum spanning tree using OpenCl.

### How to build and run
```
$ git clone https://github.com/KonstantinPronin/gpu-mst.git
$ cd gpu-mst
$ mkdir ./build && cd ./build && cmake .. && make
$ ./gpu_mst ../kernel.cl
```

### Input parameters
Simple graph (not directed):
* V (vertex number)
* E (edges number)
* v1 v2 w (edge parameters for all E edges: source vertex, destination vertex, weight)

### Output
Edges that make up the minimum spanning tree

### Run example:
```
$ ./gpu_mst ../kernel.cl
7 11
0 1 7
0 3 4
1 2 11
1 3 9
1 4 10
2 4 5
3 4 15
3 5 6
4 5 12
4 6 8
5 6 13
RESULT:
0 1 7
0 3 4
3 5 6
1 4 10
2 4 5
4 6 8
```
Author: Byung Lee
Date: Fall 2013

This project is TSP solved by c++ and openMPI. The solution applies a parallel technique with message passing between nodes(different computers).

The machine requires openMPI installed.

To run:
(example)
mpic++ main4.cpp tsp.cpp -o filename
mpirun -np 4 filename







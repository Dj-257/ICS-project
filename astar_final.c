#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define INF INT_MAX

// Directions for moving up, down, left, and right
int dir[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

// Structure to represent maze data
typedef struct {
    int size;               // Size of the maze
    int **maze;             // Original maze
    int **astar;            // Solved maze using A*
    int pathlength_astar;   // Path length for A* solution
    double runtime_astar;   // Runtime for A* algorithm in microseconds
} MazeData;

// A structure to represent a node in the maze
typedef struct {
    int x, y;       // Coordinates
    int gCost;      // Cost from start to current node
    int hCost;      // Heuristic cost to the goal
    int fCost;      // Total cost (gCost + hCost)
    int parentX;    // Parent node coordinates (for backtracking)
    int parentY;
} Node;

// Function to calculate the heuristic (Manhattan distance)
int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Function to check if a cell is within maze bounds and is passable
int isValid_astar(int x, int y, MazeData *mazeData) {
    return (x >= 0 && x < mazeData->size && y >= 0 && y < mazeData->size &&
            (mazeData->maze[x][y] == 1 || mazeData->maze[x][y] == 4));
}

// Function to allocate a 2D array dynamically
int **allocateMaze(int size) {
    int **maze = (int **)malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        maze[i] = (int *)malloc(size * sizeof(int));
    }
    return maze;
}

// Function to deallocate a 2D array
void freeMaze(int **maze, int size) {
    for (int i = 0; i < size; i++) {
        free(maze[i]);
    }
    free(maze);
}

// Function to allocate a dynamic 2D array for nodes
Node **allocateNodes(int size) {
    Node **nodes = (Node **)malloc(size * sizeof(Node *));
    for (int i = 0; i < size; i++) {
        nodes[i] = (Node *)malloc(size * sizeof(Node));
    }
    return nodes;
}

// Function to deallocate the dynamic node array
void freeNodes(Node **nodes, int size) {
    for (int i = 0; i < size; i++) {
        free(nodes[i]);
    }
    free(nodes);
}

// Function to trace the path from the exit to the start
void tracePath(Node **nodes, MazeData *mazeData, int endX, int endY) {
    // Copy original maze into astar for storing the solution
    for (int i = 0; i < mazeData->size; i++) {
        for (int j = 0; j < mazeData->size; j++) {
            mazeData->astar[i][j] = mazeData->maze[i][j];
        }
    }

    int x = endX, y = endY;
    mazeData->pathlength_astar = nodes[endX][endY].gCost;

    while (nodes[x][y].parentX != -1 && nodes[x][y].parentY != -1) {
        mazeData->astar[x][y] = 2;
        int tempX = nodes[x][y].parentX;
        int tempY = nodes[x][y].parentY;
        x = tempX;
        y = tempY;
    }

    // Mark entry (3) and exit (4) points
    mazeData->astar[x][y] = 3;
    mazeData->astar[endX][endY] = 4;
}

// A* algorithm to solve the maze
void aStarMaze(MazeData *mazeData) {
    int startX = -1, startY = -1, endX = -1, endY = -1;

    // Locate entry (3) and exit (4) points
    for (int i = 0; i < mazeData->size; i++) {
        for (int j = 0; j < mazeData->size; j++) {
            if (mazeData->maze[i][j] == 3) {
                startX = i;
                startY = j;
            }
            if (mazeData->maze[i][j] == 4) {
                endX = i;
                endY = j;
            }
        }
    }

    // Allocate memory for nodes dynamically
    Node **nodes = allocateNodes(mazeData->size);

    // Dynamically allocate openList and closedList
   int **openList = allocateList(mazeData->size);
    int **closedList = allocateList(mazeData->size);

    // Initialize nodes
    for (int i = 0; i < mazeData->size; i++) {
        for (int j = 0; j < mazeData->size; j++) {
            nodes[i][j].x = i;
            nodes[i][j].y = j;
            nodes[i][j].gCost = INF;
            nodes[i][j].hCost = INF;
            nodes[i][j].fCost = INF;
            nodes[i][j].parentX = -1;
            nodes[i][j].parentY = -1;
        }
    }

    // Initialize start node
    nodes[startX][startY].gCost = 0;
    nodes[startX][startY].hCost = heuristic(startX, startY, endX, endY);
    nodes[startX][startY].fCost = nodes[startX][startY].hCost;

    openList[startX][startY] = 1;

    clock_t start, end;
    start = clock(); // Start measuring time

    while (1) {
        // Find the node with the lowest fCost in the open list
        int minFCost = INF;
        int currentX = -1, currentY = -1;

        for (int i = 0; i < mazeData->size; i++) {
            for (int j = 0; j < mazeData->size; j++) {
                if (openList[i][j] && nodes[i][j].fCost < minFCost) {
                    minFCost = nodes[i][j].fCost;
                    currentX = i;
                    currentY = j;
                }
            }
        }

        if (currentX == -1 || currentY == -1) {
            return; // Just exit if no path found
        }

        if (currentX == endX && currentY == endY) {
            tracePath(nodes, mazeData, endX, endY);
            end = clock(); // End measuring time

            // Calculate runtime in microseconds and store in mazeData
            mazeData->runtime_astar = ((double)(end - start)) / CLOCKS_PER_SEC * 1e6;
            break;
        }

        openList[currentX][currentY] = 0;
        closedList[currentX][currentY] = 1;

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int newX = currentX + dir[i][0];
            int newY = currentY + dir[i][1];

            if (isValid_astar(newX, newY, mazeData) && !closedList[newX][newY]) {
                int newGCost = nodes[currentX][currentY].gCost + 1;
                int newHCost = heuristic(newX, newY, endX, endY);
                int newFCost = newGCost + newHCost;

                if (!openList[newX][newY] || newGCost < nodes[newX][newY].gCost) {
                    nodes[newX][newY].gCost = newGCost;
                    nodes[newX][newY].hCost = newHCost;
                    nodes[newX][newY].fCost = newFCost;
                    nodes[newX][newY].parentX = currentX;
                    nodes[newX][newY].parentY = currentY;
                    openList[newX][newY] = 1;
                }
            }
        }
    }

    // Free dynamically allocated nodes array
    freeNodes(nodes, mazeData->size);
}

// Main function
int main() {
    MazeData mazeData;
    mazeData.maze = allocateMaze(mazeData.size);
    mazeData.astar = allocateMaze(mazeData.size);

    // Initialize maze with your desired values

    aStarMaze(&mazeData);

    // You can access the solution through mazeData.astar
    // Path length can be accessed with mazeData.pathlength_astar
    // Runtime can be accessed with mazeData.runtime_astar

    freeMaze(mazeData.maze, mazeData.size);
    freeMaze(mazeData.astar, mazeData.size);

    return 0;
}

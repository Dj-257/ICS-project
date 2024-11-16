//a*
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define N 30  // Size of the maze
#define INF INT_MAX

// Directions for moving up, down, left, and right
int dir[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

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
int isValid_astar(int x, int y, int maze[N][N]) {
    return (x >= 0 && x < N && y >= 0 && y < N && (maze[x][y] == 1 || maze[x][y] == 4));
}

// Function to trace the path from the exit to the start
void tracePath(Node nodes[N][N], int maze[N][N], int endX, int endY) {
    int x = endX, y = endY;

    while (nodes[x][y].parentX != -1 && nodes[x][y].parentY != -1) {
        maze[x][y] = 2;
        int tempX = nodes[x][y].parentX;
        int tempY = nodes[x][y].parentY;
        x = tempX;
        y = tempY;
    }

    // Mark entry (3) and exit (4) points
    maze[x][y] = 3;
    maze[endX][endY] = 4;
}

// A* algorithm to solve the maze
void aStarMaze(int maze[N][N]) {
    int startX = -1, startY = -1, endX = -1, endY = -1;

    // Locate entry (3) and exit (4) points
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (maze[i][j] == 3) {
                startX = i;
                startY = j;
            }
            if (maze[i][j] == 4) {
                endX = i;
                endY = j;
            }
        }
    }

    if (startX == -1 || startY == -1 || endX == -1 || endY == -1) {
        printf("Maze must contain both entry (3) and exit (4) points.\n");
        return;
    }

    Node nodes[N][N];
    int openList[N][N] = {0};
    int closedList[N][N] = {0};

    // Initialize nodes
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
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

    while (1) {
        // Find the node with the lowest fCost in the open list
        int minFCost = INF;
        int currentX = -1, currentY = -1;

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (openList[i][j] && nodes[i][j].fCost < minFCost) {
                    minFCost = nodes[i][j].fCost;
                    currentX = i;
                    currentY = j;
                }
            }
        }

        if (currentX == -1 || currentY == -1) {
            printf("No path found from entry to exit.\n");
            return;
        }

        if (currentX == endX && currentY == endY) {
            tracePath(nodes, maze, endX, endY);
            printf("Path found with length: %d\n", nodes[endX][endY].gCost);
            printf("Maze with solution path:\n");
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    printf("%d ", maze[i][j]);
                }
                printf("\n");
            }
            return;
        }

        openList[currentX][currentY] = 0;
        closedList[currentX][currentY] = 1;

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int newX = currentX + dir[i][0];
            int newY = currentY + dir[i][1];

            if (isValid_astar(newX, newY, maze) && !closedList[newX][newY]) {
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
}

// Main function
int main() {






int maze[N][N] = {
    {3, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0},
    {1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0},
    {1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0},
    {1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
    {1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
    {1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0},
    {1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0},
    {1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
    {1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0},
    {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
    {1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0},
    {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 4, 0}
};




    clock_t start, end;
    start = clock(); // Start measuring time

    aStarMaze(maze);

    end = clock(); // End measuring time

    // Calculate runtime in microseconds
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1e6;
    printf("Runtime: %.2f microseconds\n", time_taken);

    return 0;
}

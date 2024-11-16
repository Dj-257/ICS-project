//dijkstra
#include <stdio.h>
#include <limits.h>
#include <time.h>
#define N 30
#define INF INT_MAX 

// Directions for moving up, down, left, and right
int dir[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

// Function to check if a cell is within maze bounds and is passable
int isValid(int x, int y, int maze[N][N], int visited[N][N]) {
    return (x >= 0 && x < N && y >= 0 && y < N && (maze[x][y] == 1 || maze[x][y] == 4) && !visited[x][y]);
}

// Dijkstra's algorithm for shortest path in a maze
void dijkstraMaze(int maze[N][N]) {
    int dist[N][N];          // Distance matrix to store the shortest path
    int visited[N][N] = {0}; // Visited array to mark visited cells
    int pred[N][N][2];       // Predecessor array to store the previous cell

    // Initialize distances as infinite and predecessors as invalid (-1, -1)
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            dist[i][j] = INF;
            pred[i][j][0] = -1;
            pred[i][j][1] = -1;
        }

    // Find the entry (3) and exit (4) points
    int startX = -1, startY = -1, endX = -1, endY = -1;
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

    dist[startX][startY] = 0; // Starting point distance
    int x = startX, y = startY;

    while (1) {
        visited[x][y] = 1; // Mark the cell as visited

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int newX = x + dir[i][0];
            int newY = y + dir[i][1];

            if (isValid(newX, newY, maze, visited)) {
                int newDist = dist[x][y] + 1;
                if (newDist < dist[newX][newY]) {
                    dist[newX][newY] = newDist;
                    pred[newX][newY][0] = x; // Update predecessor
                    pred[newX][newY][1] = y;
                }
            }
        }

        // Find the unvisited cell with the smallest distance
        int minDist = INF;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (!visited[i][j] && dist[i][j] < minDist) {
                    minDist = dist[i][j];
                    x = i;
                    y = j;
                }
            }
        }

        // If we reach the end or there's no reachable unvisited cell, exit
        if (minDist == INF || (x == endX && y == endY)) break;
    }

    // Output result
    if (dist[endX][endY] == INF) {
        printf("No path found from entry to exit.\n");
    } else {
        printf("Shortest path from entry to exit has length: %d\n", dist[endX][endY]);

        // Trace the path from the end to the start
        int pathX = endX, pathY = endY;

        // Backtrack using the predecessors and mark the path in the maze
        while (pathX != -1 && pathY != -1) {
            if (maze[pathX][pathY] != 3 && maze[pathX][pathY] != 4) {
                maze[pathX][pathY] = 2; // Mark the solution path as 2
            }
            int tempX = pred[pathX][pathY][0];
            int tempY = pred[pathX][pathY][1];
            pathX = tempX;
            pathY = tempY;
        }

        // Print the maze with the solution path
        printf("Maze with solution path:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d ", maze[i][j]);
            }
            printf("\n");
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

    dijkstraMaze(maze);

    end = clock(); // End measuring time

    // Calculate runtime in microseconds
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1e6;
    printf("Runtime: %.2f microseconds\n", time_taken);

    return 0;
}

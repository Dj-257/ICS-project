#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define INF INT_MAX

// Directions for moving up, down, left, and right
int dir[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

// Define a structure to store maze data
typedef struct {
    int size;        // Size of the maze
    int **maze;      // The maze itself
    int **dijkstra;  // Solution of the maze using Dijkstra's algorithm
} MazeData;

// Function to allocate memory for a 2D array dynamically
int **allocate2DArray(int size) {
    int **array = (int **)malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        array[i] = (int *)malloc(size * sizeof(int));
    }
    return array;
}

// Function to free the memory of a dynamically allocated 2D array
void free2DArray(int **array, int size) {
    for (int i = 0; i < size; i++) {
        free(array[i]);
    }
    free(array);
}

// Function to check if a cell is within maze bounds and is passable
int isValid_dijkstra(int x, int y, MazeData *mazeData, int **visited) {
    return (x >= 0 && x < mazeData->size && y >= 0 && y < mazeData->size &&
            (mazeData->maze[x][y] == 1 || mazeData->maze[x][y] == 4) && !visited[x][y]);
}

// Dijkstra's algorithm for shortest path in a maze
void dijkstraMaze(MazeData *mazeData) {
    int N = mazeData->size;

    // Allocate memory for distance and visited arrays
    int **dist = allocate2DArray(N);
    int **visited = allocate2DArray(N);

    // Allocate memory for the predecessor array
    int ***pred = (int ***)malloc(N * sizeof(int **));
    for (int i = 0; i < N; i++) {
        pred[i] = allocate2DArray(N);
    }

    // Initialize distances as infinite, visited as false, and copy the maze to the solution
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            dist[i][j] = INF;
            visited[i][j] = 0;
            pred[i][j][0] = -1;
            pred[i][j][1] = -1;
            mazeData->dijkstra[i][j] = mazeData->maze[i][j];
        }
    }

    // Find the entry (3) and exit (4) points
    int startX = -1, startY = -1, endX = -1, endY = -1;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
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

    if (startX == -1 || startY == -1 || endX == -1 || endY == -1) {
        printf("Maze must contain both entry (3) and exit (4) points.\n");
        free2DArray(dist, N);
        free2DArray(visited, N);
        for (int i = 0; i < N; i++) {
            free2DArray(pred[i], N);
        }
        free(pred);
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

            if (isValid_dijkstra(newX, newY, mazeData, visited)) {
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

        // Backtrack using the predecessors and mark the path in the solution maze
        while (pathX != -1 && pathY != -1) {
            if (mazeData->maze[pathX][pathY] != 3 && mazeData->maze[pathX][pathY] != 4) {
                mazeData->dijkstra[pathX][pathY] = 2; // Mark the solution path as 2
            }
            int tempX = pred[pathX][pathY][0];
            int tempY = pred[pathX][pathY][1];
            pathX = tempX;
            pathY = tempY;
        }
    }

    // Free allocated memory
    free2DArray(dist, N);
    free2DArray(visited, N);
    for (int i = 0; i < N; i++) {
        free2DArray(pred[i], N);
    }
    free(pred);
}

// Main function
int main() {
    MazeData mazeData; // Declare a MazeData structure

    // Get the size of the maze
    printf("Enter the size of the maze (N): ");
    scanf("%d", &mazeData.size);

    // Allocate memory for the maze and the solution arrays
    mazeData.maze = allocate2DArray(mazeData.size);
    mazeData.dijkstra = allocate2DArray(mazeData.size);

    // Take the maze input from the user
    printf("Enter the maze (0 for walls, 1 for paths, 3 for entry, 4 for exit):\n");
    for (int i = 0; i < mazeData.size; i++) {
        for (int j = 0; j < mazeData.size; j++) {
            scanf("%d", &mazeData.maze[i][j]);
        }
    }

    clock_t start, end;
    start = clock(); // Start measuring time

    dijkstraMaze(&mazeData);

    end = clock(); // End measuring time

    // Calculate runtime in microseconds
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1e6;
    printf("Runtime: %.2f microseconds\n", time_taken);

    // Free the allocated memory
    free2DArray(mazeData.maze, mazeData.size);
    free2DArray(mazeData.dijkstra, mazeData.size);

    return 0;
}

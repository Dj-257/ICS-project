#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define INF INT_MAX

// Directions for moving up, down, left, and right
int dir[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

// Typedef structure to store maze data and solution results
typedef struct {
    int size;           // Size of the maze
    int **maze;         // Pointer to the maze array
    int **dijkstra;     // Solved maze with the solution path
    int pathlength_dijkstra; // Length of the solution path
    double runtime_dijkstra; // Runtime of Dijkstra's algorithm
} MazeData;

// Function to dynamically allocate a 2D array
int **allocate2DArray(int size) {
    int **array = (int **)malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        array[i] = (int *)malloc(size * sizeof(int));
    }
    return array;
}

// Function to deallocate a 2D array
void free2DArray(int **array, int size) {
    for (int i = 0; i < size; i++) {
        free(array[i]);
    }
    free(array);
}

// Function to check if a cell is valid for Dijkstra's algorithm
int isValid_dijkstra(int x, int y, int **maze, int **visited, int size) {
    return (x >= 0 && x < size && y >= 0 && y < size && 
           (maze[x][y] == 1 || maze[x][y] == 4) && !visited[x][y]);
}

// Function to solve the maze using Dijkstra's algorithm
void dijkstraMaze(MazeData *mazeData) {
    int **maze = mazeData->maze;
    int size = mazeData->size;

    int **dist = allocate2DArray(size);
    int **visited = allocate2DArray(size);
    int pred[size][size][2]; // Predecessor array

    // Initialize distances and visited array
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            dist[i][j] = INF;
            visited[i][j] = 0;
            pred[i][j][0] = -1;
            pred[i][j][1] = -1;
        }
    }

    // Locate start (3) and end (4) points
    int startX = -1, startY = -1, endX = -1, endY = -1;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
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

    dist[startX][startY] = 0;
    int x = startX, y = startY;

    clock_t start, end;
    start = clock(); // Start time measurement

    while (1) {
        visited[x][y] = 1;

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int newX = x + dir[i][0];
            int newY = y + dir[i][1];

            if (isValid_dijkstra(newX, newY, maze, visited, size)) {
                int newDist = dist[x][y] + 1;
                if (newDist < dist[newX][newY]) {
                    dist[newX][newY] = newDist;
                    pred[newX][newY][0] = x;
                    pred[newX][newY][1] = y;
                }
            }
        }

        // Find the unvisited cell with the smallest distance
        int minDist = INF;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (!visited[i][j] && dist[i][j] < minDist) {
                    minDist = dist[i][j];
                    x = i;
                    y = j;
                }
            }
        }

        if (minDist == INF || (x == endX && y == endY)) break;
    }

    end = clock(); // End time measurement

    mazeData->runtime_dijkstra = ((double)(end - start)) / CLOCKS_PER_SEC * 1e6;

    if (dist[endX][endY] == INF) {
        mazeData->pathlength_dijkstra = -1; // No solution
    } else {
        mazeData->pathlength_dijkstra = dist[endX][endY];

        // Trace back the path
        int pathX = endX, pathY = endY;
        mazeData->dijkstra = allocate2DArray(size);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                mazeData->dijkstra[i][j] = maze[i][j];
            }
        }

        while (pathX != -1 && pathY != -1) {
            if (maze[pathX][pathY] != 3 && maze[pathX][pathY] != 4) {
                mazeData->dijkstra[pathX][pathY] = 2; // Mark solution path
            }
            int tempX = pred[pathX][pathY][0];
            int tempY = pred[pathX][pathY][1];
            pathX = tempX;
            pathY = tempY;
        }
    }

    free2DArray(dist, size);
    free2DArray(visited, size);
}

// Main function
int main() {
    MazeData mazeData;

    // Allocate memory for the maze
    mazeData.maze = allocate2DArray(mazeData.size);

    // Solve the maze using Dijkstra's algorithm
    dijkstraMaze(&mazeData);

    // Free the maze arrays
    free2DArray(mazeData.maze, mazeData.size);
    free2DArray(mazeData.dijkstra, mazeData.size);
    
    return 0;
}

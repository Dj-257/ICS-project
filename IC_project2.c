#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>  // Measure runtime

#define N 5  // Maze size

// Directions to move up, down, left, right
int row[] = { -1, 1, 0, 0 };
int col[] = { 0, 0, -1, 1 };

// Maze: 3 = start, 1 = path, 0 = wall, 4 = goal
int maze[N][N] = {
    {3, 1, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 1, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 1, 4}
};

// Cell structure for position and heuristic
typedef struct {
    int x, y;
    int heuristic;
} Cell;

// Manhattan distance heuristic
int calculateHeuristic(int x, int y, int goalX, int goalY) {
    return abs(x - goalX) + abs(y - goalY);
}

// Check if cell valid: inside maze, is path, not visited
bool isValid(int x, int y, bool visited[N][N]) {
    return (x >= 0 && x < N && y >= 0 && y < N && (maze[x][y] == 1 || maze[x][y] == 4) && !visited[x][y]);
}

// Greedy Best-First Search
bool greedyBFS(int startX, int startY, int goalX, int goalY) {
    bool visited[N][N] = {false};  // Track visited cells
    Cell current = {startX, startY, calculateHeuristic(startX, startY, goalX, goalY)};
    int pathLength = 0;  // Track path length

    visited[startX][startY] = true;  // Mark start visited

    // Loop till goal found
    while (!(current.x == goalX && current.y == goalY)) {
        int minHeuristic = 10000000;
        Cell nextCell = current;

        // Check all 4 directions
        for (int i = 0; i < 4; i++) {
            int newX = current.x + row[i];
            int newY = current.y + col[i];

            // If valid cell
            if (isValid(newX, newY, visited)) {
                int heuristic = calculateHeuristic(newX, newY, goalX, goalY);
                if (heuristic < minHeuristic) {
                    minHeuristic = heuristic;
                    nextCell = (Cell){newX, newY, heuristic};
                }
            }
        }

        // No valid cell found, no path
        if (nextCell.x == current.x && nextCell.y == current.y) {
            printf("No path found.\n");
            return false;
        }

        // Move to best cell
        current = nextCell;
        visited[current.x][current.y] = true;
        maze[current.x][current.y] = 2;  // Mark as part of path
        pathLength++;

        // Print move
        printf("Move to: (%d, %d)\n", current.x, current.y);
    }

    printf("Path found!\n");
    printf("Path length: %d\n", pathLength);
    return true;
}

int main() {
    int startX = 0, startY = 0;
    int goalX = N - 1, goalY = N - 1;

    clock_t startTime = clock();  // Start time

    if (!greedyBFS(startX, startY, goalX, goalY)) {
        printf("The maze cannot be solved.\n");
    }

    clock_t endTime = clock();  // End time

    double runtime = (double)(endTime - startTime) / CLOCKS_PER_SEC * 1000000;
    printf("Runtime: %.2f microseconds\n", runtime);
maze[N-1][N-1]=4;
    // Print final maze
    printf("Final maze:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", maze[i][j]);
        }
        printf("\n");
    }

    return 0;
}
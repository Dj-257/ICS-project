#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#define MAX_EXTRA_PATHS 15 
#define INF INT_MAX

int dir[4][2] = {
    {-1, 0}, // Up
    {1, 0},  // Down
    {0, -1}, // Left
    {0, 1}   // Right
};

typedef struct {
    int size;
    int **maze;
} MazeData;

typedef struct {
    int x, y;       // Coordinates
    int gCost;      // Cost from start to current node
    int hCost;      // Heuristic cost to the goal
    int fCost;      // Total cost (gCost + hCost)
    int parentX;    // Parent node coordinates (for backtracking)
    int parentY;
} Node;

int is_valid(int x, int y, int size, int **maze) {
    return (x >= 0 && x < size && y >= 0 && y < size && maze[x][y] == 0);
}

int isValid_dijkstra(int x, int y, int **maze, int **visited, int N) {
    return (x >= 0 && x < N && y >= 0 && y < N && (maze[x][y] == 1 || maze[x][y] == 4) && !visited[x][y]);
}

int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

int isValid(int x, int y, int **maze) {  // Use a large enough size for maze
    return (x >= 0 && x < 10000 && y >= 0 && y < 10000 && (maze[x][y] == 1 || maze[x][y] == 4));
}

void tracePath(Node **nodes, int **maze, int endX, int endY, int N) {
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

int **maze_astar;
void aStarMaze(int **maze, int N) {
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

    Node **nodes;  // Use a large enough size for nodes
    int openList[1000][1000] = {0};
    int closedList[1000][1000] = {0};

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
            tracePath(nodes, maze, endX, endY, N);
           // printf("Path found with length: %d\n", nodes[endX][endY].gCost);
            //printf("Maze with solution path:\n");
            for (int i = 0; i < N; i++) 
            {
                for (int j = 0; j < N; j++) {
                   *(*(maze_astar+i)+j)= maze[i][j];
                }
        }
            return;
        }

        openList[currentX][currentY] = 0;
        closedList[currentX][currentY] = 1;

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int newX = currentX + dir[i][0];
            int newY = currentY + dir[i][1];

            if (isValid(newX, newY, maze)) {
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

int **maze_dijkstra;


void dijkstraMaze(int **maze, int N) {
    int dist[1000][1000];          // Distance matrix to store the shortest path
    int **visited = {0}; // Visited array to mark visited cells
    int pred[1000][1000][2];       // Predecessor array to store the previous cell

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

            if (isValid_dijkstra(newX, newY, maze, visited, N)) {
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
        
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                *(*(maze_dijkstra+i)+j)= maze[i][j];
            }
        }
    }
}

void generate_maze(int x, int y, int size, int **maze) {
    maze[x][y] = 1;  // Mark the current cell as a path (1)

    for (int i = 0; i < 4; i++) {
        int j = rand() % 4;
        int temp[2] = {dir[i][0], dir[i][1]};
        dir[i][0] = dir[j][0];
        dir[i][1] = dir[j][1];
        dir[j][0] = temp[0];
        dir[j][1] = temp[1];
    }

    
    for (int i = 0; i < 4; i++) {
        int nx = x + dir[i][0] * 2;  // Move 2 steps in one direction
        int ny = y + dir[i][1] * 2;

        if (is_valid(nx, ny, size, maze)) {
            // If the next cell is valid, mark the wall between current and next cell as path
            maze[x + dir[i][0]][y + dir[i][1]] = 1;  // Break the wall
            generate_maze(nx, ny, size, maze);  // Recursively visit the next cell
        }
    }
}

// Function to introduce extra paths in the maze, creating multiple solutions
void add_extra_paths(int size, int **maze) {
    int extra_paths_added = 0;

    for (int i = 0; i < size && extra_paths_added < MAX_EXTRA_PATHS; i++) {
        for (int j = 0; j < size && extra_paths_added < MAX_EXTRA_PATHS; j++) {
            if (maze[i][j] == 1) {
                
                if (rand() % 5 == 0) {  
                    int nx = i + (rand() % 3 - 1); // Randomly step -1, 0, or +1 in x
                    int ny = j + (rand() % 3 - 1); // Randomly step -1, 0, or +1 in y

                    // Ensure new path is within bounds and not yet a path
                    if (nx >= 0 && nx < size && ny >= 0 && ny < size && maze[nx][ny] == 0) {
                        maze[nx][ny] = 1;  // Create a new path
                        extra_paths_added++; // Increment the path count
                    }
                }
            }
        }
    }
}

// GTK drawing function to render the maze
static void draw_maze(GtkDrawingArea *area, cairo_t *cr, gpointer user_data) {
    MazeData *data = (MazeData *)user_data;
    int size = data->size;
    int **maze = data->maze;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (maze[i][j] == 0) {
                // Draw wall (black color)
                cairo_set_source_rgb(cr, 0, 0, 0);
            } else if (i == 0 && j == 0) {
                cairo_set_source_rgb(cr, 0, 1, 0);  // Start (green)
            } else if (i == size - 2 && j == size - 2) {
                cairo_set_source_rgb(cr, 1, 0, 0);  // End (red)
            } else {
                // Draw path (white color)
                cairo_set_source_rgb(cr, 1, 1, 1);
            }
            cairo_rectangle(cr, j * 20, i * 20, 20, 20);
            cairo_fill(cr);
        }
    }
}

static void draw_maze_dijkstra(GtkDrawingArea *area, cairo_t *cr, gpointer user_data) {
    MazeData *data = (MazeData *)user_data;
    int size = data->size;
    int **maze = data->maze;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (*(*(maze_dijkstra)+i)+j==0) {
                // Draw wall (black color)
                cairo_set_source_rgb(cr, 0, 0, 0);
            } else if (*(*(maze_dijkstra)+i)+j==1) {
                cairo_set_source_rgb(cr, 0, 1, 0);  // Start (green)
            } else if (*(*(maze_dijkstra)+i)+j==4) {
                cairo_set_source_rgb(cr, 1, 0, 0);  // End (red)
            } else if(*(*(maze_dijkstra)+i)+j==2){
                cairo_set_source_rgb(cr, 0, 1, 0);                
            } else if(*(*(maze_dijkstra)+i)+j==3){
                cairo_set_source_rgb(cr, 0, 1, 0);
            }else{
                cairo_set_source_rgb(cr, 1, 1, 1);
            }
            cairo_rectangle(cr, j * 20, i * 20, 20, 20);
            cairo_fill(cr);
        }
    }
}

static void draw_maze_astar(GtkDrawingArea *area, cairo_t *cr, gpointer user_data) {
    MazeData *data = (MazeData *)user_data;
    int size = data->size;
    int **maze = data->maze;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (*(*(maze_astar)+i)+j==0) {
                // Draw wall (black color)
                cairo_set_source_rgb(cr, 0, 0, 0);
            } else if (*(*(maze_astar)+i)+j==1) {
                cairo_set_source_rgb(cr, 0, 1, 0);  // Start (green)
            } else if (*(*(maze_astar)+i)+j==4) {
                cairo_set_source_rgb(cr, 1, 0, 0);  // End (red)
            } else if(*(*(maze_astar)+i)+j==2){
                cairo_set_source_rgb(cr, 0, 1, 0);                
            } else if(*(*(maze_astar)+i)+j==3){
                cairo_set_source_rgb(cr, 0, 1, 0);
            }else{
                cairo_set_source_rgb(cr, 1, 1, 1);
            }
            cairo_rectangle(cr, j * 20, i * 20, 20, 20);
            cairo_fill(cr);
        }
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); // Seed the random number generator

    int size;
    printf("Enter the size of the maze: ");
    scanf("%d", &size);
int maze_dijkstra[size][size];
int maze_astar[size][size];
    // Dynamically allocate memory for the maze
    int **maze = malloc(sizeof(int *) * size);
    for (int i = 0; i < size; i++) {
        maze[i] = malloc(sizeof(int) * size);
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            maze[i][j] = 0;
        }
    }

    // Generate the maze starting from (0, 0)
    generate_maze(0, 0, size, maze);
    maze[0][0] = 3;  // Start point
    maze[size - 2][size - 2] = 4;  // End point

    // Add extra paths to create multiple solutions (limit the number of extra paths)
    add_extra_paths(size, maze);

    // Create a structure to pass both the size and maze data to GTK
    MazeData data = {size, maze};

    // Initialize GTK
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Random Maze");
    gtk_window_set_default_size(GTK_WINDOW(window), size * 20, size * 20);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *drawing_area1 = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area1, size * 20, size * 20);

    // Pass the maze structure to the drawing function
    g_signal_connect(drawing_area1, "draw", G_CALLBACK(draw_maze), &data);

    gtk_container_add(GTK_CONTAINER(window), drawing_area1);
    gtk_widget_show_all(window);
    gtk_main();

    /*for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d ", maze[i][j]);
        }
        printf("\n");
    }*/
   //--------------------------------------------------------------------
   /*clock_t start, end;
    start = clock(); // Start measuring time
    */

    aStarMaze(maze, size);

    //end = clock(); // End measuring time

    gtk_init(&argc, &argv);

    GtkWidget *window2 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window2), "Solved Maze");
    gtk_window_set_default_size(GTK_WINDOW(window2), size * 20, size * 20);
    g_signal_connect(window2, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *drawing_area2 = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area2, size * 20, size * 20);

    // Pass the maze structure to the drawing function
    g_signal_connect(drawing_area2, "draw", G_CALLBACK(draw_maze_dijkstra), &data);

    gtk_container_add(GTK_CONTAINER(window2), drawing_area2);
    gtk_widget_show_all(window2);
    gtk_main();

    // Calculate runtime in microseconds
    //double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1e6;
    //printf("Runtime: %.2f microseconds\n", time_taken);
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    /*clock_t start, end;
    start = clock(); // Start measuring time*/
    dijkstraMaze(maze, size);

    //end = clock(); // End measuring time

     gtk_init(&argc, &argv);

    GtkWidget *window3 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window3), "Solved Maze");
    gtk_window_set_default_size(GTK_WINDOW(window3), size * 20, size * 20);
    g_signal_connect(window3, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *drawing_area3 = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area3, size * 20, size * 20);

    // Pass the maze structure to the drawing function
    g_signal_connect(drawing_area3, "draw", G_CALLBACK(draw_maze_astar), &data);

    gtk_container_add(GTK_CONTAINER(window), drawing_area3);
    gtk_widget_show_all(window3);
    gtk_main();

    // Calculate runtime in microseconds
   // double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1e6;
    //printf("Runtime: %.2f microseconds\n", time_taken);
    //---------------------------------------------------------------------
    // Free the allocated memory for the maze
    /*for (int i = 0; i < size; i++) {
        free(maze[i]);
    }
    free(maze);*/

    return 0;
}

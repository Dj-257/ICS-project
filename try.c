#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#define MAX_EXTRA_PATHS 15 

int directions[4][2] = {
    {-1, 0}, 
    {1, 0},  
    {0, -1}, 
    {0, 1}   
};

typedef struct {
    int size;
    int **maze;
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

// A* algorithm to solve the maze
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

    // Dynamically allocate memory for nodes
    Node **nodes = (Node **)malloc(N * sizeof(Node *));
    int **openList = (int **)malloc(N * sizeof(int *));
    int **closedList = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        nodes[i] = (Node *)malloc(N * sizeof(Node));
        openList[i] = (int *)calloc(N, sizeof(int));
        closedList[i] = (int *)calloc(N, sizeof(int));
    }

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
            break;
        }

        if (currentX == endX && currentY == endY) {
            tracePath(nodes, maze, endX, endY, N);
            printf("Path found with length: %d\n", nodes[endX][endY].gCost);
            printf("Maze with solution path:\n");
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    printf("%d ", maze[i][j]);
                }
                printf("\n");
            }
            break;
        }

        openList[currentX][currentY] = 0;
        closedList[currentX][currentY] = 1;

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int newX = currentX + directions[i][0];
            int newY = currentY + directions[i][1];

            if (isValid(newX, newY, maze, N)) {
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

    // Free allocated memory
    for (int i = 0; i < N; i++) {
        free(nodes[i]);
        free(openList[i]);
        free(closedList[i]);
    }
    free(nodes);
    free(openList);
    free(closedList);
}

int is_valid(int x, int y, int size, int **maze) {
    return (x >= 0 && x < size && y >= 0 && y < size && maze[x][y] == 0);
}

void generate_maze(int x, int y, int size, int **maze) {
    maze[x][y] = 1;  

    for (int i = 0; i < 4; i++) {
        int j = rand() % 4;
        int temp[2] = {directions[i][0], directions[i][1]};
        directions[i][0] = directions[j][0];
        directions[i][1] = directions[j][1];
        directions[j][0] = temp[0];
        directions[j][1] = temp[1];
    }

    
    for (int i = 0; i < 4; i++) {
        int nx = x + directions[i][0] * 2;  
        int ny = y + directions[i][1] * 2;

        if (is_valid(nx, ny, size, maze)) {
            
            maze[x + directions[i][0]][y + directions[i][1]] = 1;  
            generate_maze(nx, ny, size, maze);  
        }
    }
}

void add_extra_paths(int size, int **maze) {
    int extra_paths_added = 0;

    for (int i = 0; i < size && extra_paths_added < MAX_EXTRA_PATHS; i++) {
        for (int j = 0; j < size && extra_paths_added < MAX_EXTRA_PATHS; j++) {
            if (maze[i][j] == 1) {
                
                if (rand() % 5 == 0) {  
                    int nx = i + (rand() % 3 - 1); 
                    int ny = j + (rand() % 3 - 1); 

                    
                    if (nx >= 0 && nx < size && ny >= 0 && ny < size && maze[nx][ny] == 0) {
                        maze[nx][ny] = 1; 
                        extra_paths_added++; 
                    }
                }
            }
        }
    }
}

static void draw_maze(GtkDrawingArea *area, cairo_t *cr, gpointer user_data) {
    MazeData *data = (MazeData *)user_data;
    int size = data->size;
    int **maze = data->maze;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (maze[i][j] == 0) {
                
                cairo_set_source_rgb(cr, 0, 0, 0);
            } else if (i == 0 && j == 0) {
                cairo_set_source_rgb(cr, 0, 1, 0);  
            } else if (maze[i][j]==4) {
                cairo_set_source_rgb(cr, 1, 0, 0);
            } else if ((i == size-2 && j == size-1)||(i == size-1 && j == size-2)){
                cairo_set_source_rgb(cr, 1, 1, 1); 
            } else {
                cairo_set_source_rgb(cr, 1, 1, 1);
            }
            cairo_rectangle(cr, j * 20, i * 20, 20, 20);
            cairo_fill(cr);
        }
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); // Seed the random number generator

    int SIZE = 30;  

    int **maze = malloc(sizeof(int *) * SIZE);
    for (int i = 0; i < SIZE; i++) {
        maze[i] = malloc(sizeof(int) * SIZE);
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            maze[i][j] = 0;
        }
    }

    generate_maze(0, 0, SIZE, maze);
    maze[0][0] = 3;  // Start point
    maze[SIZE-1][SIZE-2]=1;
    maze[SIZE-2][SIZE-1]=1;
    if(SIZE%2==0)
    {
        maze[SIZE - 2][SIZE - 2] = 4;  // End point
    }
    else if(SIZE%2!=0)
    {
        maze[SIZE - 1][SIZE - 1] = 4;
    } 

    add_extra_paths(SIZE, maze);

    MazeData data = {SIZE, maze};

    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Random Maze");
    gtk_window_set_default_size(GTK_WINDOW(window), SIZE * 20, SIZE * 20);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, SIZE * 20, SIZE * 20);

    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_maze), &data);

    gtk_container_add(GTK_CONTAINER(window), drawing_area);
    gtk_widget_show_all(window);
    gtk_main();

    for (int i = 0; i < SIZE; i++) {
        free(maze[i]);
    }
    free(maze);

    return 0;
}

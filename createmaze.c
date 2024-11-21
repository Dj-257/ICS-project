//Latest mazegen program

#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#define MAX_EXTRA_PATHS 0 

int directions[4][2] = {
    {-1, 0}, // Up
    {1, 0},  // Down
    {0, -1}, // Left
    {0, 1}   // Right
};

typedef struct {
    int size;
    int **maze;
} MazeData;

// Disjoint-set data structure to manage maze cell connections
typedef struct {
    int *parent;
    int *rank;
    int size;
} DisjointSet;

DisjointSet* create_set(int size) {
    DisjointSet *ds = malloc(sizeof(DisjointSet));
    ds->parent = malloc(size * sizeof(int));
    ds->rank = malloc(size * sizeof(int));
    ds->size = size;

    for (int i = 0; i < size; i++) {
        ds->parent[i] = i;
        ds->rank[i] = 0;
    }

    return ds;
}

int find(DisjointSet *ds, int x) {
    if (ds->parent[x] != x)
        ds->parent[x] = find(ds, ds->parent[x]);
    return ds->parent[x];
}

void union_sets(DisjointSet *ds, int x, int y) {
    int rootX = find(ds, x);
    int rootY = find(ds, y);

    if (rootX != rootY) {
        if (ds->rank[rootX] > ds->rank[rootY])
            ds->parent[rootY] = rootX;
        else if (ds->rank[rootX] < ds->rank[rootY])
            ds->parent[rootX] = rootY;
        else {
            ds->parent[rootY] = rootX;
            ds->rank[rootX]++;
        }
    }
}

int is_valid(int x, int y, int size, int **maze) {
    return (x >= 0 && x < size && y >= 0 && y < size && maze[x][y] == 0);
}

void generate_maze(int x, int y, int size, int **maze, DisjointSet *ds) {
    maze[x][y] = 1;  // Mark the current cell as a path (1)

    // Randomly shuffle directions for path generation
    for (int i = 0; i < 4; i++) {
        int j = rand() % 4;
        int temp[2] = {directions[i][0], directions[i][1]};
        directions[i][0] = directions[j][0];
        directions[i][1] = directions[j][1];
        directions[j][0] = temp[0];
        directions[j][1] = temp[1];
    }

    for (int i = 0; i < 4; i++) {
        int nx = x + directions[i][0] * 2;  // Move 2 steps in one direction
        int ny = y + directions[i][1] * 2;

        if (is_valid(nx, ny, size, maze)) {
            // Check if the current direction creates a cycle
            int current_cell = x * size + y;
            int next_cell = nx * size + ny;
            if (find(ds, current_cell) != find(ds, next_cell)) {
                // If not in the same component, mark the wall as a path
                maze[x + directions[i][0]][y + directions[i][1]] = 1;  // Break the wall
                union_sets(ds, current_cell, next_cell);  // Union the two components
                generate_maze(nx, ny, size, maze, ds);  // Recursively visit the next cell
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
            } else if (maze[i][j]==4) {
                cairo_set_source_rgb(cr, 1, 0, 0);  // End (red)
            } else if ((i == size-2 && j == size-3)||(i == size-3 && j == size-2)){
                cairo_set_source_rgb(cr, 1, 1, 1);
            } else {
                // Draw path (white color)
                cairo_set_source_rgb(cr, 1, 1, 1);
            }
            cairo_rectangle(cr, j * 20, i * 20, 20, 20);
            cairo_fill(cr);
        }
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); // Seed the random number generator

    int SIZE = 35;  

    // Dynamically allocate memory for the maze
    int **maze = malloc(sizeof(int *) * SIZE);
    for (int i = 0; i < SIZE; i++) {
        maze[i] = malloc(sizeof(int) * SIZE);
    }

    // Initialize all cells as walls (0)
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            maze[i][j] = 0;
        }
    }

    // Create a disjoint set to avoid cycles
    DisjointSet *ds = create_set(SIZE * SIZE);

    // Generate the maze starting from (0, 0)
    generate_maze(0, 0, SIZE, maze, ds);

    // Ensure there's at least one path from (0, 0) to (SIZE - 1, SIZE - 1)
    maze[0][0] = 3;  // Start point
    maze[SIZE-2][SIZE-3]=1;
    maze[SIZE-3][SIZE-2]=1;

    if(SIZE%2==0)
    {
        maze[SIZE - 2][SIZE - 2] = 4;  // End point
    }
    else if(SIZE%2!=0)
    {
        maze[SIZE - 1][SIZE - 1] = 4;
    }

    // Create a structure to pass both the size and maze data to GTK
    MazeData data = {SIZE, maze};

    // Initialize GTK
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Random Maze");
    gtk_window_set_default_size(GTK_WINDOW(window), SIZE * 20, SIZE * 20);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, SIZE * 20, SIZE * 20);

    // Pass the maze structure to the drawing function
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_maze), &data);

    gtk_container_add(GTK_CONTAINER(window), drawing_area);
    gtk_widget_show_all(window);
    gtk_main();

    // Free allocated memory
    for (int i = 0; i < SIZE; i++) {
        free(maze[i]);
    }
    free(maze);

    free(ds->parent);
    free(ds->rank);
    free(ds);

    return 0;
}

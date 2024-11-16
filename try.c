#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 30
#define MAX_EXTRA_PATHS 15  // Limit on how many extra paths to add

int maze[SIZE][SIZE];  

int directions[4][2] = {
    {-1, 0}, // Up
    {1, 0},  // Down
    {0, -1}, // Left
    {0, 1}   // Right
};

// Function to check if a cell is within the maze bounds and if it is not already a path (0)
int is_valid(int x, int y) {
    return (x >= 0 && x < SIZE && y >= 0 && y < SIZE && maze[x][y] == 0);
}

// Recursive function to generate the maze using DFS
void generate_maze(int x, int y) {
    maze[x][y] = 1;  // Mark the current cell as a path (1)

    // Shuffle directions to ensure random movement
    for (int i = 0; i < 4; i++) {
        int j = rand() % 4;
        int temp[2] = {directions[i][0], directions[i][1]};
        directions[i][0] = directions[j][0];
        directions[i][1] = directions[j][1];
        directions[j][0] = temp[0];
        directions[j][1] = temp[1];
    }

    // Visit each direction (up, down, left, right)
    for (int i = 0; i < 4; i++) {
        int nx = x + directions[i][0] * 2;  // Move 2 steps in one direction
        int ny = y + directions[i][1] * 2;

        if (is_valid(nx, ny)) {
            // If the next cell is valid, mark the wall between current and next cell as path
            maze[x + directions[i][0]][y + directions[i][1]] = 1;  // Break the wall
            generate_maze(nx, ny);  // Recursively visit the next cell
        }
    }
}

// Function to introduce extra paths in the maze, creating multiple solutions
void add_extra_paths() {
    int extra_paths_added = 0;
    
    // Try to add extra paths up to a limit (MAX_EXTRA_PATHS)
    for (int i = 0; i < SIZE && extra_paths_added < MAX_EXTRA_PATHS; i++) {
        for (int j = 0; j < SIZE && extra_paths_added < MAX_EXTRA_PATHS; j++) {
            if (maze[i][j] == 1) {
                // Randomly decide whether to add a new path with a small probability (e.g., 1 in 5 chance)
                if (rand() % 5 == 0) {  // 20% chance to add a path
                    int nx = i + (rand() % 3 - 1); // Randomly step -1, 0, or +1 in x
                    int ny = j + (rand() % 3 - 1); // Randomly step -1, 0, or +1 in y

                    // Ensure new path is within bounds and not yet a path
                    if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && maze[nx][ny] == 0) {
                        maze[nx][ny] = 1;  // Create a new path
                        extra_paths_added++; // Increment the path count
                    }
                }
            }
        }
    }
}

static void draw_maze(GtkDrawingArea *area, cairo_t *cr, gpointer user_data) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (maze[i][j] == 0) {
                // Draw wall (black color)
                cairo_set_source_rgb(cr, 0, 0, 0);
            } else if (i == 0 && j == 0) {
                cairo_set_source_rgb(cr, 0, 1, 0);  // Start (green)
            } else if (i == SIZE - 2 && j == SIZE - 2) {
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

int main(int argc, char *argv[]) {
    srand(time(NULL)); // Seed the random number generator

    // Initialize the maze with walls (0s)
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            maze[i][j] = 0;  // Start with 0s (path) to mark walls during generation
        }
    }

    // Generate the maze starting from (0, 0)
    generate_maze(0, 0);
    maze[0][0] = 3;  // Start point
    maze[SIZE - 2][SIZE - 2] = 4;  // End point

    // Add extra paths to create multiple solutions (limit the number of extra paths)
    add_extra_paths();

    // Initialize GTK
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Random Maze");
    gtk_window_set_default_size(GTK_WINDOW(window), SIZE * 20, SIZE * 20);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, SIZE * 20, SIZE * 20);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_maze), NULL);
    gtk_container_add(GTK_CONTAINER(window), drawing_area);

    gtk_widget_show_all(window);
    gtk_main();

    // Print the maze for debugging
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%d ", maze[i][j]);
        }
        printf("\n");
    }

    return 0;
}
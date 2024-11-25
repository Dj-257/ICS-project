#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#define MAX_EXTRA_PATHS 0 

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

int is_valid(int x, int y, int size, int **maze) {
    return (x >= 0 && x < size && y >= 0 && y < size && maze[x][y] == 0);
}

void generate_maze(int x, int y, int size, int **maze) {
    maze[x][y] = 1;  // Mark the current cell as a path (1)

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
            // If the next cell is valid, mark the wall between current and next cell as path
            maze[x + directions[i][0]][y + directions[i][1]] = 1;  // Break the wall
            generate_maze(nx, ny, size, maze);  // Recursively visit the next cell
        }
    }
}

// Function to perform a DFS to check if there's a path from (x, y) to the end point
int is_reachable(int x, int y, int size, int **maze, int end_x, int end_y) {
    if (x < 0 || x >= size || y < 0 || y >= size || maze[x][y] != 1) {
        return 0;  // Not valid or already visited
    }
    
    // If we reach the end point
    if (x == end_x && y == end_y) {
        return 1;
    }

    maze[x][y] = 2;  // Mark as visited

    // Try all four directions
    for (int i = 0; i < 4; i++) {
        if (is_reachable(x + directions[i][0], y + directions[i][1], size, maze, end_x, end_y)) {
            return 1;
        }
    }

    return 0;  // No path found
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
            } else if (maze[i][j] == 4) {
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

    int SIZE = 20;  

    // Dynamically allocate memory for the maze
    int **maze = malloc(sizeof(int *) * SIZE);
    if (maze == NULL) {
        fprintf(stderr, "Memory allocation failed for maze rows!\n");
        exit(1);
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            maze[i][j] = 0;  // Initially mark all cells as walls
        }
    }

    // Generate the maze starting from (0, 0)
    generate_maze(0, 0, SIZE, maze);
    maze[0][0] = 3;  // Start point (mark it as '3')

    // Set the end point at a valid location, checking for reachable cells
    int end_x = (SIZE % 2 == 0) ? SIZE - 2 : SIZE - 1;
    int end_y = (SIZE % 2 == 0) ? SIZE - 2 : SIZE - 1;
    maze[end_x][end_y] = 4;  // End point (mark it as '4')

    // Ensure the end point is reachable from the start
    if (!is_reachable(0, 0, SIZE, maze, end_x, end_y)) {
        //printf("End point is not reachable, regenerating maze...\n");
        // Optionally regenerate the maze or fix the end point connection manually
        // You can either call generate_maze() again or forcefully connect the end point
        maze[end_x][end_y] = 0;  // Reset the end point
        generate_maze(0, 0, SIZE, maze);  // Regenerate maze
        maze[end_x][end_y] = 4;  // Reassign the end point
    }

    // Add extra paths to create multiple solutions (limit the number of extra paths)
    add_extra_paths(SIZE, maze);

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

    // Free the allocated memory for the maze
    for (int i = 0; i < SIZE; i++) {
        free(maze[i]);
    }
    free(maze);

    return 0;
}

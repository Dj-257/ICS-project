#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 30  // Size of the maze (SIZE x SIZE)

int maze[SIZE][SIZE];

// Directions for moving in the maze (up, down, left, right)
int directions[4][2] = {
    {-1, 0}, // Up
    {1, 0},  // Down
    {0, -1}, // Left
    {0, 1}   // Right
};

// Function to check if a cell is within the maze bounds
int is_valid(int x, int y) {
    return (x >= 0 && x < SIZE && y >= 0 && y < SIZE && maze[x][y] == 0);
}

// Recursive function to generate the maze using DFS
void generate_maze(int x, int y) {
    maze[x][y] = 1;  // Mark the cell as a path

    // Shuffle directions to ensure random movement
    for (int i = 0; i < 4; i++) {
        int j = rand() % 4;
        int temp[2] = {directions[i][0], directions[i][1]};
        directions[i][0] = directions[j][0];
        directions[i][1] = directions[j][1];
        directions[j][0] = temp[0];
        directions[j][1] = temp[1];
    }

    // Visit each direction
    for (int i = 0; i < 4; i++) {
        int nx = x + directions[i][0] * 2;
        int ny = y + directions[i][1] * 2;

        if (is_valid(nx, ny)) {
            maze[x + directions[i][0]][y + directions[i][1]] = 1; // Remove wall
            generate_maze(nx, ny);
        }
    }
}

// Function to draw the maze in the GTK window
static void draw_maze(GtkDrawingArea *area, cairo_t *cr, gpointer user_data) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
           if (maze[i][j] == 0) {
                // Draw wall (black color)
                cairo_set_source_rgb(cr, 0, 0, 0);
            }
            else if(i==0 && j==0)
            {
                cairo_set_source_rgb(cr, 0, 1, 0);
            }
            else if(i==SIZE-2 && j==SIZE-2)
            {
                cairo_set_source_rgb(cr, 1, 0, 0);
            }
            else if(i==SIZE-1 && j==SIZE-1)
            {
                cairo_set_source_rgb(cr, 0, 0, 0);
            }
            else
            {     // Draw path (white color)
                cairo_set_source_rgb(cr, 1, 1, 1);
            }
            cairo_rectangle(cr, j * 20, i * 20, 20, 20);
            cairo_fill(cr);
        }
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); // Seed the random number generator

    // Initialize the maze with walls
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            maze[i][j] = 0; // Initialize walls
        }
    }

    // Generate the maze starting from (0, 0)
    generate_maze(0, 0);
    maze[0][0]=3;
    maze[SIZE-2][SIZE-2]=4;

    // Initialize GTK
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Random Maze");
    gtk_window_set_default_size(GTK_WINDOW(window), SIZE * 20, SIZE * 20);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, SIZE * 20, SIZE * 20);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_maze), NULL);
    gtk_container_add(GTK_CONTAINER(window), drawing_area);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
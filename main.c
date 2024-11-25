#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#define INF INT_MAX

//global variables  for widgets
GtkWidget *window, *main_window, *scrolled_window;
GtkWidget *stack;
GtkWidget *login_fixed, *register_fixed;
GtkWidget *box ,*grid;
GtkWidget *logout_button, *generate_button, *solve_button1, *solve_button2, *solve_button3;
GtkWidget *maze_size_entry, *maze_size_label;
GtkWidget *maze_area, *solved_maze_area, *drawing_area, *drawing_area_dijkstra, *drawing_area_astar;
GtkWidget *dijkstra_label, *a_star_label, *dijkstra_label_header, *a_star_label_header;
GtkWidget *username, *password;
GtkWidget *register_username, *register_password;
GtkWidget *login_button, *register_button;
GtkWidget *register_button_new, *back_button;
GtkWidget *welcome_label;
GtkWidget *label1, *label2, *label3, *label4, *label5;
GtkWidget *timepath_label_dijkstra, *timepath_label_astar;
GtkWidget *error_label = NULL, *success_label = NULL, *username_error_label = NULL, *empty_error_label = NULL;

GtkCssProvider *css_provider;

//structures for maze data
int directions[4][2] = {
    {-1, 0}, 
    {1, 0},  
    {0, -1}, 
    {0, 1}   
};

typedef struct {
    int size;
    int **maze;
    int **sdijkstra;
    int pathlength_dijkstra; 
    double runtime_dijkstra;
    int **sastar;
    int pathlength_astar;
    double runtime_astar;
} MazeData;

MazeData maze_data;

typedef struct {
    int x, y;       
    int gCost;      
    int hCost;      
    int fCost;      
    int parentX;    
    int parentY;
} Node;


//maze generation functions


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

int is_reachable(int x, int y, int size, int **maze, int end_x, int end_y) {
    if (x < 0 || x >= size || y < 0 || y >= size || maze[x][y] != 1) {
        return 0;  
    }
    
    if (x == end_x && y == end_y) {
        return 1;
    }

    maze[x][y] = 2;

    for (int i = 0; i < 4; i++) {
        if (is_reachable(x + directions[i][0], y + directions[i][1], size, maze, end_x, end_y)) {
            return 1;
        }
    }

    return 0; 
}

static void on_generate_button_clicked(GtkWidget *button, gpointer user_data) {

    const char *size_text = gtk_entry_get_text(GTK_ENTRY(maze_size_entry));
    int size = atoi(size_text);

    if (size <= 3 || size > 300) { 
        g_print("Invalid maze size. Please enter a positive integer up to 300 and greater than 3.\n");
        return;
    }

    if (maze_data.maze) {
        for (int i = 0; i < maze_data.size; i++) {
            free(maze_data.maze[i]);
        }
        free(maze_data.maze);
        maze_data.maze = NULL;    
    }

    if (maze_data.sdijkstra) {
        for (int i = 0; i < maze_data.size; i++) {
            free(maze_data.sdijkstra[i]);
        }
        free(maze_data.sdijkstra);
        maze_data.sdijkstra = NULL;
    }

    if (maze_data.sastar) {
        for (int i = 0; i < maze_data.size; i++) {
            free(maze_data.sastar[i]);
        }
        free(maze_data.sastar);
        maze_data.sastar = NULL;
    }

    maze_data.size = size;
    maze_data.maze = malloc(sizeof(int *) * size);
    for (int i = 0; i < size; i++) {
        maze_data.maze[i] = malloc(sizeof(int) * size);
        for (int j = 0; j < size; j++) {
            maze_data.maze[i][j] = 0;
        }
    }

    generate_maze(0, 0, size, maze_data.maze);

    maze_data.maze[0][0] = 3;
    int end_x = (size % 2 == 0) ? size - 2 : size - 1;
    int end_y = (size % 2 == 0) ? size - 2 : size - 1;
    maze_data.maze[end_x][end_y] = 4; 

    if (!is_reachable(0, 0, size, maze_data.maze, end_x, end_y)) {
        maze_data.maze[end_x][end_y] = 0; 
        generate_maze(0, 0, size, maze_data.maze); 
        maze_data.maze[end_x][end_y] = 4;
        maze_data.maze[0][0]=3;
    }


    gtk_widget_hide(maze_area);
    gtk_widget_hide(drawing_area_dijkstra);
    gtk_widget_hide(drawing_area_astar);
    gtk_widget_hide(timepath_label_dijkstra);
    gtk_widget_hide(timepath_label_astar);

    gtk_widget_queue_draw(GTK_WIDGET(drawing_area));

}

static void draw_maze_callback(GtkDrawingArea *area, cairo_t *cr, gpointer user_data) {
    MazeData *data = (MazeData *)user_data;
    int size = data->size;
    int **maze = data->maze;

    if (!maze) return;

    double cell_size = 600.0 / size; 

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (maze[i][j] == 0) {
                cairo_set_source_rgb(cr, 0, 0, 0); 
            } else if (maze[i][j] == 3) {
                cairo_set_source_rgb(cr, 0, 1, 0); 
            } else if (maze[i][j] == 4) {
                cairo_set_source_rgb(cr, 1, 0, 0); 
            } else if ((i == size-2 && j == size-3)||(i == size-3 && j == size-2)){
                cairo_set_source_rgb(cr, 1, 1, 1);
            } else {
                cairo_set_source_rgb(cr, 1, 1, 1); 
            }
            cairo_rectangle(cr, j * cell_size, i * cell_size, cell_size, cell_size);
            cairo_fill(cr);
        }
    }
}

//message display functions

void display_message_timepath(GtkWidget **label, char *message, int x) {
    if (*label != NULL) {
        gtk_widget_destroy(*label);
    }
    *label = gtk_label_new(message);
    gtk_grid_attach(GTK_GRID(grid), *label, 0, x, 2, 1);
    gtk_widget_show(*label);
}


//solving maze

//dijkstra

int **allocate2DArray(int size) {
    int **array = (int **)malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        array[i] = (int *)malloc(size * sizeof(int));
    }
    return array;
}

void free2DArray(int **array, int size) {
    for (int i = 0; i < size; i++) {
        free(array[i]);
    }
    free(array);
}

int isValid_dijkstra(int x, int y, int **maze, int **visited, int size) {
    return (x >= 0 && x < size && y >= 0 && y < size && 
           (maze[x][y] == 1 || maze[x][y] == 4) && !visited[x][y]);
}

void dijkstraMaze(MazeData *mazeData) {
    int **maze = mazeData->maze;
    int size = mazeData->size;

    int **dist = allocate2DArray(size);
    int **visited = allocate2DArray(size);
    int pred[size][size][2]; 

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            dist[i][j] = INF;
            visited[i][j] = 0;
            pred[i][j][0] = -1;
            pred[i][j][1] = -1;
        }
    }

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
    start = clock(); 

    while (1) {
        visited[x][y] = 1;

        for (int i = 0; i < 4; i++) {
            int newX = x + directions[i][0];
            int newY = y + directions[i][1];

            if (isValid_dijkstra(newX, newY, maze, visited, size)) {
                int newDist = dist[x][y] + 1;
                if (newDist < dist[newX][newY]) {
                    dist[newX][newY] = newDist;
                    pred[newX][newY][0] = x;
                    pred[newX][newY][1] = y;
                }
            }
        }

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

    end = clock();

    if (size < 20) {
        mazeData->runtime_dijkstra = ((double)(end - start)) / CLOCKS_PER_SEC * 1e9;
    } else {
        mazeData->runtime_dijkstra = ((double)(end - start)) / CLOCKS_PER_SEC * 1e6;
    }
    
    if (dist[endX][endY] == INF) {
        mazeData->pathlength_dijkstra = -1; 
    } else {
        mazeData->pathlength_dijkstra = dist[endX][endY];

        int pathX = endX, pathY = endY;
        mazeData->sdijkstra = allocate2DArray(size);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                mazeData->sdijkstra[i][j] = maze[i][j];
            }
        }

        while (pathX != -1 && pathY != -1) {
            if (maze[pathX][pathY] != 3 && maze[pathX][pathY] != 4) {
                mazeData->sdijkstra[pathX][pathY] = 2; 
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

//solved maze generation
static void on_solve_button_clicked_dijkstra(GtkWidget *button, gpointer user_data) {

    if (maze_data.size == 0) {
        g_print("Please generate a maze first.\n");
        return;
    }
    
    int size = maze_data.size;

    if (maze_data.sdijkstra) {
        for (int i = 0; i < size; i++) {
            free(maze_data.sdijkstra[i]);
        }
        free(maze_data.sdijkstra);
        maze_data.sdijkstra = NULL;
    }

    maze_data.sdijkstra = malloc(sizeof(int *) * size);
    for (int i = 0; i < size; i++) {
        maze_data.sdijkstra[i] = malloc(sizeof(int) * size);
        for (int j = 0; j < size; j++) {
            maze_data.sdijkstra[i][j] = 0;
        }
    }

    dijkstraMaze(&maze_data);
    gtk_widget_queue_draw(GTK_WIDGET(drawing_area_dijkstra));
    gtk_widget_show(drawing_area_dijkstra);

    char timepathlabel[256];
    if (maze_data.pathlength_dijkstra == -1) {
        sprintf(timepathlabel, "The program executes in %.9f microseconds and no path exists using Djikstra", maze_data.runtime_dijkstra);
    } 
    else {
    sprintf(timepathlabel, "The program executes in %.9f microseconds and path length is %d units using Djikstra", maze_data.runtime_dijkstra, maze_data.pathlength_dijkstra);
    }
    display_message_timepath(&timepath_label_dijkstra, timepathlabel, 6);


}

static void draw_maze_dijkstra_callback(GtkDrawingArea *area, cairo_t *cr, gpointer user_data) {
    MazeData *data = (MazeData *)user_data;
    int size = data->size;
    int **dijkstra_maze = data->sdijkstra;

    if (!dijkstra_maze) return;

    double cell_size = 600.0 / size; 

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (dijkstra_maze[i][j] == 0) {
                cairo_set_source_rgb(cr, 0, 0, 0); 
            } else if (dijkstra_maze[i][j] == 3) {
                cairo_set_source_rgb(cr, 0, 1, 0); 
            } else if (dijkstra_maze[i][j] == 4) {
                cairo_set_source_rgb(cr, 1, 0, 0); 
            } else if (dijkstra_maze[i][j] == 2) {
                cairo_set_source_rgb(cr, 0, 1, 0); 
            } else {
                cairo_set_source_rgb(cr, 1, 1, 1); 
            }
            cairo_rectangle(cr, j * cell_size, i * cell_size, cell_size, cell_size);
            cairo_fill(cr);
        }
    }
}

//a star

int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

int isValid_astar(int x, int y, MazeData *mazeData) {
    return (x >= 0 && x < mazeData->size && y >= 0 && y < mazeData->size &&
            (mazeData->maze[x][y] == 1 || mazeData->maze[x][y] == 4));
}

Node **allocateNodes(int size) {
    Node **nodes = (Node **)malloc(size * sizeof(Node *));
    for (int i = 0; i < size; i++) {
        nodes[i] = (Node *)malloc(size * sizeof(Node));
    }
    return nodes;
}

void tracePath(Node **nodes, MazeData *mazeData, int endX, int endY) {

    for (int i = 0; i < mazeData->size; i++) {
        for (int j = 0; j < mazeData->size; j++) {
            mazeData->sastar[i][j] = mazeData->maze[i][j];
        }
    }

    int x = endX, y = endY;
    mazeData->pathlength_astar = nodes[endX][endY].gCost;

    while (nodes[x][y].parentX != -1 && nodes[x][y].parentY != -1) {
        mazeData->sastar[x][y] = 2;
        int tempX = nodes[x][y].parentX;
        int tempY = nodes[x][y].parentY;
        x = tempX;
        y = tempY;
    }

    mazeData->sastar[x][y] = 3;
    mazeData->sastar[endX][endY] = 4;
}

void aStarMaze(MazeData *mazeData) {
    int startX = -1, startY = -1, endX = -1, endY = -1;

    for (int i = 0; i < mazeData->size; i++) {
        for (int j = 0; j < mazeData->size; j++) {
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

    Node **nodes = allocateNodes(mazeData->size);

    int **openList = allocate2DArray(mazeData->size);
    int **closedList = allocate2DArray(mazeData->size);
    for (int i = 0; i < mazeData->size; i++) {
    for (int j = 0; j < mazeData->size; j++) {
        openList[i][j] = 0;  
        closedList[i][j] = 0; 
    }
}

    for (int i = 0; i < mazeData->size; i++) {
        for (int j = 0; j < mazeData->size; j++) {
            nodes[i][j].x = i;
            nodes[i][j].y = j;
            nodes[i][j].gCost = INF;
            nodes[i][j].hCost = INF;
            nodes[i][j].fCost = INF;
            nodes[i][j].parentX = -1;
            nodes[i][j].parentY = -1;
        }
    }

    nodes[startX][startY].gCost = 0;
    nodes[startX][startY].hCost = heuristic(startX, startY, endX, endY);
    nodes[startX][startY].fCost = nodes[startX][startY].hCost;

    openList[startX][startY] = 1;

    clock_t start, end;
    start = clock(); 

    while (1) {

        int minFCost = INF;
        int currentX = -1, currentY = -1;

        for (int i = 0; i < mazeData->size; i++) {
            for (int j = 0; j < mazeData->size; j++) {
                if (openList[i][j] && nodes[i][j].fCost < minFCost) {
                    minFCost = nodes[i][j].fCost;
                    currentX = i;
                    currentY = j;
                }
            }
        }

        if (currentX == -1 || currentY == -1) {
            mazeData->pathlength_astar = -1;
            return; 
        }

        if (currentX == endX && currentY == endY) {
            tracePath(nodes, mazeData, endX, endY);
            end = clock(); 

            mazeData->runtime_astar = ((double)(end - start)) / CLOCKS_PER_SEC * 1e6;
            break;
        }

        openList[currentX][currentY] = 0;
        closedList[currentX][currentY] = 1;

        for (int i = 0; i < 4; i++) {
            int newX = currentX + directions[i][0];
            int newY = currentY + directions[i][1];

            if (isValid_astar(newX, newY, mazeData) && !closedList[newX][newY]) {
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


    for (int i = 0; i < mazeData->size; i++) {
        free(nodes[i]); 
    }
    free(nodes);  

    free2DArray(openList, mazeData->size);
    free2DArray(closedList, mazeData->size);
}    

//solved maze generation
static void on_solve_button_clicked_astar(GtkWidget *button, gpointer user_data) {

    if (maze_data.size == 0) {
        g_print("Please generate a maze first.\n");
        return;
    }
    
    int size = maze_data.size;

    if (maze_data.sastar) {
        for (int i = 0; i < size; i++) {
            free(maze_data.sastar[i]);
        }
        free(maze_data.sastar);
        maze_data.sastar = NULL;
    }

    maze_data.sastar = malloc(sizeof(int *) * size);
    for (int i = 0; i < size; i++) {
        maze_data.sastar[i] = malloc(sizeof(int) * size);
        for (int j = 0; j < size; j++) {
            maze_data.sastar[i][j] = 0; 
        }
    }

    aStarMaze(&maze_data);
    gtk_widget_queue_draw(GTK_WIDGET(drawing_area_astar));
    gtk_widget_show(drawing_area_astar);

    char timepathlabel[256];
    if (maze_data.pathlength_astar == -1) {
        sprintf(timepathlabel, "The program executes in %.9f microseconds and no path exists using A Star", maze_data.runtime_astar);
    } 
    else {
    sprintf(timepathlabel, "The program executes in %.9f microseconds and path length is %d units using A Star", maze_data.runtime_astar, maze_data.pathlength_astar);
    }
    display_message_timepath(&timepath_label_astar, timepathlabel, 7);

}

static void draw_maze_astar_callback(GtkDrawingArea *area, cairo_t *cr, gpointer user_data) {
    MazeData *data = (MazeData *)user_data;
    int size = data->size;
    int **astar_maze = data->sastar;

    if (!astar_maze) return;

    double cell_size = 600.0 / size; 

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (astar_maze[i][j] == 0) {
                cairo_set_source_rgb(cr, 0, 0, 0);
            } else if (astar_maze[i][j] == 3) {
                cairo_set_source_rgb(cr, 0, 1, 0);
            } else if (astar_maze[i][j] == 4) {
                cairo_set_source_rgb(cr, 1, 0, 0);
            } else if (astar_maze[i][j] == 2) {
                cairo_set_source_rgb(cr, 0, 1, 0); 
            } else {
                cairo_set_source_rgb(cr, 1, 1, 1); 
            }
            cairo_rectangle(cr, j * cell_size, i * cell_size, cell_size, cell_size);
            cairo_fill(cr);
        }
    }

}

//general functions for displaying messages
void display_message(GtkWidget **label, GtkWidget *parent, const char *message, int x, int y) {
    if (*label != NULL) {
        gtk_widget_destroy(*label);
    }
    *label = gtk_label_new(message);
    gtk_fixed_put(GTK_FIXED(parent), *label, x, y);
    gtk_widget_show(*label);
}


void on_register_button_clicked(GtkWidget *button, gpointer user_data) {

    if (GTK_IS_WIDGET(empty_error_label)) {
    gtk_widget_destroy(empty_error_label);
    empty_error_label = NULL; 
    }

    if (GTK_IS_WIDGET(username_error_label)) {
    gtk_widget_destroy(username_error_label);
    username_error_label = NULL; 
    }

    if (GTK_IS_WIDGET(success_label)) {
    gtk_widget_destroy(success_label);
    success_label = NULL; 
    }

    if (GTK_IS_WIDGET(error_label)) { 
        gtk_widget_destroy(error_label);
        error_label = NULL; 
    }

    GtkStack *stack = GTK_STACK(user_data);
    gtk_stack_set_visible_child_name(stack, "register");
}

void on_back_button_clicked(GtkWidget *button, gpointer user_data) {

    if (GTK_IS_WIDGET(empty_error_label)) {
    gtk_widget_destroy(empty_error_label);
    empty_error_label = NULL; 
    }

    if (GTK_IS_WIDGET(username_error_label)) {
    gtk_widget_destroy(username_error_label);
    username_error_label = NULL; 
    }

    if (GTK_IS_WIDGET(success_label)) {
    gtk_widget_destroy(success_label);
    success_label = NULL; 
    }

    if (GTK_IS_WIDGET(error_label)) { 
        gtk_widget_destroy(error_label);
        error_label = NULL; 
    }

    GtkStack *stack = GTK_STACK(user_data);
    gtk_stack_set_visible_child_name(stack, "login");
}

void on_logout_button_clicked(GtkWidget *button, gpointer user_data) {
    gtk_widget_hide(main_window);
    gtk_widget_show(window);
}


//main window
void main_window_create(const char *username) {

    maze_data.size = 0;
    maze_data.maze = NULL;
   
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "Maze Solver");
    gtk_window_maximize(GTK_WINDOW(main_window));
    gtk_window_set_resizable(GTK_WINDOW(main_window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);

    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(main_window), scrolled_window);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(scrolled_window), box);

    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);

    
    grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

  
    welcome_label = gtk_label_new(NULL);
    char welcome_text[256];
    snprintf(welcome_text, sizeof(welcome_text), "Welcome, %s!", username);
    gtk_label_set_text(GTK_LABEL(welcome_label), welcome_text);
    gtk_widget_set_halign(welcome_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), welcome_label, 0, 0, 1, 1);

    logout_button = gtk_button_new_with_label("Logout");
    gtk_widget_set_halign(logout_button, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), logout_button, 1, 0, 1, 1);

    maze_size_label = gtk_label_new("Maze Size:");
    gtk_widget_set_halign(maze_size_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), maze_size_label, 0, 1, 1, 1);

    maze_size_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), maze_size_entry, 1, 1, 1, 1);

    generate_button = gtk_button_new_with_label("Generate Maze");
    gtk_widget_set_halign(generate_button, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), generate_button, 0, 2, 2, 1);

    maze_area = gtk_label_new("Maze will appear here.");
    gtk_widget_set_hexpand(maze_area, TRUE);
    gtk_widget_set_vexpand(maze_area, TRUE);
    gtk_widget_set_halign(maze_area, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(maze_area, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), maze_area, 0, 3, 2, 1);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 600, 600);
    gtk_widget_set_hexpand(drawing_area, TRUE);
    gtk_widget_set_vexpand(drawing_area, TRUE);
    gtk_widget_set_halign(drawing_area, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(drawing_area, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), drawing_area, 0, 4, 2, 1);

    dijkstra_label_header = gtk_label_new("DIJKSTRA ALGORITHM");
    dijkstra_label = gtk_label_new(NULL); 
    gtk_label_set_use_markup(GTK_LABEL(dijkstra_label), TRUE); 
    const char *markup_text =
    "Time: Comparatively slower, explores all paths.\n"
    "Complexity: O((V + E) log V)\n"
    "Path: Finds the shortest path, but may explore unnecessary paths.\n"
    "Memory consumption: It is slightly more memory efficient.\n"
    "Application: Suitable when there is no goal or when exploring all nodes is necessary.\n";
    gtk_label_set_markup(GTK_LABEL(dijkstra_label), markup_text);
    gtk_widget_set_halign(dijkstra_label, GTK_ALIGN_START);
    gtk_widget_set_halign(dijkstra_label_header, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), dijkstra_label_header, 0, 2, 1, 2);
    gtk_grid_attach(GTK_GRID(grid), dijkstra_label, 0, 4, 1, 2);

    a_star_label_header = gtk_label_new("A STAR ALGORITHM                                                                                                      ");
    a_star_label = gtk_label_new(NULL);
    gtk_label_set_use_markup(GTK_LABEL(a_star_label), TRUE);
    const char *markup_text2 =
    "Time: Comparatively Faster, uses a heuristic to prioritize paths.\n"
    "Complexity: O((V + E) log V)\n"
    "Path: Finds the shortest path with a good heuristic, but can be longer with a bad one.\n"
    "Memory consumption: A* stores information about each node. In big mazes,\nthis can lead to significant memory usage.\n"
    "Application: Ideal when a goal is known, as the heuristic helps focus the search toward the goal.\n";
    gtk_label_set_markup(GTK_LABEL(a_star_label), markup_text2);
    gtk_widget_set_halign(a_star_label, GTK_ALIGN_END);
    gtk_widget_set_halign(a_star_label_header, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), a_star_label_header, 1, 2, 1, 2);
    gtk_grid_attach(GTK_GRID(grid), a_star_label, 1, 4, 1, 2);

    solve_button1 = gtk_button_new_with_label("Solve Maze Using Dijkstra Algorithm");
    gtk_widget_set_halign(solve_button1, GTK_ALIGN_START); 
    gtk_grid_attach(GTK_GRID(grid), solve_button1, 0, 5, 2, 1);

    solve_button2 = gtk_button_new_with_label("Solve Maze using A Star Algorithm");
    gtk_widget_set_halign(solve_button2, GTK_ALIGN_END); 
    gtk_grid_attach(GTK_GRID(grid), solve_button2, 0, 5, 2, 1);


    drawing_area_dijkstra = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area_dijkstra, 600, 600);
    gtk_widget_set_hexpand(drawing_area_dijkstra, TRUE);
    gtk_widget_set_vexpand(drawing_area_dijkstra, TRUE);
    gtk_widget_set_halign(drawing_area_dijkstra, GTK_ALIGN_START);
    gtk_widget_set_valign(drawing_area_dijkstra, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), drawing_area_dijkstra, 0, 7, 2, 1);

    drawing_area_astar = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area_astar, 600, 600);
    gtk_widget_set_hexpand(drawing_area_astar, TRUE);
    gtk_widget_set_vexpand(drawing_area_astar, TRUE);
    gtk_widget_set_halign(drawing_area_astar, GTK_ALIGN_END);
    gtk_widget_set_valign(drawing_area_astar, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), drawing_area_astar, 0, 7, 2, 1);


    timepath_label_dijkstra= gtk_label_new(NULL);
    gtk_widget_set_halign(timepath_label_dijkstra, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), timepath_label_dijkstra, 0, 6, 2, 1);

    timepath_label_astar = gtk_label_new(NULL);
    gtk_widget_set_halign(timepath_label_astar, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), timepath_label_astar, 0, 7, 2, 1);



    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_maze_callback), &maze_data);
    g_signal_connect(drawing_area_dijkstra, "draw", G_CALLBACK(draw_maze_dijkstra_callback), &maze_data);
    g_signal_connect(drawing_area_astar, "draw", G_CALLBACK(draw_maze_astar_callback), &maze_data);

    g_signal_connect(generate_button, "clicked", G_CALLBACK(on_generate_button_clicked), drawing_area);
    g_signal_connect(solve_button1, "clicked", G_CALLBACK(on_solve_button_clicked_dijkstra), drawing_area_dijkstra);
    g_signal_connect(solve_button2, "clicked", G_CALLBACK(on_solve_button_clicked_astar), drawing_area_astar);
    g_signal_connect(logout_button, "clicked", G_CALLBACK(on_logout_button_clicked), NULL);

   
    gtk_widget_show_all(main_window);
}


//login and register functions
void on_login_button_clicked(GtkWidget *button, gpointer user_data) {


    const char *usrnm = gtk_entry_get_text(GTK_ENTRY(username));
    const char *psswd = gtk_entry_get_text(GTK_ENTRY(password));

    if (GTK_IS_WIDGET(empty_error_label)) {
    gtk_widget_destroy(empty_error_label);
    empty_error_label = NULL; 
    }

    if (GTK_IS_WIDGET(username_error_label)) {
    gtk_widget_destroy(username_error_label);
    username_error_label = NULL; 
    }

    if (GTK_IS_WIDGET(success_label)) {
    gtk_widget_destroy(success_label);
    success_label = NULL; 
    }

    if (GTK_IS_WIDGET(error_label)) { 
        gtk_widget_destroy(error_label);
        error_label = NULL; 
    }

    if (strlen(usrnm) == 0 || strlen(psswd) == 0) {
        display_message(&empty_error_label, login_fixed, "Username or password cannot be empty", 80, 270);
        return;
    }

    //authentication
    FILE *fptr;
    gboolean is_auth = FALSE;
    fptr = fopen("users.txt", "r");
    char usrnmf[100], psswdf[100];
    while (fscanf(fptr, "%s %s", usrnmf, psswdf) != EOF) {
        if (strcmp(usrnm, usrnmf) == 0 && strcmp(psswd, psswdf) == 0) {
            is_auth= TRUE;
        }
    }
    fclose(fptr);

    if (is_auth) {
        gtk_widget_hide(window);
        main_window_create(usrnm);
    }
    else {
        display_message(&error_label, login_fixed, "Invalid username or password", 100, 270);
    }


}

gboolean switch_to_login(gpointer user_data) {
    GtkStack *stack = GTK_STACK(user_data);
    gtk_stack_set_visible_child_name(stack, "login");
    return FALSE; 
}

void on_register_button_clicked_final(GtkWidget *button, gpointer user_data) {

    const char *usrnm = gtk_entry_get_text(GTK_ENTRY(register_username));
    const char *psswd = gtk_entry_get_text(GTK_ENTRY(register_password));

    if (GTK_IS_WIDGET(empty_error_label)) {
    gtk_widget_destroy(empty_error_label);
    empty_error_label = NULL; 
    }

    if (GTK_IS_WIDGET(username_error_label)) {
    gtk_widget_destroy(username_error_label);
    username_error_label = NULL; 
    }

    if (GTK_IS_WIDGET(success_label)) {
    gtk_widget_destroy(success_label);
    success_label = NULL; 
    }

    if (GTK_IS_WIDGET(error_label)) { 
        gtk_widget_destroy(error_label);
        error_label = NULL; 
    }

    if (strlen(usrnm) == 0 || strlen(psswd) == 0) {
        display_message(&empty_error_label, register_fixed, "Username or password cannot be empty", 80, 250);
        return;
    }

    //checking for unique username
    FILE *fptr;
    fptr = fopen("users.txt", "r");
    char usrnmf[100], psswdf[100];
    while (fscanf(fptr, "%s %s", usrnmf, psswdf) != EOF) {
        if (strcmp(usrnm, usrnmf) == 0) {
            display_message(&username_error_label, register_fixed, "Username already exists", 120, 250);
            return;
        }
    }
    fclose(fptr);

    //new account
    fptr = fopen("users.txt", "a");
    fprintf(fptr, "%s %s\n", usrnm, psswd);
    fclose(fptr);
    display_message(&success_label, register_fixed, "Account created successfully", 100, 250);
    g_timeout_add(2000, switch_to_login, user_data);

}


//login window
int main(int argc, char *argv[]) {

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Maze Solver");
    gtk_window_set_default_size(GTK_WINDOW(window), 380, 300);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_file(css_provider, g_file_new_for_path("style.css"), NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    
    stack = gtk_stack_new();
    login_fixed = gtk_fixed_new();
    register_fixed = gtk_fixed_new();

    gtk_stack_add_named(GTK_STACK(stack), login_fixed, "login");
    gtk_stack_add_named(GTK_STACK(stack), register_fixed, "register");
    gtk_stack_set_transition_type(GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 500);

    // Login page

    username = gtk_entry_new();
    password = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password), FALSE);
    login_button = gtk_button_new_with_label("Login");
    register_button = gtk_button_new_with_label("Register");

    label1 = gtk_label_new("Username");
    label2 = gtk_label_new("Password");
    label3 = gtk_label_new("Don't have an account?");


    gtk_fixed_put(GTK_FIXED(login_fixed), label1, 50, 55);
    gtk_fixed_put(GTK_FIXED(login_fixed), label2, 50, 105);
    gtk_fixed_put(GTK_FIXED(login_fixed), username, 150, 50);
    gtk_fixed_put(GTK_FIXED(login_fixed), password, 150, 100);
    gtk_fixed_put(GTK_FIXED(login_fixed), login_button, 160, 150);
    gtk_fixed_put(GTK_FIXED(login_fixed), label3, 120, 195);
    gtk_fixed_put(GTK_FIXED(login_fixed), register_button, 155, 225);

    // Register page

    register_username= gtk_entry_new();
    register_password= gtk_entry_new();
    register_button_new= gtk_button_new_with_label("Register");
    gtk_entry_set_visibility(GTK_ENTRY(register_password), FALSE);
    back_button= gtk_button_new_with_label("Back to login");

    label4 = gtk_label_new("Username");
    label5 = gtk_label_new("Password");


    gtk_fixed_put(GTK_FIXED(register_fixed), label4, 50, 55);
    gtk_fixed_put(GTK_FIXED(register_fixed), label5, 50, 105);
    gtk_fixed_put(GTK_FIXED(register_fixed), register_username, 150, 50);
    gtk_fixed_put(GTK_FIXED(register_fixed), register_password, 150, 100);
    gtk_fixed_put(GTK_FIXED(register_fixed), register_button_new, 160, 150);
    gtk_fixed_put(GTK_FIXED(register_fixed), back_button, 145, 200);

    g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_button_clicked), stack);
    g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_button_clicked), stack);
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), stack);
    g_signal_connect(register_button_new, "clicked", G_CALLBACK(on_register_button_clicked_final), stack);

    gtk_container_add(GTK_CONTAINER(window), stack);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

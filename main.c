#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>



//global variables
GtkWidget *window, *main_window;
GtkWidget *stack;
GtkWidget *login_fixed, *register_fixed;
GtkWidget *box ,*grid;
GtkWidget *username, *password;
GtkWidget *register_username, *register_password;
GtkWidget *login_button, *register_button;
GtkWidget *register_button_new, *back_button;
GtkWidget *label1, *label2, *label3, *label4, *label5;
GtkWidget *error_label = NULL, *success_label = NULL, *username_error_label = NULL, *empty_error_label = NULL;

#define SIZE 40
int maze[SIZE][SIZE];

int directions[4][2] = {
    {-1, 0}, // Up
    {1, 0},  // Down
    {0, -1}, // Left
    {0, 1}   // Right
};

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

void main_window_create() {
    
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "Maze Solver");
    gtk_window_maximize(GTK_WINDOW(main_window));
    gtk_window_set_resizable(GTK_WINDOW(main_window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);

    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);


    box=gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(main_window), box);

    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);

    grid=gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    GtkWidget *welcome_label = gtk_label_new("Welcome, Username!");
    gtk_widget_set_halign(welcome_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), welcome_label, 0, 0, 1, 1);

    GtkWidget *logout_button = gtk_button_new_with_label("Logout");
    gtk_widget_set_halign(logout_button, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), logout_button, 1, 0, 1, 1);

    GtkWidget *maze_size_label = gtk_label_new("Maze Size:");
    gtk_widget_set_halign(maze_size_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), maze_size_label, 0, 1, 1, 1);

    GtkWidget *maze_size_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), maze_size_entry, 1, 1, 1, 1);

    GtkWidget *generate_button = gtk_button_new_with_label("Generate Maze");
    gtk_widget_set_halign(generate_button, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), generate_button, 0, 2, 2, 1);

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


    GtkWidget *maze_area = gtk_label_new("Maze will appear here.");
    gtk_widget_set_hexpand(maze_area, TRUE);
    gtk_widget_set_vexpand(maze_area, TRUE);
    gtk_widget_set_halign(maze_area, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(maze_area, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), maze_area, 0, 3, 2, 1);
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, SIZE * 20, SIZE * 20);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_maze), NULL);
    gtk_container_add(GTK_CONTAINER(window), drawing_area);
    gtk_grid_attach(GTK_GRID(grid), drawing_area, 0, 4, 2, 1);





    GtkWidget *solve_button = gtk_button_new_with_label("Solve Maze");
    gtk_widget_set_halign(solve_button, GTK_ALIGN_CENTER); 
    gtk_grid_attach(GTK_GRID(grid), solve_button, 0, 5, 2, 1);

    GtkWidget *solved_maze_area = gtk_label_new("Solved maze will appear here.");
    gtk_widget_set_hexpand(solved_maze_area, TRUE);
    gtk_widget_set_vexpand(solved_maze_area, TRUE);
    gtk_widget_set_halign(solved_maze_area, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(solved_maze_area, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), solved_maze_area, 0, 6, 2, 1);
    

    
    gtk_widget_show_all(main_window);
    
}

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
        main_window_create();
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

int main(int argc, char *argv[]) {

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Maze Solver");
    gtk_window_set_default_size(GTK_WINDOW(window), 380, 300);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    GtkCssProvider *css_provider = gtk_css_provider_new();
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
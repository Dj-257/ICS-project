#include <gtk/gtk.h>

void on_register_button_clicked(GtkWidget *button, gpointer user_data) {
    GtkStack *stack = GTK_STACK(user_data);
    gtk_stack_set_visible_child_name(stack, "register");
}

void on_back_button_clicked(GtkWidget *button, gpointer user_data) {
    GtkStack *stack = GTK_STACK(user_data);
    gtk_stack_set_visible_child_name(stack, "login");
}

int main(int argc, char *argv[]) {

    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *login_fixed, *register_fixed;
    GtkWidget *stack;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Maze Solver");
    gtk_window_set_default_size(GTK_WINDOW(window), 380, 300);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    
    stack = gtk_stack_new();
    login_fixed = gtk_fixed_new();
    register_fixed = gtk_fixed_new();

    gtk_stack_add_named(GTK_STACK(stack), login_fixed, "login");
    gtk_stack_add_named(GTK_STACK(stack), register_fixed, "register");

    // Login page

    GtkWidget *username, *password;
    GtkWidget *login_button, *register_button;
    GtkWidget *label1, *label2, *label3;

    username = gtk_entry_new();
    password = gtk_entry_new();
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
    gtk_fixed_put(GTK_FIXED(login_fixed), label3, 120, 200);
    gtk_fixed_put(GTK_FIXED(login_fixed), register_button, 150, 240);

    // Register page

    GtkWidget *register_username, *register_password;
    GtkWidget *register_button_new, *back_button;
    GtkWidget *label4, *label5;

    register_username= gtk_entry_new();
    register_password= gtk_entry_new();
    register_button_new= gtk_button_new_with_label("Register");
    back_button= gtk_button_new_with_label("Back to login");

    label4 = gtk_label_new("Username");
    label5 = gtk_label_new("Password");

    gtk_fixed_put(GTK_FIXED(register_fixed), label4, 50, 55);
    gtk_fixed_put(GTK_FIXED(register_fixed), label5, 50, 105);
    gtk_fixed_put(GTK_FIXED(register_fixed), register_username, 150, 50);
    gtk_fixed_put(GTK_FIXED(register_fixed), register_password, 150, 100);
    gtk_fixed_put(GTK_FIXED(register_fixed), register_button_new, 160, 150);
    gtk_fixed_put(GTK_FIXED(register_fixed), back_button, 150, 200);

    g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_button_clicked), stack);
    g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_button_clicked), stack);


    gtk_container_add(GTK_CONTAINER(window), stack);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
#define GTK_DISABLE_DEPRECATED
#define GTK_DISABLE_SINGLE_INCLUDES

#include <gtk/gtk.h>

int main(int argc, char **argv)
{
  GtkWidget *notebook, *tabContainer, *tabCloseButton, *window, *tabLabel;
   
  gtk_init(&argc, &argv);

  /* Update style for notebook tab button */
  gtk_rc_parse_string (
         "style \"my-button-style\"\n"
         "{\n"
         "  GtkWidget::focus-padding = 0\n"
         "  GtkWidget::focus-line-width = 0\n"
         "  xthickness = 0\n"
         "  ythickness = 0\n"
         "}\n"
         "widget \"*.my-close-button\" style \"my-button-style\"");
   
   
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
        gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
        g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
        
        notebook = gtk_notebook_new();
        tabLabel = gtk_label_new("notebook tab");
        tabContainer = gtk_hbox_new(FALSE, 3);
        tabCloseButton = gtk_button_new();

      /* Set buttons name so the style will affect it */
        gtk_widget_set_name( tabCloseButton, "my-close-button" );
        gtk_button_set_relief(GTK_BUTTON(tabCloseButton), GTK_RELIEF_NONE);

        gtk_container_add(GTK_CONTAINER(window), notebook);
        gtk_box_pack_start(GTK_BOX(tabContainer), tabLabel, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(tabContainer),
        tabCloseButton, FALSE, FALSE, 0);
        gtk_container_add(GTK_CONTAINER(tabCloseButton),
        gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU));
        gtk_notebook_prepend_page(GTK_NOTEBOOK(notebook), gtk_label_new("example"), tabContainer);
       
        gtk_widget_show_all(window);
        gtk_widget_show_all(tabContainer);

        gtk_main();
        return 0;
}



#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <locale.h>

#include "giw.h"


/*------------------------------ GTK Interface and GIW ---------------------------*/
const static struct
{
  int r, g, b;
} palette[] =
{
  {255, 0, 0},                         /* Red */
  {255, 0, 255},                       /* Magenta */
  {0, 0, 255},                         /* Blue */
  {0, 255, 255},                       /* Cyan */
  {0, 255, 0},                         /* Green */
  {255, 255, 0},                       /* Yellow */
  {127, 0, 0},                         /* Dark Red */
  {127, 0, 127},                       /* Dark Magenta */
  {0, 0, 127},                         /* Dark Blue */
  {0, 127, 127},                       /* Dark Cyan */
  {0, 127, 0},                         /* Dark Green */
  {130, 127, 0},                       /* Dark Yellow */
  {0, 0, 0},                           /* Black */
  {25, 25, 25},                        /* Gray 10% */
  {51, 51, 51},                        /* Gray 20% */
  {76, 76, 76},                        /* Gray 30% */
  {102, 102, 102},                     /* Gray 40% */
  {127, 127, 127},                     /* Gray 50% */
  {153, 153, 153},                     /* Gray 60% */
  {178, 178, 178},                     /* Gray 70% */
  {204, 204, 204},                     /* Gray 80% */
  {229, 229, 229},                     /* Gray 90% */
  {255, 255, 255},                     /* White */
};

enum plot_type
{
  PT_VALUE, PT_DIFF
};

struct _log_plot_config_item
{
  enum plot_type type;
  char title[32];
  char pattern[14];
  char pattern2[14];
};

struct _log_time_config_item
{
  char title[32];
  char pattern[16];
  int tsk;
  int num;
  double temp_time;
  double total_time;
};

struct _log_plot_data
{
  int thread_no;
  int n_data;
  XYPlotData *xyplotdata;
  struct _log_plot_config_item *items;
  union Spec
  {
    struct _plot_spec_diff
    {
      int n1, n2;
    } diff;
  } *spec;
  double xmin, xmax;
  double ymin, ymax;
  GtkWidget *xyplot;
};

void quit();

void *umalloc(size_t size);

void *urealloc(void *ptr, size_t size);

void ufree(void *ptr);

void curve_toggled(GtkToggleButton * togglebutton, gpointer user_data);

struct gtkinter {
  static GtkWidget *window1;
  static GtkWidget *plot;  
  static GtkListStore *store;
  static GtkWidget *list;
  static GtkCellRenderer *renderer;
  static GtkTreeViewColumn *column;
  static GtkWidget *notebook;
  static _log_plot_config_item *glob_log_items;
  static _log_plot_config_item *loc_log_items;

  static GtkWidget *gen_log_plot(int nb, struct _log_plot_config_item *items,int thread) {
     struct _log_plot_data *data = (struct _log_plot_data *)malloc(sizeof(struct _log_plot_data));
     XYPlotData *d;
     int i;
     GtkWidget *main_hbox;
     GtkWidget *hbox;
     GtkWidget *vbox;
     GtkWidget *button;
     GtkWidget *label;

     memset(data, 0, sizeof(*data));

     data->thread_no = thread;
     data->n_data = nb;
     data->xyplotdata = (XYPlotData *)malloc(nb * sizeof(XYPlotData));
     memset(data->xyplotdata, 0, nb * sizeof(XYPlotData));
     for (i = 0; i < nb; i++)
     {
       d = &data->xyplotdata[i];
       d->l_width = 2;
       d->line_style = GDK_LINE_SOLID;
       d->p_color.red = palette[i].r << 8;
       d->p_color.green = palette[i].g << 8;
       d->p_color.blue = palette[i].b << 8;
       d->l_color.red = palette[i].r << 8;
       d->l_color.green = palette[i].g << 8;
       d->l_color.blue = palette[i].b << 8;
     }

     data->items = items;
     data->spec = umalloc(nb * sizeof(*data->spec));
     memset(data->spec, 0, nb * sizeof(*data->spec));

     data->xyplot = giw_xyplot_new();
     giw_xyplot_set_legends_digits(GIW_XYPLOT(data->xyplot), 5);

     giw_xyplot_set_ticks(GIW_XYPLOT(data->xyplot), 11, 1, 11, 1);

     for (i = 0; i < nb; i++)
       giw_xyplot_add_data(GIW_XYPLOT(data->xyplot), data->xyplotdata + i);

     main_hbox = gtk_hbox_new(FALSE, 0);
     gtk_box_pack_start(GTK_BOX(main_hbox), data->xyplot, TRUE, TRUE, 0);
     gtk_widget_show(data->xyplot);

     vbox = gtk_vbox_new(FALSE, 0);
     gtk_box_pack_start(GTK_BOX(main_hbox), vbox, FALSE, FALSE, 0);
     gtk_widget_show(vbox);

     for (i = 0; i < nb; i++)
     {
       hbox = gtk_hbox_new(FALSE, 0);
       button = gtk_color_button_new_with_color(&data->xyplotdata[i].l_color);
       gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 0);
       gtk_widget_show(button);
       label = gtk_check_button_new_with_label(items[i].title);
       gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
       gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(label), TRUE);
       g_object_set_data(G_OBJECT(label), "xyplot", data->xyplot);
       g_signal_connect(G_OBJECT(label), "toggled", G_CALLBACK(curve_toggled),
                        data->xyplotdata + i);
   
       gtk_widget_show(label);
       gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
       gtk_widget_show(hbox);
     }

     g_object_set_data(G_OBJECT(main_hbox), "log_plot_data", data);
     g_object_ref(G_OBJECT(main_hbox));

     return main_hbox;
  }

  static void init(int argc,char **argv) {
     gtk_init(&argc,&argv);

     window1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
     gtk_window_set_title(GTK_WINDOW(window1), "Bob++ log viewer");
     gtk_container_border_width(GTK_CONTAINER(window1), 0);
     gtk_signal_connect(GTK_OBJECT(window1), "destroy", GTK_SIGNAL_FUNC(quit), NULL);

     notebook = gtk_notebook_new();
     plot = gen_log_plot(1, glob_log_items,-1);
     gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
                           plot, gtk_label_new("Global view"));
     if (n_glob_log_items > 0)
       gtk_widget_show(plot);
     if (glob.n_tsk >= 2)
     {
       for (i = 0; i < glob.n_tsk; i++)
       {
         plot = gen_log_plot(1, loc_log_items, i);
         sprintf(buf, "Task #%d", i);
         gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
                               plot, gtk_label_new(buf));
         gtk_widget_show(plot);
       }
     }
     if (n_time_items) {
       const static char * hd[] = { "Task", "Title", "Value", "%" };
       int j;

       store = gtk_list_store_new(N_COLUMNS, G_TYPE_INT, G_TYPE_STRING,
        G_TYPE_DOUBLE, G_TYPE_DOUBLE);
       list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
       for (j=0; j<N_COLUMNS; j++) {
         renderer = gtk_cell_renderer_text_new();
         column = gtk_tree_view_column_new_with_attributes(hd[j], renderer, 
             "text", j, NULL);
         gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
       }
       gtk_notebook_append_page(GTK_NOTEBOOK(glob.notebook),
                             list, gtk_label_new("Times"));
       gtk_widget_show(list);
     }

     glob.timer = gtk_timeout_add(1000, log_plot_update, NULL);
     gtk_container_add(GTK_CONTAINER(window1), glob.notebook);
     gtk_widget_show(glob.notebook);

     gtk_widget_show(window1);
     gtk_main();
     return 0;
  }
  static void quit() {
     int i;
     GtkWidget *w;
     int n_plots = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook))-1;

     for (i=0; i<n_plots; i++) {
       w = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), i);
       delete_log_plot(w);
     }
     if( loc_log_items )
       ufree( loc_log_items );
     if( glob_log_items )
       ufree( glob_log_items );
     if( time_items )
       ufree( time_items );

     gtk_main_quit();
  }
      
};

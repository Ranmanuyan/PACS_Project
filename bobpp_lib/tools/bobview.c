#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <locale.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "giw.h"

#define GROWLOG 5

#define strzcpy(dest, src) {strncpy((dest),(src),sizeof(dest)-1); \
                            (dest)[sizeof(dest)-1] = '\0';}

enum
{
  TASK_COLUMN, TITLE_COLUMN, VALUE_COLUMN, PERC_COLUMN, N_COLUMNS
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

/* Global plot items array */
struct _log_plot_config_item *glob_log_items = NULL;
int n_glob_log_items = 0;

/* Local plot items array */
struct _log_plot_config_item *loc_log_items = NULL;
int n_loc_log_items = 0;

/* time items array */
struct _log_time_config_item *time_items = NULL;
int n_time_items = 0;

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

struct _log_plot_data
{
  int thread_no;
  int n_data;
  XYPlotData *xyplotdata;
  struct _log_plot_config_item *items;
  union
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

#define BUF_SIZE 4096

struct _glob
{
  int in_fd;
  char buffer[BUF_SIZE];
  int remain;
  char *bp;
  int n_plots;
  int n_tsk;
  GtkWidget *notebook;
  gint32 timer;
} glob;



/* reads a text line from the input file descriptor.
  this is a blocking function.

returns :
  1 = one line read in buf (trailing '\n' character removed).
  -1 = end of file
*/
int blk_read_line(char *buf)
{
  char *pos;
  int len;

  if (glob.remain == 0)
  {
    glob.remain = read(glob.in_fd, glob.buffer, BUF_SIZE);
    glob.bp = glob.buffer;
    if (glob.remain == 0)
    {
      /* end of file */
      return -1;
    }
  }
  pos = memchr(glob.bp, '\n', glob.remain);
  if (pos == NULL)
  {
    len = glob.remain;
    memcpy(buf, glob.bp, len);
    glob.remain = 0;
    return blk_read_line(buf + len);
  }
  len = pos - glob.bp;
  memcpy(buf, glob.bp, len);
  buf[len] = '\0';
  glob.remain -= len + 1;
  glob.bp = pos + 1;
  return 1;
}

/* reads a text line from the input file descriptor.
  doesn't block if no data is available.

returns :
  1 = one line read in buf (trailing '\n' character removed).
  0 = no data available
  -1 = end of file
*/
int read_line(char *buf)
{
  char *pos;
  int len;

  if (glob.remain == 0)
  {
    int retval;
    fd_set rfds;
    static struct timeval tv = { 0, 0 };

    FD_ZERO(&rfds);
    FD_SET(glob.in_fd, &rfds);

    retval = select(glob.in_fd + 1, &rfds, NULL, NULL, &tv);
    if (retval == -1)
      perror("select"), abort();
    if (retval == 0)
      return 0;

    glob.remain = read(glob.in_fd, glob.buffer, BUF_SIZE);
    glob.bp = glob.buffer;
    if (glob.remain == 0)
    {
      /* end of file */
      return -1;
    }
  }
  pos = memchr(glob.bp, '\n', glob.remain);
  if (pos == NULL)
  {
    len = glob.remain;
    memcpy(buf, glob.bp, len);
    glob.remain = 0;
    return read_line(buf + len);
  }
  len = pos - glob.bp;
  memcpy(buf, glob.bp, len);
  buf[len] = '\0';
  glob.remain -= len + 1;
  glob.bp = pos + 1;
  return 1;
}


static void *umalloc(size_t size)
{
  void *p = malloc(size);

  if (p == NULL)
    perror("umalloc"), abort();
  return p;
}

static void *urealloc(void *ptr, size_t size)
{
  ptr = realloc(ptr, size);
  if (ptr == NULL)
    perror("urealloc"), abort();
  return ptr;
}

#if 0
static char *ustrdup(const char *s)
{
  char *ptr = strdup(s);

  if (ptr == NULL)
    perror("ustrdup"), abort();
  return ptr;
}
#endif

static void ufree(void *ptr)
{
  free(ptr);
}

void add_log_item(struct _log_plot_config_item **_items, int *_n_items, enum
    plot_type type, const char *title, const char *pattern, const char *pattern2)
{
  struct _log_plot_config_item *items = *_items;
  int n_items = *_n_items;
  int size = 0;
  int item;

  if (n_items)
    size = 4;
  while (n_items > size)
    size *= 2;

  item = n_items;
  n_items += 1;
  if (n_items > size) {
    if (size == 0)
      size = 4;
    else
      size *= 2;
    items = urealloc(items, size * sizeof(items[0]));
    *_items = items;
  }
  items[item].type = type;
  strzcpy(items[item].title, title);
  strzcpy(items[item].pattern, pattern);
  if (pattern2)
    strzcpy(items[item].pattern2, pattern2);
  *_n_items = n_items;
}

void add_time_item( const char *title, const char *pattern, int local )
{
  int size = 0;
  int n = 1;
  struct _log_time_config_item *item;
  int i;

  if (n_time_items>0)
    size = 4;
  while (n_time_items>size)
    size *= 2;

  if (local)
    n = glob.n_tsk;

  for (i=0; i<n; i++) {
    n_time_items++;
    if (n_time_items > size) {
      if (size == 0)
        size = 4;
      while (n_time_items>size)
        size *= 2;
      time_items = urealloc( time_items, size*sizeof(time_items[0]) );
    }
    item = &time_items[n_time_items-1];
    strzcpy(item->title, title);
    strzcpy(item->pattern, pattern);
    item->tsk = (local ? i : -1);
    item->num = -1;
    item->temp_time = 0.0;
    item->total_time = 0.0;
  }
}

void init_log_items(void)
{
  char buf[80];
  char *p;
  int istime;
  enum plot_type type;

  do {
    if (blk_read_line(buf)==-1)
      goto err;
  } while (strcmp(buf,"{"));
  if (blk_read_line(buf)==-1)
    goto err;
  while (strcmp(buf,"}")) {
    istime = 0;
    type = PT_VALUE;
    if ((p=strstr(buf,"COUNT"))!=NULL) {
      p[-1] = '\0';
      p += 6;
    } else if ((p=strstr(buf,"TIME"))!=NULL) {
      p[-1] = '\0';
      p += 5;
      istime = 1;
    }
    if (istime)
      add_time_item(p, buf+2, buf[0] == 'l');
    else {
      if (glob.n_tsk < 2 || buf[0] == 'g')
        add_log_item(&glob_log_items, &n_glob_log_items, type, p, buf+2, NULL);
      else
        add_log_item(&loc_log_items, &n_loc_log_items, type, p, buf+2, NULL);
    }
    if (blk_read_line(buf)==-1)
      goto err;
  }
  return;
err:
  fprintf(stderr,"invalid file format\n");
  exit(1);
}

const static struct
{
  double value;
  int major;
  int minor;
} thresholds[] =
{
  {1.5, 9, 2},
  {2, 11, 1},
  {3, 7, 1},
  {5, 11, 1},
  {7, 8, 1},
  {8, 9, 1}
};

#define N_THRESHOLDS (sizeof(thresholds)/sizeof(thresholds[0]))

static double calc_max(double x)
{
  double l, d, f;
  int i;

  l = log10(x);
  d = floor(l);
  f = l - d;
  for (i = 0; i < N_THRESHOLDS && f >= log10(thresholds[i].value); i++);
  if (i < N_THRESHOLDS)
    return thresholds[i].value * pow(10, d);
  else
    return pow(10, d + 1);
}

static void calc_interval(double x, int *major, int *minor)
{
  double l, d, f;
  int i;

  l = log10(x);
  d = floor(l);
  f = l - d - 0.001;
  for (i = 0; i < N_THRESHOLDS && f > log10(thresholds[i].value); i++);
  if (i < N_THRESHOLDS)
  {
    *major = thresholds[i].major;
    *minor = thresholds[i].minor;
  }
  else
  {
    *major = 11;
    *minor = 1;
  }
}

void plot_add_point(XYPlotData * d, double x, double y)
{
  int nval;

  if ((d->size & ((-1) << GROWLOG)) == d->size)
  {
    size_t new_size = (d->size + (1 << GROWLOG)) * sizeof(double);

    d->x = urealloc(d->x, new_size);
    d->y = urealloc(d->y, new_size);
  }
  nval = d->size++;
  d->x[nval] = x;
  d->y[nval] = y;
}

gint log_plot_update(gpointer _data)
{
  GtkWidget *w;
  char buf[256];
  char *pattern;
  int len, i, j, retval, update_range = 0;
  double x, y;
  struct _plot_spec_diff *psd = NULL;
  char *locale = setlocale(LC_NUMERIC, "C");
  int n_plots = gtk_notebook_get_n_pages(GTK_NOTEBOOK(glob.notebook))-1;
  struct _log_plot_data *datas[n_plots], *data;
  int thread;
  struct _log_time_config_item *item;
  GtkListStore *store;
  GtkTreeIter iter;

  x = 0.0;
  for (i = 0; i < n_plots; i++)
  {
    w = gtk_notebook_get_nth_page(GTK_NOTEBOOK(glob.notebook), i);
    datas[i] = g_object_get_data(G_OBJECT(w), "log_plot_data");
  }

  while ((retval = read_line(buf)) == 1)
  {
    if ( *buf=='{' || *buf=='l' || *buf=='g' || *buf=='}'  ) {
      fprintf(stderr,"Warning : The log file contains 2 execution of a solver\n");
      fprintf(stderr,"This application does not handle a log file with several solvers executions\n");
      fprintf(stderr,"very properly !!!!\n");
      continue;
    }
    pattern = strchr(buf, ' ') + 1;
    if (isdigit(*pattern))
    {
      /* the algorithm is multithreaded or distributed */
      sscanf(pattern, "%d-", &thread);
      pattern = strchr(pattern, '-') + 1;
      printf("Thr :%d ",thread);
    }
    else
    {
      thread = -1;
      pattern += 2;
    }
    x = strtod(buf, NULL);
      printf("x :%f ",x);
    /* printf("%s\n", pattern); */
    for (j = 0; j < n_plots; j++)
    {
      data = datas[j];
      if (data->thread_no >= 0 && thread != data->thread_no)
        continue;
      for (i = 0; i < data->n_data; i++)
      {
        int found_value = 0, found_diff_1 = 0, found_diff_2 = 0;

        switch (data->items[i].type)
        {
        case PT_VALUE:
          /* printf("toto\n"); */
           len = strlen(data->items[i].pattern);
          if (strncmp(pattern, data->items[i].pattern, len) == 0)
            found_value = 1;
          break;
        case PT_DIFF:
          psd = &data->spec[i].diff;
          if (!strncmp(pattern, data->items[i].pattern, len =
                       strlen(data->items[i].pattern)))
            found_diff_1 = 1;
          else if (!strncmp(pattern, data->items[i].pattern2, len =
                            strlen(data->items[i].pattern2)))
            found_diff_2 = 1;
          break;
        }
        if (found_value || found_diff_1 || found_diff_2)
        {
          y = strtod(pattern + len + 1, NULL);
          printf("p :%s %f",data->items[i].pattern,y);
          if (found_diff_1)
            psd->n1 = y;
          else if (found_diff_2)
            psd->n2 = y;
          if (found_diff_1 || found_diff_2)
            y = psd->n1 - psd->n2;
          if (x > data->xmax)
          {
            data->xmax = calc_max(x);
            update_range = 1;
          }
          if (y > data->ymax)
          {
            data->ymax = calc_max(y);
            update_range = 1;
          }
          plot_add_point(data->xyplotdata + i, x, y);
        }
      }
    }

    /* Times handling */
    for (item=time_items; item<time_items+n_time_items; item++)
    {
      len = strlen(item->pattern);
      if (item->tsk == thread && strncmp(pattern, item->pattern, len) == 0)
      {
        y = strtod(pattern + len + 3, NULL);
        if (pattern[len+1] == 's') {
          item->num = (int)y;
          item->temp_time = x;
        } else if (item->num == (int)y)
          item->total_time += x - item->temp_time;
        printf( "%s %d %d\n", pattern, item->tsk, thread );
      }
      else if (strncmp(pattern, item->pattern, len) == 0)
        printf( "%s %d %d\n", pattern, item->tsk, thread );
    }
  }
  setlocale(LC_NUMERIC, locale);
  /* giw_xyplot_set_autorange( GIW_XYPLOT(data->xyplot),NULL); */
  if (update_range)
  {
    int xmajor, xminor, ymajor, yminor;

    for (i = 0; i < n_plots; i++)
    {
      data = datas[i];
      giw_xyplot_set_range(GIW_XYPLOT(data->xyplot),
                           data->xmin, data->xmax, data->ymin, data->ymax);
      calc_interval(data->xmax, &xmajor, &xminor);
      calc_interval(data->ymax, &ymajor, &yminor);
      giw_xyplot_set_ticks(GIW_XYPLOT(data->xyplot), xmajor, xminor, ymajor,
                           yminor);
    }
  }
  for (i = 0; i < n_plots; i++)
    gtk_widget_queue_draw(datas[i]->xyplot);

  w = gtk_notebook_get_nth_page(GTK_NOTEBOOK(glob.notebook), n_plots);
  store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(w)));
  gtk_list_store_clear(store);
  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter,
      TASK_COLUMN, -1,
      TITLE_COLUMN, "Total time",
      VALUE_COLUMN, x,
      PERC_COLUMN, 100.0,
      -1 );

  for (i = 0; i < n_time_items; i++) {
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
        TASK_COLUMN, time_items[i].tsk,
        TITLE_COLUMN, time_items[i].title,
        VALUE_COLUMN, time_items[i].total_time,
        PERC_COLUMN, time_items[i].total_time / x * 100,
        -1 );
  }
  if (retval == -1)
    return FALSE;
  return TRUE;
}

void curve_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  GtkWidget *xyplot;
  gboolean status;

  /* printf("curve_toggled\n"); */
  status = gtk_toggle_button_get_active(togglebutton);
  xyplot = g_object_get_data(G_OBJECT(togglebutton), "xyplot");
  if (status)
    giw_xyplot_add_data(GIW_XYPLOT(xyplot), user_data);
  else
    giw_xyplot_remove_data(GIW_XYPLOT(xyplot), user_data);
  gtk_widget_queue_draw(xyplot);
}

GtkWidget *gen_log_plot(int nb, struct _log_plot_config_item *items,
                        int thread)
{
  struct _log_plot_data *data = umalloc(sizeof(struct _log_plot_data));
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
  data->xyplotdata = umalloc(nb * sizeof(XYPlotData));
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

void delete_log_plot(GtkWidget * plot)
{
  /* struct _log_plot_data *data = g_object_get_data(G_OBJECT(plot),
                                                  "log_plot_data"); */

  /* TODO : détruire toutes les infos du data */
  /* g_object_unref(G_OBJECT(plot)); */
}

void quit()
{
  int i;
  GtkWidget *w;
  int n_plots = gtk_notebook_get_n_pages(GTK_NOTEBOOK(glob.notebook))-1;

  for (i=0; i<n_plots; i++) {
    w = gtk_notebook_get_nth_page(GTK_NOTEBOOK(glob.notebook), i);
    delete_log_plot(w);
  }
  close(glob.in_fd);
  if( loc_log_items )
    ufree( loc_log_items );
  if( glob_log_items )
    ufree( glob_log_items );
  if( time_items )
    ufree( time_items );

  gtk_main_quit();
}

void usage(const char *cmd)
{
  fprintf(stderr, "usage: %s [switches] LOGFILE\n"
          "Display logfile data. LOGFILE may be '-' (standard input)\n"
          "Switches are :\n"
          "  -t NUMBER : Number of tasks (default=0)\n", cmd);
  exit(1);
}

int main(int argc, char *argv[])
{
  GtkWidget *window1;
  GtkWidget *plot;
  int i;
  char **a;
  char *arg_filename = NULL;
  char buf[32];

  gtk_init(&argc, &argv);

  glob.n_tsk = 1;
  a = argv + 1;
  while (*a)
  {
    if (!strcmp(*a, "-t"))
    {
      if (!*++a)
        usage(argv[0]);
      glob.n_tsk = atoi(*a);
    }
    else if (arg_filename)
      usage(argv[0]);
    else
      arg_filename = *a;
    a++;
  }
  if (arg_filename == NULL)
    usage(argv[0]);

  if (!strcmp(arg_filename, "-"))
    glob.in_fd = 0;
  else
  {
    glob.in_fd = open(arg_filename, O_RDONLY);
    if (glob.in_fd < 0)
      perror(arg_filename), exit(1);
  }
  glob.remain = 0;

  init_log_items();

  window1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window1), "Bob++ log viewer");
  gtk_container_border_width(GTK_CONTAINER(window1), 0);
  gtk_signal_connect(GTK_OBJECT(window1), "destroy", GTK_SIGNAL_FUNC(quit),
                     NULL);

  glob.notebook = gtk_notebook_new();
  plot = gen_log_plot(n_glob_log_items, glob_log_items, -1);
  gtk_notebook_append_page(GTK_NOTEBOOK(glob.notebook),
                           plot, gtk_label_new("Global view"));
  if (n_glob_log_items > 0)
    gtk_widget_show(plot);
  if (glob.n_tsk >= 2)
  {
    for (i = 0; i < glob.n_tsk; i++)
    {
      plot = gen_log_plot(n_loc_log_items, loc_log_items, i);
      sprintf(buf, "Task #%d", i);
      gtk_notebook_append_page(GTK_NOTEBOOK(glob.notebook),
                               plot, gtk_label_new(buf));
      gtk_widget_show(plot);
    }
  }

  if (n_time_items) {
    const static char * hd[] = { "Task", "Title", "Value", "%" };
    GtkListStore *store;
    GtkWidget *list;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
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

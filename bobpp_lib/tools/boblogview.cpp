
#include"boblog.hpp"

/*void quit()
{
  //gtkinter::quit();
}

void *umalloc(size_t size)
{
  void *p = malloc(size);

  if (p == NULL)
    perror("umalloc"), abort();
  return p;
}

void *urealloc(void *ptr, size_t size)
{
  ptr = realloc(ptr, size);
  if (ptr == NULL)
    perror("urealloc"), abort();
  return ptr;
}

void ufree(void *ptr)
{
  free(ptr);
}

void curve_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  GtkWidget *xyplot;
  gboolean status;

   printf("curve_toggled\n"); 
  status = gtk_toggle_button_get_active(togglebutton);
  xyplot = g_object_get_data(G_OBJECT(togglebutton), "xyplot");
  if (status)
    giw_xyplot_add_data(GIW_XYPLOT(xyplot), user_data);
  else
    giw_xyplot_remove_data(GIW_XYPLOT(xyplot), user_data);
  gtk_widget_queue_draw(xyplot);
}

*/

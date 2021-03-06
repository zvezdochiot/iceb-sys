/*$Id: zar_sprav_r.c,v 1.17 2013/09/26 09:47:00 sasa Exp $*/
/*17.05.2018	24.11.2006	Белых А.И.	zar_sprav_r.c
Расчёт справок о зарплате
*/
#include <errno.h>
#include "buhg_g.h"
#include "zar_sprav.h"

class zar_sprav_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zar_sprav_rek *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_sprav_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_sprav_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_sprav_r_data *data);
gint zar_sprav_r1(class zar_sprav_r_data *data);
void  zar_sprav_r_v_knopka(GtkWidget *widget,class zar_sprav_r_data *data);

void zarsprprw(class zar_sprav_rek *rek,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *bar,GtkWidget *wpredok);
void zarsrbolw(class zar_sprav_rek *data,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *bar,GtkWidget *wpredok);
void zarsprvmnw(class zar_sprav_rek *data,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *bar,GtkWidget *wpredok);
void sprtabotpw(class zar_sprav_rek *data,GtkWidget *bar,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);
void zarsdrvw(class zar_sprav_rek *data,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *bar,GtkWidget *wpredok);

extern SQL_baza bd;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_sprav_r(class zar_sprav_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_sprav_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать свод начислений и удержаний по категориям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_sprav_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Распечатать свод начислений и удержаний по категориям"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,wpredok));
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_sprav_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);

gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_sprav_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_sprav_r_v_knopka(GtkWidget *widget,class zar_sprav_r_data *data)
{
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_sprav_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_sprav_r_data *data)
{
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}

/******************************************/
/******************************************/

gint zar_sprav_r1(class zar_sprav_r_data *data)
{
time_t vremn;
time(&vremn);
class iceb_clock sss(data->window);
data->rk->oth.clear();

if(data->rk->metka_vr == 1)
 zarsprprw(data->rk,data->view,data->buffer,data->bar,data->window);

if(data->rk->metka_vr == 2)
  zarsrbolw(data->rk,data->view,data->buffer,data->bar,data->window);

if(data->rk->metka_vr == 3)
 zarsdrvw(data->rk,data->view,data->buffer,data->bar,data->window);

if(data->rk->metka_vr == 4)
 zarsprvmnw(data->rk,data->view,data->buffer,data->bar,data->window);




data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}

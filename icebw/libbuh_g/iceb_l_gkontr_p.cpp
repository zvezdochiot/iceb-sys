/*$Id: iceb_l_gkontr_p.c,v 1.19 2013/08/25 08:27:05 sasa Exp $*/
/*28.02.2016	31.12.2003	Белых А.И.	iceb_l_gkontr_p.c
Меню для ввода реквизитов поиска 
*/
#include "iceb_libbuh.h"
#include "iceb_l_gkontr.h"
enum
 {
  E_KOD,
  E_NAIM,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class iceb_l_gkontr_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class gkontr_rek *rk;
  
  iceb_l_gkontr_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      g_signal_emit_by_name(entry[i],"activate");
     }
   }
 };


void gkontr_p_clear(class iceb_l_gkontr_p_data *data);
void    gkontr_p_vvod(GtkWidget *widget,class iceb_l_gkontr_p_data *data);
void  gkontr_p_knopka(GtkWidget *widget,class iceb_l_gkontr_p_data *data);
gboolean   gkontr_p_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_gkontr_p_data *data);

int iceb_l_gkontr_p(class gkontr_rek *rek_poi,GtkWidget *wpredok)
{
class iceb_l_gkontr_p_data data;
char strsql[512];
int gor=0,ver=0;

data.rk=rek_poi;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(gkontr_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }



GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Код"));
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),10);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(gkontr_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk->kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),80);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(gkontr_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk->naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));




sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[PFK2],"clicked",G_CALLBACK(gkontr_p_knopka),&data);
gtk_widget_set_name(data.knopka[PFK2],iceb_u_inttochar(PFK2));

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[PFK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[PFK4],"clicked",G_CALLBACK(gkontr_p_knopka),&data);
gtk_widget_set_name(data.knopka[PFK4],iceb_u_inttochar(PFK4));


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka[PFK10],"clicked",G_CALLBACK(gkontr_p_knopka),&data);
gtk_widget_set_name(data.knopka[PFK10],iceb_u_inttochar(PFK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
return(data.voz);
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   gkontr_p_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_gkontr_p_data *data)
{
//char  bros[512];

//printf("vgkontr_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[PFK2],"clicked");
    return(FALSE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[PFK4],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[PFK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  gkontr_p_knopka(GtkWidget *widget,class iceb_l_gkontr_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case PFK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);

    return;  

  case PFK4:
    gkontr_p_clear(data);
    return;  


  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    gkontr_p_vvod(GtkWidget *widget,class iceb_l_gkontr_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KOD:
    data->rk->kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NAIM:
    data->rk->naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }
enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Очистка меню от введенной информации*/
/**************************************/
void gkontr_p_clear(class iceb_l_gkontr_p_data *data)
{

data->rk->clear_zero();
for(int i=0; i< KOLENTER; i++)
 gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");

gtk_widget_grab_focus(data->entry[0]);

}

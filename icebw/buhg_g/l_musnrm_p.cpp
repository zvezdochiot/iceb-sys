/*$Id:$*/
/*09.03.2017	22.06.2006	Белых А.И.	l_musnrm_p.cpp
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "l_musnrm.h"
enum
 {
  E_KODD,
  E_NAIMD,
  E_KODM,
  E_NAIMM,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_musnrm_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_musnrm_rek *rk;
  
  
  l_musnrm_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->kodd.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODD])));
    rk->naimd.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIMD])));
    rk->kodm.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODM])));
    rk->naimm.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIMM])));
   }

  void clear_data()
   {
    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void l_musnrm_p_clear(class l_musnrm_p_data *data);
void    l_musnrm_p_vvod(GtkWidget *widget,class l_musnrm_p_data *data);
void  l_musnrm_p_knopka(GtkWidget *widget,class l_musnrm_p_data *data);
gboolean   l_musnrm_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_musnrm_p_data *data);
void  l_musnrm_p_e_knopka(GtkWidget *widget,class l_musnrm_p_data *data);


int l_musnrm_p(class l_musnrm_rek *rek_poi,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class l_musnrm_p_data data;
char strsql[512];
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_musnrm_p_key_press),&data);

if(wpredok != NULL)
 {

  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);



GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
 gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 0);


sprintf(strsql,"%s (,,)",gettext("Код детали"));
data.knopka_enter[E_KODD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODD]), data.knopka_enter[E_KODD], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KODD],"clicked",G_CALLBACK(l_musnrm_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODD],iceb_u_inttochar(E_KODD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODD],gettext("Выбор детали"));

data.entry[E_KODD] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KODD]), data.entry[E_KODD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODD], "activate",G_CALLBACK(l_musnrm_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODD]),data.rk->kodd.ravno());
gtk_widget_set_name(data.entry[E_KODD],iceb_u_inttochar(E_KODD));/******************/

sprintf(strsql,"%s (,,)",gettext("Наименование детали"));
GtkWidget *label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIMD]), label, FALSE, FALSE, 0);

data.entry[E_NAIMD] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIMD]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIMD]), data.entry[E_NAIMD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIMD], "activate",G_CALLBACK(l_musnrm_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMD]),data.rk->naimd.ravno());
gtk_widget_set_name(data.entry[E_NAIMD],iceb_u_inttochar(E_NAIMD));/******************/



sprintf(strsql,"%s (,,)",gettext("Код материала"));
data.knopka_enter[E_KODM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODM]), data.knopka_enter[E_KODM], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KODM],"clicked",G_CALLBACK(l_musnrm_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODM],iceb_u_inttochar(E_KODM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODM],gettext("Выбор материала"));

data.entry[E_KODM] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODM]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KODM]), data.entry[E_KODM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODM], "activate",G_CALLBACK(l_musnrm_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODM]),data.rk->kodm.ravno());
gtk_widget_set_name(data.entry[E_KODM],iceb_u_inttochar(E_KODM));/******************/



sprintf(strsql,"%s (,,)",gettext("Наименование материала"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIMM]), label, FALSE, FALSE, 0);

data.entry[E_NAIMM] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIMM]),80);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIMM]), data.entry[E_NAIMM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIMM], "activate",G_CALLBACK(l_musnrm_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMM]),data.rk->naimm.ravno());
gtk_widget_set_name(data.entry[E_NAIMM],iceb_u_inttochar(E_NAIMM));/******************/






sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_musnrm_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_musnrm_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_musnrm_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_musnrm_p_e_knopka(GtkWidget *widget,class l_musnrm_p_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {

  case E_KODD:

    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->rk->kodd.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODD]),data->rk->kodd.ravno());
     }

    return;  

  case E_KODM:

    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->rk->kodm.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODM]),data->rk->kodm.ravno());
     }

    return;  



 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_musnrm_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_musnrm_p_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(FALSE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
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
void  l_musnrm_p_knopka(GtkWidget *widget,class l_musnrm_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    data->rk->metka_poi=1;
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    data->rk->metka_poi=0;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_musnrm_p_vvod(GtkWidget *widget,class l_musnrm_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
switch (enter)
 {
  case E_KODD:
    data->rk->kodd.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_KODD])));
    break;
  case E_NAIMD:
    data->rk->naimd.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NAIMD])));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}

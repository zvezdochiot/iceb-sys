/*$Id: rnn_imp_m.c,v 1.12 2013/09/26 09:46:54 sasa Exp $*/
/*26.12.2012	06.05.2008	Белых А.И.	rnn_imp_m.c
Меню для ввода реквизитов
*/

#include "buhg_g.h"
#include "rnn_imp.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAN,
  E_DATAK,
  E_INN,
  E_NNN,
  KOLENTER
 };

class rnn_imp_m_data
 {
  public:
  class rnn_imp_data *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_naim_kontr;

  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  rnn_imp_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rk->inn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_INN])));
    rk->nnn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NNN])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rk->clear();
   }
 };


gboolean   rnn_imp_v_key_press(GtkWidget *widget,GdkEventKey *event,class rnn_imp_m_data *data);
void    rnn_imp_v_vvod(GtkWidget *widget,class rnn_imp_m_data *data);
void  rnn_imp_v_knopka(GtkWidget *widget,class rnn_imp_m_data *data);
void  rnn_imp_v_e_knopka(GtkWidget *widget,class rnn_imp_m_data *data);

int rnn_imp_m_provr(class rnn_imp_m_data *data);

extern SQL_baza bd;

int rnn_imp_m(class rnn_imp_data *rek_ras,GtkWidget *wpredok)
{
char strsql[512];


class rnn_imp_m_data data;

data.rk=rek_ras;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт налоговых накладных"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rnn_imp_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label=NULL;
label=gtk_label_new(gettext("Импорт налоговых накладных"));
data.label_naim_kontr=gtk_label_new("");

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (vbox), data.label_naim_kontr, FALSE, FALSE,1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE,1);

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE,1);


sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(rnn_imp_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(rnn_imp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));


sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(rnn_imp_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(rnn_imp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));

sprintf(strsql,"%s",gettext("Индивидуальный налоговый номер"));
data.knopka_enter[E_INN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.knopka_enter[E_INN], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_INN],"clicked",G_CALLBACK(rnn_imp_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_INN],iceb_u_inttochar(E_INN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_INN],gettext("Выбор даты конца отчёта"));

data.entry[E_INN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_INN]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.entry[E_INN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_INN], "activate",G_CALLBACK(rnn_imp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INN]),data.rk->inn.ravno());
gtk_widget_set_name(data.entry[E_INN],iceb_u_inttochar(E_INN));


label=gtk_label_new(gettext("Номер налоговой накладной"));
gtk_box_pack_start (GTK_BOX (hbox[E_NNN]), label, FALSE, FALSE, 0);

data.entry[E_NNN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NNN]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NNN]), data.entry[E_NNN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NNN], "activate",G_CALLBACK(rnn_imp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NNN]),data.rk->nnn.ravno());
gtk_widget_set_name(data.entry[E_NNN],iceb_u_inttochar(E_NNN));



sprintf(strsql,"F2 %s",gettext("Импорт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать импорт документов"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(rnn_imp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(rnn_imp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(rnn_imp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

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
void  rnn_imp_v_e_knopka(GtkWidget *widget,class rnn_imp_m_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int knop=atoi(gtk_widget_get_name(widget));
/*g_print("rnn_imp_v_e_knopka knop=%d\n",knop);*/

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rk->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rk->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
      
    return;  

  case E_INN:
    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
     {
      sprintf(strsql,"select innn from Kontragent where kodkon='%s'",kod.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {      
        gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),naim.ravno());
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_INN]),row[0]);
       }
     }    
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rnn_imp_v_key_press(GtkWidget *widget,GdkEventKey *event,class rnn_imp_m_data *data)
{

//printf("rnn_imp_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);


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
void  rnn_imp_v_knopka(GtkWidget *widget,class rnn_imp_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(rnn_imp_m_provr(data) != 0)
     return;
    
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    rnn_imp_v_vvod(GtkWidget *widget,class rnn_imp_m_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("rnn_imp_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************************/
/*Проверка реквизитов*/
/*******************************/
int rnn_imp_m_provr(class rnn_imp_m_data *data)
{

if(iceb_rsdatp(data->rk->datan.ravno(),data->rk->datak.ravno(),data->window) != 0)
 return(1);

return(0);
}



/*$Id: xrnn_rasp_m.c,v 1.18 2014/02/28 05:21:01 sasa Exp $*/
/*17.05.2013	05.05.2008	Белых А.И.	xrnn_rasp_m.c
Меню для ввода реквизитов
*/

#include "buhg_g.h"
#include "xrnn_poiw.h"
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
  E_NOMPZ1,
  E_NOMPZ2,
  E_KOD_GR1,
  E_KOD_GR2,
  E_PROC_DOT,
  KOLENTER
 };

class xrnn_rasp_m_data
 {
  public:
  class xrnn_poi *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *radio1[3];
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  xrnn_rasp_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rk->kodgr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_GR1])));
    rk->kodgr1.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_GR2])));
    rk->proc_dot.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PROC_DOT])));
  
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio1[0])) == TRUE)
     rk->pr_ras=0;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio1[1])) == TRUE)
     rk->pr_ras=1;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio1[2])) == TRUE)
     rk->pr_ras=2;

   }

  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rk->clear_data();
   }
 };


gboolean   xrnn_rasp_v_key_press(GtkWidget *widget,GdkEventKey *event,class xrnn_rasp_m_data *data);
void    xrnn_rasp_v_vvod(GtkWidget *widget,class xrnn_rasp_m_data *data);
void  xrnn_rasp_v_knopka(GtkWidget *widget,class xrnn_rasp_m_data *data);
void  xrnn_rasp_v_e_knopka(GtkWidget *widget,class xrnn_rasp_m_data *data);

int xrnn_rasp_m_provr(class xrnn_rasp_m_data *data);

extern SQL_baza bd;

int xrnn_rasp_m(class xrnn_poi *rek_ras)
{
char strsql[512];


class xrnn_rasp_m_data data;

data.rk=rek_ras;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать реестр налоговых накладных"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(xrnn_rasp_v_key_press),&data);

GtkWidget *label=NULL;
label=gtk_label_new(gettext("Распечатать реестр налоговых накладных"));

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

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE,1);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator3=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_box_pack_start (GTK_BOX (vbox), separator1, TRUE, TRUE, 1);

data.radio1[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Все документы"));
data.radio1[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radio1[0]),gettext("Только полученные"));
data.radio1[2]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radio1[1]),gettext("Только выданные"));

gtk_box_pack_start (GTK_BOX (vbox), data.radio1[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radio1[1], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radio1[2], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radio1[data.rk->metka_poi]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox), separator3, TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE,1);


sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(xrnn_rasp_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(xrnn_rasp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));


sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(xrnn_rasp_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(xrnn_rasp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));

sprintf(strsql,"%s",gettext("Процент для реестра перерабатывающего производства"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC_DOT]), label, FALSE, FALSE,1);

data.entry[E_PROC_DOT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PROC_DOT]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC_DOT]), data.entry[E_PROC_DOT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_PROC_DOT], "activate",G_CALLBACK(xrnn_rasp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROC_DOT]),data.rk->proc_dot.ravno());
gtk_widget_set_name(data.entry[E_PROC_DOT],iceb_u_inttochar(E_PROC_DOT));



sprintf(strsql,"%s (,,)",gettext("Код группы полученных документов"));
data.knopka_enter[E_KOD_GR1]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR1]), data.knopka_enter[E_KOD_GR1], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_GR1],"clicked",G_CALLBACK(xrnn_rasp_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_GR1],iceb_u_inttochar(E_KOD_GR1));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_GR1],gettext("Выбор группы"));

data.entry[E_KOD_GR1] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR1]), data.entry[E_KOD_GR1], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_GR1], "activate",G_CALLBACK(xrnn_rasp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_GR1]),data.rk->kodgr.ravno());
gtk_widget_set_name(data.entry[E_KOD_GR1],iceb_u_inttochar(E_KOD_GR1));


sprintf(strsql,"%s (,,)",gettext("Код группы выданных документов"));
data.knopka_enter[E_KOD_GR2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR2]), data.knopka_enter[E_KOD_GR2], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_GR2],"clicked",G_CALLBACK(xrnn_rasp_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_GR2],iceb_u_inttochar(E_KOD_GR2));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_GR2],gettext("Выбор группы"));

data.entry[E_KOD_GR2] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR2]), data.entry[E_KOD_GR2], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_GR2], "activate",G_CALLBACK(xrnn_rasp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_GR2]),data.rk->kodgr1.ravno());
gtk_widget_set_name(data.entry[E_KOD_GR2],iceb_u_inttochar(E_KOD_GR2));





sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(xrnn_rasp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(xrnn_rasp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(xrnn_rasp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  xrnn_rasp_v_e_knopka(GtkWidget *widget,class xrnn_rasp_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
/*g_print("xrnn_rasp_v_e_knopka knop=%d\n",knop);*/

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

  case E_KOD_GR1:

    if(l_xrnngpn(1,&kod,&naim,data->window) == 0)
     data->rk->kodgr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_GR1]),data->rk->kodgr.ravno());
      
    return;  

  case E_KOD_GR2:

    if(l_xrnngvn(1,&kod,&naim,data->window) == 0)
     data->rk->kodgr1.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_GR2]),data->rk->kodgr1.ravno());
      
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   xrnn_rasp_v_key_press(GtkWidget *widget,GdkEventKey *event,class xrnn_rasp_m_data *data)
{

//printf("xrnn_rasp_v_key_press\n");
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
void  xrnn_rasp_v_knopka(GtkWidget *widget,class xrnn_rasp_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(xrnn_rasp_m_provr(data) != 0)
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

void    xrnn_rasp_v_vvod(GtkWidget *widget,class xrnn_rasp_m_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("xrnn_rasp_v_vvod enter=%d\n",enter);

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
int xrnn_rasp_m_provr(class xrnn_rasp_m_data *data)
{

if(iceb_rsdatp(data->rk->datan.ravno(),&data->rk->datak,data->window) != 0)
 return(1);

return(0);
}



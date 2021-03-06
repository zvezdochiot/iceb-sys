/*$Id: dooc_m.c,v 1.17 2013/09/26 09:46:47 sasa Exp $*/
/*10.07.2015	21.10.2004	Белых А.И.	dooc_m.c
Меню для задания дооценки
*/
#include "buhg_g.h"
#include "dooc.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAD,
  E_SUMADOOC,
  E_KODOP,
  E_OSNOV,
  E_KODMAT,
  KOLENTER  
 };

class dooc_m_data
 {
  public:
  dooc_data  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  dooc_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datad.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAD])));
    rk->sumadooc.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMADOOC])));

    rk->osnov.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_OSNOV])));

    rk->kodmat.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODMAT])));
    rk->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP])));
//    for(int i=0; i < KOLENTER; i++)
//      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rk->clear_data();
   }
 };



gboolean   dooc_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class dooc_m_data *data);
void    dooc_m_v_vvod(GtkWidget *widget,class dooc_m_data *data);
void  dooc_m_v_knopka(GtkWidget *widget,class dooc_m_data *data);
void   dooc_m_rekviz(class dooc_m_data *data);
void  dooc_m_e_knopka(GtkWidget *widget,class dooc_m_data *data);
int dooc_m_f2(class dooc_m_data *data);

extern SQL_baza bd;

int   dooc_m(class dooc_data *rek_ras,GtkWidget *wpredok)
{
dooc_m_data data;

data.rk=rek_ras;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Дооценить документ"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(dooc_m_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Дооценить документ"));

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

/****************
sprintf(strsql,"%s",gettext("Дата документов (д.м.г)"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), label, FALSE, FALSE, 0);
******************/
sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.knopka_enter[E_DATAD], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAD],"clicked",G_CALLBACK(dooc_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAD],iceb_u_inttochar(E_DATAD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAD],gettext("Выбор даты"));

data.entry[E_DATAD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.entry[E_DATAD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAD], "activate",G_CALLBACK(dooc_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAD]),data.rk->datad.ravno());
gtk_widget_set_name(data.entry[E_DATAD],iceb_u_inttochar(E_DATAD));


label=gtk_label_new(gettext("Сумма дооценки"));
data.entry[E_SUMADOOC] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMADOOC]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMADOOC]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMADOOC]), data.entry[E_SUMADOOC], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMADOOC], "activate",G_CALLBACK(dooc_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMADOOC]),data.rk->sumadooc.ravno());
gtk_widget_set_name(data.entry[E_SUMADOOC],iceb_u_inttochar(E_SUMADOOC));

sprintf(strsql,"%s",gettext("Основание"));
label=gtk_label_new(strsql);
data.entry[E_OSNOV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_OSNOV]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), data.entry[E_OSNOV], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_OSNOV], "activate",G_CALLBACK(dooc_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OSNOV]),data.rk->osnov.ravno());
gtk_widget_set_name(data.entry[E_OSNOV],iceb_u_inttochar(E_OSNOV));

sprintf(strsql,"%s (,,)",gettext("Код операции дооценки"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KODOP],"clicked",G_CALLBACK(dooc_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODOP],iceb_u_inttochar(E_KODOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODOP],gettext("Выбор операции"));


data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(dooc_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));


sprintf(strsql,"%s (,,)",gettext("Код материалла"));
data.knopka_enter[E_KODMAT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.knopka_enter[E_KODMAT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KODMAT],"clicked",G_CALLBACK(dooc_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODMAT],iceb_u_inttochar(E_KODMAT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODMAT],gettext("Выбор материалла"));


data.entry[E_KODMAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.entry[E_KODMAT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODMAT], "activate",G_CALLBACK(dooc_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODMAT]),data.rk->kodmat.ravno());
gtk_widget_set_name(data.entry[E_KODMAT],iceb_u_inttochar(E_KODMAT));




sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(dooc_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(dooc_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(dooc_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
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
void  dooc_m_e_knopka(GtkWidget *widget,class dooc_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATAD:

    if(iceb_calendar(&data->rk->datad,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->rk->datad.ravno());
      
    return;  


  case E_KODMAT:

    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->rk->kodmat.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),data->rk->kodmat.ravno());
     }

    return;  

  case E_KODOP:

    if(iceb_l_opmup(1,&kod,&naim,data->window) == 0)
     {
      data->rk->kodop.plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk->kodop.ravno());
     }

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dooc_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class dooc_m_data *data)
{

//printf("dooc_m_v_key_press\n");
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
void  dooc_m_v_knopka(GtkWidget *widget,class dooc_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    if(dooc_m_f2(data) != 0)
     return;

    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    dooc_m_v_vvod(GtkWidget *widget,class dooc_m_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("dooc_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAD:
    data->rk->datad.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SUMADOOC:
    data->rk->sumadooc.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_OSNOV:
    data->rk->osnov.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODMAT:
    data->rk->kodmat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************************/
/*Чтение реквизитов меню и их проверка*/
/*****************************************/
int dooc_m_f2(class dooc_m_data *data)
{
data->read_rek(); //Читаем реквизиты меню
if(data->rk->datad.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не верно введена дата !"),data->window);
  return(1);       
 }

if(data->rk->sumadooc.ravno_atof() <= 0.)
 {
  iceb_menu_soob(gettext("Не верно введена сумма дооценки !"),data->window);
  return(1);
 }

if(data->rk->kodop.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели код операции!"),data->window);
  return(1);
 }

SQL_str row;
class SQLCURSOR cur;

char strsql[1024];
sprintf(strsql,"select vido from Prihod where kod='%s'",data->rk->kodop.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("В справочнике приходных операций не найден код операции"),data->rk->kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(atof(row[0]) != 2)
 {
  iceb_menu_soob(gettext("Вид приходной операции должен быть \"Смена стоимотси\""),data->window);
  return(1);
 }

sprintf(strsql,"select vido from Rashod where kod='%s'",data->rk->kodop.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("В справочнике расходных операций не найден код операции"),data->rk->kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(atof(row[0]) != 2)
 {
  iceb_menu_soob(gettext("Вид расходной операции должен быть \"Смена стоимости\""),data->window);
  return(1);
 }

return(0);
}

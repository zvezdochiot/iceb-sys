/*$Id: l_taxikz_v.c,v 1.6 2013/08/13 06:10:24 sasa Exp $*/
/*30.05.2009	21.06.2006	Белых А.И.	l_taxikz_v.c
Ввод и корректировка категорий работников
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "taxi.h"
#include "l_taxikz.h"
enum
 {
  E_KOD,
  E_NAIM,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_taxikz_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_taxikz_rek rk;
  class iceb_u_str kod_gr;  
  
  l_taxikz_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    rk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM])));
   }
 };


gboolean   l_taxikz_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_taxikz_v_data *data);
void  l_taxikz_v_knopka(GtkWidget *widget,class l_taxikz_v_data *data);
void    l_taxikz_v_vvod(GtkWidget *widget,class l_taxikz_v_data *data);
int l_taxikz_pk(const char *god,GtkWidget*);
int l_taxikz_zap(class l_taxikz_v_data *data);
void l_taxikz_kkvt(const char *kods,const char *kodn,GtkWidget *wpredok);


extern SQL_baza  bd;
extern char      *name_system;

int l_taxikz_v(class iceb_u_str *kod_gr,GtkWidget *wpredok)
{

class l_taxikz_v_data data;
char strsql[300];
iceb_u_str kikz;
data.kod_gr.new_plus(kod_gr->ravno());

data.rk.clear_data();

if(data.kod_gr.getdlinna() >  1)
 {
  data.rk.kod.new_plus(data.kod_gr.ravno());
  sprintf(strsql,"select * from Taxikzz where kod=%s",data.kod_gr.ravno());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.rk.naim.new_plus(row[1]);
  kikz.plus(iceb_kikz(row[2],row[3],wpredok));
  
 }
else
 {
  sprintf(strsql,"%d",iceb_get_new_kod("Taxikzz",0,wpredok));
  data.rk.kod.new_plus(strsql);
 } 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.kod_gr.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",name_system,gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_taxikz_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Код"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);

data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(l_taxikz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Наименование"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);

data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),60);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_taxikz_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_taxikz_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_taxikz_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 kod_gr->new_plus(data.rk.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_taxikz_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_taxikz_v_data *data)
{
//char  bros[300];

//printf("l_taxikz_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");

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
void  l_taxikz_v_knopka(GtkWidget *widget,class l_taxikz_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_taxikz_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_taxikz_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_taxikz_v_vvod(GtkWidget *widget,class l_taxikz_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_taxikz_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->rk.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NAIM:
    data->rk.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_taxikz_zap(class l_taxikz_v_data *data)
{
char strsql[1000];
//SQL_str row;
time_t   vrem;
time(&vrem);


if(data->rk.kod.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введен код !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }

if(data->rk.naim.ravno()[0] == '\0')
 {
  iceb_u_str sp;
  sp.plus(gettext("Не введено наименование !"));
  iceb_menu_soob(&sp,data->window);
  return(1);
 }


if(iceb_u_SRAV(data->kod_gr.ravno(),data->rk.kod.ravno(),0) != 0)
  if(l_taxikz_pk(data->rk.kod.ravno(),data->window) != 0)
     return(1);
if(isdigit(data->rk.kod.ravno()[0]) == 0)
 {
  iceb_menu_soob(gettext("Код должен быть цифровым !"),data->window);
  return(1);
 }
short metkakor=0;

if(data->kod_gr.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Taxikzz \
values (%d,'%s',%d,%ld)",
   data->rk.kod.ravno_atoi(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem);
 }
else
 {
  
  if(iceb_u_SRAV(data->kod_gr.ravno(),data->rk.kod.ravno(),0) != 0)
   {

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Taxikzz \
set \
kod=%d,\
naik='%s',\
ktoi=%d,\
vrem=%ld \
where kod=%d",
   data->rk.kod.ravno_atoi(),
   data->rk.naim.ravno_filtr(),
   iceb_getuid(data->window),vrem,
   data->kod_gr.ravno_atoi());
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(metkakor == 1)
 l_taxikz_kkvt(data->kod_gr.ravno(),data->rk.kod.ravno(),data->window);

return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_taxikz_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[300];

sprintf(strsql,"select kod from Taxikzz where kod=%s",kod);
//printf("l_taxikz_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такая запись уже есть !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void l_taxikz_kkvt(const char *skod,const char *nkod,GtkWidget *wpredok)
{

char strsql[300];

sprintf(strsql,"update Kartb set kateg=%s where kateg=%s",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_u_str SOOB;
   SOOB.plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&SOOB,wpredok);
   return;
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
 }
sprintf(strsql,"update Zarn set kateg=%s where kateg=%s",nkod,skod);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_u_str SOOB;
   SOOB.plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&SOOB,wpredok);
   return;
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
 }

}

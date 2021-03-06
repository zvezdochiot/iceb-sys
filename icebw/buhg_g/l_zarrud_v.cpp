/*$Id: l_zarrud_v.c,v 1.4 2013/08/25 08:26:46 sasa Exp $*/
/*03.03.2016	03.11.2011	Белых А.И.	l_zarrud_v.c
Ввод и корректировка в справочник регулярных удержаний с работника
*/
#include "buhg_g.h"

enum
 {
  E_TABNOM,
  E_KOD_UD,
  E_SUMA,
  E_DATAD,
  E_KOMENT,
  E_METKA,  /*отнимается при чтении*/
  KOLENTER
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_zarrud_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt1;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str tabnomz;  

  class iceb_u_str tabnomv;
  class iceb_u_str kod_ud;
  class iceb_u_str suma;  
  class iceb_u_str datad;
  class iceb_u_str koment;
  int metka_pr;
    
  l_zarrud_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    tabnomz.new_plus("");
    clear();
  
   }

  void read_rek()
   {
    tabnomv.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM])));
    kod_ud.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_UD])));
    suma.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA])));
    datad.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAD])));
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
    metka_pr=gtk_combo_box_get_active (GTK_COMBO_BOX(opt1));

   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER-1; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    clear();    
   }

  void clear()
   {
    tabnomv.new_plus("");
    kod_ud.new_plus("");
    suma.new_plus("");    
    suma.new_plus("");
    datad.new_plus("");
    koment.new_plus("");
    metka_pr=0;
   }
 };


gboolean   l_zarrud_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarrud_v_data *data);
void  l_zarrud_v_knopka(GtkWidget *widget,class l_zarrud_v_data *data);
void    l_zarrud_v_vvod(GtkWidget *widget,class l_zarrud_v_data *data);
int l_zarrud_zap(class l_zarrud_v_data *data);
void  l_zarrud_v_e_knopka(GtkWidget *widget,class l_zarrud_v_data *data);


extern SQL_baza  bd;

int l_zarrud_v(class iceb_u_str *tabnom,GtkWidget *wpredok)
{

class l_zarrud_v_data data;
char strsql[512];
class iceb_u_str kikz;
int gor=0,ver=0;
data.name_window.plus(__FUNCTION__);
data.tabnomz.new_plus(tabnom->ravno());
data.tabnomv.new_plus(tabnom->ravno());

if(tabnom->getdlinna() >  1)
 {
  sprintf(strsql,"select * from Zarrud where tn=%d",tabnom->ravno_atoi());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  
  data.kod_ud.new_plus(row[1]);
  data.suma.new_plus(row[2]);

  data.metka_pr=atoi(row[3]);
  data.datad.new_plus(iceb_u_datzap_mg(row[4]));
  data.koment.new_plus(row[5]);
       
  kikz.plus(iceb_kikz(row[6],row[7],wpredok));
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(data.tabnomv.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zarrud_v_key_press),&data);

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
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(gettext("Табельный номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_TABNOM],"clicked",G_CALLBACK(l_zarrud_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_TABNOM],iceb_u_inttochar(E_TABNOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"));

data.entry[E_TABNOM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_TABNOM]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABNOM], "activate",G_CALLBACK(l_zarrud_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.tabnomv.ravno());
gtk_widget_set_name(data.entry[E_TABNOM],iceb_u_inttochar(E_TABNOM));

//label=gtk_label_new(gettext("Код удержания"));
//gtk_box_pack_start (GTK_BOX (hbox[E_KOD_UD]), label, FALSE, FALSE, 0);
data.knopka_enter[E_KOD_UD]=gtk_button_new_with_label(gettext("Код удержания"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_UD]), data.knopka_enter[E_KOD_UD], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_UD],"clicked",G_CALLBACK(l_zarrud_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_UD],iceb_u_inttochar(E_KOD_UD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_UD],gettext("Выбор кода удержания"));

data.entry[E_KOD_UD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_UD]),8);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_UD]), data.entry[E_KOD_UD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_UD], "activate",G_CALLBACK(l_zarrud_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_UD]),data.kod_ud.ravno());
gtk_widget_set_name(data.entry[E_KOD_UD],iceb_u_inttochar(E_KOD_UD));

label=gtk_label_new(gettext("Значение"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);

data.entry[E_SUMA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA], "activate",G_CALLBACK(l_zarrud_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno());
gtk_widget_set_name(data.entry[E_SUMA],iceb_u_inttochar(E_SUMA));



label=gtk_label_new(gettext("Метка"));
gtk_box_pack_start (GTK_BOX (hbox[E_METKA]), label, FALSE, FALSE, 0);

class iceb_u_spisok menu_vib;

menu_vib.plus(gettext("Cумма"));
menu_vib.plus(gettext("Процент"));

iceb_pm_vibor(&menu_vib,&data.opt1,data.metka_pr);
gtk_box_pack_start (GTK_BOX (hbox[E_METKA]), data.opt1, FALSE, FALSE, 0);

label=gtk_label_new(gettext("По какую дату действует (м.г)"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), label, FALSE, FALSE, 0);

data.entry[E_DATAD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.entry[E_DATAD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAD], "activate",G_CALLBACK(l_zarrud_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAD]),data.datad.ravno());
gtk_widget_set_name(data.entry[E_DATAD],iceb_u_inttochar(E_DATAD));

label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),255);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_zarrud_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_zarrud_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_zarrud_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_zarrud_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 tabnom->new_plus(data.tabnomv.ravno());
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_zarrud_v_e_knopka(GtkWidget *widget,class l_zarrud_v_data *data)
{
class iceb_u_str naim("");
class iceb_u_str kod("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->tabnomv.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->tabnomv.ravno());
    return;  

  case E_KOD_UD:
    if(l_zarud(1,&kod,&naim,data->window) == 0)
     data->kod_ud.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_UD]),data->kod_ud.ravno());
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarrud_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarrud_v_data *data)
{

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
  //  printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_zarrud_v_knopka(GtkWidget *widget,class l_zarrud_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    if(l_zarrud_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_zarrud_v_vvod(GtkWidget *widget,class l_zarrud_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_zarrud_zap(class l_zarrud_v_data *data)
{
char strsql[1024];
short d=0,m=0,g=0;

if(data->datad.ravno()[0] != '\0')
 {
  if(iceb_u_rsdat1(&m,&g,data->datad.ravno()) != 0)
   {
    iceb_menu_soob(gettext("Не правильно введена дата"),data->window);
    return(1);
   }
  else
   d=1;
 }
 
/*Проверяем код удержания*/
sprintf(strsql,"select kod from Uder where kod=%d",data->kod_ud.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден код удержания"),data->kod_ud.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

/*проверяем табельный номер*/
sprintf(strsql,"select tabn from Kartb where tabn=%d",data->tabnomv.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден табельный номер"),data->tabnomv.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

/*проверяем может уже такая запись есть*/
if(data->tabnomv.ravno_atoi() != data->tabnomz.ravno_atoi())
 {
  sprintf(strsql,"select tn from Zarrud where tn=%d",data->tabnomv.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) > 0)
   {
    iceb_menu_soob(gettext("Такая запись уже есть!"),data->window);
    return(1);
   }
 }

if(data->tabnomz.ravno_atoi() == 0) /*новая запись*/
 {
  sprintf(strsql,"insert into Zarrud values(%d,%d,%.2f,%d,'%04d-%02d-%02d','%s',%d,%ld)",
  data->tabnomv.ravno_atoi(),
  data->kod_ud.ravno_atoi(),
  data->suma.ravno_atof(),
  data->metka_pr,
  g,m,d,  
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),time(NULL));
  
 }
else /*корректировка записи*/
 {
  sprintf(strsql,"update Zarrud set \
tn=%d,\
ku=%d,\
sp=%.2f,\
mu=%d,\
dd='%04d-%02d-%02d',\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where tn=%d",
  data->tabnomv.ravno_atoi(),
  data->kod_ud.ravno_atoi(),
  data->suma.ravno_atof(),
  data->metka_pr,
  g,m,d,  
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),
  time(NULL),
  data->tabnomz.ravno_atoi());      
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);
return(0);

}

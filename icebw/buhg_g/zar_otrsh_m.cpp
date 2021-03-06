/*$Id: zar_otrsh_m.c,v 1.13 2013/08/25 08:27:00 sasa Exp $*/
/*13.07.2015	21.11.2006	Белых А.И.	zar_otrsh_m.c
Меню для расчёта свода отработанного времени по категориям
*/
#include "buhg_g.h"
#include "zar_otrsh.h"
enum
 {
  E_DATAN,
  E_DATAK,
  E_TABNOM,
  E_PODR,
  E_KOD_KAT,
  E_KOD_TAB,
  KOLENTER  
 };

enum
 {
  FK2,
  FK3,
  FK4,
  FK5,
  FK10,
  KOL_FK
 };

class zar_otrsh_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class zar_otrsh_rek *rk;
  
  zar_otrsh_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODR])));
    rk->kod_kat.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_KAT])));
    rk->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM])));
    rk->kod_tab.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_TAB])));
   }
  
  void clear_rek()
   {

    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);

   }
 };


gboolean   zar_otrsh_m_key_press(GtkWidget *widget,GdkEventKey *event,class zar_otrsh_m_data *data);
void  zar_otrsh_m_knopka(GtkWidget *widget,class zar_otrsh_m_data *data);
void    zar_otrsh_m_vvod(GtkWidget *widget,class zar_otrsh_m_data *data);


void  zar_otrsh_m_e_knopka(GtkWidget *widget,class zar_otrsh_m_data *data);

extern SQL_baza  bd;

int zar_otrsh_m(class zar_otrsh_rek *rek,GtkWidget *wpredok)
{

class zar_otrsh_m_data data;
char strsql[512];
iceb_u_str kikz;
data.rk=rek;
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать свод отработанного времени по категориям"));
label=gtk_label_new(gettext("Распечатать свод отработанного времени по категориям"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_otrsh_m_key_press),&data);

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
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);



gtk_container_add (GTK_CONTAINER (vbox), hboxknop);



//sprintf(strsql,"%s %s",gettext("Дата начала"),gettext("(м.г)"));
//label=gtk_label_new(strsql);
//gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), label, FALSE, FALSE, 0);

sprintf(strsql,"%s %s",gettext("Дата начала"),gettext("(м.г)"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(zar_otrsh_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),7);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(zar_otrsh_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));


//sprintf(strsql,"%s %s",gettext("Дата конца"),gettext("(м.г)"));
//label=gtk_label_new(strsql);
//gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), label, FALSE, FALSE, 0);
sprintf(strsql,"%s %s",gettext("Дата конца"),gettext("(м.г)"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(zar_otrsh_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),7);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(zar_otrsh_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));


sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_TABNOM],"clicked",G_CALLBACK(zar_otrsh_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_TABNOM],iceb_u_inttochar(E_TABNOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"));

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABNOM], "activate",G_CALLBACK(zar_otrsh_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.rk->tabnom.ravno());
gtk_widget_set_name(data.entry[E_TABNOM],iceb_u_inttochar(E_TABNOM));



sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(zar_otrsh_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор подразделения"));

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(zar_otrsh_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));



sprintf(strsql,"%s (,,)",gettext("Код категории"));
data.knopka_enter[E_KOD_KAT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KAT]), data.knopka_enter[E_KOD_KAT], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_KAT],"clicked",G_CALLBACK(zar_otrsh_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_KAT],iceb_u_inttochar(E_KOD_KAT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_KAT],gettext("Выбор категории"));

data.entry[E_KOD_KAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KAT]), data.entry[E_KOD_KAT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_KAT], "activate",G_CALLBACK(zar_otrsh_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_KAT]),data.rk->kod_kat.ravno());
gtk_widget_set_name(data.entry[E_KOD_KAT],iceb_u_inttochar(E_KOD_KAT));


sprintf(strsql,"%s (,,)",gettext("Код табеля"));
data.knopka_enter[E_KOD_TAB]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TAB]), data.knopka_enter[E_KOD_TAB], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_TAB],"clicked",G_CALLBACK(zar_otrsh_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_TAB],iceb_u_inttochar(E_KOD_TAB));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_TAB],gettext("Выбор счёта в плане счетов"));

data.entry[E_KOD_TAB] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TAB]), data.entry[E_KOD_TAB], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_TAB], "activate",G_CALLBACK(zar_otrsh_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_TAB]),data.rk->kod_tab.ravno());
gtk_widget_set_name(data.entry[E_KOD_TAB],iceb_u_inttochar(E_KOD_TAB));




sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(zar_otrsh_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Расчёт"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Начать расчёт с использованием файла настройки"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(zar_otrsh_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(zar_otrsh_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F5 %s",gettext("Настройка"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Настройка расчёта"));
g_signal_connect(data.knopka[FK5],"clicked",G_CALLBACK(zar_otrsh_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK5], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(zar_otrsh_m_knopka),&data);
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
void  zar_otrsh_m_e_knopka(GtkWidget *widget,class zar_otrsh_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATAN:
    iceb_calendar1(&data->rk->datan,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
    return;

  case E_DATAK:
    iceb_calendar1(&data->rk->datak,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
    return;

   
  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->rk->tabnom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->rk->tabnom.ravno());
    return;
  
  case E_PODR:
    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     data->rk->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
    return;
  

  case E_KOD_KAT:
    if(l_zarkateg(1,&kod,&naim,data->window) == 0)
     data->rk->kod_kat.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_KAT]),data->rk->kod_kat.ravno());
    return;

  case E_KOD_TAB:

    if(l_zarvidtab(1,&kod,&naim,data->window) == 0)
     data->rk->kod_tab.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_TAB]),data->rk->kod_tab.ravno());

    return;  
    
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_otrsh_m_key_press(GtkWidget *widget,GdkEventKey *event,class zar_otrsh_m_data *data)
{
//char  bros[512];

//printf("zar_otrsh_m_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");

    return(TRUE);


  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");

    return(TRUE);


  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");

    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");

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
void  zar_otrsh_m_knopka(GtkWidget *widget,class zar_otrsh_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("zar_otrsh_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:

    data->rk->metka_ras=0;

fk2:;
    data->read_rek();
    if(data->rk->datan.prov_dat1() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата начала !"),data->window);
      return;
     }

    if(data->rk->datak.prov_dat1() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата конца !"),data->window);
      return;
     }

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK3:
    data->rk->metka_ras=1;
    goto fk2;
    
  case FK4:
    data->clear_rek();
    return;

  case FK5:
    iceb_f_redfil("zarpodrk.alx",0,data->window);
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

void    zar_otrsh_m_vvod(GtkWidget *widget,class zar_otrsh_m_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("zar_otrsh_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_TABNOM:
    data->rk->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_PODR:
    data->rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOD_TAB:
    data->rk->kod_tab.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOD_KAT:
    data->rk->kod_kat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}

/*$Id: kasothw_m.c,v 1.20 2013/09/26 09:46:49 sasa Exp $*/
/*11.07.2015	10.03.2006	Белых А.И.	kasothw_m.c
Меню для получения отчёта 
*/
#include "buhg_g.h"
#include "kasothw.h"

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
  E_KODOP,
  E_KASSA,
  E_SHET,
  E_KOD_KONTR,
  KOLENTER  
 };

class kasothw_data
 {
  public:
  kasothw_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt1;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  kasothw_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rk->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP])));
    rk->kassa.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KASSA])));
    rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    rk->kod_kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_KONTR])));
    rk->metka_pr=gtk_combo_box_get_active (GTK_COMBO_BOX(opt1));
   }

  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }

 };




gboolean   kasothw_v_key_press(GtkWidget *widget,GdkEventKey *event,class kasothw_data *data);
void    kasothw_v_vvod(GtkWidget *widget,class kasothw_data *data);
void  kasothw_v_knopka(GtkWidget *widget,class kasothw_data *data);
void  kasothw_v_e_knopka(GtkWidget *widget,class kasothw_data *data);


extern SQL_baza bd;

int kasothw_m(class kasothw_rr *rek_ras,GtkWidget *wpredok)
{
kasothw_data data;

data.rk=rek_ras;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),data.rk->naim_oth.ravno());
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(kasothw_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

class iceb_u_str repl;
repl.plus(gettext("Учёт кассовых ордеров"));

repl.ps_plus(data.rk->naim_oth.ravno());
GtkWidget *label=gtk_label_new(repl.ravno());

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
GtkWidget *hboxopt1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxopt1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, FALSE, FALSE, 0);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], FALSE, FALSE, 0);

label=gtk_label_new(gettext("Документы"));
gtk_box_pack_start (GTK_BOX (hboxopt1), label, FALSE, FALSE, 1);

class iceb_u_spisok bal_st;
bal_st.plus(gettext("Все документы"));
bal_st.plus(gettext("Только приходные документы"));
bal_st.plus(gettext("Только расходные документы"));

iceb_pm_vibor(&bal_st,&data.opt1,data.rk->metka_pr);
gtk_box_pack_start (GTK_BOX (hboxopt1), data.opt1, TRUE, TRUE,1);


gtk_box_pack_start (GTK_BOX (vbox),hboxopt1, FALSE, FALSE, 0);

gtk_box_pack_start (GTK_BOX (vbox),hboxknop, FALSE, FALSE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(kasothw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(kasothw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(kasothw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(kasothw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));


sprintf(strsql,"%s (,,)",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODOP],"clicked",G_CALLBACK(kasothw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODOP],iceb_u_inttochar(E_KODOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODOP],gettext("Выбор операции"));

data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(kasothw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));


sprintf(strsql,"%s (,,)",gettext("Код кассы"));
data.knopka_enter[E_KASSA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.knopka_enter[E_KASSA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KASSA],"clicked",G_CALLBACK(kasothw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KASSA],iceb_u_inttochar(E_KASSA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KASSA],gettext("Выбор кассы"));

data.entry[E_KASSA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.entry[E_KASSA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KASSA], "activate",G_CALLBACK(kasothw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KASSA]),data.rk->kassa.ravno());
gtk_widget_set_name(data.entry[E_KASSA],iceb_u_inttochar(E_KASSA));

sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(kasothw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта"));

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(kasothw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KOD_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KONTR]), data.knopka_enter[E_KOD_KONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_KONTR],"clicked",G_CALLBACK(kasothw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_KONTR],iceb_u_inttochar(E_KOD_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_KONTR],gettext("Выбор контрагента"));

data.entry[E_KOD_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KONTR]), data.entry[E_KOD_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_KONTR], "activate",G_CALLBACK(kasothw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_KONTR]),data.rk->kod_kontr.ravno());
gtk_widget_set_name(data.entry[E_KOD_KONTR],iceb_u_inttochar(E_KOD_KONTR));




sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(kasothw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(kasothw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(kasothw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  kasothw_v_e_knopka(GtkWidget *widget,class kasothw_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

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

  case E_KODOP:
    vib_kodop_kas(&data->rk->kodop,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk->kodop.ravno());
    return;  


  case E_KASSA:
    if(l_spis_kas(1,&kod,&naim,data->window) == 0)
      data->rk->kassa.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KASSA]),data->rk->kassa.ravno());
    return;

  case E_SHET:
    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno());
    return;

  case E_KOD_KONTR:
    iceb_vibrek(0,"Kontragent",&data->rk->kod_kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_KONTR]),data->rk->kod_kontr.ravno());
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kasothw_v_key_press(GtkWidget *widget,GdkEventKey *event,class kasothw_data *data)
{

//printf("kasothw_v_key_press\n");
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
void  kasothw_v_knopka(GtkWidget *widget,class kasothw_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
//    if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
    if(iceb_rsdatp_str(&data->rk->datan,&data->rk->datak,data->window) != 0)
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

void    kasothw_v_vvod(GtkWidget *widget,class kasothw_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("kasothw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODOP:
    data->rk->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_KASSA:
    data->rk->kassa.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET:
    data->rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}

/*$Id: ukrrpw_m.c,v 1.12 2013/09/26 09:46:56 sasa Exp $*/
/*27.02.2008	27.02.2008	Белых А.И.	ukrrpw_m.c
Меню для ввода реквизитов
*/

#include "buhg_g.h"
#include "ukrrpw.h"
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
  E_SHETU,
  E_KONTR,
  E_VIDKOM,
  KOLENTER  
 };

class ukrrpw_m_data
 {
  public:
  class ukrrpw_data *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  ukrrpw_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rk->clear();
   }
 };


gboolean   ukrrpw_v_key_press(GtkWidget *widget,GdkEventKey *event,class ukrrpw_m_data *data);
void    ukrrpw_v_vvod(GtkWidget *widget,class ukrrpw_m_data *data);
void  ukrrpw_v_knopka(GtkWidget *widget,class ukrrpw_m_data *data);
void  ukrrpw_v_e_knopka(GtkWidget *widget,class ukrrpw_m_data *data);

int ukrrpw_m_provr(class ukrrpw_m_data *data);

extern SQL_baza bd;

int ukrrpw_m(class ukrrpw_data *rek_ras)
{
int gor=0;
int ver=0;
char strsql[512];


class ukrrpw_m_data data;

data.rk=rek_ras;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
  if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);


sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт реестра проводок по видам командировок"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(ukrrpw_v_key_press),&data);

GtkWidget *label=NULL;
label=gtk_label_new(gettext("Расчёт реестра проводок по видам командировок"));

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

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE,1);


sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(ukrrpw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(ukrrpw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));


sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(ukrrpw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(ukrrpw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));



sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(ukrrpw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR],gettext("Выбор контрагента"));

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(ukrrpw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk->kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));



sprintf(strsql,"%s (,,)",gettext("Счёт учёта"));
data.knopka_enter[E_SHETU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHETU],"clicked",G_CALLBACK(ukrrpw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETU],iceb_u_inttochar(E_SHETU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETU],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHETU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SHETU], "activate",G_CALLBACK(ukrrpw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.rk->shetu.ravno());
gtk_widget_set_name(data.entry[E_SHETU],iceb_u_inttochar(E_SHETU));

sprintf(strsql,"%s (,,)",gettext("Вид командировки"));
data.knopka_enter[E_VIDKOM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_VIDKOM]), data.knopka_enter[E_VIDKOM], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_VIDKOM],"clicked",G_CALLBACK(ukrrpw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_VIDKOM],iceb_u_inttochar(E_VIDKOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_VIDKOM],gettext("Выбор вида командировки"));

data.entry[E_VIDKOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_VIDKOM]), data.entry[E_VIDKOM], TRUE, TRUE,1);
g_signal_connect(data.entry[E_VIDKOM], "activate",G_CALLBACK(ukrrpw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VIDKOM]),data.rk->vidkom.ravno());
gtk_widget_set_name(data.entry[E_VIDKOM],iceb_u_inttochar(E_VIDKOM));





sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(ukrrpw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(ukrrpw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(ukrrpw_v_knopka),&data);
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
void  ukrrpw_v_e_knopka(GtkWidget *widget,class ukrrpw_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
/*g_print("ukrrpw_v_e_knopka knop=%d\n",knop);*/

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


  case E_SHETU:

    if(iceb_l_plansh(1,&kod,&naim,data->window) == 0)
      data->rk->shetu.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->rk->shetu.ravno());
      
    return;  

  case E_VIDKOM:

    if(l_ukrvk(1,&kod,&naim,data->window) == 0)
     data->rk->vidkom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VIDKOM]),data->rk->vidkom.ravno());
      
    return;  


  case E_KONTR:

    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
     data->rk->kontr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk->kontr.ravno());
      
    return;  

   

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ukrrpw_v_key_press(GtkWidget *widget,GdkEventKey *event,class ukrrpw_m_data *data)
{

//printf("ukrrpw_v_key_press\n");
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
void  ukrrpw_v_knopka(GtkWidget *widget,class ukrrpw_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(ukrrpw_m_provr(data) != 0)
     return;
    
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    data->voz=1;
    iceb_sizww(data->name_window.ravno(),data->window);
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    ukrrpw_v_vvod(GtkWidget *widget,class ukrrpw_m_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_KONTR:
    data->rk->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHETU:
    data->rk->shetu.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_VIDKOM:
    data->rk->vidkom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int ukrrpw_m_provr(class ukrrpw_m_data *data)
{

if(iceb_rsdatp(data->rk->datan.ravno(),data->rk->datak.ravno(),data->window) != 0)
 return(1);

return(0);
}



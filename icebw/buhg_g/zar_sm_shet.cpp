/*$Id: zar_sm_shet.c,v 1.14 2013/08/25 08:27:01 sasa Exp $*/
/*13.07.2015	23.10.2006	Белых А.И.	zar_sm_shet.c
Смена счёта в списке начислений/удержаний 
*/
#include "buhg_g.h"
#include "l_nahud.h"
enum
 {
  E_SHET_K_SM,
  E_SHET_N_SM,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class zar_sm_shet_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str shet_k_sm;
  class iceb_u_str shet_n_sm;
    
  class iceb_u_str zapros;
    
  zar_sm_shet_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_rek();
   }

  void read_rek()
   {
    shet_k_sm.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_K_SM])));
    shet_n_sm.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_N_SM])));
   }

  void clear_data()
   {
    clear_rek();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
  void clear_rek()
   {
    shet_k_sm.new_plus("");
    shet_n_sm.new_plus("");
   }
 };

void zar_sm_shet_clear(class zar_sm_shet_data *data);
void    zar_sm_shet_vvod(GtkWidget *widget,class zar_sm_shet_data *data);
void  zar_sm_shet_knopka(GtkWidget *widget,class zar_sm_shet_data *data);
gboolean   zar_sm_shet_key_press(GtkWidget *widget,GdkEventKey *event,class zar_sm_shet_data *data);

void  zar_sm_shet_e_knopka(GtkWidget *widget,class zar_sm_shet_data *data);

void zar_sm_shet_r(const char *shet1,const char *shet2,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok);

extern SQL_baza bd;

int zar_sm_shet(const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
class zar_sm_shet_data data;
char strsql[512];


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Смена счёта"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_sm_shet_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Счёт, который нужно заменить"));
data.knopka_enter[E_SHET_K_SM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_K_SM]), data.knopka_enter[E_SHET_K_SM], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET_K_SM],"clicked",G_CALLBACK(zar_sm_shet_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET_K_SM],iceb_u_inttochar(E_SHET_K_SM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET_K_SM],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET_K_SM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET_K_SM]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_K_SM]), data.entry[E_SHET_K_SM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET_K_SM], "activate",G_CALLBACK(zar_sm_shet_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_K_SM]),data.shet_k_sm.ravno());
gtk_widget_set_name(data.entry[E_SHET_K_SM],iceb_u_inttochar(E_SHET_K_SM));

sprintf(strsql,"%s",gettext("Счёт, на который нужно заменить"));
data.knopka_enter[E_SHET_N_SM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_N_SM]), data.knopka_enter[E_SHET_N_SM], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET_N_SM],"clicked",G_CALLBACK(zar_sm_shet_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET_N_SM],iceb_u_inttochar(E_SHET_N_SM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET_N_SM],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET_N_SM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET_N_SM]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_N_SM]), data.entry[E_SHET_N_SM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET_N_SM], "activate",G_CALLBACK(zar_sm_shet_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_N_SM]),data.shet_n_sm.ravno());
gtk_widget_set_name(data.entry[E_SHET_N_SM],iceb_u_inttochar(E_SHET_N_SM));



sprintf(strsql,"F2 %s",gettext("Смена счёта"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK2],gettext("Начать смену счёта"));
g_signal_connect(data.knopka[PFK2],"clicked",G_CALLBACK(zar_sm_shet_knopka),&data);
gtk_widget_set_name(data.knopka[PFK2],iceb_u_inttochar(PFK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[PFK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[PFK4],"clicked",G_CALLBACK(zar_sm_shet_knopka),&data);
gtk_widget_set_name(data.knopka[PFK4],iceb_u_inttochar(PFK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[PFK10],gettext("Закончить работу в этом окне"));
g_signal_connect(data.knopka[PFK10],"clicked",G_CALLBACK(zar_sm_shet_knopka),&data);
gtk_widget_set_name(data.knopka[PFK10],iceb_u_inttochar(PFK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

zar_sm_shet_r(data.shet_k_sm.ravno(),data.shet_n_sm.ravno(),zapros,poisk,wpredok);

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  zar_sm_shet_e_knopka(GtkWidget *widget,class zar_sm_shet_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_SHET_K_SM:

    iceb_vibrek(1,"Plansh",&data->shet_k_sm,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_K_SM]),data->shet_k_sm.ravno());

    return;  

  case E_SHET_N_SM:

    iceb_vibrek(1,"Plansh",&data->shet_n_sm,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_N_SM]),data->shet_n_sm.ravno());

    return;  

 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_sm_shet_key_press(GtkWidget *widget,GdkEventKey *event,class zar_sm_shet_data *data)
{
//char  bros[512];

//printf("vl_gruppod_key_press\n");
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
void  zar_sm_shet_knopka(GtkWidget *widget,class zar_sm_shet_data *data)
{
class OPSHET rek_shet;
int knop=atoi(gtk_widget_get_name(widget));
//g_print("zar_sm_shet_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    if(data->shet_k_sm.ravno()[0] != '*')
     if(iceb_prsh1(data->shet_k_sm.ravno(),&rek_shet,data->window) != 0)
      return;    

    if(iceb_prsh1(data->shet_n_sm.ravno(),&rek_shet,data->window) != 0)
     return;    

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case PFK4:
    data->clear_data();
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

void    zar_sm_shet_vvod(GtkWidget *widget,class zar_sm_shet_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("zar_sm_shet_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SHET_K_SM:
    data->shet_k_sm.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_SHET_N_SM:
    data->shet_n_sm.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/***********************************/
/*Смена счёта*/
/************************************/

void zar_sm_shet_r(const char *shet1,const char *shet2,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);

SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }


short d,m,g;
int tabn=0;
float kolstr1=0.;
char strsql[1024];
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  //проверяем блокировку карточки
  sprintf(strsql,"select blok from Zarn where tabn=%s and god=%d and mes=%d",row[1],g,m);
  
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   if(atoi(row1[0]) != 0)
    {
     class iceb_u_str repl;
     sprintf(strsql,"%s: %s %s",gettext("Табельный номер"),row[1],row[15]);
     repl.plus(strsql);
     
     sprintf(strsql,"%s %s",gettext("Карточка заблокирована !"),iceb_kszap(atoi(row1[0]),wpredok));

     repl.ps_plus(strsql);
     
     iceb_menu_soob(&repl,wpredok);
     continue;
    }

  if(iceb_u_SRAV(shet1,"*",0) == 0)
    sprintf(strsql,"update Zarp set shet='%s' where datz='%s' and tabn=%s and prn=%s and \
knah=%s and godn=%s and mesn=%s and podr=%s and nomz=%s",
    shet2,row[0],row[1],row[2],row[3],row[6],row[7],row[13],row[9]);
  else
    sprintf(strsql,"update Zarp set shet='%s' where shet='%s' and datz='%s' and tabn=%s and prn=%s and \
knah=%s and godn=%s and mesn=%s and podr=%s and nomz=%s",
    shet2,shet1,row[0],row[1],row[2],row[3],row[6],row[7],row[13],row[9]);
//  printw("%s\n",strsql);

  iceb_sql_zapis(strsql,1,0,wpredok);
  
  sprintf(strsql,"update Zarn1 set shet='%s' where shet='%s' and tabn=%s and prn=%s and knah=%s",
  shet2,shet1,row[1],row[2],row[3]);

  iceb_sql_zapis(strsql,1,0,wpredok);

  tabn=atoi(row[1]);  
  zaravprw(tabn,m,g,NULL,wpredok);
    
 }



}

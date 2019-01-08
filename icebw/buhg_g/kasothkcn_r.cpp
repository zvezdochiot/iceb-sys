/*$Id: kasothkcn_r.c,v 1.23 2014/08/31 06:19:20 sasa Exp $*/
/*23.05.2016	01.10.2006	Белых А.И.	kasothkcn_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "kasothkcn.h"

class kasothkcn_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class kasothkcn_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  kasothkcn_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   kasothkcn_r_key_press(GtkWidget *widget,GdkEventKey *event,class kasothkcn_r_data *data);
gint kasothkcn_r1(class kasothkcn_r_data *data);
void  kasothkcn_r_v_knopka(GtkWidget *widget,class kasothkcn_r_data *data);


extern SQL_baza bd;
extern double   okrg1;
extern double	okrcn;

int kasothkcn_r(class kasothkcn_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class kasothkcn_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка целевой кассовой книги"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(kasothkcn_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Распечатка целевой кассовой книги"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,wpredok));
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(kasothkcn_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)kasothkcn_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  kasothkcn_r_v_knopka(GtkWidget *widget,class kasothkcn_r_data *data)
{
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kasothkcn_r_key_press(GtkWidget *widget,GdkEventKey *event,class kasothkcn_r_data *data)
{
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}

/************************/
/*Распечатка шапки*/
/**********************/

void kasothcn_sap(class iceb_u_int *mkcn,FILE *ff,GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];

int kol_kcn=mkcn->kolih();

class iceb_u_str strok_p;
strok_p.plus("--------------------");
for(int ii=0; ii <= kol_kcn; ii++)
 strok_p.plus("------------------");

fprintf(ff,"%s\n",strok_p.ravno());

fprintf(ff,"%s",gettext("  Дата    | Номер  |"));
class iceb_u_str naim("");
for(int ii=0; ii < kol_kcn; ii++)
 {
  sprintf(strsql,"select naik from Kascn where kod=%d",mkcn->ravno(ii));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");
  fprintf(ff,"%-*.*s|",iceb_u_kolbait(17,naim.ravno()),iceb_u_kolbait(17,naim.ravno()),naim.ravno());
 }
fprintf(ff,"%-*.*s|\n",iceb_u_kolbait(17,gettext("Итого")),iceb_u_kolbait(17,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s",gettext("          |докумен.|"));
for(int ii=0; ii <= kol_kcn; ii++) 
 {
  fprintf(ff,"%s",gettext(" Приход | Расход |"));
 }
fprintf(ff,"\n");

fprintf(ff,"%s\n",strok_p.ravno());

}
/*****************************/
/*распечатка массива*/
/***********************/

void kasothcn_rm(int kolih,double *prihod,double *rashod,FILE *ff)
{
for(int ii=0; ii < kolih; ii++)
 {
  if(prihod[ii] == 0.)
   fprintf(ff,"%8s ","");
  else
   fprintf(ff,"%8.2f ",prihod[ii]);

  if(rashod[ii] == 0.)
   fprintf(ff,"%8s ","");
  else
   fprintf(ff,"%8.2f ",rashod[ii]);
  
 }

}

/******************************************/
/******************************************/

gint kasothkcn_r1(class kasothkcn_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
SQLCURSOR cur,cur1;

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_int mkcn; //массив кодов целевого назначения
class iceb_u_double mscn; //массив сумм сальдо целевого назначения
int kolstr=0;
//читаем список кодов целевого назначения
sprintf(strsql,"select kod from Kascn");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rk->kod_cn.ravno(),row[0],0,0) != 0)
   continue;
  mkcn.plus(atoi(row[0]));
 }

//Читаем сальдо по кодам целевого назначения
sprintf(strsql,"select kod,ks,saldo from Kascnsl where god=%d",gn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kolih_kcn=mkcn.kolih();

mscn.make_class(kolih_kcn);

int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rk->kassa.ravno(),row[0],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->kod_cn.ravno(),row[1],0,0) != 0)
   continue;

  nomer=mkcn.find(atoi(row[0]));
  mscn.plus(atof(row[2]),nomer);
 }


//читаем документы

sprintf(strsql,"select kassa,tp,datd,nomd,shetk,kodop from Kasord where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by datd,tp asc",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ff;
char imaf[64];
sprintf(imaf,"kokcn%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Распечатка целевой кассовой книги"));

iceb_zagolov(gettext("Распечатка целевой кассовой книги"),dn,mn,gn,dk,mk,gk,ff,data->window);

if(data->rk->shet.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
if(data->rk->kassa.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Касса"),data->rk->kassa.ravno());
if(data->rk->kodop.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
if(data->rk->metka_pr != 0)
 { 
  if(data->rk->metka_pr == 1)
   fprintf(ff,"%s:%s\n",gettext("Приход/расход"),"+");
  if(data->rk->metka_pr == 2)
   fprintf(ff,"%s:%s\n",gettext("Приход/расход"),"-");
 }
if(data->rk->kod_cn.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код целевого назначения"),data->rk->kod_cn.ravno());
 

kasothcn_sap(&mkcn,ff,data->window);

short d,m,g;
double prihod[kolih_kcn];
double rashod[kolih_kcn];
memset(prihod,'\0',sizeof(prihod));
memset(rashod,'\0',sizeof(rashod));

double suma=0.,sumapod=0.;
double prih_itogo=0.;
double rash_itogo=0.;

int tipz=0;
short metka_saldo=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kassa.ravno(),row[0],0,0) != 0)
   continue;
  
  if(iceb_u_proverka(data->rk->shet.ravno(),row[4],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->kodop.ravno(),row[5],0,0) != 0)
   continue;
  
  tipz=atoi(row[1]);
  if(data->rk->metka_pr == 1 && tipz == 2)
   continue;

  if(data->rk->metka_pr == 2 && tipz == 1)
   continue;

  //читаем код целевого назначения этой операции
  if(tipz == 1)
    sprintf(strsql,"select kcn from Kasop1 where kod='%s'",row[5]);
  if(tipz == 2)
    sprintf(strsql,"select kcn from Kasop2 where kod='%s'",row[5]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
   if(atoi(row1[0]) != 0)
    {
     if(iceb_u_proverka(data->rk->kod_cn.ravno(),row1[0],0,0) != 0)
      continue;
    }
   else 
    continue;
   }
  if((nomer=mkcn.find(atoi(row1[0]))) < 0)
   {
    sprintf(strsql,"Не найден код %d в массиве кодов целевого назначения !",atoi(row1[0]));
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  iceb_u_rsdat(&d,&m,&g,row[2],2);
  
  sumkasorw(row[0],tipz,row[3],g,&suma,&sumapod,data->window);
  
  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0) //до начала периода
   {
    if(tipz == 2)
      sumapod*=-1; 
    mscn.plus(sumapod,nomer);
   }
  else //период
   {
    if(metka_saldo == 0)
     {
      memset(prihod,'\0',sizeof(prihod));
      memset(rashod,'\0',sizeof(rashod));
      for(int ii=0; ii < kolih_kcn; ii++)
       {
        if(mscn.ravno(ii) < 0)
         rashod[ii]=mscn.ravno(ii);
        else
         prihod[ii]=mscn.ravno(ii);
       }

      fprintf(ff,"%*s ",iceb_u_kolbait(19,gettext("Сальдо")),gettext("Сальдо"));
      
      kasothcn_rm(kolih_kcn,prihod,rashod,ff);
      if(mscn.suma() > 0)
       {
        fprintf(ff,"%8.2f\n",mscn.suma());
        prih_itogo+=mscn.suma();
       }
      else 
       {
        fprintf(ff,"%8s %8.2f\n","",mscn.suma());
        rash_itogo+=mscn.suma();
       }
      metka_saldo=1;
     }

    memset(prihod,'\0',sizeof(prihod));
    memset(rashod,'\0',sizeof(rashod));
//    printw("nomer=%d sumapod=%f\n",nomer,sumapod);
    if(tipz == 1)
     {
      prihod[nomer]=sumapod;    
      prih_itogo+=sumapod;
     }
    if(tipz == 2)
     {
      rashod[nomer]=sumapod;    
      rash_itogo+=sumapod;
     }
    
    fprintf(ff,"%02d.%02d.%04d %-*s ",d,m,g,iceb_u_kolbait(8,row[3]),row[3]);

    kasothcn_rm(kolih_kcn,prihod,rashod,ff);
    if(tipz == 1)
      fprintf(ff,"%8.2f\n",sumapod);
    if(tipz == 2)
      fprintf(ff,"%8s %8.2f\n","",sumapod);
    
    if(tipz == 2)
      sumapod*=-1; 
    mscn.plus(sumapod,nomer);
   }

 }

memset(prihod,'\0',sizeof(prihod));
memset(rashod,'\0',sizeof(rashod));

for(int ii=0; ii < kolih_kcn; ii++)
 {
  if(mscn.ravno(ii) < 0)
   rashod[ii]=mscn.ravno(ii);
  else
   prihod[ii]=mscn.ravno(ii);
 }

fprintf(ff,"%*s ",iceb_u_kolbait(19,gettext("Сальдо")),gettext("Сальдо"));

kasothcn_rm(kolih_kcn,prihod,rashod,ff);

if(prih_itogo > rash_itogo)
 fprintf(ff,"%8.2f\n",prih_itogo-rash_itogo);
else
 fprintf(ff,"%8s %8.2f\n","",prih_itogo-rash_itogo);
 

iceb_podpis(ff,data->window);
fclose(ff);

iceb_ustpeh(imaf,0,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}

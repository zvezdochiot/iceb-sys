/*$Id:$*/
/*22.03.2017	17.11.2004	Белых А.И.	sprozn_r.c
Списание розничной торговли
*/
#include <math.h>
#include "buhg_g.h"

class sprozn_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
    
  iceb_u_str *datan;
  iceb_u_str *datak;
  class iceb_u_str *kodop;  
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  sprozn_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   sprozn_r_key_press(GtkWidget *widget,GdkEventKey *event,class sprozn_r_data *data);
gint sprozn_r1(class sprozn_r_data *data);
void  sprozn_r_v_knopka(GtkWidget *widget,class sprozn_r_data *data);

int	sproznd(short dn,short mn,short gn,short dk,short mk,short gk,int nds,const char *kodop,class sprozn_r_data*);
int    rizdel(char *sklad,char *kodm,double kolih,iceb_u_spisok *ZAPIS,iceb_u_double *KOLIH,iceb_u_double *SUMA,GtkWidget *wpredok);

extern SQL_baza bd;
extern double okrcn;
extern double okrg1;

int sprozn_r(class iceb_u_str *datan,class iceb_u_str *datak,class iceb_u_str *kodop,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_spisok repl_s;
class sprozn_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.datan=datan;
data.datak=datak;
data.kodop=kodop;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Списание розничной торговли"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(sprozn_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Списание розничной торговли"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(sprozn_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)sprozn_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  sprozn_r_v_knopka(GtkWidget *widget,class sprozn_r_data *data)
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

gboolean   sprozn_r_key_press(GtkWidget *widget,GdkEventKey *event,class sprozn_r_data *data)
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

/******************************************/
/******************************************/

gint sprozn_r1(class sprozn_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_u_str repl;


SQLCURSOR cur;
SQL_str row;
//Проверяем есть ли этот код операции в расходах
sprintf(strsql,"select kod from Rashod where kod='%s'",data->kodop->ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),data->kodop->ravno());

  iceb_menu_soob(strsql,data->window);

  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }
short dn,mn,gn;
short dk,mk,gk;

iceb_rsdatp(&dn,&mn,&gn,data->datan->ravno(),&dk,&mk,&gk,data->datak->ravno(),data->window);

sprintf(strsql,"%s %d.%d.%d => %d.%d.%d\n",
gettext("Списание розничной продажи"),
dn,mn,gn,dk,mk,gk);

int koldok=0;

koldok=sproznd(dn,mn,gn,dk,mk,gk,0,data->kodop->ravno(),data); //Списать товары с НДС
koldok+=sproznd(dn,mn,gn,dk,mk,gk,1,data->kodop->ravno(),data); //Списать товары без НДС

if(koldok == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
 }
else
 if(koldok > 0)
  {
   repl.new_plus(gettext("Удалить загруженные записи ?"));

   if(iceb_menu_danet(&repl,2,data->window) == 1)
    {

     sprintf(strsql,"delete from Roznica where \
datrp >= '%d-%d-%d' and  datrp <= '%d-%d-%d' and metka=1",
     gn,mn,dn,gk,mk,dk);

     if(sql_zap(&bd,strsql) != 0)
       iceb_msql_error(&bd,"",strsql,data->window);
    }
  } 


data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);
}
/*************************************/
/*Формирование документов на списание*/
/**************************************/

int	sproznd(short dn,short mn,short gn,
short dk,short mk,short gk,
int nds, //0-с НДС 1-без НДС
const char *kodop,
class sprozn_r_data *data)
{
char 		strsql[512];
int		kolstr=0;
SQL_str		row,row1;
int		skladz=0;
int		sklad=0;
class iceb_u_str nomd("");
time_t		tmm;
int		metkapropuska=0;
double		cena=0.;
double		kolih=0.;
int		kodm=0;
class iceb_u_str ei("");
double		ndss=0.;
int		koldok=0;
double		suma=0.;

float pnds=iceb_pnds(data->window);

sprintf(strsql,"select kodm,ei,kolih,cena,sklad from Roznica where \
datrp >= '%d-%d-%d' and  datrp <= '%d-%d-%d' and metka=1 and nds=%d. order by sklad asc",
gn,mn,dn,gk,mk,dk,nds);

//printw("%s\n",strsql);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  return(-1);
 }
if(kolstr == 0)
  return(0);
memset(strsql,'\0',sizeof(strsql));  
if(nds == 0)
  sprintf(strsql,"*%02d%02d/1",dk,mk);
if(nds == 1)
  sprintf(strsql,"*%02d%02d/2",dk,mk);
nomd.new_plus(strsql);
float	kolstr1=0;

iceb_u_spisok ZAPIS;
iceb_u_double KOLIH;
iceb_u_double SUMA;
int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);

  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(LINES-1,0,kolstr,++kolstr1);

  kolih=atof(row[2]);
  if(rizdel(row[4],row[0],kolih,&ZAPIS,&KOLIH,&SUMA,data->window) != 0)
    continue;

  sprintf(strsql,"%s|%s|%s",
  row[4],row[0],row[1]);
  
  cena=atof(row[3]);
  cena=iceb_u_okrug(cena,okrg1);
  
  suma=kolih*cena;
  suma=iceb_u_okrug(suma,okrg1); //Определяем сумму для того чтобы вычислить среднюю цену если у материалла были разные цены
  
  if((nomer=ZAPIS.find(strsql)) < 0)
    ZAPIS.plus(strsql);  

  
  KOLIH.plus(kolih,nomer);
  SUMA.plus(suma,nomer);

}

int kolzap=ZAPIS.kolih();
int kolkar=0;
double kolihp=0.,kolihpi=0;
int nomkar=0;
SQLCURSOR cur1;

for(nomer=0; nomer < kolzap; nomer++)
 {
  iceb_u_polen(ZAPIS.ravno(nomer),strsql,sizeof(strsql),1,'|');
  
  sklad=atoi(strsql);

  if(skladz != sklad)
   {
    if(skladz  > 0 )
     {
      podtdok1w(dk,mk,gk,sklad,nomd.ravno(),2,dk,mk,gk,1,data->window);
      podvdokw(dk,mk,gk,nomd.ravno(),sklad,data->window);
     }     

    int nomerdok=0;    
    class iceb_u_str nomerdokch(nomd.ravno());
//    strcpy(nomerdokch,nomd);

nazad:;

    //Проверяем может такой документ уже введен
    sprintf(strsql,"select nomd from Dokummat1 where datd >= '%d-%d-%d' \
and datd <= '%d-%d-%d' and sklad=%d and nomd='%s'",
    gk,1,1,gk,12,31,sklad,nomerdokch.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
     {

//      sprintf(nomerdokch,"%s/%d",nomd,++nomerdok);
      nomerdokch.plus("/");
      nomerdokch.plus(++nomerdok);
      goto nazad;      
     }

    nomd.new_plus(nomerdokch.ravno());

    time(&tmm);
    if(zap_s_mudokw(2,dk,mk,gk,sklad,"00",nomd.ravno(),"",kodop,0,0,"",0,0,0,1,"00",pnds,data->window) != 0)
    if(sql_zap(&bd,strsql) != 0)
     {
      iceb_msql_error(&bd,"",strsql,data->window);
      return(0);
     }
    koldok++;
    
    if(nds == 0)
     {
      sprintf(strsql,"insert into Dokummat2 values (%d,%d,'%s',%d,'%s')",
      gk,sklad,nomd.ravno(),11,"0");
      ndss=pnds;
     }

    if(nds == 1)
     {
      sprintf(strsql,"insert into Dokummat2 values (%d,%d,'%s',%d,'%s')",
      gk,sklad,nomd.ravno(),11,"3");
      ndss=0.;
     }
     
    if(sql_zap(&bd,strsql) != 0)
     {
      iceb_msql_error(&bd,"",strsql,data->window);
     }
    sprintf(strsql,"insert into Dokummat2 values (%d,%d,'%s',%d,'%d')",
    gk,sklad,nomd.ravno(),4,iceb_getuid(data->window));

    if(sql_zap(&bd,strsql) != 0)
     {
       iceb_msql_error(&bd,"",strsql,data->window);
     }

    skladz=sklad;
    metkapropuska=0;

    sprintf(strsql,"%s %d %s: %s\n",gettext("Склад"),sklad,
    gettext("Документ"),nomd.ravno());
    iceb_printw(strsql,data->buffer,data->view);
    
   }

  if(metkapropuska == 1)
   continue;
  
  iceb_u_polen(ZAPIS.ravno(nomer),strsql,sizeof(strsql),2,'|');
  kodm=atoi(strsql);

  iceb_u_polen(ZAPIS.ravno(nomer),&ei,3,'|');


  kolih=KOLIH.ravno(nomer);
  suma=SUMA.ravno(nomer);
  cena=suma/kolih;
  cena=iceb_u_okrug(cena,okrcn);
  
  if(nds == 0) //Узнаем цену без ндс
     cena=cena-cena*pnds/(100.+pnds);

  cena=iceb_u_okrug(cena,okrcn);
  

  iceb_u_int nomkars(0);
  iceb_u_double kolpriv(0);
    
  if((kolkar=findkarw(dk,mk,gk,sklad,kodm,kolih,&nomkars,&kolpriv,0,cena,"",data->window)) <= 0)
    zapvdokw(dk,mk,gk,sklad,0,kodm,nomd.ravno(),kolih,cena,ei.ravno(),ndss,0,0,2,0,"","",0,"",data->window);
  else
   {
    kolihpi=0.;
    for(int sht=0; sht < kolkar; sht++)
     {
      nomkar=nomkars.ravno(sht);
      kolihp=kolpriv.ravno(sht);
      kolihpi+=kolihp;
      zapvdokw(dk,mk,gk,sklad,nomkar,kodm,nomd.ravno(),kolihp,cena,ei.ravno(),ndss,0,0,2,0,"","",0,"",data->window);
     }
    if(fabs(kolih-kolihpi) > 0.000001)
      zapvdokw(dk,mk,gk,sklad,0,kodm,nomd.ravno(),kolih-kolihpi,cena,ei.ravno(),ndss,0,0,2,0,"","",0,"",data->window);
   }


 }

podtdok1w(dk,mk,gk,sklad,nomd.ravno(),2,dk,mk,gk,1,data->window);
podvdokw(dk,mk,gk,nomd.ravno(),sklad,data->window);
return(koldok);

}

/**************************************/
/*Разуздование изделия                */
/**************************************/
// 0-если не изделие 1-изделие
int    rizdel(char *sklad,char *kodm,double kolih,iceb_u_spisok *ZAPIS,
iceb_u_double *KOLIH,iceb_u_double *SUMA,GtkWidget *wpredok)
{
char strsql[512];
SQL_str  row;
SQLCURSOR cur;

sprintf(strsql,"select distinct kodi from Specif where kodi=%s",kodm);
if(sql_readkey(&bd,strsql,&row,&cur) <= 0)
 return(0);

int kodiz=atoi(kodm);
 
//int kolnuz=0;
int kolndet=0;

class iceb_razuz_data m_det;
m_det.kod_izd=kodiz;
if((kolndet=iceb_razuz_kod(&m_det,wpredok)) == 0)
  return(0);  

int nomer=0;
int nomer1=0;
double cena,suma;
class iceb_u_str ei("");
double kolmat;
int kodmat;
kolndet=m_det.kod_det_ei.kolih();
for(nomer=0; nomer < kolndet;nomer++)
 {
  if(m_det.metka_mu.ravno(nomer) == 1)
   continue;

  iceb_u_polen(m_det.kod_det_ei.ravno(nomer),&kodmat,1,'|');
  iceb_u_polen(m_det.kod_det_ei.ravno(nomer),&ei,2,'|');
  kolmat=m_det.kolih_det_ei.ravno(nomer);
  
  cena=suma=0.;

  /*Читаем наименование материалла*/
  sprintf(strsql,"select cenapr from Material where kodm=%d",
  kodmat);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    cena=atof(row[0]);
   }

  sprintf(strsql,"%s|%d|%s",sklad,kodmat,ei.ravno());
  
  suma=kolmat*kolih*cena;
  suma=iceb_u_okrug(suma,okrg1); //Определяем сумму для того чтобы вычислить среднюю цену если у материалла были разные цены
  
  if((nomer1=ZAPIS->find(strsql)) < 0)
    ZAPIS->plus(strsql);  

  
  KOLIH->plus(kolmat*kolih,nomer1);
  SUMA->plus(suma,nomer1);
   
 }
//OSTANOV();

return(1);

}

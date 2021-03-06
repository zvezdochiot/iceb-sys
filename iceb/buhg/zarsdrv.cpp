/*$Id: zarsdrv.c,v 5.3 2013/09/26 09:43:45 sasa Exp $*/
/*02.10.2014	27.04.2013	Белых А.И.	zarsdrv.c
Расчёт справки для расчёта выплат на случай безработицы
Расчёт производится с учётом даты в счёт которой были сделаны выплаты
Нужно знать общюю сумму с которой брасля единый социальный взнос и сумму взноса
для того чтобы расчитать какая часть этого взноса которая приходися по месяцам 
выплат
Должны отсекатся выплаты в счёт дат которые меньше даты начала расчёта
*/
#include <math.h>
#include "buhg.h"


void zarsdrv_sh1(int tabnom,FILE *ff);
void zarsdrv_end(double suma,FILE *ff);

extern class iceb_tu_str kods_esv_all;  /*Все коды удержания единого социального взноса*/

int zarsdrv(short mn,short gn,
short mk,short gk,
int tabnom,
class spis_oth *oth)
{
SQL_str row;
class SQLCURSOR cur;
SQL_str row1,row2;
class SQLCURSOR cur1,cur2;
int kolstr;
char strsql[1024];


int kolmes=period(1,mn,gn,1,mk,gk,1);
if(kolmes == 0)
 {
  iceb_t_soob(gettext("Количество месяцев в периоде равно нолю !"));
  return(1);
 }

class iceb_tu_str kod_esv_nvr(""); /*коды единого социального взноса не входящие в расчёт*/
class iceb_tu_str kod_nah_nvr(""); /*коды начислений не входящие в расчёт*/
class iceb_tu_str kod_tab_nvr(""); /*коды табеля не входящие в расчёт*/
class iceb_tu_str kod_nnah_nvr(""); /*коды начислений не входящие в начисленную зарплату*/
iceb_t_poldan("Коды единого социального взноса не входящие в расчёт",&kod_esv_nvr,"zarsruv.alx");
iceb_t_poldan("Коды начислений не входящие в расчёт",&kod_nah_nvr,"zarsruv.alx");
iceb_t_poldan("Коды табеля не входящие в расчёт",&kod_tab_nvr,"zarsruv.alx");
iceb_t_poldan("Коды начислений не входящие в начисленную зарплату",&kod_nnah_nvr,"zarsruv.alx");
zarrnesv();

class iceb_t_tmptab tabtmp;
const char *imatmptab={"zarsdrv"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
datavp DATE NOT NULL DEFAULT '0000-00-00',\
sumnahall double(12,2) not null DEFAULT 0.,\
sumnahforesv double(12,2) not null DEFAULT 0.,\
sumesv double(12,2) not null DEFAULT 0.,\
unique(datavp)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 {
  return(1);
 }  
char imaf_prot[64];
sprintf(imaf_prot,"zarsdrvp%d.lst",getpid());

class iceb_fopen ffprot;
if(ffprot.start(imaf_prot,"w") != 0)
 return(1);
 
short dr=1,mr=mn,gr=gn;
int prn=0;
int knah=0;
double suma=0.;
short dz=0,mz=0,gz=0;
class iceb_tu_str naim_nu("");
fprintf(ffprot.ff,"%s %d %s\n",gettext("Табельный номер"),tabnom,zargetfio(tabnom));
for(int nomer_mes=0; nomer_mes < kolmes; nomer_mes++)
 {
  fprintf(ffprot.ff,"\n%d.%d\n",mr,gr);
  
  sprintf(strsql,"select prn,knah,suma,godn,mesn from Zarp where tabn=%d and \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and suma <> 0. order by prn asc,knah asc",
  tabnom,gr,mr,gr,mr);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
   }

  class SPISOK data_vr;
  class masiv_din_double s_suma_all;
  class masiv_din_double s_suma_for_esv;
  class masiv_din_double s_suma_esv;
  double suma_nvr_esv=0.;  /*Сумма всех начислений которые входят в расчёт единого социального взноса*/
  double suma_esv=0.;      /*Сумма единого социального взноса*/
  
  short godn=0,mesn=0;
  
  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    prn=atoi(row[0]);
    knah=atoi(row[1]);
    suma=atof(row[2]);    
    godn=atoi(row[3]);
    mesn=atoi(row[4]);
    naim_nu.new_plus("");     

    fprintf(ffprot.ff,"%s %3s %10s %s.%s",row[0],row[1],row[2],row[4],row[3]);

    naim_nu.new_plus("");     

    if(prn == 1) /*начисления*/
     {

      double suma1=0.;
      double suma2=0.;
            

      sprintf(strsql,"%d.%d",mesn,godn);
      int nomer_v_sp=0;
      if((nomer_v_sp=data_vr.find(strsql)) < 0)
       {
        data_vr.plus(strsql);
        s_suma_esv.plus(0.,-1);
       }

      if(proverka(kod_nnah_nvr.ravno(),knah,0,1) != 0)
       suma1=suma;        

      if(proverka(kod_nah_nvr.ravno(),knah,0,1) == 0)
        {
         fprintf(ffprot.ff,"%s !\n",gettext("Не вошло в расчёт"));      
        }
       else
        suma2=suma;
        
      s_suma_all.plus(suma1,nomer_v_sp);
      s_suma_for_esv.plus(suma2,nomer_v_sp);
      suma_nvr_esv+=suma2;

      sprintf(strsql,"select naik from Nash where kod=%d",knah);    
      if(readkey(strsql,&row1,&cur1) == 1)
       naim_nu.new_plus(row1[0]);

      if(suma2 != 0.)
       fprintf(ffprot.ff,"*");
      else 
       fprintf(ffprot.ff," ");
       
      fprintf(ffprot.ff," %s",naim_nu.ravno());      
     }

    if(prn == 2) /*удержания*/
     {
      class iceb_tu_str metka(" ");
      if(proverka(kods_esv_all.ravno(),knah,0,1) == 0)
       if(proverka(kod_esv_nvr.ravno(),knah,0,1) != 0)
        {
         suma_esv+=suma*-1;

         metka.new_plus("*");
        }
      fprintf(ffprot.ff,"%s",metka.ravno());
        
      sprintf(strsql,"select naik from Uder where kod=%d",knah);    
      if(readkey(strsql,&row1,&cur1) == 1)
        naim_nu.new_plus(row1[0]);
      fprintf(ffprot.ff," %s",naim_nu.ravno());      

     }
    fprintf(ffprot.ff,"\n");
   }
  fprintf(ffprot.ff,"Сумма начислений входящих в расчёт единого социального взноса=%.2f\n",suma_nvr_esv);
  fprintf(ffprot.ff,"Сумма единого социальноговзноса=%.2f\n",suma_esv);
  fprintf(ffprot.ff,"Распределение единого социального взноса по месяцам в счёт котоых были выплаты\n");
  /*вычисляем для каждого месяца его долю единого социального взноса*/
  double isuma=0.;
  for(int nomer=0; nomer < data_vr.kolih(); nomer++)
   {
    
    suma=s_suma_for_esv.ravno(nomer)*suma_esv/suma_nvr_esv;
    suma=okrug(suma,0.01);
    isuma+=suma;
    s_suma_esv.plus(suma,nomer);
    fprintf(ffprot.ff,"%s %10.2f %10.2f\n",data_vr.ravno(nomer),s_suma_for_esv.ravno(nomer),s_suma_esv.ravno(nomer));
   }
  fprintf(ffprot.ff,"%*s %10.2f\n",
  iceb_tu_kolbait(17,gettext("Итого")),
  gettext("Итого"),
  isuma);
  
  /*Записываем во временную таблицу*/
  for(int nomer=0; nomer < data_vr.kolih(); nomer++)
   {

    rsdat1(&mz,&gz,data_vr.ravno(nomer));
    sprintf(strsql,"select datavp from %s where datavp='%04d-%02d-01'",imatmptab,gz,mz);
    if(readkey(strsql,&row1,&cur1) > 0)
      sprintf(strsql,"update %s set sumnahall=sumnahall+%.2f,sumnahforesv=sumnahforesv+%.2f,sumesv=sumesv+%.2f where datavp='%04d-%02d-01'",
      imatmptab,
      s_suma_all.ravno(nomer),
      s_suma_for_esv.ravno(nomer),
      s_suma_esv.ravno(nomer),
      gz,mz);
    else
      sprintf(strsql,"insert into %s values ('%04d-%02d-01',%.2f,%.2f,%.2f)",
      imatmptab,
      gz,mz,
      s_suma_all.ravno(nomer),
      s_suma_for_esv.ravno(nomer),
      s_suma_esv.ravno(nomer));    

    sql_zapis(strsql,1,0);    
    fprintf(ffprot.ff,"%s\n",strsql);
   }



  dpm(&dr,&mr,&gr,3);
 }

sprintf(strsql,"select * from %s where datavp >= '%04d-%02d-01' and datavp <= '%04d-%02d-01' order by datavp asc",
imatmptab,gn,mn,gk,mk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
char imaf[64];
sprintf(imaf,"zarsdrv%d.lst",getpid());
class iceb_fopen ff_spr;

if(ff_spr.start(imaf,"w") != 0)
 return(1);

zarsdrv_sh1(tabnom,ff_spr.ff);

int nomer_str_bez=0; 
class iceb_tu_str naim_mes("");
int kol_dnei=0;
double itogo[3];
memset(itogo,'\0',sizeof(itogo));
double suma1=0.;
double suma2=0.;
double suma3=0.;
int iotr=0;
fprintf(ffprot.ff,"\n\nФормируем распечатку\n");
while(cur.read_cursor(&row) != 0)
 {
  fprintf(ffprot.ff,"\n%s %10s %10s %10s\n",row[0],row[1],row[2],row[3]);
  rsdat(&dz,&mz,&gz,row[0],2);
  
  //Узнаем количество отработанных дней
  sprintf(strsql,"select kodt,dnei,kdnei from Ztab where tabn=%d and god=%d \
and mes=%d",tabnom,gz,mz);
  if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
   { 
    msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
   }
  kol_dnei=0;
  if(kolstr > 0)
  while(cur1.read_cursor(&row1) != 0)
   {
    sprintf(strsql,"select naik from Tabel where kod=%s",row1[0]);
    readkey(strsql,&row2,&cur2);
    fprintf(ffprot.ff,"%-2s %-*s %2s %2s\n",
    row1[0],
    iceb_tu_kolbait(30,row2[0]),
    row2[0],
    row1[1],
    row1[2]);

    if(proverka(kod_tab_nvr.ravno(),row1[0],0,1) != 0)
      kol_dnei+=atoi(row1[2]);
   }  

  suma1=atof(row[1]);
  suma2=atof(row[2]);
  suma3=atof(row[3]);
  itogo[0]+=suma1;
  itogo[1]+=suma2;
  itogo[2]+=suma3;
  iotr+=kol_dnei;
    
  mesc(mz,0,&naim_mes);
  fprintf(ff_spr.ff,"|%02d|%02d.%04d%15s|%-*s|%14d|%13.2f|%13.2f|%16.2f|\n",
  ++nomer_str_bez,
  mz,
  gz,
  "",
  iceb_tu_kolbait(21,naim_mes.ravno()),
  naim_mes.ravno(),
  kol_dnei,
  suma1,
  suma2,
  suma3);
  
 }

fprintf(ff_spr.ff,"\
-------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_spr.ff,"%*s|%14d|%13.2f|%13.2f|%16.2f|\n",
iceb_tu_kolbait(48,gettext("Итого")),gettext("Итого"),
iotr,itogo[0],itogo[1],itogo[2]);

fprintf(ff_spr.ff,"\
-------------------------------------------------------------------------------------------------------------\n");
double ddd=0.;
if(iotr > 0)
 ddd=itogo[0]/iotr;
zarsdrv_end(ddd,ff_spr.ff);

podpis(ffprot.ff);
podpis(ff_spr.ff);

ff_spr.end();
ffprot.end();

oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Справка для расчёта выплат на случай безработицы"));
oth->spis_imaf.plus(imaf_prot);
oth->spis_naim.plus(gettext("Протокол хода расчёта"));

for(int nomer=0; nomer < oth->spis_imaf.kolih(); nomer++)
 iceb_t_ustpeh(oth->spis_imaf.ravno(nomer),3);

return(0);
}
/*******************************************/
/*Заполнение шапки справки в фонд занятости*/
/*******************************************/
void zarsdrv_sh1(int tabnom,FILE *ff)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
short dt,mt,gt;
int nomer_str=0;
class iceb_tu_str stroka("");
char bros[512];
class iceb_tu_str inn("");
class iceb_tu_str fio("");

sprintf(strsql,"select fio,inn from Kartb where tabn=%d",tabnom);
if(readkey(strsql,&row_alx,&cur_alx) == 1)
 {
  fio.new_plus(row_alx[0]);
  inn.new_plus(row_alx[1]);
 }

poltekdat(&dt,&mt,&gt);

sprintf(strsql,"select str from Alx where fil='zarsvfz_start.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarsvfz_start.alx");
  iceb_t_soob(strsql);
  return;
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  nomer_str++;
  stroka.new_plus(row_alx[0]);
  switch(nomer_str)
   {
    case 1:
     sprintf(bros,"%02d.%02d.%d р.",dt,mt,gt);
     iceb_tu_vstav(&stroka,bros,1,13,1);
     break;

    case 5:
     iceb_tu_vstav(&stroka,fio.ravno(),10,109,1);
     break;

    case 6:
     iceb_tu_vstav(&stroka,iceb_t_get_pnk("00",0),25,109,1);
     break;
    case 7:
     iceb_tu_vstav(&stroka,inn.ravno(),23,36,1);
     break;
   }

  fprintf(ff,"%s",stroka.ravno());

 }

}
/*******************/
/*Заполение концовки*/
/********************/
void zarsdrv_end(double suma,FILE *ff)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
int nomer_str=0;
class iceb_tu_str stroka("");
class iceb_tu_str suma_prop("");
double cel=0.,drob=0.;
class iceb_t_fioruk_rk rekruk;

suma=okrug(suma,0.01); /*обязательно округлить иначе разница на копейку в сумме прописью*/
preobr(suma,&suma_prop,0);

drob=modf(suma,&cel);

sprintf(strsql,"select str from Alx where fil='zarsvfz_end.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarsvfz_end.alx");
  iceb_t_soob(strsql);
  return;
 }

int idrob=drob*100;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  stroka.new_plus(row_alx[0]);
  switch(nomer_str)
   {
    case 4:
     iceb_tu_vstav(&stroka,(int)cel,1,11,1);
     iceb_tu_vstav(&stroka,idrob,18,20,1);
     break;

    case 5:
     iceb_tu_vstav(&stroka,suma_prop.ravno(),1,108,1);
     break;

    case 28:
     iceb_t_fioruk(1,&rekruk);      
     iceb_tu_vstav(&stroka,rekruk.fio.ravno(),45,108,1);
     break;

    case 31:
     iceb_t_fioruk(2,&rekruk);      
     iceb_tu_vstav(&stroka,rekruk.fio.ravno(),45,108,1);
     break;

    case 33:
     iceb_tu_vstav(&stroka,rekruk.telef.ravno(),48,108,1);
     break;

   }

  fprintf(ff,"%s",stroka.ravno());

 }



}


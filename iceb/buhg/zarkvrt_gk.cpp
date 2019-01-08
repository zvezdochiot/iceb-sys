/*$Id: zarkvrt_gk.c,v 5.3 2013-01-15 10:17:56 sasa Exp $*/
/*16.10.2014	21.04.2005	Белых А.И.	zarkvrt_gk.c
Поиск проводок для формы 1дф
Возвращаем 0-если есть хоть одна сумма
           1-если нет ни одной суммы
*/
#include "buhg.h"

int zarkvrt_gk_r(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *shet_deb,
const char *shet_kre,
SPISOK *kod_kom,
masiv_din_double *suma_vip,
masiv_din_double *suma_nah,
int priz_nv,int,FILE*);



int zarkvrt_gk(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *shkas_shkom,
const char *prist_k_tabnom,
int kod_doh,
const char *imatmptab,
FILE *ff_prot)
{

if(shkas_shkom[0] == '\0')
 {
  fprintf(ff_prot,"Не введены настройки для поиска проводок\n");
  return(1);
 } 

printw("Код дохода:%d\nПошук проводок по рахунках:%s\n",kod_doh,shkas_shkom);
refresh();
fprintf(ff_prot,"Код дохода:%d\nПошук проводок по рахунках:%s\n",kod_doh,shkas_shkom);

char strsql[1024];
char bros[512];
class iceb_tu_str sheta_doh("");
class iceb_tu_str sheta_pnal("");


if(polen(shkas_shkom,&sheta_doh,1,'>') != 0)
  sheta_doh.new_plus(shkas_shkom);
else
  polen(shkas_shkom,&sheta_pnal,2,'>');
  
int kolpol=pole2(sheta_doh.ravno(),';');

int  priz_nv=0; //1-начислено 2-выплачено
int  priz_deb_kre; //1-дебет 2-кредит
class iceb_tu_str shet_kre("");
class iceb_tu_str shet_deb("");

class SPISOK kod_kom;
class masiv_din_double suma_vip; //Сумма выплаченного дохода
class masiv_din_double suma_nah; //Сумма начисленного дохода
class masiv_din_double podoh_nah; //Сумма начисленного подоходного налога
class masiv_din_double podoh_vip; //Сумма выплаченого подоходного налога
//Смотрим доход начисленный и выплаченный
for(int nn=0; nn < kolpol ; nn++)
 {

  polen(sheta_doh.ravno(),strsql,sizeof(strsql),nn+1,';');
  if(strsql[0] == '\0')
   continue;

  priz_nv=0;
  polen(strsql,bros,sizeof(bros),1,':');

  if(bros[0] == '+')
   priz_nv=1;

  if(bros[0] == '-')
   priz_nv=2;

  if(bros[0] == '*')
   priz_nv=3;

  if(priz_nv == 0)
   continue;

  polen(strsql,bros,sizeof(bros),2,':');
  if(bros[0] == 'd')
   priz_deb_kre=1;
  else
   priz_deb_kre=2;
    
  polen(strsql,&shet_deb,3,':');
  polen(strsql,&shet_kre,4,':');
  zarkvrt_gk_r(dn,mn,gn,dk,mk,gk,shet_deb.ravno(),shet_kre.ravno(),&kod_kom,&suma_vip,&suma_nah,priz_nv,priz_deb_kre,ff_prot);
   
 }

//Списки должны быть с одинаковым количеством элементов
podoh_vip.make_class(suma_vip.kolih());
podoh_nah.make_class(suma_nah.kolih());

if(sheta_pnal.ravno()[0] != '\0')
 {
  kolpol=pole2(sheta_pnal.ravno(),';');
  //Смотрим подоходный налог начисленный и выплаченный
  for(int nn=0; nn < kolpol ; nn++)
   {

    polen(sheta_pnal.ravno(),strsql,sizeof(strsql),nn+1,';');
    if(strsql[0] == '\0')
     continue;

    priz_nv=0;

    polen(strsql,bros,sizeof(bros),1,':');

    if(bros[0] == '+')
     priz_nv=1;

    if(bros[0] == '-')
     priz_nv=2;

    if(bros[0] == '*')
     priz_nv=3;

    if(priz_nv == 0)
     continue;

    polen(strsql,bros,sizeof(bros),2,':');
    if(bros[0] == 'd')
     priz_deb_kre=1;
    else
     priz_deb_kre=2;
      
    polen(strsql,&shet_deb,3,':');
    polen(strsql,&shet_kre,4,':');
    zarkvrt_gk_r(dn,mn,gn,dk,mk,gk,shet_deb.ravno(),shet_kre.ravno(),&kod_kom,&podoh_vip,&podoh_nah,priz_nv,priz_deb_kre,ff_prot);
     
   }
 }  

//Списки должны быть с одинаковым количеством элементов
int raznica=podoh_vip.kolih()-suma_vip.kolih();
if(raznica > 0)
 for(int i=0; i < raznica; i++)
  {
    suma_vip.plus(0.,-1);
    suma_nah.plus(0.,-1);
  }

printw("Записываем данные:%d\n",kod_kom.kolih());
refresh();
class iceb_tu_str fio;
class iceb_tu_str inn("");
class iceb_tu_str kod("");
double suma_v;
double suma_n;
double podoh_n;
double podoh_v;

SQL_str row;
SQLCURSOR cur;
for(int nom=0; nom < kod_kom.kolih(); nom++)
 {
  inn.new_plus("");
  fio.new_plus("");
  kod.new_plus(kod_kom.ravno(nom));
  
  if(strstrm(kod.ravno(),prist_k_tabnom) == 1)
   {
    sprintf(strsql,"select fio,inn from Kartb where tabn=%s",iceb_tu_adrsimv(iceb_tu_strlen(prist_k_tabnom),kod.ravno()));
    if(readkey(strsql,&row,&cur) > 0)
     {
      fio.new_plus(row[0]);
      inn.new_plus(row[1]);
     }
    
   }
  else
   {
    sprintf(strsql,"select naikon,kod,regnom from Kontragent where kodkon='%s'",kod.ravno());
    if(readkey(strsql,&row,&cur) == 1 )
     {
      if(row[2][0] == '\0')
       continue;
      fio.new_plus(row[0]);
      inn.new_plus(row[1]);
     }
    else
     continue;
   } 
  suma_v=suma_vip.ravno(nom);
  suma_n=suma_nah.ravno(nom);
  podoh_v=podoh_vip.ravno(nom);
  podoh_n=podoh_nah.ravno(nom);

  
  if(iceb_tu_strlen(inn.ravno()) < 10)
   {
    sprintf(strsql,"%s %s %s!",gettext("Идентификационный номер имеет меньше 10 знаков"),inn.ravno(),kod.ravno());
    iceb_t_soob(strsql);
   }   

  printw("%*s %s\n",iceb_tu_kolbait(6,kod.ravno()),kod.ravno(),fio.ravno());
  zarkvrt_zapvtabl(imatmptab,inn.ravno(),kod_doh,"",suma_n,suma_v,podoh_n,podoh_v,"0000-00-00","0000-00-00",fio.ravno_filtr(),kod.ravno(),ff_prot);

 }

return(0);

}
/********************************/
/*чтение проводок*/
/**********************/

int zarkvrt_gk_r(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *shet_deb,
const char *shet_kre,
SPISOK *kod_kom,
masiv_din_double *suma_vip,
masiv_din_double *suma_nah,
int priz_nv,
int priz_deb_kre,
FILE *ff_prot)
{
char strsql[512];
//printw("%s\n",__FUNCTION__);
//refresh();
//Берём строго за квартал
if(priz_deb_kre == 1)  
    sprintf(strsql,"select sh,shk,kodkon,deb,datp from Prov where datp >= '%04d-%d-%d' and \
datp <= '%04d-%d-%d' and kodkon <> '' and val=0 and deb <> 0. \
order by datp asc",gn,mn,1,gn,mn+2,31); 
else
    sprintf(strsql,"select sh,shk,kodkon,kre,datp from Prov where datp >= '%04d-%d-%d' and \
datp <= '%04d-%d-%d' and kodkon <> '' and val=0 and kre <> 0. \
order by datp asc",gn,mn,1,gn,mn+2,31); 

fprintf(ff_prot,"shet_deb=%s\nshet_kre=%s\n%s\n",shet_deb,shet_kre,strsql);


SQL_str row;
SQLCURSOR cur;
int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
//printw("kolstr=%d\n",kolstr);
if(kolstr == 0)
 {
  printw("Нужных проводок не обнаружено\n");
  fprintf(ff_prot,"Нужных проводок не обнаружено\n");
  return(1);
 }

int kolstr1=0;
int nomer;
double deb;
while(cur.read_cursor(&row) != 0)
 {
  strzag(LINES-1,0,kolstr,++kolstr1);

  if(proverka(shet_deb,row[0],1,0) != 0)
     continue;
  if(proverka(shet_kre,row[1],1,0) != 0)
     continue;

  fprintf(ff_prot,"%s %-6s %-6s %-6s %10s\n",row[4],row[0],row[1],row[2],row[3]);
  
  if((nomer=kod_kom->find_r(row[2])) < 0)
    kod_kom->plus(row[2]);


  
  deb=atof(row[3]);
  //Количество элементов массива должно быть равным
  if(priz_nv == 1)
   {
    suma_nah->plus(deb,nomer);  
    if(nomer < 0 )
     suma_vip->plus(0.,nomer);  
   }
  if(priz_nv == 2)
   {
    suma_vip->plus(deb,nomer);  
    if(nomer < 0 )
      suma_nah->plus(0.,nomer);  
   }

  if(priz_nv == 3)
   {
    suma_vip->plus(deb,nomer);  
    suma_nah->plus(deb,nomer);  
   }

 }

return(0);
}

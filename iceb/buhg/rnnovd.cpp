/*$Id: rnnovd.c,v 5.6 2013/05/17 14:56:10 sasa Exp $*/
/*15.07.2015	13.12.2009	Белых А.И.	rnnovd.c
Чтение настроек для определения вида документа
*/
#include "buhg.h"
#include "rnnovd.h"

int rnnovd(int metka,/*1-чтение настроек для приходных 2-для расходных*/
class rnnovd_rek *nvd)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
int metka_pods=0; /*1-материальный учёт 2 учёт услуг 3 учёт основных средств 4 учёт командировочных расходов*/
class iceb_tu_str imaf;
class iceb_tu_str viddok("");
class iceb_tu_str nastr("");

if(metka == 1)
 imaf.plus("rnnovdp.alx");
if(metka == 2)
 imaf.plus("rnnovdr.alx");

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf.ravno());
  iceb_t_soob(strsql);
  return(1);
 }



while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(strstrm(row_alx[0],"Материальный учёт") == 1)
   {
    metka_pods=1;
    continue;
   } 

  if(strstrm(row_alx[0],"Учёт услуг") == 1)
   {
    metka_pods=2;
    continue;
   } 

  if(strstrm(row_alx[0],"Учёт основных средств") == 1)
   {
    metka_pods=3;
    continue;
   } 

  if(strstrm(row_alx[0],"Учёт командировочных расходов") == 1)
   {
    metka_pods=4;
    continue;
   } 
  if(metka_pods == 0)
   continue;

  if(polen(row_alx[0],&viddok,1,'|') != 0)
   continue;  

  if(polen(row_alx[0],&nastr,2,'|') != 0)
   continue;  

  if(metka_pods == 1)
   {
    nvd->muvd.plus(viddok.ravno());
    nvd->muso.plus(nastr.ravno());
   }
  if(metka_pods == 2)
   {
    nvd->uuvd.plus(viddok.ravno());
    nvd->uuso.plus(nastr.ravno());
   }
  if(metka_pods == 3)
   {
    nvd->uosvd.plus(viddok.ravno());
    nvd->uosso.plus(nastr.ravno());
   }
  if(metka_pods == 4)
   {
    nvd->ukrvd.plus(viddok.ravno());
    nvd->ukrsr.plus(nastr.ravno());
   }
 }


return(0);
}


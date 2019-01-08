/*$Id: ostkar1.c,v 5.3 2013/05/17 14:56:06 sasa Exp $*/
/*30.03.2011	30.03.2011	Белых А.И.	ostkar1.c
получение реального остатка материала по всем карточкам
*/

#include "buhg.h"

double ostkar1(int kodmat,int sklad,short dost,short most,short gost)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

if(sklad != 0)
 sprintf(strsql,"select sklad,nomk from Kart where sklad=%d and kodm=%d",sklad,kodmat);
else
 sprintf(strsql,"select sklad,nomk from Kart where kodm=%d",kodmat);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  return(0.);
 }
double ostatok=0.;
class ostatokt ost;
while(cur.read_cursor(&row) != 0)
 {
  ostkar(1,1,gost,dost,most,gost,atoi(row[0]),atoi(row[1]),&ost);
  ostatok+=ost.ostg[3];  
 }
return(ostatok);
 
}


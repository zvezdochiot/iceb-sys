/* $Id: ostdok1.c,v 5.12 2013/05/17 14:56:06 sasa Exp $ */
/*07.04.2003	22.02.1999	Белых А.И.	ostdok1.c
Получение остатка материалла исходя только из записей в документах
в том числе не привязанных к карточкам и из таблицы розничной тоговли
Возвращяет значение остатка материалла
*/
#include        <math.h>
#include	"buhg.h"

extern short	startgod; /*Стартовый год просмотров*/

double ostdok1(short dk,short mk,short gk,int skl,int kodm)
{
SQL_str         row;
char		strsql[512];
int		kolstr;
double		kolih=0.;
double		suma=0.;
short		tp;
short		godn;
short		d,m,g;

godn=startgod;
if(startgod == 0 || gk < startgod )
 godn=gk;

/* 
printw("ostdok1 начал работу godn=%d startgod=%d\n\
%d.%d.%d skl=%d kodm=%d",godn,startgod,dk,mk,gk,skl,kodm);
OSTANOV();
*/

if(skl != 0)
 sprintf(strsql,"select datd,nomd,kolih,tipz from Dokummat1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and kodm=%d and \
sklad=%d order by datd asc",
 godn,1,1,gk,mk,dk,kodm,skl);
else
 sprintf(strsql,"select datd,nomd,kolih,tipz from Dokummat1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and kodm=%d \
order by datd asc",
 godn,1,1,gk,mk,dk,kodm);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(0.);
 }
while(cur.read_cursor(&row) != 0)
 {
  rsdat(&d,&m,&g,row[0],2);
  /*Пpопускаем стаpтовые остатки если это остатки не года начала поиска*/
  if(SRAV(row[1],"000",0) == 0 && g != godn)
   continue;

  tp=atoi(row[3]);
  suma=atof(row[2]);
  if(tp == 1)
    kolih+=suma;
  if(tp == 2)
    kolih-=suma;

 }

/*
printw("Просмотр таблицы реализации.\n");
refresh();
*/

if(skl != 0)
 sprintf(strsql,"select kolih from Roznica where \
datrp >= '%04d-%02d-%02d' and datrp <= '%04d-%02d-%02d' and metka=1 \
and kodm=%d and sklad=%d",
 gk,1,1,gk,mk,dk,kodm,skl);
else
 sprintf(strsql,"select kolih from Roznica where \
datrp >= '%04d-%02d-%02d' and datrp <= '%04d-%02d-%02d' and metka=1 and \
kodm=%d",
 gk,1,1,gk,mk,dk,kodm);
//printw("%s\n",strsql);
//OSTANOV();
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  printw("%s\n",sql_error(&bd));
  OSTANOV();
 }

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
  kolih-=atof(row[0]);

/*
printw("Закончил %d %f\n",kodm,kolih);
OSTANOV();
*/

if(fabs(kolih) < 0.000000001)
  kolih=0.;
return(kolih);
 
}

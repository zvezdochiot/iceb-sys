/* $Id: zapzagot.c,v 5.13 2013/08/13 05:50:01 sasa Exp $ */
/*20.04.2016    09.11.1992      Белых А.И.      zapzagot.c
Подпрограмма поиска и записи необходимых начислений
Если вернули 1 значит не введены коды начислений и удержаний
             2 у оператора доступ только на чтение
*/

#include        "buhg.h"

int	zapzagot1(short,short,long,int,int);

extern struct   passwd  ktor; /*Кто работает*/
extern short	*obud; /*Обязательные удержания*/
extern short	*obnah; /*Обязательные начисления*/
int             zapzagot(short mp,short gp,
long tabn,
int prn,  //1-начисление 2-удержание
int podr)
{
short		d,m,g;
long		kolstr;
SQL_str         row;
SQLCURSOR cur;
char		strsql[512];
time_t		vrem;
class ZARP     zp;
short		den;

if(podr == 0)
 {
  sprintf(strsql,"select podr from Kartb where tabn=%ld",tabn);
  if(readkey(strsql,&row,&cur) == 1)
   podr=atoi(row[0]);  
 }

sprintf(strsql,"select * from Zarn1 where tabn=%ld and prn='%d'",
tabn,prn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(3);
 }

if(kolstr == 0)
 {
  if(zapzagot1(mp,gp,tabn,prn,podr) == 0)
   return(1);
  else
   return(0);
 }
dpm(&den,&mp,&gp,5);
zp.dz=den;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

time(&vrem);
while(cur.read_cursor(&row) != 0)
 {
  if(rsdat(&d,&m,&g,row[3],1) != 0)
    d=m=g=0;

  if((m != 0 && m < mp) && (g != 0 && g <= gp ))
    continue;
  strcpy(zp.shet,row[4]);
  if(zapzarp(den,mp,gp,tabn,prn,atoi(row[2]),0.,row[4],mp,gp,0,0,"\0",podr,"",zp) == 3)
    return(2);

 }

return(0);

}
/****************************************************************/
/* Запись в пустую настройку обязательных начислений и удержаний*/
/****************************************************************/
/*Возвращает количество сделанных записей*/

int	zapzagot1(short mp,short gp,long tabn,int prn,int podr)
{
int		i;
short		d;
class iceb_tu_str shet("");
char		strsql[512];
SQL_str		row;
int		kolzap;
class ZARP     zp;
SQLCURSOR curr;


kolzap=0;
if(prn == 1 && obnah != NULL)
 {
  d=0;

  dpm(&d,&mp,&gp,5);
  zp.dz=d;
  zp.mesn=mp; zp.godn=gp;
  zp.nomz=0;
  zp.podr=podr;
  for(i=1; i <= obnah[0]; i++)
   {
    shet.new_plus("");
    /*Поверяем есть ли удержание в списке удержаний*/
    sprintf(strsql,"select shet from Nash where kod=%d",obnah[i]);
//    printw("kod=%d\n",obnah[i]);
//    OSTANOV();
    
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     {
      shet.new_plus(row[0]);
      if(shet.ravno()[0] == '\0')
       {
        sprintf(strsql,"select shet from Kartb where tabn=%ld",tabn);
        if(sql_readkey(&bd,strsql,&row,&curr) == 1)
         shet.new_plus(row[0]);

       }       

      if(shet.ravno()[0] != '\0')
       {
        strcpy(zp.shet,shet.ravno());
        zapzarp(d,mp,gp,tabn,prn,obnah[i],0.,shet.ravno(),mp,gp,0,0,"\0",podr,"",zp); 
        zapzarn1(tabn,(short)prn,obnah[i],0,0,0,shet.ravno());
        kolzap++;
       }
     }

   }  

 }
if(prn == 2 && obud != NULL)
 {
  d=0;

  dpm(&d,&mp,&gp,5);
  zp.dz=d;
  zp.mesn=mp; zp.godn=gp;
  zp.nomz=0;
  zp.podr=podr;
  for(i=1; i <= obud[0]; i++)
   {
    /*Поверяем есть ли удержание в списке удержаний*/
    sprintf(strsql,"select shet from Uder where kod=%d",obud[i]);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     {
      if(row[0][0] == '\0') /*Без счёта удержание не может быть записано!!!*/
       continue;
      shet.new_plus(row[0]);
      strcpy(zp.shet,shet.ravno());
      zapzarp(d,mp,gp,tabn,prn,obud[i],0.,shet.ravno(),mp,gp,0,0,"\0",podr,"",zp); 
      zapzarn1(tabn,(short)prn,obud[i],0,0,0,shet.ravno());
      kolzap++;
     }

   }  

 }
return(kolzap);
}

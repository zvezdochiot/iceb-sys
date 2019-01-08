/*$Id:$*/
/*07.06.2017 	15.05.2017	Белых А.И.	uosavp.cpp
Распечатка акта внутреннего перемещения основных средств
*/
#include "buhg.h"

int uosavp(short dd,short md,short gd,const char *nomdok)
{
int metka_u=0; /*0-налоговый 1-бухгалтерский учёт*/
char strsql[512];
char imaf[64];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
class iceb_vrvr rv;

rv.ns.plus(4);
rv.begin.plus(0);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(iceb_t_get_pnk("00",1));

rv.ns.plus(7);
rv.begin.plus(33);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(iceb_t_get_edrpou("00"));

rv.ns.plus(15);
rv.begin.plus(81);
rv.end.plus(95);
rv.hag.plus(1);
rv.str.plus(nomdok);

sprintf(strsql,"%02d.%02d.%d",dd,md,gd);
rv.ns.plus(15);
rv.begin.plus(98);
rv.end.plus(108);
rv.hag.plus(1);
rv.str.plus(strsql);

class VVOD MENU(3);
MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужное"));

MENU.VVOD_SPISOK_add_MD(gettext("Налоговый учёт"));
MENU.VVOD_SPISOK_add_MD(gettext("Бухгалтерский учёт"));
while(menu3w(stdscr,&MENU,&metka_u,-1,-1,0) != 0);

if(metka_u == -1)
 return(1);

sprintf(strsql,"select innom,bs,bsby from Uosdok1 where datd='%d.%02d.%02d' and nomd='%s'",gd,md,dd,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,__FUNCTION__,strsql);
  return(1);
 }

sprintf(imaf,"avp%d.lst",getpid());
class iceb_fopen fil;
if(fil.start(imaf,"w") != 0)
 return(1);

iceb_t_vrvr("uosavp_begin.alx",&rv,fil.ff); /*вставка реквизитов в шапку документа*/

if(kolstr == 0)
 {
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
 }

double bal_st=0.;
int itogo_kol=0;
double itogo_sum=0;
class iceb_tu_str naim_os("");
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
  if(readkey(strsql,&row1,&cur1) == 1)
   {
    naim_os.new_plus(row1[0]);
   }
  else
   {
    naim_os.new_plus("");
   }

  if(metka_u == 0)
   {
    bal_st=atof(row[1]);
   }
  else
   {
    bal_st=atof(row[2]);
   }   

  fprintf(fil.ff,"|%-*.*s|%-15s|%-*s|%9s|%13.2f|%12s|%8s|\n",
  iceb_tu_kolbait(40,naim_os.ravno()),
  iceb_tu_kolbait(40,naim_os.ravno()),
  naim_os.ravno(),
  row[0],
  iceb_tu_kolbait(7,"Шт"),
  "Шт",
  "1",
  bal_st,
  "",
  "");    

  itogo_kol++;    
  itogo_sum+=bal_st;
 }

fprintf(fil.ff,"----------------------------------------------------------------------------------------------------------------\n");
fprintf(fil.ff,"%*s|%9d|%13.2f|\n",
iceb_tu_kolbait(65,"Всього"),
"Всього",
itogo_kol,
itogo_sum);

rv.free_class();
class iceb_tu_str stroka("");

preobr((double)itogo_kol,&stroka,2);

//sprintf(strsql,"%d",itogo_kol);
rv.ns.plus(3);
rv.begin.plus(19);
rv.end.plus(31);
rv.hag.plus(1);
rv.str.plus(stroka.ravno());

stroka.new_plus("");
preobr(itogo_sum,&stroka,0);

//sprintf(strsql,"%.2f",itogo_sum);
rv.ns.plus(3);
rv.begin.plus(45);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(stroka.ravno());

iceb_t_vrvr("uosavp_end.alx",&rv,fil.ff); /*вставка концовки*/

podpis(fil.ff);
fil.end();

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Распечатка акта внутреннего перемещения основных средств"));

iceb_t_ustpeh(imaf,3);

iceb_t_rabfil(&oth,"");


return(0);
}

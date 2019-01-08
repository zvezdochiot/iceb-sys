/*$Id: vib_kodop_mu.c,v 1.8 2013/05/17 14:58:29 sasa Exp $*/
/*22.11.2005	07.04.2005	Белых А.И.	vib_kodop_mu.c
Выбор кода операции для материальног учёта
*/
#include "buhg_g.h"


int vib_kodop_mu(class iceb_u_str *kodop,GtkWidget *wpredok)
{
short nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр списка операций приходов"));
punkt_m.plus(gettext("Просмотр списка операций расходов"));

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);


switch (nomer)
 {
  case -1:
    return(1);
    
  case 0:
    if(iceb_vibrek(0,"Prihod",kodop,wpredok) == 0)
     return(0);
    break;

  case 1:
    if(iceb_vibrek(0,"Rashod",kodop,wpredok) == 0)
     return(0);
    break;

 }
return(1);
}


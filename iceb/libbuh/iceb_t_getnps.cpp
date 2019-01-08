/*$Id: iceb_t_getnps.c,v 5.4 2013/05/17 14:56:28 sasa Exp $*/
/*06.09.2010	06.09.2010	Белых А.И.	iceb_t_getnps.c
Получение наименования подсистемы
*/
#include "buhl.h"

extern int iceb_kod_podsystem;

const char *iceb_t_getnps()
{

switch(iceb_kod_podsystem)
 {
  case ICEB_PS_GK:
   return(gettext("Главная книга"));
  case ICEB_PS_MU:
   return(gettext("Материальный учёт"));
  case ICEB_PS_PD:
   return(gettext("Платёжные документы"));
  case ICEB_PS_ZP:
   return(gettext("Заработная плата"));
  case ICEB_PS_UOS:
   return(gettext("Учёт основных средств"));
  case ICEB_PS_UU:
   return(gettext("Учёт услуг"));
  case ICEB_PS_UKO:
   return(gettext("Учёт кассовых ордеров"));
  case ICEB_PS_UKR:
   return(gettext("Учёт командировочных расходов"));
  case ICEB_PS_UPL:
   return(gettext("Учёт путевых листов"));
  case ICEB_PS_RNN:
   return(gettext("Реестр налоговых накладных"));
  case ICEB_PS_UD:
   return(gettext("Учёт доверенностей"));
    
 };

return(gettext("Неизвестная подсистема"));

}
/*$Id: buhmesshet.c,v 5.6 2013/05/17 14:55:56 sasa Exp $*/
/*09.02.2011	02.09.2009	Белых А.И.	buhmesshet.c
Распечтака шахматки месяц-счета
*/
#include <math.h>
#include "buhg.h"


void buhmesshet(class SPISOK *mes,class masiv_din_double *mes_shet,class SPISOK *sheta,FILE *ff)
{
int kolih_shet=sheta->kolih();

class iceb_tu_str liniq("--------");
for(int nomer=0; nomer < kolih_shet+1; nomer++)
 liniq.plus("-----------");

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff," Дата  |");

for(int nomer_sheta=0; nomer_sheta < kolih_shet; nomer_sheta++)
  fprintf(ff,"%*.*s|",
  iceb_tu_kolbait(10,sheta->ravno(nomer_sheta)),
  iceb_tu_kolbait(10,sheta->ravno(nomer_sheta)),
  sheta->ravno(nomer_sheta));

fprintf(ff,"%-*.*s|\n",
iceb_tu_kolbait(10,gettext("Итого")),
iceb_tu_kolbait(10,gettext("Итого")),
gettext("Итого"));

fprintf(ff,"%s\n",liniq.ravno());
class masiv_din_double itogo_kol;
itogo_kol.make_class(kolih_shet);
double suma=0.;
double itogo_gor=0.;
for(int nomer_mes=0; nomer_mes < mes->kolih(); nomer_mes++)
 {
  itogo_gor=0.;
  fprintf(ff,"%s|",mes->ravno(nomer_mes));
  for(int nomer_sheta=0; nomer_sheta < kolih_shet; nomer_sheta++)
   {
    suma=mes_shet->ravno(nomer_mes*kolih_shet+nomer_sheta);
    if(fabs(suma) > 0.009)
     {
      fprintf(ff,"%10.2f|",suma);
      itogo_gor+=suma;
      itogo_kol.plus(suma,nomer_sheta);
     }
    else
     fprintf(ff,"%10s|","");
  
   }
  fprintf(ff,"%10.2f|\n",itogo_gor);
  fprintf(ff,"%s\n",liniq.ravno());
 }

fprintf(ff,"%*.*s|",
iceb_tu_kolbait(7,gettext("Итого")),
iceb_tu_kolbait(7,gettext("Итого")),
gettext("Итого"));

for(int nomer=0; nomer < kolih_shet; nomer++)
 fprintf(ff,"%10.2f|",itogo_kol.ravno(nomer));
fprintf(ff,"%10.2f|\n",itogo_kol.suma());
fprintf(ff,"%s\n",liniq.ravno());

}


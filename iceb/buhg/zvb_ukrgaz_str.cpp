/*$Id: zvb_ukrgaz_str.c,v 5.4 2013/05/17 14:56:19 sasa Exp $*/
/*29.10.2009	22.10.2009	Белых А.И.	zvb_ukrgaz_str.c
Вывод строк в файлы
*/
#include "buhg.h"

void zvb_ukrgaz_str(int nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *tabnom,
FILE *ff_dbf_tmp,FILE *ffras)
{

fprintf(ff_dbf_tmp," %-*.*s%-*.*s%-*.*s%10.2f",
iceb_tu_kolbait(40,fio),iceb_tu_kolbait(40,fio),fio,
iceb_tu_kolbait(10,inn),iceb_tu_kolbait(10,inn),inn,
iceb_tu_kolbait(20,nomersh),iceb_tu_kolbait(20,nomersh),nomersh,
suma);

fprintf(ffras,"\
%6d %-*s %-*s %-*s %-*s %10.2f\n",
nomerpp,
iceb_tu_kolbait(6,tabnom),tabnom,
iceb_tu_kolbait(40,fio),fio,
iceb_tu_kolbait(10,inn),inn,
iceb_tu_kolbait(14,nomersh),nomersh,
suma);


}

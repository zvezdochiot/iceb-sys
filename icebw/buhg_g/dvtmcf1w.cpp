/*$Id: dvtmcf1w.c,v 1.9 2013/08/13 06:09:34 sasa Exp $*/
/*23.11.2004	22.11.2004	Белых А.И.	dvtmcf1w.c
Расчёт движения товарно-материальных ценностей форма 1
*/
#include "buhg_g.h"
#include "dvtmcf1.h"

int   dvtmcf1_m(class dvtmcf1_rr *rek_ras);
int dvtmcf1_r(class dvtmcf1_rr *datark,GtkWidget *wpredok);

iceb_u_str dvtmcf1_rr::datan;
iceb_u_str dvtmcf1_rr::datak;
iceb_u_str dvtmcf1_rr::shet;
iceb_u_str dvtmcf1_rr::sklad;
iceb_u_str dvtmcf1_rr::grupa;
iceb_u_str dvtmcf1_rr::kodmat;
iceb_u_str dvtmcf1_rr::nds;
int        dvtmcf1_rr::metka_sort;

void dvtmcf1w()
{
class dvtmcf1_rr data;
if(data.datan.getdlinna() == 0)
 data.clear_data();
 
if(dvtmcf1_m(&data) != 0)
 return;

if(dvtmcf1_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,NULL);

}

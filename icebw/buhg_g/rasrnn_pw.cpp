/*$Id: rasrnn_pw.c,v 1.42 2014/08/31 06:19:21 sasa Exp $*/
/*01.12.2014	01.12.2014	Белых А.И.	rasrnn_pw.c
Распечатка реестра полученных налоговых накладных 
*/
#include "buhg_g.h"
#include "xrnn_poiw.h"
#include "rnn_d5w.h"

int rasrnn_p7w(class xrnn_poi *data,int kol_strok,int period_type,FILE *ff,FILE *ff_prot,FILE *ff_xml,FILE *ff_dot,FILE *ff_xml_dot,FILE *ff_xml_rozkor,FILE *ff_lst_rozkor,class rnn_d5 *dod5,GtkWidget *bar,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);
int rasrnn_p8w(class xrnn_poi *data,int period_type,FILE *ff,FILE *ff_prot,FILE *ff_xml,FILE *ff_dot,FILE *ff_xml_dot,FILE *ff_xml_rozkor,FILE *ff_lst_rozkor,class rnn_d5 *dod5,GtkWidget *bar,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);

int rasrnn_pw(class xrnn_poi *data,
int period_type,
FILE *ff,FILE *ff_prot,FILE *ff_xml,
FILE *ff_dot,FILE *ff_xml_dot,
FILE *ff_xml_rozkor,
FILE *ff_lst_rozkor,
class rnn_d5 *dod5,
GtkWidget *bar,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
if(iceb_u_sravmydat(data->datan.ravno(),"1.12.2014") < 0)
 {
  int kol_strok=0;
  return(rasrnn_p7w(data,kol_strok,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,ff_xml_rozkor,ff_lst_rozkor,dod5,bar,view,buffer,wpredok));
 }
else
  return(rasrnn_p8w(data,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,ff_xml_rozkor,ff_lst_rozkor,dod5,bar,view,buffer,wpredok));

return(1);
}
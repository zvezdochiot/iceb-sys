/*$Id: f1df_xml_zag.c,v 5.13 2014/04/30 06:12:26 sasa Exp $*/
/*19.12.2014	23.04.2007	Белых А.И.	f1df_xml_zag.c
Заголовок файла xml формы 1ДФ 
*/
#include "buhg.h"

void f1df_xml_zag(int kvrt,
int god,
const char *kod,
int TYP,
int metka_oth,
int kolih_v_htate,
int kolih_po_sovm,
const char *naim_kontr,
char *imaf_xml,
FILE **ff_xml)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];

class iceb_tu_str naim_ni("");
class iceb_tu_str kodni("");
iceb_t_poldan("Код налоговой инспекции",&kodni,"zarsdf1df.alx");

iceb_t_poldan("Название налоговой инспекции",&naim_ni,"zarsdf1df.alx");
class iceb_tu_str adres("");
/*узнаём адрес*/
sprintf(strsql,"select adres from Kontragent where kodkon='00'");
if(readkey(strsql,&row,&cur) == 1)
 adres.new_plus(row[0]);

short mn=0;
short mk=0;
short dk=31;
int period_type=0;
if(kvrt == 1)
 {
  mn=1;
  mk=3;
 }
if(kvrt == 2)
 {
  mn=4;
  mk=6;
 }
if(kvrt == 3)
 {
  mn=7;
  mk=9;
 }
if(kvrt == 4)
 {
  mn=10;
  mk=12;
 }

class iceb_rnfxml_data rek_zag_nn;
iceb_t_rnfxml(&rek_zag_nn); /*чтение данных для xml файла*/
/******************
if(iceb_t_openxml(1,mn,god,dk,mk,god,imaf_xml,"J05","001","04",1,&period_type,&rek_zag_nn,ff_xml) != 0)
 return;

iceb_t_rnn_sap_xml(mn,god,mk,"J05","001",4,1,"J0500104.XSD",period_type,&rek_zag_nn,*ff_xml);
************************/
if(iceb_t_openxml(1,mn,god,dk,mk,god,imaf_xml,iceb_t_getkoddok(1,mn,god,6),1,&period_type,&rek_zag_nn,ff_xml) != 0)
 return;

iceb_t_rnn_sap_xml(mn,god,mk,iceb_t_getkoddok(1,mn,god,6),1,period_type,&rek_zag_nn,*ff_xml);

fprintf(*ff_xml," <DECLARBODY>\n");
fprintf(*ff_xml,"  <HTIN>%s</HTIN>\n",kod);
fprintf(*ff_xml,"  <HPAGES>1</HPAGES>\n");
/****************
if(TYP == 0)
 fprintf(*ff_xml,"  <HJ>1</HJ>\n");
else
 fprintf(*ff_xml,"  <HF>1</HF>\n");
***************/
if(TYP == 1)
 fprintf(*ff_xml,"  <HF>1</HF>\n");
else
 fprintf(*ff_xml,"  <HF>1</HF>\n");

if(metka_oth == 0)
 fprintf(*ff_xml,"  <HZ>1</HZ>\n");
else
 fprintf(*ff_xml,"  <HZ></HZ>\n");

if(metka_oth == 1)
 fprintf(*ff_xml,"  <HZN>1</HZN>\n");
else
 fprintf(*ff_xml,"  <HZN></HZN>\n");

if(metka_oth == 2)
 fprintf(*ff_xml,"  <HZU>1</HZU>\n");
else
 fprintf(*ff_xml,"  <HZU></HZU>\n");

fprintf(*ff_xml,"  <HNAME>%s</HNAME>\n",iceb_tu_filtr_xml(naim_kontr));
fprintf(*ff_xml,"  <HLOC>%s</HLOC>\n",iceb_tu_filtr_xml(adres.ravno()));
fprintf(*ff_xml,"  <HTINSTI>%s</HTINSTI>\n",kodni.ravno());
fprintf(*ff_xml,"  <HSTI>%s</HSTI>\n",naim_ni.ravno_filtr_xml());
fprintf(*ff_xml,"  <HZKV>%d</HZKV>\n",kvrt);
fprintf(*ff_xml,"  <HZY>%d</HZY>\n",god);

fprintf(*ff_xml,"  <R00G01I>%d</R00G01I>\n",kolih_v_htate);

fprintf(*ff_xml,"  <R00G02I>%d</R00G02I>\n",kolih_po_sovm);
}

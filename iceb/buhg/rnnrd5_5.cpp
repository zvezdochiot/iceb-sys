/*$Id: rnnrd5.c,v 5.40 2014/07/31 07:09:17 sasa Exp $*/
/*15.07.2015	03.04.2008	Белых А.И.	rnnrd5_5.c
Распечатка додатка 5
*/
#include <errno.h>
#include <ctype.h>
#include "buhg.h"
#include "rnn_d5.h"

void rnnrd5_sap2(FILE *ff);
//void rnnrd5_end(const char *fio_ruk,const char *fio_buh,FILE *ff);
void rnnrd5_sap(FILE *ff_svod);
void rnn_rk5_kon1(double suma_bez_nds2,double suma_nds2,double os_sr21,double os_sr22,double inh1,double inh2,FILE *ff_svod,FILE*);
void rnnrd5_endras(class rnn_d5 *dod5,double,int,FILE *ff_svod,FILE *ff_xml);

extern class iceb_rnfxml_data rek_zag_nn;

int rnnrd5_5(short dn,short mn,short gn,short dk,short mk,short gk,
char *imaf_xml,char *imaf_svod_naim,
char *imaf_xml_dot,char *imaf_svod_dot, /*Дополнительные отчёты*/
double proc_dot, /*Процент дотации*/
class rnn_d5 *dod5)
{
int period_type;
FILE *ff_xml=NULL;
FILE *ff_xml_dot=NULL;
/*********************
if(iceb_t_openxml(dn,mn,gn,dk,mk,gk,imaf_xml,"J02","151","05",1,&period_type,&rek_zag_nn,&ff_xml) != 0)
 return(1);

iceb_t_rnn_sap_xml(mn,gn,mk,"J02","151",5,1,"J0215105.XSD",period_type,&rek_zag_nn,ff_xml);
*************************/
if(iceb_t_openxml(dn,mn,gn,dk,mk,gk,imaf_xml,iceb_t_getkoddok(dn,mn,gn,4),1,&period_type,&rek_zag_nn,&ff_xml) != 0)
 return(1);

iceb_t_rnn_sap_xml(mn,gn,mk,iceb_t_getkoddok(dn,mn,gn,4),1,period_type,&rek_zag_nn,ff_xml);

fprintf(ff_xml," <DECLARBODY>\n");
fprintf(ff_xml,"   <HZ>1</HZ>\n"); /*1-звітний */
fprintf(ff_xml,"   <HZY>%d</HZY>\n",gk);
fprintf(ff_xml,"   <HZM>%d</HZM>\n",mk);
fprintf(ff_xml,"   <HZYP xsi:nil=\"true\"/>\n");
fprintf(ff_xml,"   <HNAME>%s</HNAME>\n",iceb_tu_filtr_xml(rek_zag_nn.naim_kontr.ravno()));
//fprintf(ff_xml,"   <HLOC>%s</HLOC>\n",iceb_tu_filtr_xml(rek_zag_nn.adres));
fprintf(ff_xml,"   <HTIN>%s</HTIN>\n",rek_zag_nn.kod_edrpuo.ravno());
fprintf(ff_xml,"   <HDDGVSD xsi:nil=\"true\"/>\n");
fprintf(ff_xml,"   <HNDGVSD xsi:nil=\"true\"/>\n");
fprintf(ff_xml,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn.ravno());
fprintf(ff_xml,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds.ravno());

if(proc_dot != 0.)
 {
/***********************
  if(iceb_t_openxml(dn,mn,gn,dk,mk,gk,imaf_xml_dot,"J02","851","06",1,&period_type,&rek_zag_nn,&ff_xml_dot) != 0)
   return(1);

  iceb_t_rnn_sap_xml(mn,gn,mk,"J02","851",6,1,"J0285106.XSD",period_type,&rek_zag_nn,ff_xml_dot);
*****************************/
  if(iceb_t_openxml(dn,mn,gn,dk,mk,gk,imaf_xml_dot,iceb_t_getkoddok(dn,mn,gn,5),1,&period_type,&rek_zag_nn,&ff_xml_dot) != 0)
   return(1);

  iceb_t_rnn_sap_xml(mn,gn,mk,iceb_t_getkoddok(dn,mn,gn,5),1,period_type,&rek_zag_nn,ff_xml_dot);

  fprintf(ff_xml_dot," <DECLARBODY>\n");
  fprintf(ff_xml_dot,"   <HZ>1</HZ>\n"); /*1-звітний */
  fprintf(ff_xml_dot,"   <HZY>%d</HZY>\n",gk);
  fprintf(ff_xml_dot,"   <HZM>%d</HZM>\n",mk);
  fprintf(ff_xml_dot,"   <HZYP xsi:nil=\"true\"/>\n");
  fprintf(ff_xml_dot,"   <HNAME>%s</HNAME>\n",iceb_tu_filtr_xml(rek_zag_nn.naim_kontr.ravno()));
  //fprintf(ff_xml_dot,"   <HLOC>%s</HLOC>\n",iceb_tu_filtr_xml(rek_zag_nn.adres));
  fprintf(ff_xml_dot,"   <HTINJ>%s</HTINJ>\n",rek_zag_nn.kod_edrpuo.ravno());
  fprintf(ff_xml_dot,"   <HDDGVSD xsi:nil=\"true\"/>\n");
  fprintf(ff_xml_dot,"   <HNDGVSD xsi:nil=\"true\"/>\n");
  fprintf(ff_xml_dot,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn.ravno());
  fprintf(ff_xml_dot,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds.ravno());
 
 }


FILE *ff_svod_dot=NULL;


if(proc_dot != 0.)
 {
  sprintf(imaf_svod_dot,"rnnsd%d.lst",getpid());

  if((ff_svod_dot=fopen(imaf_svod_dot,"w")) == NULL)
   {
    error_op_nfil(imaf_svod_dot,errno,"");
    return(1);
   }
 }

FILE *ff_svod_naim;

sprintf(imaf_svod_naim,"rnnsn%d.lst",getpid());

if((ff_svod_naim=fopen(imaf_svod_naim,"w")) == NULL)
 {
  error_op_nfil(imaf_svod_naim,errno,"");
  return(1);
 }

iceb_t_zagolov("Додаток 5",dn,mn,gn,dk,mk,gk,ff_svod_naim);



char kvrt[8];
char mes[8];
memset(kvrt,'\0',sizeof(kvrt));
memset(mes,'\0',sizeof(mes));
if(mn == mk && gn == gk)
  sprintf(mes,"%d",mk);
else
 {
  int kvt=0;
  if(mk <= 3)
   kvt=1;
  if( mk <= 6)
   kvt=2;
  if( mk <= 9)
   kvt=3;
  if( mk > 9)
   kvt=4;
   
  sprintf(kvrt,"%d",kvt);
 }
if(ff_svod_dot != NULL)
  iceb_t_zagolov("Додаток 2",dn,mn,gn,dk,mk,gk,ff_svod_dot);

if(ff_svod_dot != NULL)
  rnnrd5_sap(ff_svod_dot);

double inh1=0.,inh2=0.;


fprintf(ff_svod_naim,"Розділ І. Податкові зобов'язання\n");
fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod_naim,"\
  N  |Платник податку -   |Період складання |  Обсяги  | Сумма ПДВ|\n\
     |покупець (індивіду- |  податкових     | поставки |          |\n\
     |альний податковий   |  накладних      |(без ПДВ) |          |\n\
     | номер)             |                 |          |          |\n");
fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");


int nomer_str=0;
double suma_bez_nds=0.;
double suma_nds=0.;
double i_suma_bez_nds=0.;
double i_suma_nds=0.;
class iceb_tu_str period("");
short m=0,g=0;
class iceb_tu_str in2("");

for(int kkk=0; kkk < dod5->innn2.kolih(); kkk++)
 {
  for(int ii=0; ii < dod5->innn_per2.kolih(); ii++)
   {
    polen(dod5->innn_per2.ravno(ii),&in2,1,'|');  
    if(SRAV(dod5->innn2.ravno(kkk),in2.ravno(),0) != 0)
     continue;

    if(in2.ravno_atoi() == 0 || SRAV(in2.ravno(),"400000000000",0) == 0 )  /*Значит нет индивидуального налогового номера*/
     {
      inh1+=dod5->suma_bez_nds2.ravno(ii);
      inh2+=dod5->suma_nds2.ravno(ii);
      continue;
     }

    if(dod5->suma_bez_nds2.ravno(ii) == 0. && dod5->suma_nds2.ravno(ii) == 0.)
     continue;
    
    suma_bez_nds=dod5->suma_bez_nds2.ravno(ii);
    suma_nds=dod5->suma_nds2.ravno(ii);
    if(proc_dot != 0.)
     {
      suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
      suma_bez_nds=okrug(suma_bez_nds,0.01);

      suma_nds=suma_nds*(100.0-proc_dot)/100.;
      suma_nds=okrug(suma_nds,0.01);
      
     }
    
    i_suma_bez_nds+=suma_bez_nds;
    i_suma_nds+=suma_nds;

    polen(dod5->innn_per2.ravno(ii),&period,2,'|');  
    rsdat1(&m,&g,period.ravno());
    
    nomer_str++;
    
    if(ff_svod_dot != NULL)
     {
      fprintf(ff_svod_dot,"%5d|%-*s|%02d.%04d          |%10.2f|%10.2f|\n",
      nomer_str,
      iceb_tu_kolbait(20,in2.ravno()),in2.ravno(),
      m,g,
      dod5->suma_bez_nds2.ravno(ii)-suma_bez_nds,
      dod5->suma_nds2.ravno(ii)-suma_nds);
     }
    fprintf(ff_svod_naim,"%5d|%-*s|%02d.%04d          |%10.2f|%10.2f|%s\n",
    nomer_str,
    iceb_tu_kolbait(20,in2.ravno()),in2.ravno(),
    m,g,dod5->suma_bez_nds2.ravno(ii),dod5->suma_nds2.ravno(ii),dod5->naim_kontr2.ravno(ii));

    fprintf(ff_xml,"   <T1RXXXXG2 ROWNUM=\"%d\">%s</T1RXXXXG2>\n",nomer_str,in2.ravno());
    fprintf(ff_xml,"   <T1RXXXXG2A ROWNUM=\"%d\">%d</T1RXXXXG2A>\n",nomer_str,m);
    fprintf(ff_xml,"   <T1RXXXXG2B ROWNUM=\"%d\">%d</T1RXXXXG2B>\n",nomer_str,g);
    fprintf(ff_xml,"   <T1RXXXXG3 ROWNUM=\"%d\">%.2f</T1RXXXXG3>\n",nomer_str,suma_bez_nds);
    fprintf(ff_xml,"   <T1RXXXXG4 ROWNUM=\"%d\">%.2f</T1RXXXXG4>\n",nomer_str,suma_nds);
    
    if(ff_xml_dot != NULL)
     {
      fprintf(ff_xml_dot,"   <T1RXXXXG2 ROWNUM=\"%d\">%s</T1RXXXXG2>\n",nomer_str,in2.ravno());
      fprintf(ff_xml_dot,"   <T1RXXXXG2A ROWNUM=\"%d\">%d</T1RXXXXG2A>\n",nomer_str,m);
      fprintf(ff_xml_dot,"   <T1RXXXXG2B ROWNUM=\"%d\">%d</T1RXXXXG2B>\n",nomer_str,g);
      fprintf(ff_xml_dot,"   <T1RXXXXG3 ROWNUM=\"%d\">%.2f</T1RXXXXG3>\n",nomer_str,dod5->suma_bez_nds2.ravno(ii)-suma_bez_nds);
      fprintf(ff_xml_dot,"   <T1RXXXXG4 ROWNUM=\"%d\">%.2f</T1RXXXXG4>\n",nomer_str,dod5->suma_nds2.ravno(ii)-suma_nds);
     } 
   }
 }

double os1=dod5->os_sr2[0];
double os2=dod5->os_sr2[1];
double inhi1=inh1;
double inhi2=inh2;
if(proc_dot != 0)
 {
  os1=os1*(100.0-proc_dot)/100.;
  os2=os2*(100.0-proc_dot)/100.;
  os1=okrug(os1,0.01);  
  os2=okrug(os2,0.01);  

  inhi1=inhi1*(100.0-proc_dot)/100.;
  inhi1=okrug(inhi1,0.01);
  inhi2=inhi2*(100.0-proc_dot)/100.;
  inhi2=okrug(inhi2,0.01);
 }

rnn_rk5_kon1(i_suma_bez_nds+inh1,i_suma_nds+inh2,os1,os2,inhi1,inhi2,ff_svod_naim,ff_xml);

if(ff_svod_dot != NULL)
  rnn_rk5_kon1(dod5->suma_bez_nds2.suma()-i_suma_bez_nds+inh1-inhi1,
  dod5->suma_nds2.suma()-i_suma_nds+inh2-inhi2,\
  dod5->os_sr2[0]-os1,dod5->os_sr2[1]-os2,inh1-inhi1,inh2-inhi2,ff_svod_dot,ff_xml_dot);

fprintf(ff_svod_naim,"\nІнші операції, що оподатковуються з нульовою ставкою (рядок 2.2 декларації):\n");
fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");
if(ff_svod_dot != NULL)
 fprintf(ff_svod_dot,"\nІнші операції, що оподатковуються з нульовою ставкою (рядок 2.2 декларації):\n");

inh1=inh2=0.; 
i_suma_bez_nds=i_suma_nds=0.;
for(int kkk=0; kkk < dod5->innn2d.kolih(); kkk++)
 {
  for(int ii=0; ii < dod5->innn_per2d.kolih(); ii++)
   {
    polen(dod5->innn_per2d.ravno(ii),&in2,1,'|');  
    if(SRAV(dod5->innn2d.ravno(kkk),in2.ravno(),0) != 0)
     continue;

    if(in2.ravno_atoi() == 0)  /*Значит нет индивидуального налогового номера*/
     {
      inh1+=dod5->suma_bez_nds2d.ravno(ii);
//      inh2+=dod5->suma_nds2d.ravno(ii);
      continue;
     }

    if(dod5->suma_bez_nds2d.ravno(ii) == 0. && dod5->suma_nds2d.ravno(ii) == 0.)
     continue;
    
    suma_bez_nds=dod5->suma_bez_nds2d.ravno(ii);
    suma_nds=dod5->suma_nds2d.ravno(ii);
    if(proc_dot != 0.)
     {
      suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
      suma_bez_nds=okrug(suma_bez_nds,0.01);

      suma_nds=suma_nds*(100.0-proc_dot)/100.;
      suma_nds=okrug(suma_nds,0.01);
      
     }
    
    i_suma_bez_nds+=suma_bez_nds;
    i_suma_nds+=suma_nds;

    polen(dod5->innn_per2d.ravno(ii),&period,2,'|');  
    rsdat1(&m,&g,period.ravno());
    
    nomer_str++;
    
    if(ff_svod_dot != NULL)
     {
      fprintf(ff_svod_dot,"%5d|%-*s|%02d.%04d          |%10.2f|%10.2f|\n",
      nomer_str,
      iceb_tu_kolbait(20,in2.ravno()),in2.ravno(),
      m,g,
      dod5->suma_bez_nds2d.ravno(ii)-suma_bez_nds,
      dod5->suma_nds2d.ravno(ii)-suma_nds);
     }
    fprintf(ff_svod_naim,"%5d|%-*s|%02d.%04d          |%10.2f|%10.2f|%s\n",
    nomer_str,
    iceb_tu_kolbait(20,in2.ravno()),in2.ravno(),
    m,g,dod5->suma_bez_nds2d.ravno(ii),dod5->suma_nds2d.ravno(ii),dod5->naim_kontr2d.ravno(ii));

    fprintf(ff_xml,"   <T4RXXXXG2 ROWNUM=\"%d\">%s</T4RXXXXG2>\n",nomer_str,in2.ravno());
    fprintf(ff_xml,"   <T4RXXXXG3A ROWNUM=\"%d\">%d</T4RXXXXG3A>\n",nomer_str,m);
    fprintf(ff_xml,"   <T4RXXXXG3B ROWNUM=\"%d\">%d</T4RXXXXG3B>\n",nomer_str,g);
    fprintf(ff_xml,"   <T4RXXXXG4 ROWNUM=\"%d\">%.2f</T4RXXXXG4>\n",nomer_str,suma_bez_nds);
    
    if(ff_xml_dot != NULL)
     {
      fprintf(ff_xml_dot,"   <T4RXXXXG2 ROWNUM=\"%d\">%s</T4RXXXXG2>\n",nomer_str,in2.ravno());
      fprintf(ff_xml_dot,"   <T4RXXXXG3A ROWNUM=\"%d\">%d</T4RXXXXG3A>\n",nomer_str,m);
      fprintf(ff_xml_dot,"   <T4RXXXXG3B ROWNUM=\"%d\">%d</T4RXXXXG3B>\n",nomer_str,g);
      fprintf(ff_xml_dot,"   <T4RXXXXG4 ROWNUM=\"%d\">%.2f</T4RXXXXG4>\n",nomer_str,dod5->suma_bez_nds2d.ravno(ii)-suma_bez_nds);
     } 
   }
 }
inhi1=inh1;
if(proc_dot != 0)
 {
  inhi1=inhi1*(100.0-proc_dot)/100.;
  inhi1=okrug(inhi1,0.01);
 }

if(inh1 != 0.)
 {
  fprintf(ff_xml,"   <R004G4>%.2f</R004G4>\n",inhi1);
  if(ff_xml_dot != NULL)
   fprintf(ff_xml_dot,"   <R004G4>%.2f</R004G4>\n",inh1-inhi1);
 }
fprintf(ff_xml,"   <R040G4>%.2f</R040G4>\n",i_suma_bez_nds+inhi1);
if(ff_xml_dot != NULL)
  fprintf(ff_xml_dot,"   <R040G4>%.2f</R040G4>\n",dod5->suma_bez_nds2d.suma()-(i_suma_bez_nds+inhi1));


if(inh1 == 0.)
  fprintf(ff_svod_naim,"\
Інші                                   |               |          |\n");
else
  fprintf(ff_svod_naim,"\
Інші                                   |%15.2f|%10s|\n",inh1,"");


fprintf(ff_svod_naim,"\
Усього за звітний період (рядок 2.2)   |%15.2f|%10s|\n",i_suma_bez_nds+inh1,"");

fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");

if(ff_svod_dot != NULL)
 {
  if(inh1 == 0.)
    fprintf(ff_svod_dot,"\
Інші                                   |               |          |\n");
  else
    fprintf(ff_svod_dot,"\
Інші                                   |%15.2f|%10s|\n",inh1,"");


  fprintf(ff_svod_dot,"\
Усього за звітний період (рядок 2.2)   |%15.2f|%10s|\n",dod5->suma_bez_nds2d.suma()-(i_suma_bez_nds+inhi1),"");

  fprintf(ff_svod_dot,"\
-------------------------------------------------------------------\n");
 }

fprintf(ff_svod_naim,"\nОперації які звільнені від оподаткування(рядок 5 декларації):\n");
if(ff_svod_dot != NULL)
 fprintf(ff_svod_dot,"\nОперації які звільнені від оподаткування(рядок 5 декларації):\n");
inh1=inh2=0.; 
i_suma_bez_nds=i_suma_nds=0.;
for(int kkk=0; kkk < dod5->innn2d1.kolih(); kkk++)
 {
  for(int ii=0; ii < dod5->innn_per2d1.kolih(); ii++)
   {
    polen(dod5->innn_per2d1.ravno(ii),&in2,1,'|');  
    if(SRAV(dod5->innn2d1.ravno(kkk),in2.ravno(),0) != 0)
     continue;

    if(in2.ravno_atoi() == 0)  /*Значит нет индивидуального налогового номера*/
     {
       
      inh1+=dod5->suma_bez_nds2d1.ravno(ii);
  //    inh2+=dod5->suma_nds2d1.ravno(ii);
      continue;
     }

    if(dod5->suma_bez_nds2d1.ravno(ii) == 0. && dod5->suma_nds2d1.ravno(ii) == 0.)
     continue;
    
    suma_bez_nds=dod5->suma_bez_nds2d1.ravno(ii);
    suma_nds=dod5->suma_nds2d1.ravno(ii);
    if(proc_dot != 0.)
     {
      suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
      suma_bez_nds=okrug(suma_bez_nds,0.01);

      suma_nds=suma_nds*(100.0-proc_dot)/100.;
      suma_nds=okrug(suma_nds,0.01);
      
     }
    
    i_suma_bez_nds+=suma_bez_nds;
    i_suma_nds+=suma_nds;

    polen(dod5->innn_per2d1.ravno(ii),&period,2,'|');  
    rsdat1(&m,&g,period.ravno());
    
    nomer_str++;
    
    if(ff_svod_dot != NULL)
     {
      fprintf(ff_svod_dot,"%5d|%-*s|%02d.%04d          |%10.2f|%10.2f|\n",
      nomer_str,
      iceb_tu_kolbait(20,in2.ravno()),in2.ravno(),
      m,g,
      dod5->suma_bez_nds2d1.ravno(ii)-suma_bez_nds,
      dod5->suma_nds2d1.ravno(ii)-suma_nds);
     }
    fprintf(ff_svod_naim,"%5d|%-*s|%02d.%04d          |%10.2f|%10.2f|%s\n",
    nomer_str,
    iceb_tu_kolbait(20,in2.ravno()),in2.ravno(),
    m,g,dod5->suma_bez_nds2d1.ravno(ii),dod5->suma_nds2d1.ravno(ii),dod5->naim_kontr2d1.ravno(ii));

    fprintf(ff_xml,"   <T5RXXXXG2 ROWNUM=\"%d\">%s</T5RXXXXG2>\n",nomer_str,in2.ravno());
    fprintf(ff_xml,"   <T5RXXXXG3A ROWNUM=\"%d\">%d</T5RXXXXG3A>\n",nomer_str,m);
    fprintf(ff_xml,"   <T5RXXXXG3B ROWNUM=\"%d\">%d</T5RXXXXG3B>\n",nomer_str,g);
    fprintf(ff_xml,"   <T5RXXXXG4 ROWNUM=\"%d\">%.2f</T5RXXXXG4>\n",nomer_str,suma_bez_nds);
    
    if(ff_xml_dot != NULL)
     {
      fprintf(ff_xml_dot,"   <T5RXXXXG2 ROWNUM=\"%d\">%s</T5RXXXXG2>\n",nomer_str,in2.ravno());
      fprintf(ff_xml_dot,"   <T5RXXXXG3A ROWNUM=\"%d\">%d</T5RXXXXG3A>\n",nomer_str,m);
      fprintf(ff_xml_dot,"   <T5RXXXXG3B ROWNUM=\"%d\">%d</T5RXXXXG3B>\n",nomer_str,g);
      fprintf(ff_xml_dot,"   <T5RXXXXG4 ROWNUM=\"%d\">%.2f</T5RXXXXG4>\n",nomer_str,dod5->suma_bez_nds2d1.ravno(ii)-suma_bez_nds);
     } 
   }
 }

inhi1=inh1;
if(proc_dot != 0)
 {

  inhi1=inhi1*(100.0-proc_dot)/100.;
  inhi1=okrug(inhi1,0.01);
 }

if(inh1 != 0.)
 {
  fprintf(ff_xml,"   <R005G4>%.2f</R005G4>\n",inhi1);
  if(ff_xml_dot != NULL)
   fprintf(ff_xml_dot,"   <R005G4>%.2f</R005G4>\n",inh1-inhi1);
 }
fprintf(ff_xml,"   <R040G4>%.2f</R040G4>\n",i_suma_bez_nds+inhi1);
if(ff_xml_dot != NULL)
  fprintf(ff_xml_dot,"   <R050G4>%.2f</R050G4>\n",dod5->suma_bez_nds2d1.suma()-(i_suma_bez_nds+inhi1));


if(inh1 == 0.)
  fprintf(ff_svod_naim,"\
Інші                                   |               |          |\n");
else
  fprintf(ff_svod_naim,"\
Інші                                   |%15.2f|%10s|\n",inh1,"");


fprintf(ff_svod_naim,"\
Усього за звітний період (рядок 5)     |%15.2f|%10s|\n",i_suma_bez_nds+inh1,"");

fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");

if(ff_svod_dot != NULL)
 {
  if(inh1 == 0.)
    fprintf(ff_svod_dot,"\
Інші                                   |               |          |\n");
  else
    fprintf(ff_svod_dot,"\
Інші                                   |%15.2f|%10s|\n",inh1,"");


  fprintf(ff_svod_dot,"\
Усього за звітний період (рядок 5)     |%15.2f|%10s|\n",dod5->suma_bez_nds2d1.suma()-(i_suma_bez_nds+inhi1),"");

  fprintf(ff_svod_dot,"\
-------------------------------------------------------------------\n");
 }




rnnrd5_sap2(ff_svod_naim);

if(ff_svod_dot != NULL)
 rnnrd5_sap2(ff_svod_dot);
                  

class iceb_tu_str in1("");
nomer_str=0;
i_suma_bez_nds=i_suma_nds=0.;

for(int kkk=0; kkk < dod5->innn1.kolih(); kkk++)
 {
  for(int ii=0; ii < dod5->innn_per1.kolih(); ii++)
   {
    polen(dod5->innn_per1.ravno(ii),&in1,1,'|');  
    if(SRAV(dod5->innn1.ravno(kkk),in1.ravno(),0) != 0)
     continue;
    polen(dod5->innn_per1.ravno(ii),&period,2,'|');  

    if(dod5->suma_bez_nds1.ravno(ii) == 0. && dod5->suma_nds1.ravno(ii) == 0.)   
     continue;

    rsdat1(&m,&g,period.ravno());
    
    suma_bez_nds=dod5->suma_bez_nds1.ravno(ii);
    suma_nds=dod5->suma_nds1.ravno(ii);

    if(proc_dot != 0.)
     {
      suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
      suma_bez_nds=okrug(suma_bez_nds,0.01);

      suma_nds=suma_nds*(100.0-proc_dot)/100.;
      suma_nds=okrug(suma_nds,0.01);
      
     }
    
    i_suma_bez_nds+=suma_bez_nds;
    i_suma_nds+=suma_nds;
    nomer_str++;    
    if(ff_svod_dot != NULL)
      fprintf(ff_svod_dot,"%5d|%-*s|%-17s|%10.2f|%10.2f|\n",
      nomer_str,
      iceb_tu_kolbait(20,in1.ravno()),in1.ravno(),
      period.ravno(),dod5->suma_bez_nds1.ravno(ii)-suma_bez_nds,dod5->suma_nds1.ravno(ii)-suma_nds);

    fprintf(ff_svod_naim,"%5d|%-*s|%-17s|%10.2f|%10.2f|%s\n",
    nomer_str,
    iceb_tu_kolbait(20,in1.ravno()),in1.ravno(),
    period.ravno(),dod5->suma_bez_nds1.ravno(ii),dod5->suma_nds1.ravno(ii),dod5->naim_kontr1.ravno(ii));
    

    fprintf(ff_xml,"   <T2RXXXXG2 ROWNUM=\"%d\">%s</T2RXXXXG2>\n",nomer_str,in1.ravno());
    fprintf(ff_xml,"   <T2RXXXXG3A ROWNUM=\"%d\">%d</T2RXXXXG3A>\n",nomer_str,m);
    fprintf(ff_xml,"   <T2RXXXXG3B ROWNUM=\"%d\">%d</T2RXXXXG3B>\n",nomer_str,g);
    fprintf(ff_xml,"   <T2RXXXXG4 ROWNUM=\"%d\">%.2f</T2RXXXXG4>\n",nomer_str,suma_bez_nds);
    fprintf(ff_xml,"   <T2RXXXXG5 ROWNUM=\"%d\">%.2f</T2RXXXXG5>\n",nomer_str,suma_nds);
    
    if(ff_xml_dot != NULL)
     {
      fprintf(ff_xml_dot,"   <T2RXXXXG2 ROWNUM=\"%d\">%s</T2RXXXXG2>\n",nomer_str,in1.ravno());
      fprintf(ff_xml_dot,"   <T2RXXXXG3A ROWNUM=\"%d\">%d</T2RXXXXG3A>\n",nomer_str,m);
      fprintf(ff_xml_dot,"   <T2RXXXXG3B ROWNUM=\"%d\">%d</T2RXXXXG3B>\n",nomer_str,g);
      fprintf(ff_xml_dot,"   <T2RXXXXG4 ROWNUM=\"%d\">%.2f</T2RXXXXG4>\n",nomer_str,dod5->suma_bez_nds1.ravno(ii)-suma_bez_nds);
      fprintf(ff_xml_dot,"   <T2RXXXXG5 ROWNUM=\"%d\">%.2f</T2RXXXXG5>\n",nomer_str,dod5->suma_nds1.ravno(ii)-suma_nds);
     }    
   }
 }
//fprintf(ff_svod_naim,"\x1B\x2D\x30"); /*Выключить подчеркивание*/

fprintf(ff_svod_naim,"\
-------------------------------------------------------------------\n");
fprintf(ff_svod_naim,"\
Разом                                       |%10.2f|%10.2f|\n",
dod5->suma_bez_nds1.suma(),dod5->suma_nds1.suma());

rnnrd5_endras(dod5,proc_dot,0,ff_svod_naim,ff_xml);

if(ff_svod_dot != NULL)
  rnnrd5_endras(dod5,proc_dot,1,ff_svod_dot,ff_xml_dot);


return(0);

}
/**********************************/
/*шапка додатка 5 и 2*/
/**********************************/

void rnnrd5_sap(FILE *ff_svod)
{


fprintf(ff_svod,"Розділ І. Податкові зобов'язання\n");
fprintf(ff_svod,"\
-------------------------------------------------------------------\n");
fprintf(ff_svod,"\
  N  |Платник податку -   |Період складання |  Обсяги  | Сумма ПДВ|\n\
     |покупець (індивіду- |  податкових     | поставки |          |\n\
     |альний податковий   |  накладних      |(без ПДВ) |          |\n\
     | номер)             |                 |          |          |\n");

fprintf(ff_svod,"\
-------------------------------------------------------------------\n");


}
/***************************************/
/*Первый итог раздела 1 */
/***************************************/
void rnn_rk5_kon1(double suma_bez_nds2,
double suma_nds2,
double os_sr21,
double os_sr22,
double inh1,double inh2,FILE *ff_svod,FILE *ff_xml)
{

if(inh1 == 0. && inh2 == 0.)
 {
  fprintf(ff_xml,"   <R001G3 xsi:nil=\"true\"/>\n");
  fprintf(ff_xml,"   <R001G4 xsi:nil=\"true\"/>\n");
 }
else
 {
  fprintf(ff_xml,"   <R001G3>%.2f</R001G3>\n",inh1);
  fprintf(ff_xml,"   <R001G4>%.2f</R001G4>\n",inh2);
 } 

fprintf(ff_xml,"   <R010G3>%.2f</R010G3>\n",suma_bez_nds2);
fprintf(ff_xml,"   <R010G4>%.2f</R010G4>\n",suma_nds2);

if(os_sr21 == 0.)
 fprintf(ff_xml,"   <R011G3 xsi:nil=\"true\"/>\n");
else
 fprintf(ff_xml,"   <R011G3>%.2f</R011G3>\n",os_sr21);

if(os_sr22 == 0.)
 fprintf(ff_xml,"   <R011G4 xsi:nil=\"true\"/>\n");
else
 fprintf(ff_xml,"   <R011G4>%.2f</R011G4>\n",os_sr22);


fprintf(ff_svod,"\
-------------------------------------------------------------------\n");
if(inh1 == 0. && inh2 == 0.)
  fprintf(ff_svod,"\
Інші                                   |               |          |\n");
else
  fprintf(ff_svod,"\
Інші                                   |%15.2f|%10.2f|\n",inh1,inh2);

fprintf(ff_svod,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod,"\
Усього за звітний період  (рядок 1 дек-|               |          |\n\
ларації),в тому числі:                 |%15.2f|%10.2f|\n",
suma_bez_nds2,suma_nds2);


fprintf(ff_svod,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod,"\
постачання необоротних активів         |%15.2f|%10.2f|\n",os_sr21,os_sr22);

fprintf(ff_svod,"\
-------------------------------------------------------------------\n");


}
/******************/
/*концовка*/
/******************/
void rnnrd5_endras(class rnn_d5 *dod5,
double proc_dot,
int metka_v,    /*0-основной 1-дополнительный*/
FILE *ff_svod,FILE *ff_xml)
{
double suma_bez_nds=dod5->suma_bez_nds1.suma();
double suma_nds=dod5->suma_nds1.suma();

double os1=dod5->os_sr1[0];
double os2=dod5->os_sr1[1];

if(proc_dot != 0.)
 {
  suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
  suma_bez_nds=okrug(suma_bez_nds,0.01);

  suma_nds=suma_nds*(100.0-proc_dot)/100.;
  suma_nds=okrug(suma_nds,0.01);
  
  os1=os1*(100.0-proc_dot)/100.;
  os2=os2*(100.0-proc_dot)/100.;
  os1=okrug(os1,0.01);  
  os2=okrug(os2,0.01);  

  if(metka_v == 1)
   {
    suma_bez_nds=dod5->suma_bez_nds1.suma()-suma_bez_nds;
    suma_nds=dod5->suma_nds1.suma()-suma_nds;
    os1=dod5->os_sr1[0]-os1;
    os2=dod5->os_sr1[1]-os2;
   } 
  
 }





//fprintf(ff_svod,"\x1B\x2D\x30"); /*Выключить подчеркивание*/

fprintf(ff_svod,"\
-------------------------------------------------------------------\n");
fprintf(ff_svod,"\
Усього за місяць (квартал)(рядок 10.1 декла-|          |          |\n\
рації), в тому числі:                       |%10.2f|%10.2f|\n",
suma_bez_nds,suma_nds);


fprintf(ff_svod,"\
-------------------------------------------------------------------\n");
fprintf(ff_svod,"\
придбання (будівництво, спорудження)        |%10.2f|%10.2f|\n\
основних фондів                             |          |          |\n",os1,os2);

fprintf(ff_svod,"\
-------------------------------------------------------------------\n");
fprintf(ff_svod,"\
проведення розрахунків комісіонером/повірен-|          |          |\n\
ним за товари (послуги) на умовах передба-  |          |          |\n\
чених п.4.7 ст. 4 Закону                    |          |          |\n");

fprintf(ff_svod,"\
-------------------------------------------------------------------\n");





fprintf(ff_xml,"   <R020G4>%.2f</R020G4>\n",suma_bez_nds);
fprintf(ff_xml,"   <R020G5>%.2f</R020G5>\n",suma_nds);

if(os1 == 0.)
  fprintf(ff_xml,"   <R021G4 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R021G4>%.2f</R021G4>\n",os1);

if(os2 == 0.)
  fprintf(ff_xml,"   <R021G5 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R021G5>%.2f</R021G5>\n",os2);

double i_suma_bez_nds=0.;
double i_suma_nds=0.;
class iceb_tu_str period("");
short m=0,g=0;
class iceb_tu_str in1("");

fprintf(ff_svod,"\
Операції з придбання з ПДВ, які не надають права формування\n\
            податкового кредиту\n");


fprintf(ff_svod,"\
-------------------------------------------------------------------\n");


for(int kkk=0; kkk < dod5->innn1d.kolih(); kkk++)
 for(int nomer=0; nomer < dod5->innn_per1d.kolih(); nomer++)
  {
   polen(dod5->innn_per1d.ravno(nomer),&in1,1,'|');  
   if(SRAV(dod5->innn1d.ravno(kkk),in1.ravno(),0) != 0)
     continue;

   polen(dod5->innn_per1d.ravno(nomer),&period,2,'|');  
   rsdat1(&m,&g,period.ravno());

   suma_bez_nds=dod5->suma_bez_nds1d.ravno(nomer);
   suma_nds=dod5->suma_nds1d.ravno(nomer);

   if(proc_dot != 0.)
    {
     suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
     suma_bez_nds=okrug(suma_bez_nds,0.01);

      suma_nds=suma_nds*(100.0-proc_dot)/100.;
      suma_nds=okrug(suma_nds,0.01);
      if(metka_v == 1)
       {
        suma_bez_nds=dod5->suma_bez_nds1.suma()-suma_bez_nds;
        suma_nds=dod5->suma_nds1.suma()-suma_nds;
       }
     }

    i_suma_bez_nds+=suma_bez_nds;
    i_suma_nds+=suma_nds;

    fprintf(ff_svod,"%5d|%-*s|%02d.%04d          |%10.2f|%10.2f|%s\n",
    nomer+1,
    iceb_tu_kolbait(20,dod5->innn1d.ravno(kkk)),dod5->innn1d.ravno(kkk),
    m,g,suma_bez_nds,suma_nds,dod5->naim_kontr1d.ravno(kkk));

    if(dod5->innn1d.ravno(kkk)[0] == '\0' ||  isdigit(dod5->innn1d.ravno(kkk)[0]) == 0)  //Значит символ не число
     fprintf(ff_xml,"   <T3RXXXXG2 xsi:nil=\"true\" ROWNUM=\"%d\"/>\n",nomer+1);
    else  
      fprintf(ff_xml,"   <T3RXXXXG2 ROWNUM=\"%d\">%s</T3RXXXXG2>\n",nomer+1,dod5->innn1d.ravno(kkk));

    fprintf(ff_xml,"   <T3RXXXXG3A ROWNUM=\"%d\">%d</T3RXXXXG3A>\n",nomer+1,m);
    fprintf(ff_xml,"   <T3RXXXXG3B ROWNUM=\"%d\">%d</T3RXXXXG3B>\n",nomer+1,g);
    fprintf(ff_xml,"   <T3RXXXXG4 ROWNUM=\"%d\">%.2f</T3RXXXXG4>\n",nomer+1,suma_bez_nds);
    fprintf(ff_xml,"   <T3RXXXXG5  ROWNUM=\"%d\">%.2f</T3RXXXXG5>\n",nomer+1,suma_nds);

  }

if(dod5->innn1d.kolih() == 0)
 {
  fprintf(ff_svod,"\
     |                    |                 |          |          |\n");
 }
fprintf(ff_svod,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod,"\
Усього за звітний період                    |%10.2f|%10.2f|\n",i_suma_bez_nds,i_suma_nds);
fprintf(ff_svod,"\
-------------------------------------------------------------------\n");




if(i_suma_bez_nds == 0.)
  fprintf(ff_xml,"   <R03G4 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R03G4>%.2f</R03G4>\n",i_suma_bez_nds);

if(i_suma_nds == 0.)
  fprintf(ff_xml,"   <R03G5 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R03G5>%.2f</R03G5>\n",i_suma_nds);


fprintf(ff_svod,"Операції з придбання без податку на додану вартість\n");


fprintf(ff_svod,"\
-------------------------------------------------------------------\n");
for(int kkk=0; kkk < dod5->innn1d1.kolih(); kkk++)
 for(int nomer=0; nomer < dod5->innn_per1d1.kolih(); nomer++)
  {
   polen(dod5->innn_per1d1.ravno(nomer),&in1,1,'|');  
   if(SRAV(dod5->innn1d1.ravno(kkk),in1.ravno(),0) != 0)
     continue;

   polen(dod5->innn_per1d1.ravno(nomer),&period,2,'|');  
   rsdat1(&m,&g,period.ravno());

   suma_bez_nds=dod5->suma_bez_nds1d1.ravno(nomer);
   suma_nds=dod5->suma_nds1d1.ravno(nomer);

   if(proc_dot != 0.)
    {
     suma_bez_nds=suma_bez_nds*(100.0-proc_dot)/100.;
     suma_bez_nds=okrug(suma_bez_nds,0.01);

      suma_nds=suma_nds*(100.0-proc_dot)/100.;
      suma_nds=okrug(suma_nds,0.01);
      if(metka_v == 1)
       {
        suma_bez_nds=dod5->suma_bez_nds1.suma()-suma_bez_nds;
        suma_nds=dod5->suma_nds1.suma()-suma_nds;
       }
     }

    i_suma_bez_nds+=suma_bez_nds;
    i_suma_nds+=suma_nds;

    fprintf(ff_svod,"%5d|%-*s|%02d.%04d          |%10.2f|%10.2f|%s\n",
    nomer+1,
    iceb_tu_kolbait(20,dod5->innn1d1.ravno(kkk)),dod5->innn1d1.ravno(kkk),
    m,g,suma_bez_nds,suma_nds,dod5->naim_kontr1d1.ravno(kkk));

    if(dod5->innn1d1.ravno(kkk)[0] == '\0' ||  isdigit(dod5->innn1d1.ravno(kkk)[0]) == 0)  //Значит символ не число
     fprintf(ff_xml,"   <T6RXXXXG2 xsi:nil=\"true\" ROWNUM=\"%d\"/>\n",nomer+1);
    else  
      fprintf(ff_xml,"   <T6RXXXXG2 ROWNUM=\"%d\">%s</T6RXXXXG2>\n",nomer+1,dod5->innn1d1.ravno(kkk));

    fprintf(ff_xml,"   <T6RXXXXG3A ROWNUM=\"%d\">%d</T6RXXXXG3A>\n",nomer+1,m);
    fprintf(ff_xml,"   <T6RXXXXG3B ROWNUM=\"%d\">%d</T6RXXXXG3B>\n",nomer+1,g);
    fprintf(ff_xml,"   <T6RXXXXG4 ROWNUM=\"%d\">%.2f</T6RXXXXG4>\n",nomer+1,suma_bez_nds);


  }

if(dod5->innn1d1.kolih() == 0)
 {
  fprintf(ff_svod,"\
     |                    |                 |          |          |\n");
 }
fprintf(ff_svod,"\
-------------------------------------------------------------------\n");

fprintf(ff_svod,"\
Усього за звітний період                    |%10.2f|%10.2f|\n",i_suma_bez_nds,i_suma_nds);
fprintf(ff_svod,"\
-------------------------------------------------------------------\n");




if(i_suma_bez_nds == 0.)
  fprintf(ff_xml,"   <R06G4 xsi:nil=\"true\"/>\n");
else
  fprintf(ff_xml,"   <R06G4>%.2f</R06G4>\n",i_suma_bez_nds);



class iceb_t_fioruk_rk rekruk;
class iceb_t_fioruk_rk rekbuh;

iceb_t_fioruk(1,&rekruk);
iceb_t_fioruk(2,&rekbuh);


//rnnrd5_end(rekruk.fio.ravno(),rekbuh.fio.ravno(),ff_svod);

fprintf(ff_xml,"   <HFILL>%02d%02d%04d</HFILL>\n",rek_zag_nn.dt,rek_zag_nn.mt,rek_zag_nn.gt);



fprintf(ff_xml,"   <HBOS>%s</HBOS>\n",rekruk.fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HBUH>%s</HBUH>\n",rekbuh.fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HFO xsi:nil=\"true\"/>\n");

fprintf(ff_xml," </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");

podpis(ff_svod);
fclose(ff_svod);

fclose(ff_xml);

}
/***********************/
/*шапка второго раздела*/
/***********************/
void rnnrd5_sap2(FILE *ff)
{
fprintf(ff,"\nРозділ ІІ. Податковий кредит\n");
fprintf(ff,"\
-------------------------------------------------------------------\n");

fprintf(ff,"\
  N  |Платник податку -   |Період виписки   |  Обсяги  | Сумма ПДВ|\n\
     |покупець (індивіду- |податкових накла-| поставки |          |\n\
     |альний податковий   |дних, інших доку-|(без ПДВ) |          |\n\
     | номер)             |ментів, що дають |          |          |\n\
     |                    |право на податко-|          |          |\n\
     |                    |вий кредит       |          |          |\n");

/********
12345 12345678901234567890 12345678901234567 1234567890 1234567890
***********/
fprintf(ff,"\
-------------------------------------------------------------------\n");
fprintf(ff,"\
Операції з придбання з ПДВ, які надають право формування податкового\n\
                  кредиту\n");
fprintf(ff,"\
-------------------------------------------------------------------\n");
}
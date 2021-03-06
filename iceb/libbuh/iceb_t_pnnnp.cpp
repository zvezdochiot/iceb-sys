/*$Id: iceb_t_pnnnp.c,v 5.6 2013/05/17 14:56:29 sasa Exp $*/
/*26.09.2012	13.11.2009	Белых А.И.	iceb_t_pnnnp.c
Проверка номера налоговой накладной поставщика - введена ли она в шапку какого-нибудь документа
Если вернуди 0-не введена
             1-введена
*/
#include "buhl.h"

int iceb_t_pnnnp(const char *datann, /*Дата получения налоговой накладной*/
const char *datavnn,/*Дата выписки налоговой накладной*/
const char *kodkontr,const char *nomnn,
const char *datadok,const char *nomdok,
int metka_ps) /*1-материальный учёт
                2-учёт услуг
                3-учёт основных средств
                */
{

if(datadok[0] == '\0')
 return(0);

if(nomnn[0] == '\0')
 return(0);

if(kodkontr[0] == '\0')
 return(0);

if(datann[0] == '\0')
 return(0);

if(nomdok[0] == '\0')
 return(0);

if(metka_ps == 0)
 return(0);

short dd,md,gd;
rsdat(&dd,&md,&gd,datadok,1);
 
char strsql[2048];
SQL_str row,row1;
class SQLCURSOR cur,cur1;

if(metka_ps == 1)
 sprintf(strsql,"select datd,nomd from Dokummat where datpnn='%s' and kontr='%s' and nomnn='%s' \
and nomd <> '%s' and tip=1 limit 1",
 iceb_tu_tosqldat(datann),kodkontr,nomnn,nomdok);
else
 sprintf(strsql,"select datd,nomd from Dokummat where datpnn='%s' and kontr='%s' and nomnn='%s' and tip=1 limit 1",
 iceb_tu_tosqldat(datann),kodkontr,nomnn);

if(readkey(strsql,&row,&cur) > 0)
 {
  sprintf(strsql,"%s\n%s\n%s:%s %s:%s",
  gettext("Такой номер налоговой накладной уже введён!"),
  gettext("Материальный учёт"),
  gettext("Дата документа"),
  iceb_tu_datzap(row[0]),
  gettext("Номер документа"),
  row[1]);
  iceb_t_soob(strsql);      
  return(1);
 }

if(datavnn[0] != '\0')
 {
  sprintf(strsql,"select sklad,nomd,sodz from Dokummat2 where god=%d and nomerz=14",gd);
  int kolstr=0;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    msql_error(&bd,"",strsql);
   }  
  int kolstr1=0;

  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    if(sravmydat(row[2],datavnn) != 0)
     continue;

    sprintf(strsql,"select datd,nomd,kontr,nomnn from Dokummat where datd >= '%04d-01-01' \
  and datd <= '%04d-12-31' and sklad=%s and nomd='%s' and tip=1",
    gd,gd,row[0],row[1]);
    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
     {
      msql_error(&bd,"",strsql);
      continue;
     }  
    while(cur1.read_cursor(&row1) != 0)
     {

      if(SRAV(kodkontr,row[2],0) != 0)
       continue;
      if(SRAV(nomnn,row[3],0) != 0)
       continue;

      sprintf(strsql,"%s\n%s\n%s:%s %s:%s",
      gettext("Такой номер налоговой накладной уже введён!"),
      gettext("Материальный учёт"),
      gettext("Дата документа"),
      iceb_tu_datzap(row1[0]),
      gettext("Номер документа"),
      row1[1]);
      iceb_t_soob(strsql);      
      return(1);
    }
     
   }
 }

if(metka_ps == 2)
 sprintf(strsql,"select datd,nomd from Usldokum  where datpnn='%s' and kontr='%s' and nomnn='%s' \
and nomd <> '%s' and tp=1 limit 1",
 iceb_tu_tosqldat(datann),kodkontr,nomnn,nomdok);
else
 sprintf(strsql,"select datd,nomd from Usldokum where datpnn='%s' and kontr='%s' and nomnn='%s' and tp=1 limit 1",
 iceb_tu_tosqldat(datann),kodkontr,nomnn);

if(readkey(strsql,&row,&cur) > 0)
 {
  sprintf(strsql,"%s\n%s\n%s:%s %s:%s",
  gettext("Такой номер налоговой накладной уже введён!"),
  gettext("Учёт услуг"),
  gettext("Дата документа"),
  iceb_tu_datzap(row[0]),
  gettext("Номер документа"),
  row[1]);
  iceb_t_soob(strsql);      
  return(1);
 }

if(datavnn[0] != '\0')
 {
  if(metka_ps == 2)
   sprintf(strsql,"select datd,nomd from Usldokum  where datdp='%s' and kontr='%s' and nomnn='%s' \
and nomd <> '%s' and tp=1 limit 1",
   iceb_tu_tosqldat(datavnn),kodkontr,nomnn,nomdok);
  else
   sprintf(strsql,"select datd,nomd from Usldokum where datdp='%s' and kontr='%s' and nomnn='%s' and tp=1 limit 1",
   iceb_tu_tosqldat(datavnn),kodkontr,nomnn);

  if(readkey(strsql,&row,&cur) > 0)
   {
    sprintf(strsql,"%s\n%s\n%s:%s %s:%s",
    gettext("Такой номер налоговой накладной уже введён!"),
    gettext("Учёт услуг"),
    gettext("Дата документа"),
    iceb_tu_datzap(row[0]),
    gettext("Номер документа"),
    row[1]);
    iceb_t_soob(strsql);      
    return(1);
   }

 }

if(metka_ps == 3)
 sprintf(strsql,"select datd,nomd from Uosdok  where datpnn='%s' and kontr='%s' and nomnn='%s' \
and nomd <> '%s' and tipz=1 limit 1",
 iceb_tu_tosqldat(datann),kodkontr,nomnn,nomdok);
else
 sprintf(strsql,"select datd,nomd from Uosdok where datpnn='%s' and kontr='%s' and nomnn='%s' and tipz=1 limit 1",
 iceb_tu_tosqldat(datann),kodkontr,nomnn);

if(readkey(strsql,&row,&cur) > 0)
 {
  sprintf(strsql,"%s\n%s\n%s:%s %s:%s",
  gettext("Такой номер налоговой накладной уже введён!"),
  gettext("Учёт основных средств"),
  gettext("Дата документа"),
  iceb_tu_datzap(row[0]),
  gettext("Номер документа"),
  row[1]);
  iceb_t_soob(strsql);      
  return(1);
 }

if(datavnn[0] != '\0')
 {
  if(metka_ps == 3)
   sprintf(strsql,"select datd,nomd from Uosdok  where dvnn='%s' and kontr='%s' and nomnn='%s' \
and nomd <> '%s' and tipz=1 limit 1",
   iceb_tu_tosqldat(datavnn),kodkontr,nomnn,nomdok);
  else
   sprintf(strsql,"select datd,nomd from Uosdok where dvnn='%s' and kontr='%s' and nomnn='%s' and tipz=1 limit 1",
   iceb_tu_tosqldat(datavnn),kodkontr,nomnn);

  if(readkey(strsql,&row,&cur) > 0)
   {
    sprintf(strsql,"%s\n%s\n%s:%s %s:%s",
    gettext("Такой номер налоговой накладной уже введён!"),
    gettext("Учёт основных средств"),
    gettext("Дата документа"),
    iceb_tu_datzap(row[0]),
    gettext("Номер документа"),
    row[1]);
    iceb_t_soob(strsql);      
    return(1);
   }
 }

char koment[128];
sprintf(koment,"нн%s",nomnn);

sprintf(strsql,"select sh,shk,komen from Prov where datp='%s' and kodkon='%s' and komen like '%s%%' limit 1",
iceb_tu_tosqldat(datann),kodkontr,koment);

if(readkey(strsql,&row,&cur) > 0)
 {
  sprintf(strsql,"%s\n%s\n%s %s %s %s",
  gettext("Такой номер налоговой накладной уже введён!"),
  gettext("Главная книга"),
  datann,row[0],row[1],row[2]);
  iceb_t_soob(strsql);      
  return(1);
 }


return(0);
}


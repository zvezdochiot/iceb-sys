/*$Id: rasactmu.c,v 5.18 2013/09/26 09:43:38 sasa Exp $*/
/*15.07.2015	19.07.2003	Белых А.И.	rasactmu.c
Распечатка акта приема передачи материалов
*/
#include        <errno.h>
#include	"buhg.h"

extern double	okrg1;

void rasactmu(short dd,short md,short gd, //дата документа
int skl,  //Склад
const char *nomdok) //Номер документа
{
char		strsql[512];
SQL_str		row,row1;
int 		kolstr=0;
char		imaf[64];
FILE		*ff;
class iceb_tu_str shetu("");
class iceb_tu_str shetz("");
class iceb_tu_str naim("");
int		kolstrok=0;
int		kollist=0;
class iceb_tu_str datv("");
class iceb_tu_str innom("");
class iceb_tu_str regnom("");

//printw("\nrasactmu %d.%d.%d %d %s\n",dd,md,gd,skl,nomdok);
//OSTANOV();
GDITE();

sprintf(strsql,"select sklad,nomkar,kodm,kolih,cena,ei from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",gd,md,dd,skl,nomdok);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 { 
  iceb_t_soob(gettext("Не найдено ни одной записи !"));
  return;
 } 

class iceb_t_tmptab tabtmp;
const char *imatmptab={"rasactmuw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(24) not null,\
naim char(112) not null,\
kolih double(16,6) not null,\
cena double(15,6) not null,\
ei char(24) not null,\
datv DATE not null,\
innom char(24) not null,\
regnom char(24) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp) != 0)
 return;

while(cur.read_cursor(&row) != 0)
 {
  //Узнаем счёт учета материалла
  sprintf(strsql,"select shetu,datv,innom,rnd from Kart where sklad=%s and nomk=%s",row[0],row[1]);
  shetu.new_plus("");
  regnom.new_plus("");
  innom.new_plus("");
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    shetu.new_plus(row1[0]);
    datv.new_plus(row[1]);
    innom.new_plus(row1[2]);
    regnom.new_plus(row1[3]);
   }
  //Узнаем наименование материалла
  naim.new_plus("");
  sprintf(strsql,"select naimat from Material where kodm=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   naim.new_plus(row1[0]);

  sprintf(strsql,"insert into %s values ('%s','%s',%s,%s,'%s','%s','%s','%s')",
  imatmptab,
  shetu.ravno(),naim.ravno_filtr(),row[3],row[4],row[5],datv.ravno(),innom.ravno(),regnom.ravno());

  sql_zapis(strsql,1,0);    

 }

sprintf(strsql,"select * from %s order by sh asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

sprintf(imaf,"actm%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return;
 }

hrasact(&kolstrok,&kollist,ff);


double kolih=0.;
double cena=0.;
class iceb_tu_str ei("");
int  nomerstr=0;
double summa=0;
double kolihi=0.;
double sumai=0.;
class iceb_tu_str naimshet("");
double kolihshet=0.;
double sumashet=0.;
short dv,mv,gv;
class iceb_tu_str dopstr("");

while(cur.read_cursor(&row) != 0)
 {

  shetu.new_plus(row[0]);
  naim.new_plus(row[1]);
  kolih=atof(row[2]);
  cena=atof(row[3]);
  ei.new_plus(row[4]);
  rsdat(&dv,&mv,&gv,row[5],2);
  innom.new_plus(row[6]);
  regnom.new_plus(row[7]);
  
  cena=okrug(cena,okrg1);
  summa=kolih*cena;
  
  if(SRAV(shetz.ravno(),shetu.ravno(),0) != 0)
   {

    if(shetz.ravno()[0] != '\0')
     {
      sapkk1iv(&kolstrok,&kollist,ff);
      sprintf(strsql,"%s %s %s",gettext("Итого по счёту"),shetz.ravno(),naimshet.ravno());
      fprintf(ff,"%*.*s %10.10g %10.2f\n",
      iceb_tu_kolbait(62,strsql),iceb_tu_kolbait(62,strsql),strsql,kolihshet,sumashet);
     }

    sprintf(strsql,"select nais from Plansh where ns=%s",shetu.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     naimshet.new_plus(row1[0]);
    else
     naimshet.new_plus("");

    sapkk1iv(&kolstrok,&kollist,ff);
    fprintf(ff,"%20s ******%s %s %s*******\n"," ",gettext("Счёт учета"),shetu.ravno(),naimshet.ravno());

    shetz.new_plus(shetu.ravno());
    kolihshet=0.;
    sumashet=0.;
   }  
  
  
  sumai+=summa;
  kolihi+=kolih;
  sumashet+=summa;
  kolihshet+=kolih;  
  
  sapkk1iv(&kolstrok,&kollist,ff);
  
  fprintf(ff,"%3d|%-*.*s|%-*s|%10.10g|%10.10g|%10.2f|%10s|%10s|%10s|%10s|\n",
  ++nomerstr,
  iceb_tu_kolbait(40,naim.ravno()),iceb_tu_kolbait(40,naim.ravno()),naim.ravno(),
  iceb_tu_kolbait(6,ei.ravno()),ei.ravno(),
  cena,kolih,summa," "," "," "," ");
  dopstr.new_plus("");
  if(innom.ravno()[0] != '\0')
   {
    if( regnom.ravno()[0] == '\0')
     {
//      sprintf(dopstr,"%s:%s",gettext("Инвентарный номер"),innom.ravno());
      dopstr.new_plus(gettext("Инвентарный номер"));
      dopstr.plus(":",innom.ravno());
      if(dv != 0)
       {
        memset(strsql,'\0',sizeof(strsql));
        sprintf(strsql," %s:%d.%d.%d%s",
        gettext("Дата ввода в эксплуатацию"),
        dv,mv,gv,gettext("г."));

        dopstr.plus(strsql);
       }
     }        
    else
     {
//      sprintf(dopstr,"%s:%s",gettext("Регистрационный номер"),regnom.ravno());
      dopstr.new_plus(gettext("Регистрационный номер"));
      dopstr.plus(":",regnom.ravno());
      
      memset(strsql,'\0',sizeof(strsql));
      sprintf(strsql," %s:%s",gettext("Серия"),innom.ravno());
      dopstr.plus(strsql);
      if(dv != 0)
       {
        memset(strsql,'\0',sizeof(strsql));
        sprintf(strsql," %s:%d.%d.%d%s",
        gettext("Дата регистрации"),
        dv,mv,gv,gettext("г."));
        dopstr.plus(strsql);
       }
     }
   }
  if(dopstr.ravno()[0] != '\0')
   {
    sapkk1iv(&kolstrok,&kollist,ff);
    fprintf(ff,"%3s %s\n"," ",dopstr.ravno());
   }
  sapkk1iv(&kolstrok,&kollist,ff);
  fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    
 }
krasact(shetz.ravno(),naimshet.ravno(),kolihshet,sumashet,kolihi,sumai,ff);
fclose(ff);

class spis_oth oth;
oth.spis_imaf.plus(imaf);
oth.spis_naim.plus(gettext("Акт приема-передачи товароматериальных ценностей"));
iceb_t_ustpeh(oth.spis_imaf.ravno(0),1);
iceb_t_rabfil(&oth,"");

}

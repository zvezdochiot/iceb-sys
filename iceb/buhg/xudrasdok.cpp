/*$Id: xudrasdok.c,v 5.20 2014/07/31 07:09:18 sasa Exp $*/
/*01.09.2015	02.04.2009	Белых А.И.	xudrasdok.c
Распечатка доверенности
*/
#include <errno.h>
#include "buhg.h"

int xudrasdok_r(class SPISOK *prug);


int xudrasdok(class iceb_tu_str *datdov,class iceb_tu_str *nomdov,
class spis_oth *oth)
{
FILE *ff;
char imaf[64];
char strsql[1024];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
class iceb_tu_str komu("");
class iceb_tu_str post("");
class iceb_tu_str datad("");
class iceb_tu_str nomnar("");
class iceb_tu_str datanar("");
class iceb_tu_str inn("");
class iceb_tu_str nspnds("");
class iceb_tu_str telef("");
class iceb_tu_str adres_pol("");
class iceb_tu_str adres_plat("");
class iceb_tu_str mfo("");
class iceb_tu_str nomshet("");
class iceb_tu_str naim_bank("");
class iceb_tu_str naim_pol("");
class iceb_tu_str naim_plat("");
class iceb_tu_str dolg("");
class iceb_tu_str dokum("");
class iceb_tu_str seriq("");
class iceb_tu_str nomerdok("");
class iceb_tu_str datavid("");
class iceb_tu_str vidan("");
class iceb_tu_str kod_edrp("");
class SPISOK prug;

short metka_list=0; /*0-с двух строн 1-на одной строне листа*/

if(iceb_t_poldan("Печать доверенности на одной стороне листа",strsql,"nastud.alx") == 0)
 if(SRAV("Вкл",strsql,1) == 0)
  metka_list=1;
 

/*Читаем шапку документа*/
sprintf(strsql,"select * from Uddok where datd='%s' and nomd='%s'",
datdov->ravno_sqldata(),nomdov->ravno());

if(readkey(strsql,&row,&cur) == 1)
 {
  komu.new_plus(row[3]);
  post.new_plus(row[4]);

  datad.new_plus(iceb_tu_datzap(row[2]));
  nomnar.new_plus(row[8]);
  datanar.new_plus(iceb_tu_datzap(row[7]));
  dokum.new_plus(row[5]);
  seriq.new_plus(row[9]);
  nomerdok.new_plus(row[10]);
  datavid.new_plus(iceb_tu_datzap(row[11]));
  vidan.new_plus(row[12]);
  dolg.new_plus(row[13]);
 }

class iceb_tu_str kod_pol("");
class iceb_tu_str kod_plat("");

iceb_t_poldan("Код контрагента получателя",&kod_pol,"nastud.alx");
iceb_t_poldan("Код контрагента плательщика",&kod_plat,"nastud.alx");

if(kod_pol.getdlinna() <= 1)
 kod_pol.new_plus("00");
if(kod_plat.getdlinna() <= 1)
 kod_plat.new_plus(kod_pol.ravno());
 
sprintf(strsql,"select * from Kontragent where kodkon='%s'",kod_pol.ravno_filtr());
if(readkey(strsql,&row,&cur) == 1)
 {
  naim_pol.new_plus(row[1]);
  if(row[16][0] != '\0')
   naim_pol.new_plus(row[16]);
  kod_edrp.new_plus(row[5]);
  inn.new_plus(row[8]);
  nspnds.new_plus(row[9]);
  adres_pol.new_plus(row[3]);
  if(polen(row[10],&telef,1,' ') != 0)
   telef.new_plus(row[10]);
 }

sprintf(strsql,"select * from Kontragent where kodkon='%s'",kod_plat.ravno_filtr());
if(readkey(strsql,&row,&cur) == 1)
 {
  naim_plat.new_plus(row[1]);
  if(row[16][0] != '\0')
   naim_plat.new_plus(row[16]);


  adres_plat.new_plus(row[3]);

  mfo.new_plus(row[6]);
  nomshet.new_plus(row[7]);
  naim_bank.new_plus(row[2]);
 }

sprintf(imaf,"dov%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  error_op_nfil(imaf,errno,"");
  return(1);
 }

startfil(ff);
char stroka[1024];

xudrasdok_r(&prug); /*читаем текст который должен быть в правом углу*/
int kolih_str=prug.kolih();
//fprintf(ff,"\x1b\x6C%c",3); /*Установка левого поля*/
fprintf(ff,"\x1B\x4D"); //12 знаков на дюйм
if(kolih_str == 0)
 fprintf(ff,"Одержувач:%-*.*s\n",
 iceb_tu_kolbait(45,naim_pol.ravno()),
 iceb_tu_kolbait(45,naim_pol.ravno()),
 naim_pol.ravno());
else
 fprintf(ff,"Одержувач:%-*.*s %*.*s",
 iceb_tu_kolbait(45,naim_pol.ravno()),
 iceb_tu_kolbait(45,naim_pol.ravno()),
 naim_pol.ravno(),
 iceb_tu_kolbait(38,prug.ravno(0)),
 iceb_tu_kolbait(38,prug.ravno(0)),
 prug.ravno(0));

if(iceb_tu_strlen(naim_pol.ravno()) > 45)
 fprintf(ff,"%s\n",iceb_tu_adrsimv(45,naim_pol.ravno()));
if(kolih_str < 1) 
 fprintf(ff,"Індивідуальний податковий номер:%-17s\n",inn.ravno());
else
 fprintf(ff,"Індивідуальний податковий номер:%-17s        %*.*s",
 inn.ravno(),
 iceb_tu_kolbait(37,prug.ravno(1)),
 iceb_tu_kolbait(37,prug.ravno(1)),
 prug.ravno(1));

if(kolih_str < 2)
 fprintf(ff,"Номер свідоцтва платника ПДВ:%-26s\n",nspnds.ravno());
else
 fprintf(ff,"Номер свідоцтва платника ПДВ:%-26s %*.*s",
 nspnds.ravno(),
 iceb_tu_kolbait(37,prug.ravno(2)),
 iceb_tu_kolbait(37,prug.ravno(2)),
 prug.ravno(2));

fprintf(ff,"Телефон одержувача:%-10s\n",telef.ravno());
int dlinna_str=50;
sprintf(stroka,"Адреса одержувача: %s",adres_pol.ravno());
for(int ii=0; ii < iceb_tu_strlen(stroka); ii+=dlinna_str)
 {
  fprintf(ff,"%-*.*s\n",
  iceb_tu_kolbait(dlinna_str,iceb_tu_adrsimv(ii,stroka)),
  iceb_tu_kolbait(dlinna_str,iceb_tu_adrsimv(ii,stroka)),
  iceb_tu_adrsimv(ii,stroka));
 }
fprintf(ff,"Ідентифікаційний код ЄДРПОУ:%-20s          Код за УКУД\n",kod_edrp.ravno());

fprintf(ff,"Платник:%.*s\n",iceb_tu_kolbait(85,naim_plat.ravno()),naim_plat.ravno());
if(iceb_tu_strlen(naim_plat.ravno()) > 85)
 fprintf(ff,"%s\n",iceb_tu_adrsimv(85,naim_plat.ravno()));
sprintf(stroka,"Адреса платника: %s",adres_plat.ravno());

for(int ii=0; ii < iceb_tu_strlen(stroka); ii+=dlinna_str)
 fprintf(ff,"%.*s\n",
 iceb_tu_kolbait(dlinna_str,iceb_tu_adrsimv(ii,stroka)),
 iceb_tu_adrsimv(ii,stroka));


fprintf(ff,"Рахунок:%s MФО:%s\n",nomshet.ravno(),mfo.ravno());
fprintf(ff,"Банк:%-*.*s Довіреність дійсна до %s р.\n\n",
iceb_tu_kolbait(50,naim_bank.ravno()),iceb_tu_kolbait(50,naim_bank.ravno()),naim_bank.ravno(),datad.ravno());


fprintf(ff,"\x1B\x50"); /*10-знаков*/

fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины*/
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff,"          ДОВІРЕНІСТЬ N%s",nomdov->ravno());
fprintf(ff,"\x14"); /*Выключение режима удвоенной ширины*/
fprintf(ff,"\n");
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта
fprintf(ff,"\x1B\x50"); /*10-знаков*/

fprintf(ff,"                      Дата видачі %s р.\n\n",datdov->ravno());
class iceb_tu_str vidano(dolg.ravno());
vidano.plus(" ",komu.ravno());

fprintf(ff,"Видано");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
//fprintf(ff," %s %s\n",dolg.ravno(),komu.ravno());
fprintf(ff,"%-*.*s\n",iceb_tu_kolbait(70,vidano.ravno()),iceb_tu_kolbait(70,vidano.ravno()),vidano.ravno());
if(iceb_tu_strlen(vidano.ravno()) > 70)
 fprintf(ff,"%s\n",iceb_tu_adrsimv(70,vidano.ravno()));
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"\nДокумент що засвідчує особу");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s\n",dokum.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта


fprintf(ff,"серія");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %-*s",iceb_tu_kolbait(5,seriq.ravno()),seriq.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff," N");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %-*s",iceb_tu_kolbait(20,nomerdok.ravno()),nomerdok.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff," від");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s р.\n",datavid.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"\nВиданий");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s\n",vidan.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"\nНа отримання від");

fprintf(ff,"\x1B\x45"); //режим выделенного шрифта

fprintf(ff," %.*s\n",iceb_tu_kolbait(60,post.ravno()),post.ravno());
if(iceb_tu_strlen(post.ravno()) > 60)
 fprintf(ff,"%s\n",iceb_tu_adrsimv(60,post.ravno()));
 
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"цінностей за");
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff," %s %s\n",nomnar.ravno(),datanar.ravno());
fprintf(ff,"\x1B\x46"); //режим отмена выделенного шрифта

fprintf(ff,"\x1B\x4D"); //12 знаков на дюйм

if(metka_list == 0)
  fprintf(ff,"\f");

fprintf(ff,"\
Перелік цінностей, які належить отримати:\n");
fprintf(ff,"\
------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 N |         Найменування цінностей         |Од.вим.|     Кількість (прописом)     |\n");
fprintf(ff,"\
------------------------------------------------------------------------------------\n");
/*
123 1234567890123456789012345678901234567890 1234567 123456789012345678901234567890
*/
/*Читаем записи в документе*/
sprintf(strsql,"select zapis,ei,kol from Uddok1 where datd='%s' and nomd='%s'",
datdov->ravno_sqldata(),nomdov->ravno());
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }
char kolih[2048];

int nomzap=0;
int kol_str=0;
while(cur.read_cursor(&row) != 0)
 {
  memset(kolih,'\0',sizeof(kolih));
  strncpy(kolih,iceb_tu_nofwords(atof(row[2])),sizeof(kolih)-1);
//  strncpy(kolih,kkk(atof(row[2])),sizeof(kolih)-1);
  kol_str++;
  
  fprintf(ff,"%3d|%-*.*s|%-*.*s|%-*.*s|\n",
  ++nomzap,
  iceb_tu_kolbait(40,row[0]),iceb_tu_kolbait(40,row[0]),row[0],
  iceb_tu_kolbait(7,row[1]),iceb_tu_kolbait(7,row[1]),row[1],
  iceb_tu_kolbait(30,kolih),iceb_tu_kolbait(30,kolih),kolih);

  /**********1 дополнительная строка*********************/  
  if(iceb_tu_strlen(row[0]) > 40 && iceb_tu_strlen(kolih) <= 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-*.*s|%-7.7s|%-30.30s|\n","",
    iceb_tu_kolbait(40,iceb_tu_adrsimv(40,row[0])),
    iceb_tu_kolbait(40,iceb_tu_adrsimv(40,row[0])),
    iceb_tu_adrsimv(40,row[0]),
    "","");
   }
  if(iceb_tu_strlen(row[0]) > 40 && iceb_tu_strlen(kolih) > 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-*.*s|%-7.7s|%-*.*s|\n",
    "",
    iceb_tu_kolbait(40,iceb_tu_adrsimv(40,row[0])),
    iceb_tu_kolbait(40,iceb_tu_adrsimv(40,row[0])),
    iceb_tu_adrsimv(40,row[0]),
    "",
    iceb_tu_kolbait(30,iceb_tu_adrsimv(30,kolih)),
    iceb_tu_kolbait(30,iceb_tu_adrsimv(30,kolih)),
    iceb_tu_adrsimv(30,kolih));
   }

  if(iceb_tu_strlen(row[0]) <= 40 && iceb_tu_strlen(kolih) > 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-40.40s|%-7.7s|%-*.*s|\n","","","",
    iceb_tu_kolbait(30,iceb_tu_adrsimv(30,kolih)),
    iceb_tu_kolbait(30,iceb_tu_adrsimv(30,kolih)),
    iceb_tu_adrsimv(30,kolih));
   }

  /**********2 дополнительная строка*********************/  
  if(iceb_tu_strlen(row[0]) > 80 && iceb_tu_strlen(kolih) <= 60)
   {
    kol_str++;
    fprintf(ff,"%3s|%-*.*s|%-7.7s|%-30.30s|\n","",
    iceb_tu_kolbait(40,iceb_tu_adrsimv(80,row[0])),
    iceb_tu_kolbait(40,iceb_tu_adrsimv(80,row[0])),
    iceb_tu_adrsimv(80,row[0]),
    "","");
   }
  if(iceb_tu_strlen(row[0]) > 40 && iceb_tu_strlen(kolih) > 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-*.*s|%-7.7s|%-*.*s|\n",
    "",
    iceb_tu_kolbait(40,iceb_tu_adrsimv(80,row[0])),
    iceb_tu_kolbait(40,iceb_tu_adrsimv(80,row[0])),
    iceb_tu_adrsimv(80,row[0]),
    "",
    iceb_tu_kolbait(30,iceb_tu_adrsimv(60,kolih)),
    iceb_tu_kolbait(30,iceb_tu_adrsimv(60,kolih)),
    iceb_tu_adrsimv(60,kolih));
   }

  if(iceb_tu_strlen(row[0]) <= 40 && iceb_tu_strlen(kolih) > 30)
   {
    kol_str++;
    fprintf(ff,"%3s|%-40.40s|%-7.7s|%-*.*s|\n","","","",
    iceb_tu_kolbait(30,iceb_tu_adrsimv(60,kolih)),
    iceb_tu_kolbait(30,iceb_tu_adrsimv(60,kolih)),
    iceb_tu_adrsimv(60,kolih));
   }

  kolstr++;
  fprintf(ff,"\
------------------------------------------------------------------------------------\n");
 }

for(; kol_str < 10; kol_str+=2)
 {
  fprintf(ff,"%3s|%-40.40s|%-7.7s|%-30.30s|\n","","","","");
  fprintf(ff,"\
------------------------------------------------------------------------------------\n");
 }
fprintf(ff,"\n\
          Підпис_________________________________________________засвідчую\n\
                 (зразок підпису особи, що одержала довіреність)\n");
class iceb_t_fioruk_rk ruk;
class iceb_t_fioruk_rk glbuh;
iceb_t_fioruk(1,&ruk);
iceb_t_fioruk(2,&glbuh);
fprintf(ff,"\n\
          Керівник підприємства                         %s\n\n",
          ruk.famil_inic.ravno());
fprintf(ff,"\
          Головний бухгалтер                            %s\n\n",glbuh.famil_inic.ravno());

fprintf(ff,"\
          Місце печатки\n");


fclose(ff);


oth->spis_imaf.plus(imaf);
oth->spis_naim.plus(gettext("Распечатка доверенности"));



return(0);
}
/********************************************/
/*чтение содержимого правого уголка*/
/*************************************/
int xudrasdok_r(class SPISOK *prug)
{
class iceb_tu_str stroka("");
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select str from Alx where fil='xudrasdok.alx' order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {

  prug->plus(row[0]);
 }

return(0);

}

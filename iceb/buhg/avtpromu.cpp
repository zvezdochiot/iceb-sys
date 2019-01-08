/* $Id: avtpromu.c,v 5.78 2013/10/09 07:05:11 sasa Exp $ */
/*27.02.2017    19.10.1994      Белых А.И.      avtpromu.c
Автоматическое проведение проводок в материальном учете
*/
#include        <math.h>
#include        "buhg.h"

void 	vnis(const char*,short*,char,const char*,const char*,const char*,const char*,class SPISOK*,class masiv_din_double*,int metka_vkontr,FILE*);

extern class masiv_din_int dp,mp,gp; /*Даты последнего подтверждения*/
extern class masiv_din_double snn; /*Суммы по накладных*/
extern class masiv_din_double sn; /*Суммы по карточкам*/
extern class masiv_din_double sp; /*Суммы в проводках*/
extern SPISOK spsh;
extern class masiv_din_double sbnds; /*Суммы без НДС*/
extern double	*kofusl; //Коэффициенты распределения услуг
extern double	sumusl; //Сумма услуг по документа
extern char     *mprov; /*Массив проводок*/
extern double	*mdk;   /*Массив дебетов и кредитов к масиву проводок*/
extern SPISOK   vspsh; /*Список пар счетов для документов списания / внутреннего перемещения и изменения стоимости*/
extern double   *vsnn1; /*Суммы по приходному документа для пар счетов*/
extern double   *vsnn2; /*Суммы по расходному документа для пар счетов*/
extern short    ggg;
extern char	*kodopsp; //Коды операций сторнирования для приходов
extern char	*kodopsr; //Коды операций сторнирования для расходов

void avtpromu(int tipz, //1-приход 2-расход
short dd,short md,short gd, //Дата документа
const char *kop, //Код операции
const char *nn, //Номер документа
int skk, //Склад
const char *kor, //Код контрагента
double sumkor, //Сумма корректирвки суммы документа
const char *nn1, //Номер парного документа
float pnds,
FILE *ff_prot)
{
char		bros[512];
double          bb=0.,deb=0.,kre=0.;
char            shp1[64],shp2[64],shp3[64],shp31[64],shp4[64],shp5[64];
char            shp6[64],shp7[64],shp8[64];
class iceb_tu_str kontr1("");
class iceb_tu_str kontr2("");
class iceb_tu_str kontr3("");
class iceb_tu_str kontr31("");
class iceb_tu_str kontr4("");
class iceb_tu_str kontr5("");
class iceb_tu_str kontr6("");
class iceb_tu_str kontr7("");
class iceb_tu_str kontr8("");
char            bros1[512];
int             i,i1,i2;
class iceb_tu_str kto(ICEB_MP_MATU);
short		mvd;
class iceb_tu_str kvd("");
char		mopr=0;
char		kopz[64];
char		storno=0; /*0-Обычная 1-сторно*/
int		tpz;
short		mon,mvz; /*Метка оплаты накладной / метка выдачи запроса*/
class iceb_tu_str nai("");
time_t		vrem;
class iceb_tu_str korr("");
short		kp=0;
double		prockor;
char		strsql[1024];
SQL_str         row;
short		variant;
struct OPSHET	shetv;
double		suma_nds=0.;
double		suma_po_dokum=0.;
double		suma_s_nds=0.;
double          suma_ush=0.;
int		kolpr=2;
int		status_shu=0; //Статус счета учета материалла
double		sumandspr=0.; //Сумма НДС для приходных документов введенная вручную
int		metkands=0; //Для введенной вручную суммы ндс делать одну проводку на всю сумму
short           mshet=0; //Метка отсутствия настройки для счёта учета
int kekv=0; //код экономической классификации расходов
SQLCURSOR cur;
class SPISOK sp_prov; /*Сприсок проводок*/
class masiv_din_double sum_prov_dk; /*Суммы по дебетам/кредитам проводок*/
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

GDITE();
if(ff_prot != NULL)
 fprintf(ff_prot,"Автоматическое выполнение проводок\n");

if(dp.suma() == 0)
 {
  sprintf(strsql,"%s\n%d.%d.%d %s",gettext("Документ не подтверждён!"),dd,md,gd,nn);
  iceb_t_soob(strsql);
  return;
 }

/*
printw("\navtpromu-tp=%d %d.%d.%d kop=%s nn=%s\n",
tp,dd,md,gd,kop,nn);
OSTANOV();
*/
prockor=0.;
if(sumkor != 0)
 {
  prockor=sumkor*100./snn.suma();
 }
kp=mvz=mon=storno=mopr=mvd=0;
korr.new_plus(kor);
tpz=tipz;
if(tipz == 1)
 {
  if(proverka(kodopsp,kop,0,1) == 0)
   {
    storno=1;
    tpz=2;
   }
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d \
and nomd='%s' and nomerz=6",gd,skk,nn);
  if(readkey(strsql,&row,&cur) == 1)
   {
    sumandspr=atof(row[0]);
    metkands=1;
   }  
 }

if(tipz == 2)
 if(proverka(kodopsr,kop,0,1) == 0)
  {
   storno=1;
   tpz=1;
  }
strcpy(kopz,kop);

/*Если возврат с реализации то ищем накладную на реализацию
определяем какая там операция и делаем проводки по этой операции
только красным*/

sprintf(strsql,"select str from Alx where fil='avtpro.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки avtpro.alx\n");
  return;
 }

int metka_vkontr=2; /*метка режима выполнения проводки для контрагента если его нет в списке счёта 0-запрос 2-автоматическая вставка*/
if(iceb_t_poldan("Автоматическая запись нового контрагента в список счёта",strsql,"matnast.alx") == 0)
 {
   if(SRAV("Вкл",strsql,1) == 0)
    metka_vkontr=2;
   else
    metka_vkontr=0;
 }
if(ff_prot != NULL)
 {
  if(metka_vkontr == 0)
   fprintf(ff_prot,"Ручная вставка кода контрагента в список счёта с развёрнутым сальдо если его там нет\n");
  if(metka_vkontr == 2)
   fprintf(ff_prot,"Автоматическая вставка кода контрагента в список счёта с развёрнутым сальдо если его там нет\n");
 }
time(&vrem);

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  kontr1.new_plus("");    
  kontr2.new_plus("");    
  kontr3.new_plus("");    
  kontr31.new_plus("");    
  kontr4.new_plus("");    
  kontr5.new_plus("");    
  kontr6.new_plus("");    
  kontr7.new_plus("");    
  kontr8.new_plus("");    

  if(polen(row_alx[0],bros,sizeof(bros),1,'|') != 0)
   continue;
  i1=strlen(bros);
  variant=i2=0;
  for(i=0; i < i1; i++)
   {
    if(bros[i] == '+')
      i2=1;
    if(bros[i] == '-')
      i2=2;
    if(bros[i] == '1')
      variant=1;
   }
  if(i2 != tipz)
    continue;

    
  polen(row_alx[0],bros,sizeof(bros),2,'|');
  if(SRAV(kopz,bros,0) != 0)
    continue;

//  printw("kopz=%s bros=%s\n",kopz,bros);
//  OSTANOV();
  
  mopr=1;
  
  shp1[0]='\0';
  shp2[0]='\0';
  shp3[0]='\0';
  shp31[0]='\0';
  shp4[0]='\0';
  shp5[0]='\0';
  shp6[0]=shp7[0]=shp8[0]='\0';

  polen(row_alx[0],shp1,sizeof(shp1),3,'|');
  if((i=spsh.find(shp1)) < 0)
   {
    continue;
   }

  mshet=1;
    
  if(mp.ravno(i) == 0 || gp.ravno(i) == 0)
   continue;

  if(iceb_t_pvglkni(mp.ravno(i),gp.ravno(i)) != 0)
   continue;

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s\n",row_alx[0]);

  polen(row_alx[0],shp2,sizeof(shp2),4,'|');

  polen(row_alx[0],bros,sizeof(bros),5,'|');
  if(polen(bros,shp3,sizeof(shp3),1,',') != 0)
    strncpy(shp3,bros,sizeof(shp3));
  else
    polen(bros,shp31,sizeof(shp31),2,',');     

  polen(row_alx[0],shp4,sizeof(shp4),6,'|');
  polen(row_alx[0],shp5,sizeof(shp5),7,'|');
  polen(row_alx[0],shp6,sizeof(shp6),8,'|');
  polen(row_alx[0],shp7,sizeof(shp7),9,'|');
  polen(row_alx[0],shp8,sizeof(shp8),10,'|');

  move(20,0);
  if(mvd == 0 && shp6[0] != '\0')
   {
    mvd++;
    VVOD  VVOD1(2);

    helstr(LINES-1,0,"F10",gettext("выход"),
    "Enter",gettext("просмотр кодов"),NULL);

    if(tpz == 2)
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код валового дохода"));
    if(tpz == 1)
      VVOD1.VVOD_SPISOK_add_MD(gettext("Введите код валового расхода"));

    kvd.new_plus("");    

    if((i1=vvod1(&kvd,10,&VVOD1,NULL,stdscr,-1,-1)) == FK10)
     {
      return;
     }
    if(i1 == ESC)
     {
      return;
     }

    if(i1 == ENTER && kvd.getdlinna() <= 1)
     {
      kvd.new_plus("");
      nai.new_plus("");
      
      if(dirskont(shp6,&kvd,&nai,1) != 0)
       {
        return;
       }
     }

    sprintf(strsql,"select kodkon from Skontr where ns='%s' and kodkon='%s'",shp6,kvd.ravno());
    if(readkey(strsql,&row,&cur) != 1)
     {
      sprintf(strsql,gettext("Не найден код %s в списке счета %s !"),kvd.ravno(),shp6);
      iceb_t_soob(strsql);
      return;
     }
    GDITE();
   }

  if(shp1[0] == '\0')
   continue;

  kontr1.new_plus(korr.ravno());   
  if(shp1[0] != '\0')
    if(provsh(&kontr1,shp1,&shetv,metka_vkontr,1) != 0)
     {
      return;
     }
  status_shu=shetv.stat;
  
  kontr2.new_plus(korr.ravno());   
  if(shp2[0] != '\0')
    if(provsh(&kontr2,shp2,&shetv,metka_vkontr,0) != 0)
     {
      return;
     }

  kontr3.new_plus(korr.ravno());   
  if(shp3[0] != '\0')
    if(provsh(&kontr3,shp3,&shetv,metka_vkontr,0) != 0)
     {
      return;
     }

  if(shp31[0] != '\0')
   {
    if(mvz == 0)
     {

      VVOD MENU(4);
      MENU.VVOD_SPISOK_add_ZAG(gettext("Выберите нужный счёт для НДС"));

      MENU.VVOD_SPISOK_add_MD(shp3);
      MENU.VVOD_SPISOK_add_MD(shp31);
      int kom2=0;
      while(menu3w(stdscr,&MENU,&kom2,-1,-1,0) != 0);
      if(kom2 == 1)
       {
        class iceb_tu_str kontrb(korr.ravno());
        if(provsh(&kontrb,shp31,&shetv,metka_vkontr,0) != 0)
          return;

        if(kontrb.getdlinna() > 1)
         korr.new_plus(kontrb.ravno());
        kontr31.new_plus(korr.ravno());        
        mon=1;
       }
     }
    if(mvz == 1 && mon == 1)
     {
      class iceb_tu_str kontrb(korr.ravno());
      if(provsh(&kontrb,shp31,&shetv,metka_vkontr,0) != 0)
       return;

        if(kontrb.getdlinna() > 1)
         korr.new_plus(kontrb.ravno());
        kontr31.new_plus(korr.ravno());        
     }
    mvz=1;

   }

  if(shp4[0] != '\0')
   {
    kontr4.new_plus(korr.ravno());   
    if(provsh(&kontr4,shp4,&shetv,metka_vkontr,0) != 0)
      return;

    if(kontr4.getdlinna() > 1)
     korr.new_plus(kontr4.ravno());
   }
   
  if(shp5[0] != '\0')
   {
    kontr5.new_plus(korr.ravno());   
    if(provsh(&kontr5,shp5,&shetv,metka_vkontr,0) != 0)
      return;
    if(kontr5.getdlinna() > 1)
     korr.new_plus(kontr5.ravno());
   }

  if(shp6[0] != '\0')
   {
    kontr6.new_plus(kvd.ravno());   
    if(provsh(&kontr6,shp6,&shetv,0,0) != 0)
      return;
    if(kontr6.getdlinna() > 1)
     kvd.new_plus(kontr6.ravno());
   }
  if(shp7[0] != '\0')
   {
    kontr7.new_plus(korr.ravno());   
    if(provsh(&kontr7,shp7,&shetv,metka_vkontr,0) != 0)
      return;
    if(kontr7.getdlinna() > 1)
     korr.new_plus(kontr7.ravno());
   }

  if(shp8[0] != '\0')
   {
    kontr8.new_plus(korr.ravno());   
    if(provsh(&kontr8,shp8,&shetv,metka_vkontr,0) != 0)
     {
      return;
     }
    if(kontr8.getdlinna() > 1)
     korr.new_plus(kontr8.ravno());
   }

  bb=sn.ravno(i);

  bb=okrug(bb,0.01);
  memset(bros1,'\0',sizeof(bros1));
/*
  printw("\n1=%s 2=%s 3=%s 31=%s 4=%s 5=%s 6=%s 7=%s 8=%s\n",
  shp1,shp2,shp3,shp31,shp4,shp5,shp6,shp7,shp8);
  OSTANOV();
*/  
  /*** ПРИХОДЫ *****/
  if(tpz == 1)
   {
    if(sumandspr != 0.)
      suma_nds=sumandspr;
    else
      suma_nds=((snn.ravno(i)+snn.ravno(i)*prockor/100.)-sbnds.ravno(i))*pnds/100.;

    suma_nds=okrug(suma_nds,0.01);

    if(status_shu == 1)
     {
      deb=bb; kre=0.;
      if(storno == 1)
        deb*=(-1);

      sprintf(bros1,"N%s",nn1);

      zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp1,"",kontr1.ravno(),kontr1.ravno(),deb,kre,bros1,1,kekv,&sp_prov,&sum_prov_dk,ff_prot);
      continue;
     }
    if(shp4[0] != '\0' && shp6[0] == '\0' && shp1[0] != '\0')
     { 
      deb=bb; kre=0.;
      if(storno == 1)
       deb*=-1;
      sprintf(bros1,"N%s",nn1);

      zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp1,shp4,kontr1.ravno(),kontr4.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
     }

    sprintf(bros1,"N%s",nn1);

    /*НДС оплачено*/
    if(shp3[0] != '\0' && shp4[0] != '\0' && mon == 0 && metkands < 2)
     {
      if(fabs(suma_nds) > 0.009)      
       {
        deb=suma_nds; kre=0.;
        if(storno == 1)
          deb*=(-1);
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp3,shp4,kontr3.ravno(),kontr4.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
      if(metkands == 1)
        metkands++;
     }       

    /*НДС не оплачено*/
    if(shp31[0] != '\0' && shp4[0] != '\0' && mon == 1 && metkands < 2)
     {

      if(fabs(suma_nds) > 0.009)      
       {
        sprintf(bros1,"N%s",nn1);
        deb=suma_nds; kre=0.;
        if(storno == 1)
          deb*=(-1);
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp31,shp4,kontr31.ravno(),kontr4.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
      if(metkands == 1)
        metkands++;
     }

    if(shp6[0] != '\0')
     {
      bb=sn.ravno(i);
      bb=okrug(bb,0.01);
                 
      if(fabs(bb) > 0.009  && shp4[0] != '\0')      
       {
        deb=bb; kre=0.;
        if(storno == 1)
          deb*=(-1);
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp6,shp4,kontr6.ravno(),kontr4.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
      bb=sn.ravno(i);
      bb=okrug(bb,0.01);

      
      if(fabs(bb) > 0.009)      
       {
        strcat(bros1," ");
        strcat(bros1,shp4);
        strcat(bros1,"-код");
        strcat(bros1,korr.ravno());
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp1,shp6,kontr1.ravno(),kontr6.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }

    /*Проводка на сумму денег выданной из кассы*/
    if(shp7[0] != '\0' && shp2[0] != '\0')
     {
      bb=snn.ravno(i)+snn.ravno(i)*prockor/100.;
      bb=bb+(bb-sbnds.ravno(i))*pnds/100.;
      bb=okrug(bb,0.01);
      if(fabs(bb) > 0.009)      
       {
        memset(bros1,'\0',sizeof(bros1));
        deb=bb; kre=0.;
        if(storno == 1)
         deb*=(-1);
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp2,shp7,kontr2.ravno(),kontr7.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }
   }

  /******** РАСХОДЫ **********/
  if(tpz == 2)
   {
    //Сумма учета материалов
    suma_ush=sn.ravno(i)+sumusl*kofusl[i];
    suma_ush=okrug(suma_ush,0.01);

    //Общая сумма по документа с учётом скидки
    suma_po_dokum=snn.ravno(i)+sumusl*kofusl[i]+(snn.ravno(i)+sumusl*kofusl[i])*prockor/100.;
    suma_po_dokum=okrug(suma_po_dokum,0.01);

    //Вычисляем НДС             сумму без ндс тоже уменьшаем на скидку
    suma_nds=(suma_po_dokum-(sbnds.ravno(i)+sbnds.ravno(i)*prockor/100.))*pnds/100.;
    suma_nds=okrug(suma_nds,0.01);

    //Сумма с НДС
    suma_s_nds=suma_po_dokum+suma_nds;
    suma_s_nds=okrug(suma_s_nds,0.01);

    if(vsnn1 != NULL && vsnn2 != NULL) //Внутреннее перемещение и изменение стоимости
     {
      vnis(korr.ravno(),&kp,storno,kontr5.ravno(),shp5,shp1,nn1,&sp_prov,&sum_prov_dk,metka_vkontr,ff_prot);
      continue;
     }

    if(status_shu == 1)
     {
      if(shp8[0] != '\0')
       {
        deb=bb; kre=0.;
        if(storno == 1)
          deb*=(-1);
        /*Проводка на сумму Учета*/  
        if(ff_prot != NULL)
         fprintf(ff_prot,"Проводка на сумму учёта\n%d.%d.%d sh=%s shk=%s deb=%.2f kre=%.2f\n",
         dp.ravno(i),mp.ravno(i),gp.ravno(i),shp8,"",deb,kre);
         
         
         
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp8,"",kontr8.ravno(),kontr1.ravno(),deb,kre,bros1,1,kekv,&sp_prov,&sum_prov_dk,ff_prot);
        continue;
       }
     }

    if(shp8[0] != '\0' && shp1[0] != '\0')
     {
      deb=bb; kre=0.;
      if(storno == 1)
       deb*=(-1);

      /*Проводка на сумму Учета*/  
      zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp8,shp1,kontr8.ravno(),kontr1.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
     }

    /*НДС оплачено*/
    if(shp3[0] != '\0' && shp2[0] != '\0' && mon == 0)
     {
      bb=suma_nds;
      if(fabs(bb) > 0.009)      
       {
        deb=bb; kre=0.;
        if(storno == 1)
           deb*=(-1);
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp2,shp3,kontr2.ravno(),kontr3.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }       

    /*НДС не оплачено*/
    if(shp2[0] != '\0' && shp31[0] != '\0' && mon == 1)
     {
      bb=suma_nds;
         
      if(fabs(bb) > 0.009)      
       {
        deb=bb; kre=0.;
        if(storno == 1)
           deb*=(-1);
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp2,shp31,kontr2.ravno(),kontr31.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }       

     /*Проводка на сумму реализации*/

    if(shp4[0] != '\0' && shp6[0] == '\0' && shp2[0] != '\0')
     {
      bb=suma_s_nds;
      if(fabs(bb) > 0.009)      
       {
        deb=bb; kre=0.;
        if(storno == 1)
         {
          kre=deb;
          deb=0.;
         }
  /*
        printw("deb=%.2f storno=%d\n",deb,storno);
        OSTANOV(); 
  */
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp4,shp2,kontr4.ravno(),kontr2.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }

    if(shp5[0] != '\0' && shp6[0] == '\0' && shp8[0] != '\0' && variant == 0)
     {
      bb=suma_ush;
      if(fabs(bb) > 0.009)      
       {
        deb=bb; kre=0.;
        if(storno == 1)
         {
          kre=deb;
          deb=0.;
         }

  /*
        printw("deb=%.2f storno=%d\n",deb,storno);
        OSTANOV(); 
  */
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp5,shp8,kontr5.ravno(),kontr8.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }

    if(shp6[0] != '\0')
     {
      bb=suma_po_dokum;
      if(variant == 0)
        bb=suma_s_nds;

      strcat(bros1," ");
      strcat(bros1,shp4);
      strcat(bros1,"-код");
      strcat(bros1,korr.ravno());

      bb=okrug(bb,0.01);

      if(fabs(bb) > 0.009 && shp2[0] != '\0')      
       {
        deb=bb; kre=0.;
        if(storno == 1)
         {
          kre=deb;
          deb=0.;
         }
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp6,shp2,kontr6.ravno(),kontr2.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
      bb=suma_po_dokum;
      if(fabs(bb) > 0.009  && shp4[0] != '\0')      
       {
        memset(bros1,'\0',sizeof(bros1));
        strcat(bros1,shp2);
        deb=bb; kre=0.;

        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp4,shp6,kontr4.ravno(),kontr6.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
      bb=suma_nds;
      if(fabs(bb) > 0.009 && shp4[0] != '\0' && variant == 0)      
       {
        memset(bros1,'\0',sizeof(bros1));

        strcat(bros1,shp2);
        strcat(bros1,"-ПДВ");
        deb=bb; kre=0.;
        if(storno == 1)
         {
          kre=deb;
          deb=0.;
         }
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp4,shp6,kontr4.ravno(),kontr6.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }

      if(fabs(bb) > 0.009 && shp2[0] != '\0' && variant == 1)      
       {
        deb=bb; kre=0.;
        if(storno == 1)
         {
          kre=deb;
          deb=0.;
         }
        memset(bros1,'\0',sizeof(bros1));
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp4,shp2,kontr4.ravno(),kontr2.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }


    /*Проводка на сумму дохода*/
    if(shp5[0] != '\0')
     {
      memset(bros1,'\0',sizeof(bros1));
      bb=suma_po_dokum;
      if(fabs(bb) > 0.009 && shp2[0] != '\0')      
       {
        deb=bb; kre=0.;
        if(storno == 1)
         {
          kre=deb;
          deb=0.;
         }
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp2,shp5,kontr2.ravno(),kontr5.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }       

    /*Проводка на сумму денег сданной в кассу*/
    if(shp7[0] != '\0' && shp2[0] != '\0')
     {
      bb=suma_s_nds;
      if(fabs(bb) > 0.009 )      
       {
        memset(bros1,'\0',sizeof(bros1));
        deb=bb; kre=0.;
        if(storno == 1)
         {
          kre=deb;
          deb=0.;
         }
        zapmpr(gp.ravno(i),mp.ravno(i),dp.ravno(i),shp7,shp2,kontr7.ravno(),kontr2.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }            
   }
 }


/*Запись проводок из памяти в базу*/

zapmpr1(nn,kop,skk,vrem,kto.ravno(),dd,md,gd,tipz,&sp_prov,&sum_prov_dk,ff_prot); /*zapmpr.c*/


if(mopr == 0)
 {
  sprintf(strsql,gettext("Для операции %s не найдена настройка проводок !"),kopz);
  iceb_t_soob(strsql);
 }   
else
 {
  if(mshet == 0)
   {
    VVOD SOOB(1);
    sprintf(strsql,"%s:%s",gettext("Операция"),kop);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    sprintf(strsql,"%s",gettext("Нет настройки для всех счетов учета этого документа"));
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
   }
 }


}
/************************************************************************/
/*Выполнение проводок для  внутреннего перемещения и изменения стоимости*/
/*************************************************************************/

void vnis(const char *kontr,short *kp,char storno,
const char *kontrraz, //Контрагент счета разницы
const char *shetraz, //Счет на который пишем разницу если она есть
const char *shetobr, //Счет который обрабатываем
const char *nn1, //Номер парного документа
class SPISOK *sp_prov,
class masiv_din_double *sum_prov_dk,
int metka_vkontr,
FILE *ff_prot)
{
char	sheta[112];
char	shet1[64];
char	shet2[64];
class iceb_tu_str kontr1("");
class iceb_tu_str kontr2("");
int	noms=0;
int	nomer=0;
double  deb=0.,kre=0.;
double  bb=0.;
struct OPSHET	shetv;
int	kolpr=2;
short	status1=0,status2=0;
char	strsql[512];
int kekv=0;

//OSTANOV();
int kolsh=vspsh.kolih();
if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\nВыполнение проводок для внутреннего перемещения\nvnis kontr=%s kp=%d kolsh=%d\n",kontr,*kp,kolsh);
  
  for(int ii=0; ii< kolsh;ii++)
   fprintf(ff_prot,"vspsh[%d]=%s\n",ii,vspsh.ravno(ii));
 }

for(nomer=0; nomer < kolsh ; nomer++ )
 {
  strncpy(sheta,vspsh.ravno(nomer),sizeof(sheta)-1);
//  printw("%s\n",sheta);
  if(sheta[0] == '\0')
    break;
  if(ff_prot != NULL)
   fprintf(ff_prot,"sheta=%s nomer=%d\n",sheta,nomer);  
  polen(sheta,shet1,sizeof(shet1),1,',');  
  if(SRAV(shet1,shetobr,0) != 0)
    continue;

  polen(sheta,shet2,sizeof(shet2),2,',');  

  kontr1.new_plus(kontr);   
  if(provsh(&kontr1,shet1,&shetv,metka_vkontr,1) != 0)
      return;

  status1=shetv.stat;
    
  kontr2.new_plus(kontr);   
  if(provsh(&kontr2,shet2,&shetv,metka_vkontr,1) != 0)
      return;
  status2=shetv.stat;
  if(status1 != status2)
   {
    VVOD SOOB(1);
    SOOB.VVOD_SPISOK_add_MD(gettext("Проводки между балансовым и небалансовым счетами невозможны !"));
    sprintf(strsql,"%s & %s",shet1,shet2);
    SOOB.VVOD_SPISOK_add_MD(strsql);
    soobshw(&SOOB,stdscr,-1,-1,0,1);
    continue;
   }

  if((noms=spsh.find(shet1)) < 0)
    continue;
  if(nn1[0] != '\0') //Для внутреннего перемещения
   {
    bb=vsnn1[nomer]-vsnn2[nomer];
    
    if(bb >= 0.)
     {
      deb=vsnn2[nomer]; kre=0.;
     }
    else
     {
      deb=vsnn1[nomer]; kre=0.;
     }
   }
  else
   {
    deb=vsnn2[nomer]; kre=0.; //Списание
   }

  if(storno == 1)
    deb*=(-1);

  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"\n%d.%d.%d %f %f shetraz=%s\n",gp.ravno(noms),mp.ravno(noms),dp.ravno(noms),vsnn2[nomer],vsnn1[nomer],shetraz);
    fprintf(ff_prot,"shet2=%s shet1=%s deb=%.2f kre=%.2f\n",shet2,shet1,deb,kre);    
   }

  if(fabs(deb) > 0.009 || fabs(kre) > 0.009)
   {
    if(status1 == 1)
     {
      zapmpr(gp.ravno(noms),mp.ravno(noms),dp.ravno(noms),shet2,"",kontr2.ravno(),kontr1.ravno(),deb,kre,"",1,kekv,sp_prov,sum_prov_dk,ff_prot);
      zapmpr(gp.ravno(noms),mp.ravno(noms),dp.ravno(noms),shet1,"",kontr2.ravno(),kontr1.ravno(),kre,deb,"",1,kekv,sp_prov,sum_prov_dk,ff_prot);
     }    
    else
      zapmpr(gp.ravno(noms),mp.ravno(noms),dp.ravno(noms),shet2,shet1,kontr2.ravno(),kontr1.ravno(),deb,kre,"",kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
   }

//Проводки на сумму разницы в цене
  if(status1 == 0)
  if(fabs(vsnn2[nomer]-vsnn1[nomer]) > 0.009 && shetraz[0] != '\0')
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Проводки на сумму разницы в цене\n");
    bb=vsnn1[nomer]-vsnn2[nomer];
    
    if(fabs(bb) > 0.)
     {
      deb=bb; kre=0.;
      if(storno == 1)
        deb*=-1;
     
      zapmpr(gp.ravno(noms),mp.ravno(noms),dp.ravno(noms),shet2,shetraz,kontr2.ravno(),kontrraz,deb,kre,"",kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
     }
    else
     {
      deb=bb*-1; kre=0.;
      if(storno == 1)
        deb*=-1;
      zapmpr(gp.ravno(noms),mp.ravno(noms),dp.ravno(noms),shetraz,shet1,kontrraz,kontr1.ravno(),deb,kre,"",kolpr,kekv,sp_prov,sum_prov_dk,ff_prot);
     }
         
   }
 }


}

/* $Id: zarnast.c,v 5.90 2014/08/31 06:18:19 sasa Exp $ */
/*01.06.2017    10.12.1993      Белых А.И.      zarnast.c
Читаем настройку на зарплату
*/
#include        "buhg.h"


float  kof_prog_min=0.; //Коэффициент прожиточного минимума
float  ppn=0.; //Процент подоходного налога
float  pomzp=0.; //Процент от минимальной заработной платы

double          okrg; /*Округление*/

short		metkarnb=0; //0-расчёт налогов для бюджета пропорциональный 1-последовательный
short           *knvr=NULL;/*Коды начислений не входящие в расчёт подоходного налога*/
short           *kn_bsl=NULL; //Коды начислений на которые не распространяется льгота по подоходному налогу
short           *kuvr=NULL;/*Коды удержаний входящие в расчёт подоходного налога*/
short           *knnf=NULL; //Коды начислений недежными формами
short           *knnf_nds=NULL; /*Коды начислений неденежными формами на которые начисляется НДС*/
short           kodpn;   /*Код подоходного налога*/
short kodvn=0; /*код военного сбора*/
short kodvs_b=0; /*код военного сбора с больничного*/
class iceb_tu_str kodvn_nvr(""); /*Коды не входящие в расчёт военного сбора*/
short kodpn_sbol=0;/*Код подоходного налога с больничного*/
class iceb_tu_str kodpn_all; /*Все коды подоходного налога*/
short kodvpn=0; /*Код возврата подоходного налога*/
short           kodperpn;   /*Код перерасчёта подоходного налога*/
short		kodindex;  /*Код индексации*/
short		kodindexb;  /*Код индексации больничного*/

short           *kuvvr_pens_sr=NULL; //Коды удержаний входящие в расчёт при отчислении с работника
short           *kuvvr_pens_sfz=NULL; //Коды удержаний входящие в расчёт при отчислении с фонда зарплаты
char            *p_shet_inv=NULL; //счет для начисления пенсионных отчислений на фонд зарплаты для инвалидов (если для них нужен другой счет)

short		*kodnvrindex=NULL; //Коды не входящие в расчёт индексации
char		*shetsoc=NULL; //Счета фонда соц-страха не бюджет/бюджет

short           *kodmp=NULL;  //Коды благотворительной помощи
short           *kodbl=NULL;  /*Коды начисления больничного*/
short		koddopmin; /*Код доплаты до минимальной зарплаты*/
short           *kodnvmin=NULL; /*Коды не входящие в расчёт доплаты до минимальной зарплаты*/
short           *kodotrs=NULL;  /*Коды отдельного расчета доплаты до минимпльной зарплаты*/

short		*obud=NULL; /*Обязательные удержания*/
short		*obnah=NULL; /*Обязательные начисления*/
short		startgodz; /*Стаpтовый год для расчёта сальдо*/
short		startgodb; /*Стартовый год просмотров главной книги*/

class iceb_tu_str shrpz; /*Счет расчётов по зарплате*/
class iceb_tu_str shrpzbt; /*Счет расчётов по зарплате бюджет*/
class SPISOK dop_zar_sheta; //Дополнительные зарплатные счёта
class SPISOK dop_zar_sheta_spn; /*Списки начислений для каждого дополнительного зарплатного счёта*/
char		*shetb=NULL; /*Бюджетные счета начислений*/
char		*shetbu=NULL; /*Бюджетные счета удержаний*/
char		*shetfb=NULL; /*Счета фонда безработицы*/
char		*shetpf=NULL; /*Счета пенсионного фонда*/

short		kuprof=0; //Код удержания в профсоюзный фонд
float		procprof=0.; //Процент отчисления в профсоюзный фонд
short		*knvrprof=NULL; //Коды не входящие в расчёт удержаний в профсоюзный фонд

double          snmpz=0.; /*Сумма не облагаемой материальной помощи заданная*/

short		pehfam=0; //0-фамилия в корешке мелко 1-крупно

short *kodkomand=NULL; //Коды командировочных расходов
short *knvr_komand=NULL; //Коды начислений не входящие в расчёт командировочных
short kodt_b_komand=0; //Код бюджетного табеля командировочных
short *ktnvr_komand=NULL; //Коды табеля не входящие в расчёт командировочных
short koddzn=0; /*Код доплаты до средней зарплаты в командировке с учётом надбавки за награду*/
short *kvvrdzn=NULL; /*Коды входящие в расчёт доплаты до средней зарплаты в командировке с учётом надбавки за награду*/
short kod_komp_z_kom=0; /*Код компенсации за командировку*/
 
short kod_ud_dnev_zar=0; //Код удержания дневного заработка
short *kodnah_nvr_dnev_zar=NULL;  //Коды начислений не входящие в расчёт удержания дневного заработка
short *kodtab_nvr_dnev_zar=NULL;  //Коды табеля не входящие в расчёт удержания однодневного заработка

int kekv_at=0;  //Код экономической классификации для атестованых
int kekv_nat=0; //Код экономической классификации для не атестованых
int kodzv_nat=0;//Код звания неатестованный
int kekv_nfo=0; //Код экономической классификации для начислений на фонд оплаты
class iceb_tu_str kod_prem_z_kvrt; /*коды премии за квартал*/

short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/

int             zarnast(void)
{
int		i;
class iceb_tu_str str("");
class iceb_tu_str bros("");


dop_zar_sheta.free_class(); /*Обнуляем список*/
dop_zar_sheta_spn.free_class(); /*Обнуляем список*/
kod_komp_z_kom=0;
koddzn=0;
kodt_b_komand=0;
kuprof=metkarnb=0;

snmpz=0.;
shrpz.new_plus("");
shrpzbt.new_plus("");

kof_prog_min=ppn=pomzp=0.;
kodpn_all.new_plus("");
kod_prem_z_kvrt.new_plus("");

kodvn=kodvs_b=0;

kodvn_nvr.new_plus("");
if(ktnvr_komand != NULL)
 {
  delete [] ktnvr_komand;
  ktnvr_komand=NULL;
 }

if(knvr_komand != NULL)
 {
  delete [] knvr_komand;
  knvr_komand=NULL;
 }

if(knvrprof != NULL)
 {
  delete [] knvrprof;
  knvrprof=NULL;
 }

if(kuvvr_pens_sfz != NULL)
 {
  delete [] kuvvr_pens_sfz;
  kuvvr_pens_sfz=NULL;
 }

if(kuvvr_pens_sr != NULL)
 {
  delete [] kuvvr_pens_sr;
  kuvvr_pens_sr=NULL;
 }
if(kodnvrindex != NULL)
 {
  delete [] kodnvrindex;
  kodnvrindex=NULL;
 }

if(obud != NULL)
 {
  delete [] obud;
  obud=NULL;
 }
if(obnah != NULL)
 {
  delete [] obnah;
  obnah=NULL;
 }



if(knvr != NULL)
 {
  delete [] knvr;
  knvr=NULL;
 }
if(knnf != NULL)
 {
  delete [] knnf;
  knnf=NULL;
 }
if(knnf_nds != NULL)
 {
  delete [] knnf_nds;
  knnf_nds=NULL;
 }
if(kn_bsl != NULL)
 {
  delete [] kn_bsl;
  kn_bsl=NULL;
 }
if(kodnvmin != NULL)
 {
  delete [] kodnvmin;
  kodnvmin=NULL;
 }
if(kodotrs != NULL)
 {
  delete [] kodotrs;
  kodotrs=NULL;
 }

if(kuvr != NULL)
 {
  delete [] kuvr;
  kuvr=NULL;
 }

if(kodmp != NULL)
 {
  delete [] kodmp;
  kodmp=NULL;
 }


if(shetb != NULL)
 {
  delete [] shetb;
  shetb=NULL;
 }
if(shetbu != NULL)
 {
  delete [] shetbu;
  shetbu=NULL;
 }

if(shetfb != NULL)
 {
  delete [] shetfb;
  shetfb=NULL;
 }
if(kodkomand != NULL)
 {
  delete [] kodkomand;
  kodkomand=NULL;
 }

if(kvvrdzn != NULL)
 {
  delete [] kvvrdzn;
  kvvrdzn=NULL;
 }

if(shetpf != NULL)
 {
  delete [] shetpf;
  shetpf=NULL;
 }
if(shetsoc != NULL)
 {
  delete [] shetsoc;
  shetsoc=NULL;
 }
if(p_shet_inv != NULL)
 {
  delete [] p_shet_inv;
  p_shet_inv=NULL;
 }

if(kodbl != NULL)
 {
  delete [] kodbl;
  kodbl=NULL;
 }

if(kodnah_nvr_dnev_zar != NULL)
 {
  delete [] kodnah_nvr_dnev_zar;
  kodnah_nvr_dnev_zar=NULL;
 }
if(kodtab_nvr_dnev_zar != NULL)
 {
  delete [] kodtab_nvr_dnev_zar;
  kodtab_nvr_dnev_zar=NULL;
 }

//nomin=0.;
procprof=0.;
pehfam=koddopmin=startgodz=0;
kod_ud_dnev_zar=0;
okrg=0.;

kodindex=kodindexb=0;
kodperpn=vplsh=kodpn=kodpn_sbol=kodvpn=0;
kekv_at=kekv_nat=kodzv_nat=kekv_nfo=0;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='zarnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  msql_error(&bd,gettext("Ошибка создания курсора !"),strsql);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarnast.alx");
  iceb_t_soob(strsql);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  polen(row_alx[0],&str,1,'|');

  class iceb_tu_str spisok_kodov("");


  if(SRAV(str.ravno(),"Бюджетные счета",0) == 0)
   {
     polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     if(i != 1)
      {
       if((shetb=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s shetb !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
        strcpy(shetb,bros.ravno());
      }
           
     continue;

   }
  if(SRAV(str.ravno(),"Бюджетные счета удержаний",0) == 0)
   {
     polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     if(i != 1)
      {
       if((shetbu=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s shetbu !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
        strcpy(shetbu,bros.ravno());
      }
     continue;

   }

  if(SRAV(str.ravno(),"Счета пенсионного отчисления",0) == 0)
   {
     polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     if(i != 1)
      {
       if((shetpf=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s shetpf !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
         strcpy(shetpf,bros.ravno());
      }
     continue;

   }
  if(SRAV(str.ravno(),"Счета фонда социального страхования",0) == 0)
   {
     polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     if(i != 1)
      {
       if((shetsoc=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s shetsoc !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
        strcpy(shetsoc,bros.ravno());
      }
     continue;

   }

  if(SRAV(str.ravno(),"Счет отчисления в пенсионный фонд инвалидами (фонд зарплаты)",0) == 0)
   {
     polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     if(i != 1)
      {
       if((p_shet_inv=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s p_shet_inv !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
        strcpy(p_shet_inv,bros.ravno());
      }
     continue;

   }

  if(SRAV(str.ravno(),"Дополнительные зарплатные счета",0) == 0)
   {
     polen(row_alx[0],&bros,2,'|');
     iceb_t_fplus(0,bros.ravno(),&dop_zar_sheta,&cur_alx);
     continue;
   }

  if(SRAV(str.ravno(),"Счета отчисления в фонд безработицы",0) == 0)
   {
     polen(row_alx[0],&bros,2,'|');
     i=strlen(bros.ravno())+1;
     if(i != 1)
      {
       if((shetfb=new char[i]) == NULL)
        {
         sprintf(strsql,"%s-%s shetfb !",__FUNCTION__,gettext("Не могу выделить память для"));
         iceb_t_soob(strsql);
        }
       else
        strcpy(shetfb,bros.ravno());
      }
     continue;

   }

  if(SRAV(str.ravno(),"Код командировочного начисления",0) == 0)
   {
    iceb_t_sozmas(&kodkomand,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str.ravno(),"Код компенсации за командировку",0) == 0)
   {
    polen(row_alx[0],&kod_komp_z_kom,2,'|');
    continue;
   }

  if(SRAV(str.ravno(),"Коды премии за квартал",0) == 0)
   {
    polen(row_alx[0],&kod_prem_z_kvrt,2,'|');
    continue;
   }

  if(SRAV(str.ravno(),"Коды, которые нужно взять в расчёт для расчёта доплаты до средней зарплаты в командировке",0) == 0)
   {
    iceb_t_sozmas(&kvvrdzn,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str.ravno(),"Код доплаты до средней зарплаты в командировке c учётом надбавки за награду",0) == 0)
   {
    polen(row_alx[0],&koddzn,2,'|');
    continue;
   }


  if(SRAV(str.ravno(),"Код бюджетного табеля командировочных",0) == 0)
   {
    polen(row_alx[0],&kodt_b_komand,2,'|');
    continue;
   }
  if(SRAV(str.ravno(),"Профсоюзный фонд-код отчисления",0) == 0)
   {
    polen(row_alx[0],&kuprof,2,'|');
    continue;
   }
  if(SRAV(str.ravno(),"Профсоюзный фонд-процент отчисления",0) == 0)
   {
    polen(row_alx[0],&procprof,2,'|');
    continue;
   }
  if(SRAV(str.ravno(),"Код удержания дневного заработка",0) == 0)
   {
    polen(row_alx[0],&kod_ud_dnev_zar,2,'|');
    continue;
   }

  if(SRAV(str.ravno(),"Стартовый год",0) == 0)
    {
     polen(row_alx[0],&startgodz,2,'|');
     continue;
    }
/****************************
  if(SRAV(str.ravno(),"Автоматическая вставка кода контрагента в список счета",0) == 0)
    {
     polen(row_alx[0],&str,2,'|');
     avkvs=0;
     if(SRAV(str.ravno(),"Вкл",1) == 0)
       avkvs=1;
     continue;
    }
**************************/
  if(SRAV(str.ravno(),"Печатать фамилию в корешке крупно",0) == 0)
    {
     polen(row_alx[0],&str,2,'|');
     pehfam=0;
     if(SRAV(str.ravno(),"Вкл",1) == 0)
       pehfam=1;
     continue;
    }

  if(SRAV(str.ravno(),"Расчёт налогов последовательный",0) == 0)
    {
     polen(row_alx[0],&str,2,'|');
     metkarnb=0;
     if(SRAV(str.ravno(),"Вкл",1) == 0)
       metkarnb=1;
     continue;
    }


  if(SRAV(str.ravno(),"Счёт расчётов по зарплате",0) == 0 )
    {
     polen(row_alx[0],&bros,2,'|');
     if(polen(bros.ravno(),&shrpzbt,2,',') == 0)
      {
       polen(bros.ravno(),&shrpz,1,',');
      }            
     else
       shrpz.new_plus(bros.ravno());

     continue;
    }

  if(SRAV(str.ravno(),"Коды начислений не входящие в расчёт удержания дневного заработка",0) == 0)
   {
    iceb_t_sozmas(&kodnah_nvr_dnev_zar,row_alx[0],&cur_alx);
    continue;
   }
  if(SRAV(str.ravno(),"Коды табеля не входящие в расчёт удержания дневного заработка",0) == 0)
   {
    iceb_t_sozmas(&kodtab_nvr_dnev_zar,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str.ravno(),"Коды не входящие в расчёт индексации",0) == 0)
   {
    iceb_t_sozmas(&kodnvrindex,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str.ravno(),"Коды начислений не входящие в расчёт командировочных",0) == 0)
   {
    iceb_t_sozmas(&knvr_komand,row_alx[0],&cur_alx);
    continue;
   }
  if(SRAV(str.ravno(),"Коды табеля не входящие в расчёт командировочных",0) == 0)
   {
    iceb_t_sozmas(&ktnvr_komand,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str.ravno(),"Профсоюзный фонд-коды начислений не входящие в расчёт",0) == 0)
    {
     iceb_t_sozmas(&knvrprof,row_alx[0],&cur_alx);
     continue;
    }

  if(SRAV(str.ravno(),"Максимальная сумма необлагаемой благотворительной помощи",0) == 0)
    {
     polen(row_alx[0],&snmpz,2,'|');
     continue;
    }
  if(SRAV(str.ravno(),"Коэффициент прожиточного минимума",0) == 0)
   {
    polen(row_alx[0],&kof_prog_min,2,'|');
    continue;
   }

  if(SRAV(str.ravno(),"Процент подоходного налога",0) == 0)
   {
    polen(row_alx[0],&ppn,2,'|');
    continue;
   }

  if(SRAV(str.ravno(),"Процент от минимальной заработной платы",0) == 0)
   {
    polen(row_alx[0],&pomzp,2,'|');
    continue;
   }

  if(SRAV(str.ravno(),"Код больничного",0) == 0)
   {
    iceb_t_sozmas(&kodbl,row_alx[0],&cur_alx);
    continue;
   }


  if(SRAV(str.ravno(),"Обязательные удержания",0) == 0)
   {
    iceb_t_sozmas(&obud,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str.ravno(),"Коды удержаний входящие в расчёт пенсионного отчисления с работника",0) == 0)
   {
    iceb_t_sozmas(&kuvvr_pens_sr,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str.ravno(),"Коды удержаний входящие в расчёт пенсионного отчисления с фонда зарплаты",0) == 0)
   {
    iceb_t_sozmas(&kuvvr_pens_sfz,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str.ravno(),"Обязательные начисления",0) == 0)
    {
     iceb_t_sozmas(&obnah,row_alx[0],&cur_alx);
     continue;
    }


  if(SRAV(str.ravno(),"Код подоходного налога",0) == 0)
    {
     polen(row_alx[0],&kodpn,2,'|');
     if(kodpn != 0)
       kodpn_all.z_plus(kodpn);
     continue;
    }

  if(SRAV(str.ravno(),"Код военного сбора",0) == 0)
    {
     polen(row_alx[0],&kodvn,2,'|');
     continue;
    }

  if(SRAV(str.ravno(),"Код военного сбора с больничного",0) == 0)
    {
     polen(row_alx[0],&kodvs_b,2,'|');
     continue;
    }

  if(SRAV(str.ravno(),"Коды не входящие в расчёт военного сбора",0) == 0)
    {
     polen(row_alx[0],&kodvn_nvr,2,'|');
     continue;
    }

  if(SRAV(str.ravno(),"Код подоходного налога с больничного",0) == 0)
    {
     polen(row_alx[0],&kodpn_sbol,2,'|');
     if(kodpn_sbol != 0)
       kodpn_all.z_plus(kodpn_sbol);
     continue;
    }


  if(SRAV(str.ravno(),"Код начисления для возврата суммы подоходного налога",0) == 0)
    {
     polen(row_alx[0],&kodvpn,2,'|');
     continue;
    }

  if(SRAV(str.ravno(),"Код перерасчёта подоходного налога",0) == 0)
    {
     polen(row_alx[0],&kodperpn,2,'|');
     if(kodperpn != 0)
      kodpn_all.z_plus(kodperpn);
    }

  if(SRAV(str.ravno(),"Код индексации",0) == 0)
   {
     polen(row_alx[0],&kodindex,2,'|');
     continue;
   }
  if(SRAV(str.ravno(),"Код индексации больничного",0) == 0)
   {
     polen(row_alx[0],&kodindexb,2,'|');
     continue;
   }


  if(SRAV(str.ravno(),"Коды благотворительной помощи",0) == 0)
   {
    iceb_t_sozmas(&kodmp,row_alx[0],&cur_alx);
    continue;
   }


  if(SRAV(str.ravno(),"Коды начислений не входящие в расчёт подоходного налога",0) == 0)
   {
    iceb_t_sozmas(&knvr,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str.ravno(),"Коды начислений неденежными формами",0) == 0)
   {
    iceb_t_sozmas(&knnf,row_alx[0],&cur_alx);
    continue;
   }
  if(SRAV(str.ravno(),"Коды начислений неденежными формами на которые начисляется НДС",0) == 0)
   {
    iceb_t_sozmas(&knnf_nds,row_alx[0],&cur_alx);
    continue;
   }
  if(SRAV(str.ravno(),"Коды начислений, на которые не распространяется социальная льгота",0) == 0)
   {
    iceb_t_sozmas(&kn_bsl,row_alx[0],&cur_alx);
    continue;
   }
  if(SRAV(str.ravno(),"Код доплаты до минимальной зарплаты",0) == 0)
   {
    polen(row_alx[0],&koddopmin,2,'|');
    continue;
   }

  if(SRAV(str.ravno(),"Код экономической классификации затрат для атестованных",0) == 0)
   {
    polen(row_alx[0],&kekv_at,2,'|');
    continue;
   }
  if(SRAV(str.ravno(),"Код экономической классификации затрат для неатестованных",0) == 0)
   {
    polen(row_alx[0],&kekv_nat,2,'|');
    continue;
   }
  if(SRAV(str.ravno(),"Код экономической классификации затрат для начислений на фонд оплаты",0) == 0)
   {
    polen(row_alx[0],&kekv_nfo,2,'|');
    continue;
   }
  if(SRAV(str.ravno(),"Код звания для неатестованных",0) == 0)
   {
    polen(row_alx[0],&kodzv_nat,2,'|');
    continue;
   }

  if(SRAV(str.ravno(),"Коды не входящие в расчёт доплаты до минимальной зарплаты",0) == 0)
   {
    iceb_t_sozmas(&kodnvmin,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str.ravno(),"Коды отдельного расчёта доплаты до минимальной зарплаты",0) == 0)
   {
    iceb_t_sozmas(&kodotrs,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str.ravno(),"Коды удержаний входящие в расчёт подоходного налога",0) == 0)
   {
    iceb_t_sozmas(&kuvr,row_alx[0],&cur_alx);
    continue;
   }

  if(SRAV(str.ravno(),"Округление",0) == 0)
   {
    polen(row_alx[0],&okrg,2,'|');
   }

 }

/*Чтение настроек для расчёта единого социального взноса*/
zarrnesv();


iceb_t_poldan("Многопорядковый план счетов",&bros,"nastrb.alx");
if(SRAV(bros.ravno(),"Вкл",1) == 0)
 vplsh=1;


startgodb=0;
if(iceb_t_poldan("Стартовый год",&bros,"nastrb.alx") == 0)
 {
  startgodb=bros.ravno_atoi();
 }

/*Ищем списки начислений для дополнительных зарплатных счетов*/

for(int kkk=0; kkk < dop_zar_sheta.kolih(); kkk++)
 {
  if(dop_zar_sheta.ravno(kkk)[0] == '\0')
   continue;
//  sprintf(str,"Начисления, для которых проводки выполняются по дополнительному счёту %s",dop_zar_sheta.ravno(kkk));
  str.new_plus("Начисления, для которых проводки выполняются по дополнительному счёту ");
  str.plus(dop_zar_sheta.ravno(kkk));
  iceb_t_poldan(str.ravno(),&bros,"zarnast.alx");
  dop_zar_sheta_spn.plus(bros.ravno());

/*  printw("dop_zar_sheta=%s-%s\n",dop_zar_sheta.ravno(kkk),dop_zar_sheta_spn.ravno(kkk));*/
 
 }


return(0);
}

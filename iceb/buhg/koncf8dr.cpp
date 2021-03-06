/*$Id: koncf8dr.c,v 5.3 2013/05/17 14:56:03 sasa Exp $*/
/*19.03.2015	02.08.2011	Белых А.И.	koncf8dr.c
Концовка распечаток  формы 1ДФ
****/
#include "buhg.h"


void koncf8dr(short kvrt,
short gn,
const char *koded,
short TYP,
double it1,double it1n,
double it2,double it2n,
double it11,double it11n,
double it21,double it21n,
int kolsh, //Количество физических лиц
short klls, //Количество листов в распечатке
int *npp1,
int metkar, //0-только распечатки 1-только для дискеты 2-распечатки и для дискеты
double vs_nd, /*военный сбор начисленный доход*/
double vs_vd, /*военный сбоз выплаченный доход*/
double vs_nn, /*военній сбор начисленный налог*/
double vs_vn, /*военный сбор выплаченный налог*/
FILE *ff1,FILE *ff2,FILE *ff4)
{
if(metkar == 0 || metkar == 2)
 {
  fprintf(ff1,"\x1B\x2D\x30");
  fprintf(ff2,"\x1B\x2D\x30");
  fprintf(ff1,"%-5s|%-10s|%10.2f|%10.2f|%10.2f|%10.2f|  %-4s|%-10s|%-10s|%-10s|%-6s|\n",
" X","    Х",it1n,it1,it2n,it2," Х","    X","    X","   X","  X");


  fprintf(ff2,"%5s %-10s %10.2f %10.2f %10.2f %10.2f\n",
" "," ",it11n,it11,it21n,it21);
  fprintf(ff2,"Кількість працівників - %d\n",kolsh);

  fprintf(ff1,"\
------------------------------------------------------------------------------------------------------------\n");

 }
class iceb_t_fioruk_rk rkruk;
iceb_t_fioruk(1,&rkruk);

*npp1+=1;

if(metkar == 1 || metkar == 2)
  fprintf(ff4, " %5d%1d%04d%-10.10s%d%10.10s%12s%-12.12s%12s%12s%2s%8s%8.8s%2s%s",
  99991,kvrt,gn,koded,TYP,rkruk.inn.ravno()," ",rkruk.telef.ravno()," "," "," "," "," "," "," ");



class iceb_t_fioruk_rk rkbuh;
iceb_t_fioruk(2,&rkbuh);


*npp1+=1;
if(metkar == 1 || metkar == 2)
 fprintf(ff4, " %5d%1d%04d%-10.10s%d%10.10s%12s%-12.12s%12s%12s%2s%8s%8.8s%2s%s",
 99992,kvrt,gn,koded,TYP,rkbuh.inn.ravno()," ",rkbuh.telef.ravno()," "," "," "," "," "," "," ");

if(metkar == 0 || metkar == 2)
 {
  fprintf(ff1,"Війсковий збір:\n\
Нараховано доходу - %10.2f\n\
Виплачено доходу  - %10.2f\n\
Нараховано податку- %10.2f\n\
Виплачено податку - %10.2f\n",
vs_nd,
vs_vd,
vs_nn,
vs_vn);

  zarsf1df(*npp1,kolsh,klls,TYP,ff1);
 }
*npp1+=1;

if(metkar == 1 || metkar == 2)
 fprintf(ff4, " %5d%1d%04d%-10.10s%d%10.10s%12.2f%12.2f%12.2f%12.2f%2s%8s%8.8s%2s%s",
 99999,kvrt,gn,koded,TYP," ",it1n,it1,it2n,it2," "," "," "," "," ");

if(metkar == 0 || metkar == 2)
 {


  podpis(ff1);
  podpis(ff2);

 }
}

/*$Id: iceb_u_pole.c,v 1.7 2013/05/17 14:58:42 sasa Exp $*/
/*11.11.2008    16.03.1989      Белых А.И.      iceb_u_pole.c
Подпрограмма извлечения из строки символов нужного номера поля
Если вернули "0" значит заказанный номер поля существует
если вернули "1" - нет
*/

#include                <stdio.h>
#include                <string.h>
#include "iceb_util.h"
/*********1-я***************************/

int iceb_u_pole(const char *D, //Строка из которой извлекают
iceb_u_str *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char bros[1024];
memset(bros,'\0',sizeof(bros));

int voz=iceb_u_pole(D,bros, N, R);

if(voz == 0)
 K->new_plus(bros);

return(voz);

}
/**********2-я**************************************/

int iceb_u_pole(const char *D, //Строка из которой извлекают
char *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
int             I,Z,V;
short           l,ll;

/*Проверяем наличие хоть одного символа*/
Z=0;
for(I=0;D[I] != '\0' && D[I] != '\r' ;I++)
 {
  if(D[I]==R)
   {
    Z++;
    break;
   } 
 }
if( Z == 0)
 {
  K[0]='\0';
  return(1);
 }

Z=1;

for(I=0;D[I] != '\0' && D[I] != '\r' ;I++)
 {
  if(D[I]==R)
   Z++;
  if(Z == N)
   {
    if(N>1)
     I++;
    V=0;
    for(; D[I]!=R ;I++)
     {
      if(D[I] == '\0' && N == 1)
       {
	K[0]='\0';
	return(1);
       }
      if(D[I] == '\0' || D[I] == '\r')
	break;
      K[V]=D[I];
      V++;
     }
    K[V]='\0';
    l=strlen(K);
    /*Убираем пробелы в конце строки*/
/*    printf("%d\n",l);*/
    for(ll=l-1; K[ll] == ' '|| K[ll] == '\t'|| K[ll] == '\n' ||
    K[ll] == '\r';ll--)
     {
      K[ll]='\0';
     }
/*    printf("R - %c поле - %s  %d %d \n",R,K,strlen(K),K[l]);*/
    return(0);
   }
  }
K[0]='\0';
return(1);
}

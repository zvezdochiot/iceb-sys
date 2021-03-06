/*$Id: prnbr.c,v 5.7 2013/08/13 05:50:32 sasa Exp $*/
/*13.01.2010    16.08.1993      Белых А.И.      prnbr.c
Подпрограмма преобразования числа в символьную строку
со вставкой точек
*/
#include	"util.h"

char *prnbr(double nb) //Число для преобразования
{
static char     str[64]; /*Символьная строка с числом*/
char            nb1[32],nb2[32]; /*настроено на этот размер*/
int           i,i1;
short           mt;

sprintf(nb1,"%.2f",nb);
i1=strlen(nb1);

memset(nb2,' ',sizeof(nb2));

nb2[sizeof(nb2)-1]='\0';

for(i=sizeof(nb2)-2; i1>=0; i--)
 {
  if(nb1[i1] == '-')
   {
    nb2[i]=nb1[i1];
    break;
   }
  if(i == sizeof(nb2)-9 || i == sizeof(nb2)-13 || i == sizeof(nb2)-17 || i == sizeof(nb2)-21  || i == sizeof(nb2)-25 || i == sizeof(nb2)-29)
   {
    nb2[i]='.';
    i--;
   }
  if(nb1[i1] == '.')
    nb2[i]='-';
  else
    nb2[i]=nb1[i1];
  i1--;
 }
memset(nb1,'\0',sizeof(nb1));
mt=i1=0;
for(i = 0 ; i < (int)sizeof(nb1) ; i++)
 {
  if(nb2[i] != ' ' && mt == 0)
    mt++;
  if(mt !=0)
   {
    nb1[i1] = nb2[i];
    i1++;
   }
 }
strcpy(str,nb1);
return(str);
}

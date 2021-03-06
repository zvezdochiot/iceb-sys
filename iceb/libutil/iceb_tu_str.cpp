/*$Id: iceb_tu_str.c,v 5.29 2013/11/24 08:23:08 sasa Exp $*/
/*16.01.2017	03.12.2003	Белых А.И.	iceb_tu_str.c
Подпрограмма динамического создания и увеличения размера строки 
*/
#include	<stdlib.h>
#include <glib.h>
#include        "util.h"

/****************************/
/*Конструктор объекта       */
/****************************/
iceb_tu_str::iceb_tu_str()
{
//printf("Конструктор\n");
stroka=NULL;
stroka_filtr=NULL;  //Всегда определяется перед запросом копированием с фильтрацией из "stroka"
dlinna=0;
}
/****************************/
/*Конструктор объекта с записью строки */
/****************************/
iceb_tu_str::iceb_tu_str(const char *vvod)
{
//printf("Конструктор\n");
stroka=NULL;
stroka_filtr=NULL;  //Всегда определяется перед запросом копированием с фильтрацией из "stroka"
dlinna=0;
plus(vvod);
}
/****************************/
/*Конструктор объекта с числа */
/****************************/
iceb_tu_str::iceb_tu_str(int vvod)
{
//printf("Конструктор\n");
stroka=NULL;
stroka_filtr=NULL;  //Всегда определяется перед запросом копированием с фильтрацией из "stroka"
dlinna=0;
plus(vvod);
}

/****************************/
/*Деструктор класса*/
/****************************/
iceb_tu_str::~iceb_tu_str()
{
 if(stroka != NULL)
  {
   free(stroka);
   stroka=NULL;
  }
 if(stroka_filtr != NULL)
  {
   free(stroka_filtr);
   stroka_filtr=NULL;
  }
 stroka=stroka_filtr=NULL;
 dlinna=0;
}

/********************************/
/*Очистка от введеной информации*/
/********************************/
void iceb_tu_str::free_class(void)
{
 if(stroka != NULL)
  {
   free(stroka);
   stroka=NULL; 
  }
 if(stroka_filtr != NULL)
  {
   free(stroka_filtr);
   stroka_filtr=NULL;
  }
 stroka=stroka_filtr=NULL;
 dlinna=0;
}
/*****************************************/
/*вставить отднобайтный символ в строку*/
/****************************************/
int iceb_tu_str::ins(char simv,int poz)
{
if(poz >= dlinna)
 return(1);

stroka[poz]=simv;

return(0);
}
/***********************************/
/*проверить правильность ввода даты*/
/************************************/
//если вернули 0-всё правильно 1-нет
int iceb_tu_str::prov_dat()
{
if(dlinna <= 1)
 return(0);
short d,m,g;
return(rsdat(&d,&m,&g,stroka,1));
}
/***********************************/
/*проверить правильность ввода даты*/
/************************************/
//если вернули 0-всё правильно 1-нет
int iceb_tu_str::prov_dat1()
{
if(dlinna <= 1)
 return(0);
short m,g;
return(rsdat1(&m,&g,stroka));
}


/*************************************************************************/
/*Записать дату преобразовав её из SQL формата в нормальное представление*/
/*************************************************************************/
void iceb_tu_str::plus_sqldata(const char *row)
{
short d,m,g;
char bros[64];
memset(bros,'\0',sizeof(bros));

rsdat(&d,&m,&g,row,2);

if(d != 0)
  sprintf(bros,"%02d.%02d.%04d",d,m,g);

plus(bros);

}
/*************************************************************************/
/*Записать только указанное количество символов*/
/*************************************************************************/
int iceb_tu_str::plus(const char *str,int kol_simv)
{
int dlina=strlen(str)+1;
if(dlina < kol_simv)
 dlina=kol_simv+1;
 
char *bros=new char[dlina];
sprintf(bros,"%.*s",iceb_tu_kolbait(kol_simv,str),str);
int voz=plus(bros);
delete [] bros;
return(voz);

}
/*************************************************************************/
/*Записать дату в символьном виде*/
/*************************************************************************/
void iceb_tu_str::plus_data(short d,short m,short g)
{
char bros[32];
sprintf(bros,"%02d.%02d.%d",d,m,g);
plus(bros);

}
/*************************************************************************/
/*Записать дату в символьном виде удалив предыдущюю строку*/
/*************************************************************************/
void iceb_tu_str::new_plus_data(short d,short m,short g)
{
char bros[32];
sprintf(bros,"%02d.%02d.%d",d,m,g);
new_plus(bros);

}
/*************************************************************************/
/*Записать дату преобразовав её из SQL формата в нормальное представление*/
/*************************************************************************/
void iceb_tu_str::new_plus_sqldata(const char *row)
 {
  free_class();
  plus_sqldata(row);
 }
/*****************************/
/*Eсли первый символ "+" вернуть 1, если "-" вернуть 2, иначе возвращаем 0*/
/****************************************/

int iceb_tu_str::ravno_pr()
{
if(stroka[0] == '+')
 return(1);
if(stroka[0] == '-')
 return(2);
return(0); 
}
 


/********************************/
/*Добавить с увеличением размера*/
/********************************/

int iceb_tu_str::plus(const char *strokaplus)
{
short metkan=0;

if(strokaplus == NULL)
 return(0);
 
if(dlinna != 0)
  metkan=1;

if(stroka == NULL)
 dlinna+=strlen(strokaplus)+1;
else
 dlinna+=strlen(strokaplus);

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-%s stroka !!!\n",__FUNCTION__,gettext("Не могу добавить память для"));
  return(-1);
 }  

if(metkan == 0)
  strcpy(stroka,strokaplus);
else
  strcat(stroka,strokaplus);

return(0);

}
/***************************/
int iceb_tu_str::plus(const char *strokaplus,const char *strokaplus1)
{
 plus(strokaplus);
 return(plus(strokaplus1));
}
/********************************/
/*Добавить с увеличением размера*/
/********************************/

int iceb_tu_str::plus(double suma)
{
char strokaplus[112];
sprintf(strokaplus,"%.10g",suma);

return(plus(strokaplus));
}
/********************************/
/*Удалить предыдущее Добавить с увеличением размера*/
/********************************/

int iceb_tu_str::new_plus(double suma)
{
char strokaplus[112];
sprintf(strokaplus,"%.10g",suma);

return(new_plus(strokaplus));

}
/********************************/
/*Добавить с увеличением размера c указанным количеством знаков после запятой*/
/********************************/

int iceb_tu_str::plus(double suma,int kolznakpz)
{
char strokaplus[112];
sprintf(strokaplus,"%.*f",kolznakpz,suma);

return(plus(strokaplus));
}
/********************************/
/*Удалить предыдущее Добавить с увеличением размера с указанным количеством знаков после запятой*/
/********************************/

int iceb_tu_str::new_plus(double suma,int kolznakpz)
{
char strokaplus[112];
sprintf(strokaplus,"%.*f",kolznakpz,suma);

return(new_plus(strokaplus));

}
/********************************/
/*Добавить с увеличением размера*/
/********************************/

int iceb_tu_str::plus(int suma)
{
char strokaplus[112];
sprintf(strokaplus,"%d",suma);


return(plus(strokaplus));

}
/********************************/
/*Удалить предыдущее Добавить с увеличением размера*/
/********************************/

int iceb_tu_str::new_plus(int suma)
{
char strokaplus[112];
sprintf(strokaplus,"%d",suma);

return(new_plus(strokaplus));

}
/********************************/
/*Добавить с увеличением размера и пероводом строки в конце*/
/********************************/

int iceb_tu_str::plus_ps(const char *strokaplus)
{
short metkan=0;

if(strokaplus == NULL)
 return(0);

if(dlinna != 0)
  metkan=1;

if(stroka == NULL)
 dlinna+=strlen(strokaplus)+2;
else
 dlinna+=strlen(strokaplus)+1;

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-%s stroka !!!\n",__FUNCTION__,gettext("Не могу добавить память для"));
  return(-1);
 }  

if(metkan == 0)
 {
  strcpy(stroka,strokaplus);
  strcat(stroka,"\n");
 }
else
 {
  strcat(stroka,strokaplus);
  strcat(stroka,"\n");
 }

return(0);

}
/********************************/
/*Добавить с увеличением размера и пероводом строки в начале*/
/********************************/

int iceb_tu_str::ps_plus(int strokaplus)
{
char bros[112];
sprintf(bros,"%d",strokaplus);
return(ps_plus(bros));
}
/********************************/
/*Добавить с увеличением размера и пероводом строки в начале*/
/********************************/

int iceb_tu_str::ps_plus(const char *strokaplus)
{
short metkan=0;

if(strokaplus == NULL)
 return(0);

if(dlinna != 0)
  metkan=1;

if(stroka == NULL)
 dlinna+=strlen(strokaplus)+2;
else
 dlinna+=strlen(strokaplus)+1;

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-%s stroka !!!\n",__FUNCTION__,gettext("Не могу добавить память для"));
  return(-1);
 }  

if(metkan == 0)
 {
  strcpy(stroka,"\n");
  strcat(stroka,strokaplus);
 }
else
 {
  strcat(stroka,"\n");
  strcat(stroka,strokaplus);
 }

return(0);

}
/********************************/
/*Записать с предварительной вставкой перевода строки все записи кроме первой*/
/********************************/

int iceb_tu_str::plus_v_spisok(const char *strokaplus)
{
if(dlinna <= 1)
 return(new_plus(strokaplus));
else 
 {
  plus("\n");
  return(plus(strokaplus));
 }
return(1);
}
/**************************************************/
/*Записать с освобождением ранее выделенной памяти*/
/**************************************************/

int iceb_tu_str::new_plus(const char *strokaplus)
{
//printf("iceb_tu_str::new_plus %s\n",strokaplus);

if(strokaplus == NULL)
 return(0);

if(stroka != NULL)
 {
  free(stroka);
  stroka=NULL;
 }


dlinna=strlen(strokaplus)+1;

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-%s stroka !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return(-1);
 }  

strcpy(stroka,strokaplus);




return(0);

}
/**************************************************/
/*Записать с освобождением ранее выделенной памяти указанного количества символов*/
/**************************************************/

int iceb_tu_str::new_plus(const char *strokaplus,int kolsimv)
{
//printf("iceb_tu_str::new_plus %s\n",strokaplus);

if(strokaplus == NULL)
 return(0);

if(stroka != NULL)
 {
  free(stroka);
  stroka=NULL;
 }

gchar *bros=g_strdup_printf("%.*s",iceb_tu_kolbait(kolsimv,strokaplus),strokaplus);

dlinna=strlen(bros)+1;

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-%s stroka !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return(-1);
 }  

strcpy(stroka,bros);
g_free(bros);



return(0);

}
/**************************************************/
/*Записать с освобождением ранее выделенной памяти и добавлением перевода строки в конце*/
/**************************************************/

int iceb_tu_str::new_plus_ps(const char *strokaplus)
{
//printf("iceb_tu_str::new_plus %s\n",strokaplus);

if(strokaplus == NULL)
 return(0);

if(stroka != NULL)
 {
  free(stroka);
  stroka=NULL;
 }


dlinna=strlen(strokaplus)+2;

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-%s stroka !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return(-1);
 }  

strcpy(stroka,strokaplus);
strcat(stroka,"\n");

return(0);

}
/*************************/
/*Вернуть адрес строки   */
/*************************/

const char *iceb_tu_str::ravno()
{
return(stroka);
}
/*************************************/
/*Отфильктровать строку для xml файла*/
/**************************************/
const char *iceb_tu_str::ravno_filtr_xml()
{
if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }
int dlinna=strlen(iceb_tu_filtr_xml(stroka))+1;

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return(NULL);
 }  

strcpy(stroka_filtr,iceb_tu_filtr_xml(stroka));

return(stroka_filtr);

}


/***************************************************/
/*Получить строку преобразованную в маленькие буквы*/
/*****************************************************/
const char *iceb_tu_str::ravno_tolower()
{
if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(stroka)+1))) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return(NULL);
 }  

strcpy(stroka_filtr,stroka);
iceb_tu_tolower_str(stroka_filtr);
return(stroka_filtr);

}
/***************************************************/
/*Получить строку преобразованную в большие буквы*/
/*****************************************************/
const char *iceb_tu_str::ravno_toupper()
{
if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(stroka)+1))) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return(NULL);
 }  

strcpy(stroka_filtr,stroka);
iceb_tu_toupper_str(stroka_filtr);
return(stroka_filtr);

}
/*************************/
/*Вернуть адрес отфильтрованной строки   */
/*************************/

const char *iceb_tu_str::ravno_filtr()
{

char prom_str[dlinna*2+1];

int i1=0;
for(int i=0; i<dlinna ; i++)
 {
  switch (stroka[i])
   {
    case '\'':
     prom_str[i1]='\'';
     i1++;
     prom_str[i1]=stroka[i];
     break;

    case '\\':
     prom_str[i1]='\\';
     i1++;
     prom_str[i1]=stroka[i];
     break;

    default:
     prom_str[i1]=stroka[i];
     break;    
   }
  
  i1++;
 }

prom_str[i1]='\0';


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }



if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(prom_str)+1))) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return(NULL);
 }  

strcpy(stroka_filtr,prom_str);



return(stroka_filtr);

}

/*************************/
/*Вернуть адрес отфильтрованной строки до первого пробела  */
/*************************/

const char *iceb_tu_str::ravno_filtr_dpp()
{

char prom_str[dlinna*2+1];
char prom_strv[dlinna*2+1];
//memset(prom_strv,'\0',sizeof(prom_strv));
int i1=0;
for(int i=0; i<dlinna ; i++)
 {
  switch (stroka[i])
   {
    case '\'':
     prom_str[i1]='\'';
     i1++;
     prom_str[i1]=stroka[i];
     break;

    default:
     prom_str[i1]=stroka[i];
     break;    
   }
  
  i1++;
 }

prom_str[i1]='\0';
if(POLE(prom_str,prom_strv,1,' ') != 0)
 strcpy(prom_strv,prom_str);

if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }



if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(prom_strv)+1))) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return(NULL);
 }  

strcpy(stroka_filtr,prom_strv);



return(stroka_filtr);

}
/*************************/
/*Вернуть строки до первого пробела  */
/*************************/

const char *iceb_tu_str::ravno_dpp()
{

char *prom_str=new char[strlen(stroka)+1];

prom_str[0]='\0';
if(POLE(stroka,prom_str,1,' ') != 0)
 strcpy(prom_str,stroka);

if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }



if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(prom_str)+1))) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу добавить память для"));
  delete [] prom_str;
  return(NULL);

 }  

strcpy(stroka_filtr,prom_str);

delete [] prom_str;

return(stroka_filtr);

}


/****************************************/
/*Получить количество символов в массиве*/
/****************************************/
int iceb_tu_str::getdlinna()
{
return(dlinna);
}
/**********************************/
/*Получить в виде double*/
/***************************/
double iceb_tu_str::ravno_atof()
{
return(ATOFM(stroka));
}
/**********************************/
/*Получить в виде int*/
/***************************/
int iceb_tu_str::ravno_atoi()
{
return(atoi(stroka));
}

/**********************************/
/*Получить год если введена дата*/
/***************************/
short iceb_tu_str::ravno_god()
{
short d,m,g;
rsdat(&d,&m,&g,stroka,1);
return(g);
}
/**********************************/
/*Получить дату в формате мм.гг*/
/***************************/
const char *iceb_tu_str::ravno_mes_god()
{
short d,m,g;
rsdat(&d,&m,&g,stroka,1);
static char bros[64];
sprintf(bros,"%02d.%04d",m,g);
return(bros);
}
/**********************************/
/*Получить в виде long int*/
/***************************/
long int iceb_tu_str::ravno_atol()
{
return(atol(stroka));
}

/*************************/
/*Вернуть дату преобразовав дд.мм.гггг -> гггг.мм.дд */
/*************************/

const char *iceb_tu_str::ravno_sqldata()
{


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }
int razmer=sizeof(char)*16;
if((stroka_filtr=(char *)realloc(stroka_filtr,razmer)) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу добавить память для"));
  return(NULL);
 }  

strncpy(stroka_filtr,iceb_tu_tosqldat(stroka),razmer);
return(stroka_filtr);

}
/*************************/
/*Вернуть дату преобразовав гггг-мм-дд -> дд.мм.гггг*/
/*************************/

const char *iceb_tu_str::ravno_fsqldata()
{


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }
int razmer=sizeof(char)*16;
if((stroka_filtr=(char *)realloc(stroka_filtr,razmer)) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу добавить память для"));
  return(NULL);
 }  

strncpy(stroka_filtr,iceb_tu_datzap(stroka),razmer);
return(stroka_filtr);

}
/*************************/
/*Вернуть дату преобразова дд.мм.ггггг -> ддммгггг */
/*************************/

const char *iceb_tu_str::ravno_xmldata()
{


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*15)) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return(NULL);
 }  
short d,m,g;

rsdat(&d,&m,&g,stroka,1);
sprintf(stroka_filtr,"%02d%02d%04d",d,m,g);

return(stroka_filtr);

}
/*************************/
/*Квотирует строку так чтобы оболочка (/bin/sh) интерпретировала её как unquoted_string.*/
/*************************/

const char *iceb_tu_str::ravno_shell_quote()
{


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(dlinna+4))) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return(NULL);
 }  
sprintf(stroka_filtr,"'%s'",stroka);
return(stroka_filtr);

}

/*************************/
/*Вернуть врумя в формате час:минута:секунда*/
/*************************/

const char *iceb_tu_str::ravno_time()
{


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*15)) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу добавить память для"));
  return(NULL);
 }  
short hh,mm,ss;

rstime(&hh,&mm,&ss,stroka);
sprintf(stroka_filtr,"%02d:%02d:%02d",hh,mm,ss);

return(stroka_filtr);

}
/*************************/
/*Вернуть врумя в формате час:минута:секунда  Если строка пустая то выдать 24:00:00*/
/*************************/

const char *iceb_tu_str::ravno_time_end()
{


if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*15)) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу добавить память для"));
  return(NULL);
 }  
short hh,mm,ss;

rstime(&hh,&mm,&ss,stroka);
if(stroka[0] == '\0')
  sprintf(stroka_filtr,"23:59:59");
else
  sprintf(stroka_filtr,"%02d:%02d:%02d",hh,mm,ss);

return(stroka_filtr);

}
/********************************/
/*Добавить с увеличением размера и вставкой запятой если запись не первая*/
/********************************/

int iceb_tu_str::z_plus(const char *strokaplus)
{
short metkan=0;

if(strokaplus == NULL)
 return(0);

if(dlinna > 1)
  metkan=1;

dlinna+=strlen(strokaplus)+1; /*прибавляем 1 для нулевого байта если строки нет и для запятой если строка есть*/

if((stroka=(char *)realloc(stroka,sizeof(char)*dlinna)) == NULL)
 {
  printf("\n%s-%s stroka !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return(-1);
 }  

if(metkan == 0)
 {
  strcpy(stroka,strokaplus);
 }
else
 {
  strcat(stroka,",");
  strcat(stroka,strokaplus);
 }

return(0);

}
/********************************/
/*Добавить с увеличением размера и вставкой запятой если запись не первая*/
/********************************/

int iceb_tu_str::z_plus(int strokaplus)
{
char str[124];
sprintf(str,"%d",strokaplus);
return(z_plus(str));
}
/******************************/
/*Записать текущюю дату*/
/************************/
void iceb_tu_str::plus_tek_dat()
{

short d,m,g;
poltekdat(&d,&m,&g);
char strok[1024];
sprintf(strok,"%02d.%02d.%d",d,m,g);

plus(strok);

}
/******************************/
/*Записать текущюю дату удалив предыдущюю информацию если она была введена*/
/************************/
void iceb_tu_str::new_plus_tek_dat()
{

short d,m,g;
poltekdat(&d,&m,&g);
char strok[1024];
sprintf(strok,"%d.%d.%d",d,m,g);

new_plus(strok);

}
/**************************/
/*Вернуть номер в списке разделённых заданным символом*/
/********************************/
int find(char *obr,char razd)
{
int kolpol=pole2(obr,razd);

class iceb_tu_str pol;

for(int ii=0; ii < kolpol; ii++)
 {
  polen(obr,&pol,ii+1,razd);
  if(SRAV(obr,pol.ravno(),0) == 0)
   return(ii);

 }

return(-1);
}
/**************************/
/*Вернуть номер в списке разделённых заданным символом*/
/********************************/
int find(int obr,char razd)
{
char obrchar[56];
sprintf(obrchar,"%d",obr);
return(find(obrchar,razd));
}

/**************************/
/*Получить строку без указанных символов*/
/********************************/
const char *iceb_tu_str::ravno_udsimv(const char *strsimv)
{
if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }



if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(stroka)+1))) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return(NULL);
 }  

strcpy(stroka_filtr,stroka);
ud_simv(stroka_filtr,strsimv);
return(stroka_filtr);


}
/**************************/
/*Удалить все указанные символы в строке - символы удаляются даже если они идут не подряд*/
/********************************/
void iceb_tu_str::udsimv(const char *strsimv)
{

if(strlen(stroka) == 0)
 return;

if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }



if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(stroka)+1))) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return;
 }  

strcpy(stroka_filtr,stroka);
ud_simv(stroka_filtr,strsimv);


free(stroka);
stroka=NULL;

if((stroka=(char *)realloc(stroka,sizeof(char)*(strlen(stroka_filtr)+1))) == NULL)
 {
  printf("\n%s-%s stroka !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return;
 }  

strcpy(stroka,stroka_filtr);

}

/**************************/
/*Удалить лидирующие пробелы*/
/********************************/
void iceb_tu_str::ud_lp()
{
if(stroka[0] != ' ')
 return;
int razmer=strlen(stroka)+1;
char *str1=new char[razmer];
memset(str1,'\0',razmer);
int sh=0;
for(int i=0; i < razmer;i++)
 {
  if(sh == 0)  
   if(stroka[i] == ' ')
    continue;
  str1[sh++]=stroka[i];
 }

strcpy(stroka,str1);
delete [] str1;
}
/***********************************/
/*Вернуть число заменив точку на запятую*/
/**********************************/
const char *iceb_tu_str::ravno_dtczp()
{
if(stroka == NULL)
 return("NULL");

if(strlen(stroka) == 0)
 return("0");

if(stroka_filtr != NULL)
 {
  free(stroka_filtr);
  stroka_filtr=NULL;
 }

if((stroka_filtr=(char *)realloc(stroka_filtr,sizeof(char)*(strlen(stroka)+1))) == NULL)
 {
  printf("\n%s-%s stroka_filtr !!!\n",__FUNCTION__,gettext("Не могу выделить память для"));
  return("");
 }  

strcpy(stroka_filtr,stroka);

for(unsigned int nom=0; nom < strlen(stroka_filtr); nom++)
 if(stroka_filtr[nom] == '.')
  {
   stroka_filtr[nom]=',';
   break;
  }

return(stroka_filtr);

}

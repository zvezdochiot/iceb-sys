/*$Id: startfil.c,v 5.4 2013/05/17 14:56:42 sasa Exp $*/
/*05.07.2002	05.07.2002	Белых А.И.	startfil.c
Выдача в файл начальной команды для определения формата файла
*/
#include        <stdio.h>


void   startfil(FILE *ff)
{

fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/

}
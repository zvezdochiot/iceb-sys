/*$Id: zar_pr_ins.c,v 5.1 2011-12-02 09:48:19 sasa Exp $*/
/*26.11.2011	26.11.2011	Белых А.И.	zar_ins_pr.c
Вставка записи в таблицу протокола ход расчёта
*/
#include <glib.h>
#include "buhg.h"


void zar_pr_ins(const char *sodzap)
{
extern int zar_pr_nomzap;
extern short zar_pr_mes;
extern short zar_pr_god;
extern int zar_pr_tabn;
extern int zar_pr_tipz;
extern FILE *zar_pr_ffp;

if(zar_pr_tipz < 0 && zar_pr_ffp == NULL)
 return;

gchar *strsql=g_strdup_printf("insert into Zarpr values(%d,%d,'%04d-%02d-01',%d,'%s')",zar_pr_tabn,zar_pr_tipz,zar_pr_god,zar_pr_mes,++zar_pr_nomzap,sql_escape_string(sodzap,0));

if(zar_pr_tipz > 0)
 sql_zapis(strsql,1,0);

if(zar_pr_ffp != NULL)
 fprintf(zar_pr_ffp,"%s",strsql);

g_free(strsql);

}

/*$Id:$*/
/*26.04.2018	21.04.2018	Белых А.И.	iceb_t_rnl.h
структура с данными для печати шапки
*/
class iceb_rnl_c
 {
  public:
   short dn,mn,gn; /*дата начала*/
   short dk,mk,gk; /*дата конца*/
   class iceb_u_str obr; /*образец начала стрoки которую надо пропустить*/

   /*эти данные задаются программой*/
   int simv_plus; /*количество символов на которые удлинняется строка*/
   int nom_list; /*номер листа*/
   int orient;   /*ориентация- 0-портрет 1- ландшафт; если больше 1 то это количество строк на листе*/
   
   iceb_rnl_c()
    {
     dn=mn=gn=dk=mk=gk=0;
     simv_plus=0;     
     orient=0;
     obr.plus("");
     nom_list=1;/*всега начинаестя с 1 так как первая шапка обязательно уже напечатана*/
    }
 };


/*$Id:$*/
/*03.10.2015	03.10.2015	Белых А.И.	zarrsov.h
Структура хранения данных по отработанному времини за период
*/


class iceb_zarrsov /*свод по отработанному времени*/
 {
  public:
   class masiv_din_int kod; /*коды табеля*/
   class SPISOK kod_mes_god; /*список код|месяц-год*/
   class masiv_din_int kolrd;   /*количество отработанных рабочих дней*/
   class masiv_din_int kolkd;   /*количество отработанных календарных дней*/
   class masiv_din_double kolhs;   /*количество отработанных часов*/

 };

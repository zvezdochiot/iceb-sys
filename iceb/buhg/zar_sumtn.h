/*$Id: zar_sumtn.h,v 5.3 2014/02/28 05:13:48 sasa Exp $*/
/*02.09.2016	03.09.2012	Белых А.И.	zar_sumtn.h
 Cуммы по табельному номеру
*/
class zar_sumtn
 {
  public:
    double suma_nah_all_podoh;/*Общая сумма всех начислений c которой берётся подоходный налог*/
    double suma_slg; /*Cумма на которую распространяестя соц. льгота*/
    double suma_blg; /*Cумма на которую не распространяестя соц. льгота*/
    double suma_ndfv_slg; /*Сумма не денежных форм выплаты на которую распространяется соц. льгота*/
    double suma_ndfv_blg; /*сумма не денежных форм выплаты без соц. льготы*/
    
//    double suma_esv_all; /*Общая сумма единого социального взноса*/
//    double suma_esv_s_zarslg; /*Сумма единого социального взноса с суммы зарплаты для которой применяется социальная льгота*/        
    double suma_lgot; /*Сумма льготы*/
    
    double suma_boln_bp; /*Cумма больничных в счёт месяца расчёта начисленная в будущих периодах*/
    
  zar_sumtn()
   {
    clear();
   }

  void clear()
   {
    suma_slg=0.; /*Cумма на которую распространяестя соц. льгота*/
    suma_blg=0.; /*Cумма на которую не распространяестя соц. льгота*/
    suma_ndfv_slg=0.; /*Сумма не денежных форм выплаты на которую распространяется соц. льгота*/
    suma_ndfv_blg=0.; /*сумма не денежных форм выплаты без соц. льготы*/    
//    suma_esv_all=0.;
    suma_nah_all_podoh=0.;    
//    suma_esv_s_zarslg=0.;
    suma_lgot=0.;
    suma_boln_bp=0.;
   }
 };

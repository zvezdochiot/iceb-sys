/*$Id: iceb_u_double.h,v 1.7 2011-01-13 13:50:05 sasa Exp $*/
/*16.08.2016	15.12.2004	Белых А.И.	iceb_u_double.h
Класс для работы с double-массивами переменной длинны
*/
class iceb_u_double
 {
private:
  int    optimiz;  //0-записывать в масив без оптимизации 1-с оптимизацией
  int	 kolz;    //Количество элементов массива
  double *masiv_double;  //Массив 
  
public:

  iceb_u_double(int); //Конструктор
  iceb_u_double(); //Конструктор
  ~iceb_u_double();      //Деструктор

  int find(double); //Найти в какой ячейке искомое значение

  int plus(double,int); //Записать новый элемент и если нужно увеличить размер массива
  int plus(double); //Записать новый элемент в добавленную ячейку массива
  int new_plus(double,int); //Перезаписать элемент и если нужно увеличить размер массива
  int plus_str(int);  /*Увеличить размер массива на заданное количество елементов*/

  int make_class(int);     //создать массив нужного размера
  void print_masiv(); //вывести содержимое массива для контроля
  double ravno(int elem); //вернуть значение нужного елемента
  void clear_class(); //Обнулить массив
  void free_class(); /*Освободить выделенную память*/
  double suma(); //Вернуть сумму элементов массива
  int kolih(); //Вернуть количество элементов в массиве

  int prov_kol(int kol_str,int kol_kol,int nom_kol); /*проверить есть ли не нулевые значения в колоке двумерного массива*/
  double suma_kol(int kol_str,int kol_kol,int nom_kol); /*получить сумму по колонке двухмерного массива*/
 };

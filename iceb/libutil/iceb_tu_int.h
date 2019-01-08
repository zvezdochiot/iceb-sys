/*$Id: iceb_tu_int.h,v 5.3 2013/05/17 14:56:41 sasa Exp $*/
/*21.12.2004	15.12.2004	Белых А.И.	iceb_tu_int.h
Класс для работы с int-массивами переменной длинны
*/
class iceb_tu_int
 {
private:
  int    optimiz;  //0-записывать в масив без оптимизации 1-с оптимизацией
  int	 kolz;    //Количество элементов массива
  int *masiv_int;  //Массив 
  
public:

  iceb_tu_int(int); //Конструктор
  iceb_tu_int(); //Конструктор
  ~iceb_tu_int();      //Деструктор

  int find(int); //Найти в какой ячейке искомое значение

  int plus(int,int);      //Записать новый элемент и если нужно увеличить размер массива
  int plus(int);      //Записать новый элемент c увеличением размера массива

  int make_masiv(int);     //создать массив нужного размера
  void print_masiv(); //вывести содержимое массива для контроля
  int ravno(int elem); //вернуть значение нужного елемента
  int ravno_max(); //вернуть максимальное значение елемента
  int kolih(); //вернуть количество элементов массива
  void masiv_delete();
 };
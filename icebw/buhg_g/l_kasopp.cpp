/*$Id: l_kasopp.c,v 1.31 2013/12/31 11:49:13 sasa Exp $*/
/*11.07.2015	18.01.2006	Белых А.И.	l_kasopp.c
Ввод и корректировка списка операций прихода для подсистемы "Учёт кассовых ордеров"
*/
#include <errno.h>
#include "buhg_g.h"
#include "l_kasop.h"
enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KOD,
 COL_NAIM,
 COL_SHET,
 COL_SHETK,
 COL_PROV,
 COL_KOD_CN,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  kasopp_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  class iceb_u_str name_window;
  
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  

  class kasop_rek poi;
  iceb_u_str zapros;

  iceb_u_str kodtv;//Код толькочто введённой записи
  iceb_u_str kodv; //Выбранный код 
  iceb_u_str naimv; //Наименование выбранной записи
  int  metka_rr;
  //Конструктор
  kasopp_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
   }      
 };

gboolean   kasopp_key_press(GtkWidget *widget,GdkEventKey *event,class kasopp_data *data);
void kasopp_vibor(GtkTreeSelection *selection,class kasopp_data *data);
void kasopp_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class kasopp_data *data);
void  kasopp_knopka(GtkWidget *widget,class kasopp_data *data);
void kasopp_add_columns (GtkTreeView *treeview);
void kasopp_create_list(class kasopp_data *data);
int  kasopp_prov_row(SQL_str row,class kasopp_data *data);

int l_kasopp_v(iceb_u_str *kodzap,GtkWidget *wpredok);
int l_kasopp_p(class kasop_rek *datap,GtkWidget *wpredok);
int  kasopp_pvu(int met,const char *kod,GtkWidget *wpredok);
void kasopp_udzap(class kasopp_data *data);
void kasopp_unk(GtkWidget *wpredok);
void kasopp_rasp(class kasopp_data *data);

extern SQL_baza	bd;
 
int l_kasopp(int metka, //0-воод и корректировка 1-выбор
iceb_u_str *kod,iceb_u_str *naimk,GtkWidget *wpredok)
{
int gor=0;
int ver=0;

class kasopp_data data;
char bros[512];
data.metka_rr=metka;
data.name_window.plus(__FUNCTION__);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_rr == 1)
 {
  if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
 }


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список приходных операций"));

gtk_window_set_title (GTK_WINDOW (data.window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(kasopp_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Список приходных операций"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки


sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(kasopp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(kasopp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(kasopp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить выбранную запись"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(kasopp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удалить неиспользуемые коды операций"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(kasopp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(kasopp_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(kasopp_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

kasopp_create_list(&data);
gtk_widget_show(data.window);

if(metka == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
if(data.metka_voz == 0)
 if(metka == 1)
  {
   kod->new_plus(data.kodv.ravno());
   naimk->new_plus(data.naimv.ravno());
  }
return(data.metka_voz);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  kasopp_knopka(GtkWidget *widget,class kasopp_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("kasopp_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    data->kodtv.new_plus("");
    l_kasopp_v(&data->kodtv,data->window);
    kasopp_create_list(data);
    return;  

  case SFK2:

    if(l_kasopp_v(&data->kodv,data->window) == 0)
     data->kodtv.new_plus(data->kodv.ravno());
    kasopp_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
//    printf("data->snanomer=%d\n",data->snanomer);
    kasopp_udzap(data);
    kasopp_create_list(data);
    return;  

  case SFK3:
    kasopp_unk(data->window);
    kasopp_create_list(data);
    return;  

  case FK4:
    l_kasopp_p(&data->poi,data->window);
    kasopp_create_list(data);
    return;  

  case FK5:
   kasopp_rasp(data);
    return;  

    
  case FK10:
    if(data->metka_rr == 1)
      iceb_sizww(data->name_window.ravno(),data->window);
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kasopp_key_press(GtkWidget *widget,GdkEventKey *event,class kasopp_data *data)
{

switch(event->keyval)
 {
  case GDK_KEY_F2:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK3],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("kasopp_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void kasopp_create_list (class kasopp_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;
iceb_u_str zagolov;

//printf("kasopp_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(kasopp_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(kasopp_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

sprintf(strsql,"select * from Kasop1 order by kod asc");

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;


while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
   if(kasopp_prov_row(row,data) != 0)
    continue;

  if(iceb_u_SRAV(data->kodtv.ravno(),row[0],0) == 0)
   data->snanomer=data->kolzap;

  //Читаем наименование кода целевого назначения
  sprintf(strsql,"select naik from Kascn where kod=%s",row[7]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    ss[COL_KOD_CN].new_plus(row[7]);
    ss[COL_KOD_CN].plus(" ");
    ss[COL_KOD_CN].plus(row1[0]);
   }
  else   
    ss[COL_KOD_CN].new_plus("");

  //Проводки
  if(row[4][0] == '0')
    ss[COL_PROV].new_plus(gettext("Не делать"));
  if(row[4][0] == '1')
    ss[COL_PROV].new_plus(gettext("Делать"));
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,row[0],
  COL_NAIM,row[1],
  COL_SHET,row[2],
  COL_SHETK,row[3],
  COL_KOD_CN,ss[COL_KOD_CN].ravno(),
  COL_PROV,ss[COL_PROV].ravno(),
  COL_DATA_VREM,iceb_u_vremzap(row[6]),
  COL_KTO,iceb_kszap(row[5],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->kodtv.new_plus("");
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

kasopp_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

//printf("metka_poi=%d\n",data->poi.metka_poi);
if(data->poi.metka_poi == 1 )
 {

// printf("Формирование заголовка с реквизитами поиска.\n");
  iceb_u_str strpoi;  

  strpoi.new_plus(gettext("Поиск"));
  strpoi.plus(" !!!");

  iceb_str_poisk(&strpoi,data->poi.kod.ravno(),gettext("Код"));
  iceb_str_poisk(&strpoi,data->poi.naim.ravno(),gettext("Наименование"));

  iceb_label_set_text_color(data->label_poisk,strpoi.ravno(),"red");
  gtk_widget_show(data->label_poisk);
 }
else
 {
  gtk_widget_hide(data->label_poisk); 
 }

zagolov.new_plus(gettext("Список приходных операций"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void kasopp_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код операции"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование операции"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт"), renderer,"text", COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт корреспондент"), renderer,"text", COL_SHETK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Проводки"), renderer,"text", COL_PROV,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код целевого назначения"), renderer,"text", COL_KOD_CN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);


}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void kasopp_vibor(GtkTreeSelection *selection,class kasopp_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;

gtk_tree_model_get(model,&iter,COL_KOD,&kod,COL_NAIM,&naim,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(kod);
data->naimv.new_plus(naim);
data->snanomer=nomer;

g_free(kod);
g_free(naim);

//printf("kasopp_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void kasopp_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class kasopp_data *data)
{

if(data->metka_rr == 0)
 {
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");
  return;
 }

iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);

data->metka_voz=0;

}
/****************************/
/*Проверка записей          */
/*****************************/

int  kasopp_prov_row(SQL_str row,class kasopp_data *data)
{
if(data->poi.metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poi.kod.ravno(),row[0],0,0) != 0)
 return(1);

//Поиск образца в строке
if(data->poi.naim.ravno()[0] != '\0' && iceb_u_strstrm(row[1],data->poi.naim.ravno()) == 0)
 return(1);


   
return(0);
}
/*****************************/
/*Удаление записи            */
/*****************************/

void kasopp_udzap(class kasopp_data *data)
{

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
 return;

if(kasopp_pvu(1,data->kodv.ravno(),data->window) != 0)
 return;

char strsql[512];

sprintf(strsql,"delete from Kasop1 where kod='%s'",
data->kodv.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;

}
/*****************************/
/*Проверить возможность удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int  kasopp_pvu(int met,const char *kod,GtkWidget *wpredok)
{
char strsql[512];
int kolstr;
SQLCURSOR cur;

sprintf(strsql,"select kodop from Kasord where kodop='%s' and tp=1 limit 1",kod);

if((kolstr=iceb_sql_readkey(strsql,wpredok)) < 0)
  return(1);
  
if(kolstr != 0)
 {
  if(met != 0)
   {
    iceb_u_str soob;
    sprintf(strsql,"%s Kasord",gettext("Удалить невозможно ! Есть записи в таблице"));
    soob.plus(strsql);
    iceb_menu_soob(&soob,wpredok);
   }
  return(1);
 }    
return(0);
}
/**************************/
/*Удаление неиспользуемых кодов групп*/
/***********************************/

void kasopp_unk(GtkWidget *wpredok)
{

if(iceb_menu_danet(gettext("Удалить неиспользуемые коды ? Вы уверены ?"),2,wpredok) == 2)
 return;

gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));



char strsql[512];
SQL_str row;
SQLCURSOR cur;
int kolstr;
sprintf(strsql,"select kod from Kasop1");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  if(kasopp_pvu(0,row[0],wpredok) != 0)
    continue;
  sprintf(strsql,"delete from Kasop1 where kod='%s'",row[0]);
  iceb_sql_zapis(strsql,1,1,wpredok);
 }
}
/**********************************/
/*Распечатать список*/
/****************************/

void kasopp_rasp(class kasopp_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Kasop1 order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"kasopp%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список приходных операций"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(gettext("Список приходных операций"),0,0,0,0,0,0,ff,data->window);
fprintf(ff,"%s:%s\n",gettext("Подсистема"),gettext("Учёт кассовых ордеров"));

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
  Код  |          Наименование        | Счёт |Счёт к.|Код ц.н.|Проводки|Дата и время запис.| Кто записал\n"));
  
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------\n");
class iceb_u_str prov("");
cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(kasopp_prov_row(row,data) != 0)
    continue;

  if(atoi(row[4]) == 0)
   prov.new_plus(gettext("Не делать"));
  else
   prov.new_plus(gettext("Делать"));
   
  fprintf(ff,"%-*s %-*.*s %-*s %-*s %-8s %-*.*s %s %s\n",
  iceb_u_kolbait(7,row[0]),
  row[0],
  iceb_u_kolbait(30,row[1]),
  iceb_u_kolbait(30,row[1]),
  row[1],
  iceb_u_kolbait(6,row[2]),
  row[2],
  iceb_u_kolbait(7,row[3]),
  row[3],
  row[7],    
  iceb_u_kolbait(8,prov.ravno()),
  iceb_u_kolbait(8,prov.ravno()),
  prov.ravno(),
  iceb_u_vremzap(row[6]),
  iceb_kszap(row[5],data->window));
  
  if(iceb_u_strlen(row[1]) > 30)
   fprintf(ff,"%7s %s\n","",iceb_u_adrsimv(30,row[1]));
 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}

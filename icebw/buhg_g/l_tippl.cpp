/*$Id: l_tippl.c,v 1.25 2013/12/31 11:49:14 sasa Exp $*/
/*21.06.2015	12.05.2006	Белых А.И.	l_tippl.c
Работа со списком типовых платежей
*/
#include <errno.h>
#include "buhg_g.h"
#include "l_tippl.h"
#include "dok4w.h"

enum
{
 FK2,
 SFK2,
 FK3,
 FK4,
 FK5,
 FK6,
 FK7,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KOD,
 COL_NAIM,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  tippl_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  

  class tippl_rek poi;
  iceb_u_str zapros;

  iceb_u_str kodtv;//Код толькочто введённой записи
  iceb_u_str kodv; //Выбранный код 
  iceb_u_str naimv; //Наименование выбранной записи
  
  char tablica[64];
  short dt,mt,gt;
  
  //Конструктор
  tippl_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
   }      
 };

gboolean   tippl_key_press(GtkWidget *widget,GdkEventKey *event,class tippl_data *data);
void tippl_vibor(GtkTreeSelection *selection,class tippl_data *data);
void tippl_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class tippl_data *data);
void  tippl_knopka(GtkWidget *widget,class tippl_data *data);
void tippl_add_columns (GtkTreeView *treeview);
void tippl_create_list(class tippl_data *data);
int  tippl_prov_row(SQL_str row,class tippl_data *data);

void tippl_udzap(class tippl_data *data);
void tippl_rasp(class tippl_data *data);
int l_tippl_p(class tippl_rek *datap,GtkWidget *wpredok);
int l_tippl_v(char *tablica,iceb_u_str *kodzap,GtkWidget *wpredok);
int l_tippl_snd(class tippl_data *data);

extern SQL_baza	bd;
extern REC rec;

void l_tippl(const char *tablica,GtkWidget *wpredok)
{
tippl_data data;
char bros[512];
strcpy(data.tablica,tablica);
iceb_u_poltekdat(&data.dt,&data.mt,&data.gt);


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

if(iceb_u_SRAV(data.tablica,"Ktnp",0) == 0)
  sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список типовых платёжных поручений"));
if(iceb_u_SRAV(data.tablica,"Ktnt",0) == 0)
  sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список типовых платёжных требований"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(tippl_key_press),&data);
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
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(tippl_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(tippl_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(tippl_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить выбранную запись"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(tippl_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(tippl_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);


sprintf(bros,"F6 %s",gettext("Типовой документ"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(tippl_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Ввод и корректировка типового документа для выбранной записи"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Документ"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(tippl_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Создание нового документа на основе типового документа для выбранной записи"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show(data.knopka[FK7]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(tippl_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

tippl_create_list(&data);
gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


//printf("l_tippl end\n");
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

//return(data.metka_voz);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  tippl_knopka(GtkWidget *widget,class tippl_data *data)
{
char tablica[64];
int knop=atoi(gtk_widget_get_name(widget));
//g_print("tippl_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {
  case FK2:
    data->kodtv.new_plus("");
    l_tippl_v(data->tablica,&data->kodtv,data->window);
    tippl_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    
    if(l_tippl_v(data->tablica,&data->kodv,data->window) == 0)
        data->kodtv.new_plus(data->kodv.ravno());
    tippl_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
//    printf("data->snanomer=%d\n",data->snanomer);
    tippl_udzap(data);
    tippl_create_list(data);
    return;  


  case FK4:
    l_tippl_p(&data->poi,data->window);
    tippl_create_list(data);
    return;  

  case FK5:
    if(data->kolzap == 0)
      return;
    tippl_rasp(data);
    return;  

 case FK6: /*ввод типового документа*/
    if(data->kolzap == 0)
      return;

    
    if(iceb_u_SRAV("Ktnp",data->tablica,0) == 0)
      strncpy(tablica,"Tpltp",sizeof(tablica)-1);
    if(iceb_u_SRAV("Ktnt",data->tablica,0) == 0)
      strncpy(tablica,"Tpltt",sizeof(tablica)-1);


    if(readpdokw(tablica,0,data->kodv.ravno(),data->window) != 0) /*документа нет ещё*/
     {
      vzrkbzw("",data->window); /*читаем реквизиты контрагента 00*/
      rec.nomdk.new_plus(data->kodv.ravno());
     }
   
    xdkdok(tablica,data->window);

    return;  

  case FK7: /*создание нового документа на основе типового*/
    l_tippl_snd(data);
    return;    

  case FK10:
//    printf("tippl_knopka F10\n");
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   tippl_key_press(GtkWidget *widget,GdkEventKey *event,class tippl_data *data)
{
iceb_u_str repl;
//printf("tippl_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {
  case GDK_KEY_F2:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);

  case GDK_KEY_F6:
    g_signal_emit_by_name(data->knopka[FK6],"clicked");
    return(TRUE);

  case GDK_KEY_F7:
    g_signal_emit_by_name(data->knopka[FK7],"clicked");
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("tippl_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void tippl_create_list (class tippl_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row;
iceb_u_str zagolov;

//printf("tippl_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(tippl_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(tippl_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

sprintf(strsql,"select * from %s order by kod asc",data->tablica);

data->zapros.new_plus(strsql);
//printf("%s-%s\n",__FUNCTION__,strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

data->kolzap=0;


while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
   if(tippl_prov_row(row,data) != 0)
    continue;

  if(iceb_u_SRAV(data->kodtv.ravno(),row[0],0) == 0)
   data->snanomer=data->kolzap;

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,row[0],
  COL_NAIM,row[1],
  COL_DATA_VREM,iceb_u_vremzap(row[3]),
  COL_KTO,iceb_kszap(row[2],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
printf("%s\n",__FUNCTION__);
data->kodtv.new_plus("");
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

tippl_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

if(data->poi.metka_poi == 1 )
 {

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


if(iceb_u_SRAV(data->tablica,"Ktnp",0) == 0)
  zagolov.new_plus(gettext("Список типовых платёжных поручений"));
if(iceb_u_SRAV(data->tablica,"Ktnt",0) == 0)
  zagolov.new_plus(gettext("Список типовых платёжных требований"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void tippl_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

printf("tippl_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

printf("tippl_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void tippl_vibor(GtkTreeSelection *selection,class tippl_data *data)
{
printf("tippl_vibor\n");
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

//printf("tippl_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void tippl_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class tippl_data *data)
{
//printf("tippl_v_row\n");
//printf("tippl_v_row корректировка\n");

g_signal_emit_by_name(data->knopka[SFK2],"clicked");

}
/****************************/
/*Проверка записей          */
/*****************************/

int  tippl_prov_row(SQL_str row,class tippl_data *data)
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

void tippl_udzap(class tippl_data *data)
{

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
 return;


char strsql[512];

sprintf(strsql,"delete from %s where kod='%s'",
data->tablica,
data->kodv.ravno());

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 return;

/*удаляем типовой документа*/

if(iceb_u_SRAV(data->tablica,"Ktnp",0) == 0)
  sprintf(strsql,"delete from Tpltp where nomd='%s'",data->kodv.ravno());
if(iceb_u_SRAV(data->tablica,"Ktnt",0) == 0)
  sprintf(strsql,"delete from Tpltt where nomd='%s'",data->kodv.ravno());

iceb_sql_zapis(strsql,1,0,data->window);

}
/**********************************/
/*Распечатать список*/
/****************************/

void tippl_rasp(class tippl_data *data)
{
char strsql[512];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from %s order by kod asc",data->tablica);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"tippl%d.lst",getpid());

imaf.plus(strsql);

if(iceb_u_SRAV(data->tablica,"Ktnp",0) == 0)
  naimot.plus(gettext("Список типовых платёжных поручений"));
if(iceb_u_SRAV(data->tablica,"Ktnt",0) == 0)
  naimot.plus(gettext("Список типовых платёжных требований"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_zagolov(naimot.ravno(0),0,0,0,0,0,0,ff,data->window);

fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Код |          Наименование        |Дата и время запис.| Кто записал\n"));
  
fprintf(ff,"\
------------------------------------------------------------------------\n");

cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(tippl_prov_row(row,data) != 0)
    continue;

  fprintf(ff,"%-4s %-*.*s %s %s\n",
  row[0],
  iceb_u_kolbait(30,row[1]),iceb_u_kolbait(30,row[1]),row[1],
  iceb_u_vremzap(row[3]),
  iceb_kszap(row[2],data->window));
  
  if(iceb_u_strlen(row[1]) > 30)
   fprintf(ff,"%4s %s\n","",iceb_u_adrsimv(30,row[1]));

 }
fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}
/******************************/
/*Создание нового документа*/
/*****************************/
int l_tippl_snd(class tippl_data *data)
{

char tablica[64];

if(iceb_u_SRAV(data->tablica,"Ktnp",0) == 0)
 strcpy(tablica,"Tpltp");
if(iceb_u_SRAV(data->tablica,"Ktnt",0) == 0)
 strcpy(tablica,"Tpltt");
 
if(readpdokw(tablica,0,data->kodv.ravno(),data->window) != 0)
 {
  iceb_menu_soob(gettext("Не найдено типового документа !"),data->window);
  return(1);
 }
char strsql[128];
sprintf(strsql,"%d.%d.%d",data->dt,data->mt,data->gt);

 
if(iceb_menu_vvod1(gettext("Введите дату"),strsql,10,"",data->window) != 0)
 return(1);


if(iceb_u_rsdat(&rec.dd,&rec.md,&rec.gd,strsql,1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
  return(1);
 }



data->dt=rec.dd; data->mt=rec.md; data->gt=rec.gd;

if(iceb_u_SRAV(data->tablica,"Ktnp",0) == 0)
 strcpy(tablica,"Pltp");
if(iceb_u_SRAV(data->tablica,"Ktnt",0) == 0)
 strcpy(tablica,"Pltt");

sprintf(strsql,"%d",iceb_nomdok(data->gt,tablica,data->window));
rec.nomdk.new_plus(strsql);

xdkdok(tablica,data->window);

return(0);

}

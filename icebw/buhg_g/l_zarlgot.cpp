/*$Id: l_zarlgot.c,v 1.4 2013/08/25 08:26:46 sasa Exp $*/
/*08.05.2015	10.11.2011	Белых А.И.	l_zarlgot.c
Работа со справочником льгот
*/
#include        <errno.h>
#include  "buhg_g.h"

enum
{
  COL_TABNOM,
  COL_FIO,
  COL_DNDL,
  COL_PROC,
  COL_KOL_LGOT,
  COL_KOD_LGOT,
  COL_KOMENT,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

enum
{
  FK2,
  FK3,
  FK5,
  FK10,
  SFK2,
  KOL_F_KL
};

class  l_zarlgot_data
 {
  public:


  class iceb_u_str tabnomv;
  class iceb_u_str tabnom_vib;
  class iceb_u_str dndlv;
  class iceb_u_str dndl_vib;
      
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       metka_voz;  //0-выбрали 1-нет  
  //Конструктор
  l_zarlgot_data()
   {
    snanomer=0;
    metka_voz=kl_shift=0;
    window=treeview=NULL;
   }      
 };

gboolean   l_zarlgot_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarlgot_data *data);
void l_zarlgot_vibor(GtkTreeSelection *selection,class l_zarlgot_data *data);
void l_zarlgot_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zarlgot_data *data);
void  l_zarlgot_knopka(GtkWidget *widget,class l_zarlgot_data *data);
void l_zarlgot_add_columns (GtkTreeView *treeview);
void l_zarlgot_udzap(class l_zarlgot_data *data);
void l_zarlgot_rasp(class l_zarlgot_data *data);
void l_zarlgot_create_list (class l_zarlgot_data *data);

int l_zarlgot_v(class iceb_u_str *tabnom,class iceb_u_str *dndl,GtkWidget *wpredok);


extern SQL_baza  bd;

void l_zarlgot(GtkWidget *wpredok)
{
l_zarlgot_data data;
char bros[512];


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Справочник льгот"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zarlgot_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Справочник льгот"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_zarlgot_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(l_zarlgot_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(l_zarlgot_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной записи"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(l_zarlgot_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_zarlgot_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

l_zarlgot_create_list(&data);

gtk_widget_show(data.window);
//if(metka_rr == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

//return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void l_zarlgot_create_list (class l_zarlgot_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur,cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(l_zarlgot_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(l_zarlgot_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select * from Zarlgot order by tn asc, dndl asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str fio("");
class iceb_u_str metka("");
data->kolzap=0;
float kolstr1=0. ;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  fio.new_plus("");  
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   fio.new_plus(row1[0]);

  metka.new_plus("");
  if(atoi(row[3]) == 1)
   metka.new_plus(gettext("Процент"));
      
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_TABNOM,row[0],
  COL_FIO,fio.ravno(),
  COL_DNDL,iceb_u_datzap_mg(row[1]),
  COL_PROC,row[2],
  COL_KOL_LGOT,row[3],
  COL_KOD_LGOT,row[4],
  COL_KOMENT,row[5],
  COL_DATA_VREM,iceb_u_vremzap(row[7]),
  COL_KTO,iceb_kszap(row[6],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

l_zarlgot_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

//Стать подсветкой стороки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Справочник льгот"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());
/*****************
if(data->metka_poi == 1)
 {
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kod.ravno(),gettext("Код"));
  iceb_str_poisk(&zagolov,data->poisk.naim.ravno(),gettext("Наименование"));
  

  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 
******************/

gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void l_zarlgot_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Таб.ном."),renderer,"text",COL_TABNOM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_TABNOM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Фамилия"),renderer,"text",COL_FIO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_FIO);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата н.д."),renderer,"text",COL_DNDL,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DNDL);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Процент"),renderer,"text",COL_PROC,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_PROC);
gtk_tree_view_append_column (treeview, column);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Количество льгот"),renderer,"text",COL_KOL_LGOT,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOL_LGOT);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Код льготы"),renderer,"text",COL_KOD_LGOT,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOD_LGOT);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Коментарий"),renderer,"text",COL_KOMENT,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KOMENT);
gtk_tree_view_append_column (treeview, column);




renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Дата и время записи"),renderer,"text",COL_DATA_VREM,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_DATA_VREM);
gtk_tree_view_append_column (treeview, column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes (gettext("Кто записал"),renderer,"text",COL_KTO,NULL);
gtk_tree_view_column_set_resizable(column,TRUE); /*Разрешение на изменение размеров колонки*/
gtk_tree_view_column_set_sort_column_id (column, COL_KTO);
gtk_tree_view_append_column (treeview, column);
}

/****************************/
/*Выбор строки*/
/**********************/

void l_zarlgot_vibor(GtkTreeSelection *selection,class l_zarlgot_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *dndl;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_TABNOM,&kod,COL_DNDL,&dndl,NUM_COLUMNS,&nomer,-1);

data->tabnom_vib.new_plus(kod);
data->dndl_vib.new_plus(dndl);
data->snanomer=nomer;

g_free(kod);
g_free(dndl);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_zarlgot_knopka(GtkWidget *widget,class l_zarlgot_data *data)
{
iceb_u_str repl;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_zarlgot_knopka knop=%d\n",knop);
data->kl_shift=0; /*обязательно сбрасываем*/

switch (knop)
 {
  case FK2:
    data->tabnomv.new_plus("");
    data->dndlv.new_plus("");
    if(l_zarlgot_v(&data->tabnomv,&data->dndlv,data->window) == 0)
      l_zarlgot_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(l_zarlgot_v(&data->tabnom_vib,&data->dndl_vib,data->window) == 0)
      l_zarlgot_create_list(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) != 1)
     return;
    l_zarlgot_udzap(data);
    l_zarlgot_create_list(data);
    return;  
  


  case FK5:
    l_zarlgot_rasp(data);
    return;  

    
  case FK10:
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarlgot_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarlgot_data *data)
{
iceb_u_str repl;

switch(event->keyval)
 {

  case GDK_KEY_F2:

    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    else
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");

    return(TRUE);
   
  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
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

    data->kl_shift=1;

    return(TRUE);

  default:
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void l_zarlgot_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class l_zarlgot_data *data)
{

g_signal_emit_by_name(data->knopka[SFK2],"clicked");
return;


}

/*****************************/
/*Удаление записи            */
/*****************************/

void l_zarlgot_udzap(class l_zarlgot_data *data)
{


char strsql[512];

sprintf(strsql,"delete from Zarlgot where tn=%d and dndl='%s'",data->tabnom_vib.ravno_atoi(),data->dndl_vib.ravno_sqldata());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void l_zarlgot_rasp(class l_zarlgot_data *data)
{
char strsql[512];
SQL_str row,row1;
FILE *ff;
SQLCURSOR cur,cur1;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Zarlgot order by tn asc,dndl asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"zarlgot%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Справочник льгот"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_u_zagolov(gettext("Справочник льгот"),0,0,0,0,0,0,iceb_get_pnk("00",0,data->window),ff);

fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Таб.номер|     Фамилия        |Дата н.дей|Процент|Кол. льгот|Код л|Коментарий\n"));
/**********
123456789 12345678901234567890 1234567890 1234567 1234567890 12345
***********/
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");
class iceb_u_str fio("");

while(cur.read_cursor(&row) != 0)
 {
  fio.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   fio.new_plus(row1[0]);

  fprintf(ff,"%9s %*.*s %10s %7s %10s %5s %s\n",
  row[0],
  
  iceb_u_kolbait(20,fio.ravno()),
  iceb_u_kolbait(20,fio.ravno()),
  fio.ravno(),
  iceb_u_datzap_mg(row[1]),
  row[2],
  row[3],
  row[4],
  row[5]);  

 }
fprintf(ff,"\
----------------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);
iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}

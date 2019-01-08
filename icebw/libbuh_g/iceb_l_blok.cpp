/*$Id: iceb_l_blok.c,v 1.12 2013/12/31 11:49:18 sasa Exp $*/
/*06.05.2015	06.09.2010	Белых А.И.	iceb_l_blok.c
Работа со списком блокировок
*/
#include        <errno.h>
#include  "iceb_libbuh.h"

enum
{
  FK2,
  SFK2,
  FK3,
  FK5,
  FK10,
  KOL_F_KL
};

enum
{
  COL_DATA,
  COL_LOGIN,
  COL_SHET,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

class  iceb_l_blok_data
 {
  public:


  short godv;
  short mesv;

  short godtv;
  short mestv;
  
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  class iceb_u_str zapros;
    
  //Конструктор
  iceb_l_blok_data()
   {
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    godv=mesv=godtv=mestv=0;
   }      
 };

gboolean   iceb_l_blok_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_blok_data *data);
void iceb_l_blok_vibor(GtkTreeSelection *selection,class iceb_l_blok_data *data);
void iceb_l_blokv_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class iceb_l_blok_data *data);
void  iceb_l_blok_knopka(GtkWidget *widget,class iceb_l_blok_data *data);
void iceb_l_blok_add_columns (GtkTreeView *treeview);
int iceb_l_blok_v(short *mes,short *god,GtkWidget*);
void iceb_l_blok_udzap(class iceb_l_blok_data *data);
void iceb_l_blok_rasp(class iceb_l_blok_data *data);
void iceb_l_blok_create_list (class iceb_l_blok_data *data);


extern SQL_baza	bd;
int iceb_kod_podsystem=0; /*Устанавливается при старте подсистемы*/

void iceb_l_blok(GtkWidget *wpredok)
{
iceb_l_blok_data data;
char bros[512];

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список заблокированых дат"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_l_blok_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Список заблокированых дат"));


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

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(iceb_l_blok_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(iceb_l_blok_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной запси"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(iceb_l_blok_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной запси"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);


sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(iceb_l_blok_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(iceb_l_blok_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

iceb_l_blok_create_list(&data);

gtk_widget_show(data.window);
gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return;

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void iceb_l_blok_create_list (class iceb_l_blok_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
//GdkColor color;

data->kl_shift=0; //0-отжата 1-нажата  


if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(iceb_l_blokv_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(iceb_l_blok_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select * from Blok where kod=%d order by god desc",iceb_kod_podsystem);
data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);


//iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  

  if(data->godtv != 0)
   if(data->godtv == atoi(row[2]) && data->mestv == atoi(row[1]))
    data->snanomer=data->kolzap;

  if(atoi(row[2]) != 0)
   sprintf(strsql,"%02d.%d",atoi(row[2]),atoi(row[1]));
  else
   sprintf(strsql,"**.%d",atoi(row[1]));
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATA,strsql,
  COL_LOGIN,row[3],
  COL_SHET,row[4],
  COL_DATA_VREM,iceb_u_vremzap(row[6]),
  COL_KTO,iceb_kszap(row[5],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->godtv=data->mestv=0;

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

iceb_l_blok_add_columns (GTK_TREE_VIEW (data->treeview));


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

//Стать подсветкой строки на нужный номер строки
iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);


iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список заблокированых дат"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
zagolov.ps_plus(iceb_getnps());

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());
/************************
if(data->metka_poi == 1)
 {
  printf("Формирование заголовка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");

  iceb_str_poisk(&zagolov,data->poisk.kod.ravno(),gettext("Код"));
  iceb_str_poisk(&zagolov,data->poisk.naim.ravno(),gettext("Наименование"));
  

  gtk_label_set_text(GTK_LABEL(data->label_poisk),zagolov.ravno());
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 
**********************/
gtk_widget_show(data->label_kolstr);

}

/*****************/
/*Создаем колонки*/
/*****************/

void iceb_l_blok_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

//printf("iceb_l_blok_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата"), renderer,"text", COL_DATA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Логин"), renderer,"text", COL_LOGIN,NULL);

if(iceb_kod_podsystem == ICEB_PS_GK)
 {
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
  -1,gettext("Счета"), renderer,"text", COL_SHET,NULL);
 }

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);
//printf("iceb_l_blok_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void iceb_l_blok_vibor(GtkTreeSelection *selection,class iceb_l_blok_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datab;
gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATA,&datab,NUM_COLUMNS,&nomer,-1);

iceb_u_polen(datab,&data->mesv,1,'.');
iceb_u_polen(datab,&data->godv,2,'.');


data->snanomer=nomer;

g_free(datab);

//printf("%s %s %d\n",data->kodv.ravno(),data->naimv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_l_blok_knopka(GtkWidget *widget,class iceb_l_blok_data *data)
{
//char    bros[512];
iceb_u_str repl;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("iceb_l_blok_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    if(iceb_l_blok_prov(data->window) != 0)
     return;
    data->godtv=data->mestv=0;
    if(iceb_l_blok_v(&data->mestv,&data->godtv,data->window) == 0)
      iceb_l_blok_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
      return;
    if(iceb_l_blok_v(&data->mesv,&data->godv,data->window) == 0)
      iceb_l_blok_create_list(data);
    return;  


  case FK3:
    if(data->kolzap == 0)
      return;
    if(iceb_l_blok_prov(data->window) != 0)
     return;
    iceb_l_blok_udzap(data);
    iceb_l_blok_create_list(data);
    return;  



  case FK5:
    if(data->kolzap == 0)
      return;
    iceb_l_blok_rasp(data);
    return;  

    
  case FK10:
//    printf("iceb_l_blok_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_l_blok_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_blok_data *data)
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
    printf("iceb_l_blok_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void iceb_l_blokv_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class iceb_l_blok_data *data)
{
g_signal_emit_by_name(data->knopka[SFK2],"clicked");

}

/*****************************/
/*Удаление записи            */
/*****************************/

void iceb_l_blok_udzap(class iceb_l_blok_data *data)
{

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
 return;


char strsql[512];

sprintf(strsql,"delete from Blok where kod=%d and god=%d and mes=%d",
iceb_kod_podsystem,data->godv,data->mesv);

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;



}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void iceb_l_blok_rasp(class iceb_l_blok_data *data)
{
char strsql[312];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Blok order by god desc");

if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"blok%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список заблокированых дат"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }

iceb_u_zagolov(naimot.ravno(0),0,0,0,0,0,0,iceb_get_pnk("00",0,data->window),ff);
fprintf(ff,"%s\n",iceb_getnps());

fprintf(ff,"\
-----------------------------------------------------------------------------------\n");

fprintf(ff,"\
Дата   |     Логины         |      Счета         |Дата и время запис.| Кто записал\n");

/*******
1234567 12345678901234567890 12345678901234567890
**********/
fprintf(ff,"\
-----------------------------------------------------------------------------------\n");

cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(atoi(row[2]) != 0)
   sprintf(strsql,"%02d.%d",atoi(row[2]),atoi(row[1]));
  else
   sprintf(strsql,"**.%d",atoi(row[1]));

  fprintf(ff,"%s|%-*.*s|%-*.*s|%s|%s\n",
  strsql,
  iceb_u_kolbait(20,row[3]),
  iceb_u_kolbait(20,row[3]),
  row[3],
  iceb_u_kolbait(20,row[4]),
  iceb_u_kolbait(20,row[4]),
  row[4],
  iceb_u_vremzap(row[6]),iceb_kszap(row[5],data->window));


 }
fprintf(ff,"\
-----------------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);

iceb_ustpeh(imaf.ravno(0),3,data->window);
iceb_rabfil(&imaf,&naimot,data->window);

}

/*******************************************************************/
/*проверка разрешения оператору вводить и корректировать информацию*/
/********************************************************************/
int iceb_l_blok_prov(GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
char name_kol[32];
switch(iceb_kod_podsystem)
 {
  case ICEB_PS_GK:
   strcpy(name_kol,"gkb");
   break;

  case ICEB_PS_MU:
   strcpy(name_kol,"mub");
   break;

  case ICEB_PS_PD:
   strcpy(name_kol,"pdb");
   break;

  case ICEB_PS_ZP:
   strcpy(name_kol,"zpb");
   break;

  case ICEB_PS_UOS:
   strcpy(name_kol,"osb");
   break;

  case ICEB_PS_UU:
   strcpy(name_kol,"uub");
   break;

  case ICEB_PS_UKO:
   strcpy(name_kol,"kob");
   break;

  case ICEB_PS_UKR:
   strcpy(name_kol,"krb");
   break;

  case ICEB_PS_UPL:
   strcpy(name_kol,"plb");
   break;

  case ICEB_PS_RNN:
   strcpy(name_kol,"nnb");
   break;

  case ICEB_PS_UD:
   strcpy(name_kol,"udb");
   break;

 };
 
sprintf(strsql,"select %s from icebuser where login='%s'",name_kol,iceb_u_getlogin());

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s\n%s %s",
  gettext("У Вас нет полномочий для выполнения этой операции!"),
  gettext("Ненайден логин"),
  iceb_u_getlogin());

  iceb_menu_soob(strsql,wpredok);

  return(1);
  
 }
if(atoi(row[0]) == 1)
 {
  iceb_menu_soob(gettext("У Вас нет полномочий для выполнения этой операции!"),wpredok);
  return(1);
  
 }


return(0);

}

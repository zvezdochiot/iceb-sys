/*$Id: l_prov_kas.c,v 1.33 2013/12/31 11:49:14 sasa Exp $*/
/*24.05.2016	03.11.2005	Белых А.И.	l_prov_kas.c
Работа с проводками для докумета в учёте кассовых ордеров
*/
#include        "buhg_g.h"
enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK6,
 FK8,
 FK10,
 KOL_F_KL
};

enum
{
 COL_DATA,
 COL_SHET,
 COL_SHET_KOR,
 COL_KONTR,
 COL_DEBET,
 COL_KREDIT,
 COL_KOMENT,
 COL_DATA_VREM,
 COL_KTO,  
 COL_KEKV,
//неотображаемые данные
 COL_VREM,
 COL_KTOZ,
 COL_VAL,
 NUM_COLUMNS
};

class prov_kas_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *radiobutton[3];
  class iceb_u_str name_window;
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       voz;

  iceb_u_str zapros;
  class iceb_u_str metkasys;
  
  //Реквизиты документа
  short dd,md,gd; //Дата документа
  iceb_u_str nomdok; //Номер документа
  iceb_u_str kassa; //Номер кассы
  iceb_u_str kodop; //Код операции
  int tipz;     //1-приход 2-расход
  iceb_u_str shetd; //Счёт документа            

  iceb_u_str shetk;
  iceb_u_str naim_kodop;
  iceb_u_str naim_kas;      
  int metka_r;
  
  //Выбранная проводка
  iceb_u_str datap_v;
  iceb_u_str shet_v;
  iceb_u_str shet_kor_v;
  iceb_u_str kontr_v;
  double     debet_v;
  double     kredit_v;
  iceb_u_str koment_v;
  time_t     vrem_v;
  int        ktoz_v;
  int        val_v;
  double     suma_deb;
  double     suma_kre;  
  int kekv;
  float pnds; //Процент НДС на момент создания документа
  prov_kas_data()
   {
    kekv=0;
    
    metka_r=0;
    treeview=NULL;
    snanomer=0;
    kolzap=0;
    voz=0;
    kl_shift=0;
    naim_kodop.new_plus("");
    naim_kas.new_plus("");
    shetk.plus("");
    metkasys.plus(ICEB_MP_KASA);
   }

 };
gboolean   prov_kas_key_press(GtkWidget *widget,GdkEventKey *event,class prov_kas_data *data);
void prov_kas_vibor(GtkTreeSelection *selection,class prov_kas_data *data);
void prov_kas_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class prov_kas_data *data);
void  prov_kas_knopka(GtkWidget *widget,class prov_kas_data *data);
void prov_kas_add_columns (GtkTreeView *treeview);
void prov_kas_create_list (class prov_kas_data *data);

void       prov_kas_radio0(GtkWidget *,class prov_kas_data *);
void       prov_kas_radio1(GtkWidget *,class prov_kas_data *);
void       prov_kas_radio2(GtkWidget *,class prov_kas_data *);

void prov_kas_sapka_menu(class prov_kas_data *data);
int prov_kas_prpvblok(class prov_kas_data *data);


extern SQL_baza  bd;

void l_prov_kas(short dd,short md,short gd, //Дата документа
const char *nomd, //Номер документа
const char *kassa, //Номер кассы
const char *kodop, //Код операции
int tipz,     //1-приход 2-расход
const char *shetd, //Счёт документа
GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class prov_kas_data data;
char strsql[512];
SQL_str row;
SQLCURSOR cur;

data.tipz=tipz;
data.dd=dd;
data.md=md;
data.gd=gd;
data.nomdok.plus(nomd);
data.kassa.plus(kassa);
data.kodop.plus(kodop);
data.shetd.plus(shetd);
data.name_window.plus(__FUNCTION__);

//Читаем наименование операции
if(tipz == 1)
  sprintf(strsql,"select naik,shetkas from Kasop1 where kod='%s'",kodop);
if(tipz == 2)
  sprintf(strsql,"select naik,shetkas from Kasop2 where kod='%s'",kodop);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  data.naim_kodop.new_plus(row[0]);
  data.shetk.new_plus(row[1]);
 }

//Читаем наименование кассы
sprintf(strsql,"select naik,shet from Kas where kod=%d",data.kassa.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  data.naim_kas.new_plus(row[0]);
  data.shetk.new_plus(row[1]);
 }

if(data.shetk.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не введено счёт для кассы"),data.kassa.ravno());
  iceb_menu_soob(strsql,wpredok);
  return;
 }
data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_default_size (GTK_WINDOW  (data.window),600,400);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),600,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),
gettext("Работа с проводками (облік кассовых ордеров)"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(prov_kas_key_press),&data);
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
gtk_widget_show_all(hbox);

data.label_kolstr=gtk_label_new ("");

iceb_u_str stroka;

prov_kas_sapka_menu(&data);



gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

GtkWidget *hboxradio = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous(GTK_BOX(hboxradio),TRUE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

//Вставляем радиокнопки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Дебет"));
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[0], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton[0], "clicked",G_CALLBACK(prov_kas_radio0),&data);

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

sprintf(strsql,"%s",gettext("Кредит"));
data.radiobutton[1]=gtk_radio_button_new_with_label(group,strsql);
g_signal_connect(data.radiobutton[1], "clicked",G_CALLBACK(prov_kas_radio1),&data);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[1], TRUE, TRUE, 0);

group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(data.radiobutton[1]));

sprintf(strsql,"%s",gettext("Все"));
data.radiobutton[2]=gtk_radio_button_new_with_label(group,strsql);
g_signal_connect(data.radiobutton[2], "clicked",G_CALLBACK(prov_kas_radio2),&data);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[2], TRUE, TRUE, 0);

gtk_box_pack_start(GTK_BOX (vbox2), hboxradio, FALSE,FALSE, 0);
gtk_widget_show_all(hboxradio);

//Кнопки

sprintf(strsql,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(prov_kas_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой проводки"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(prov_kas_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(prov_kas_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить выбранную запись"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(prov_kas_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удалить все проводки"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);

sprintf(strsql,"F4 %s",gettext("Проводки"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(prov_kas_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Выполнить все нужные проводки"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(prov_kas_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка проводок"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(prov_kas_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);

gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

prov_kas_create_list(&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void prov_kas_create_list (class prov_kas_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;
//GdkColor color;

//printf("prov_kas_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(prov_kas_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(prov_kas_vibor),data);

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
G_TYPE_STRING, 
G_TYPE_LONG, 
G_TYPE_INT,  
G_TYPE_INT,
G_TYPE_INT);

iceb_u_str zapros;

if(data->metka_r == 2)
  sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,komen,ktoi,vrem,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%02d-%02d' and oper='%s' and tz=%d order by datp asc",
  data->metkasys.ravno(),data->kassa.ravno_atoi(),data->nomdok.ravno_filtr(),
  data->gd,data->md,data->dd,data->kodop.ravno_filtr(),data->tipz);

if(data->metka_r == 0)
  sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,komen,ktoi,vrem,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%02d-%02d' and oper='%s' and tz=%d and deb <> 0. order by datp asc",
  data->metkasys.ravno(),data->kassa.ravno_atoi(),data->nomdok.ravno_filtr(),
  data->gd,data->md,data->dd,data->kodop.ravno_filtr(),data->tipz);

if(data->metka_r == 1)
  sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,komen,ktoi,vrem,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%02d-%02d' and oper='%s' and tz=%d and kre <> 0. order by datp asc",
  data->metkasys.ravno(),data->kassa.ravno_atoi(),data->nomdok.ravno_filtr(),
  data->gd,data->md,data->dd,data->kodop.ravno_filtr(),data->tipz);

data->zapros.new_plus(strsql);


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str ss;

data->kolzap=0;
data->suma_deb=0.;
data->suma_kre=0.;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
//  if(prov_kas_prov_row(row,&data->rk,data->window) != 0)
//    continue;


//  if(iceb_u_SRAV(data->vvod.god.ravno(),row[0],0) == 0 && iceb_u_SRAV(data->vvod.shet.ravno(),row[1],0) == 0)
//    data->snanomer=data->kolzap;
  

  //Дебет
  data->suma_deb+=atof(row[4]);
  //Кредит
  data->suma_kre+=atof(row[5]);
  
  
  if(atoi(row[10]) != 0)
    ss.new_plus(row[10]);
  else  
    ss.new_plus("");

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATA,iceb_u_datzap(row[0]),
  COL_KEKV,ss.ravno(),
  COL_SHET,row[1],
  COL_SHET_KOR,row[2],
  COL_KONTR,row[3],
  COL_DEBET,row[4],
  COL_KREDIT,row[5],
  COL_KOMENT,row[6],
  COL_DATA_VREM,iceb_u_vremzap(row[8]),
  COL_KTO,iceb_kszap(row[7],data->window),
  COL_VREM,atol(row[8]),
  COL_KTOZ,atoi(row[7]),
  COL_VAL,atoi(row[9]),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

prov_kas_add_columns (GTK_TREE_VIEW (data->treeview));

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

prov_kas_sapka_menu(data);

gtk_widget_show(data->label_kolstr);


}

/*****************/
/*Создаем колонки*/
/*****************/

void prov_kas_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

//printf("prov_kas_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата"), renderer,"text", COL_DATA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт"), renderer,"text",COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт кор."), renderer,"text", COL_SHET_KOR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кон-нт"), renderer,"text", COL_KONTR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дебет"), renderer,"text", COL_DEBET,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кредит"), renderer,"text", COL_KREDIT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("КЭКР"), renderer,"text", COL_KEKV,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);
printf("prov_kas_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void prov_kas_vibor(GtkTreeSelection *selection,class prov_kas_data *data)
{
//printf("prov_kas_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *shet_d;
gchar *shet_k;
gchar *datap;
gchar *debet;
gchar *kredit;
gchar *koment;
gchar *kontr;
gint ktoz;
glong vrem;
gint val;
gint nomer;
gchar *kekv;

gtk_tree_model_get(model,&iter,
COL_SHET,&shet_d,
COL_SHET_KOR,&shet_k,
COL_DATA,&datap,
COL_DEBET,&debet,
COL_KREDIT,&kredit,
COL_KONTR,&kontr,
COL_KOMENT,&koment,
COL_KTOZ,&ktoz,
COL_VREM,&vrem,
COL_VAL,&val,
COL_KEKV,&kekv,
NUM_COLUMNS,&nomer,-1);


data->shet_v.new_plus(shet_d);
data->shet_kor_v.new_plus(shet_k);
data->kontr_v.new_plus(kontr);
data->datap_v.new_plus(datap);
data->koment_v.new_plus(koment);
data->debet_v=atof(debet);
data->kredit_v=atof(kredit);
data->ktoz_v=ktoz;
data->vrem_v=vrem;
data->val_v=val;
data->snanomer=nomer;
data->kekv=atoi(kekv);

g_free(shet_d);
g_free(shet_k);
g_free(datap);
g_free(debet);
g_free(kredit);
g_free(kontr);
g_free(koment);
g_free(kekv);
/*
printf("%s %s %s %s %.2f %.2f %s %d\n",
data->datap_v.ravno(),
data->shet_v.ravno(),
data->shet_kor_v.ravno(),
data->kontr_v.ravno(),
data->debet_v,
data->kredit_v,
data->koment_v.ravno(),
data->snanomer);
*/
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  prov_kas_knopka(GtkWidget *widget,class prov_kas_data *data)
{
iceb_u_str shetd;
iceb_u_str shetk;
iceb_u_str suma;

int knop=atoi(gtk_widget_get_name(widget));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {

  case GDK_KEY_plus: //это почемуто не работает Нажата клавиша плюс на основной клавиатуре
  case GDK_KEY_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
  case FK2:
    if(prov_kas_prpvblok(data) != 0)
     return;
    if(iceb_vprov(data->metkasys.ravno(),"","","","","",data->nomdok.ravno(),data->kodop.ravno(),data->kassa.ravno_atoi(),
    data->dd,data->md,data->gd,"",data->vrem_v,data->ktoz_v,7,data->tipz,0,0,data->window) != 0)
         return;  
    
    prov_kas_create_list(data);

    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;
    if(prov_kas_prpvblok(data) != 0)
     return;
    shetd.new_plus(data->shet_v.ravno());
    shetk.new_plus(data->shet_kor_v.ravno());
    suma.new_plus(data->debet_v);
    if(data->debet_v == 0)
     {
      shetd.new_plus(data->shet_kor_v.ravno());
      shetk.new_plus(data->shet_v.ravno());
      suma.new_plus(data->kredit_v);
     }
    
    if(iceb_vprov(data->metkasys.ravno(),shetd.ravno(),shetk.ravno(),
    suma.ravno(),data->datap_v.ravno(),data->koment_v.ravno(),
    data->nomdok.ravno(),data->kodop.ravno(),data->kassa.ravno_atoi(),
    data->dd,data->md,data->gd,"",data->vrem_v,data->ktoz_v,7,data->tipz,data->val_v,data->kekv,data->window) != 0)
         return;  
    
    prov_kas_create_list(data);

    return;  

  case FK3:
    if(data->kolzap == 0)
     return;
    if(prov_kas_prpvblok(data) != 0)
     return;
    shetd.new_plus(gettext("Удалить запись ? Вы уверены ?"));
    if(iceb_menu_danet(&shetd,2,data->window) != 1)
     return;

    iceb_udprov(data->val_v,data->datap_v.ravno(),data->shet_v.ravno_filtr(),
    data->shet_kor_v.ravno_filtr(),data->vrem_v,
    data->debet_v,data->kredit_v,data->koment_v.ravno_filtr(),2,data->window);

    prov_kas_create_list(data);
    
    return;  

  case SFK3:
    if(data->kolzap == 0)
     return;
    if(prov_kas_prpvblok(data) != 0)
     return;

  
    shetd.new_plus(gettext("Удалить все проводки ? Вы уверены ?"));
    if(iceb_menu_danet(&shetd,2,data->window) != 1)
     return;
    if(iceb_udprgr(data->metkasys.ravno(),data->dd,data->md,data->gd,data->nomdok.ravno(),data->kassa.ravno_atoi(),data->tipz,data->window) != 0)
     return;

    prov_kas_create_list(data);
    return;  
    
  
  case FK4:
    if(prov_kas_prpvblok(data) != 0)
     return;

    avtprkasw(data->kassa.ravno(),data->tipz,data->nomdok.ravno(),data->dd,data->md,data->gd,
    data->shetk.ravno(),data->shetd.ravno(),data->kodop.ravno(),data->window);

    prov_kas_create_list(data);
    return;  
  
  case FK5:
    iceb_raspprov(data->zapros.ravno(),data->dd,data->md,data->gd,data->nomdok.ravno(),5,data->window);
    return;  
  

  case FK6:
    return;  
  
  
  case FK8:
    return;  

    
  case FK10:
    provprkasw(data->kassa.ravno(),data->tipz,data->nomdok.ravno(),data->dd,data->md,data->gd,data->kodop.ravno(),data->window);
    iceb_sizww(data->name_window.ravno(),data->window);

    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   prov_kas_key_press(GtkWidget *widget,GdkEventKey *event,class prov_kas_data *data)
{
iceb_u_str repl;
printf("prov_kas_key_press keyval=%d state=%d\n",
event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_KEY_F2:
    if(data->kl_shift == 1) //Нажата
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    else
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    if(data->kl_shift == 1) //Нажата
      g_signal_emit_by_name(data->knopka[SFK3],"clicked");
    else
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
//    g_signal_emit_by_name(data->knopka[FK7],"clicked");
    return(TRUE);

  case GDK_KEY_F8:
    g_signal_emit_by_name(data->knopka[FK8],"clicked");
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("prov_kas_key_press-Нажата клавиша Shift\n");

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
void prov_kas_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class prov_kas_data *data)
{
g_signal_emit_by_name(data->knopka[SFK2],"clicked");
}

/***************/
/*Шапка меню*/
/***************/

void prov_kas_sapka_menu(class prov_kas_data *data)
{
iceb_u_str stroka;

stroka.new_plus(gettext("Работа с проводками (облік кассовых ордеров)"));
stroka.ps_plus(gettext("Касса"));

stroka.plus(":");
stroka.plus(data->kassa.ravno());
stroka.plus(" ");
stroka.plus(data->naim_kas.ravno());

//stroka.plus(" ");
stroka.ps_plus(gettext("Номер документа"));
stroka.plus(":");
stroka.plus(data->nomdok.ravno());

stroka.plus(" ");
stroka.plus(gettext("Дата"));
stroka.plus(":");
stroka.plus(data->dd);
stroka.plus(".");
stroka.plus(data->md);
stroka.plus(".");
stroka.plus(data->gd);



char strsql[512];
sprintf(strsql," %s:%d",gettext("Количество записей"),
data->kolzap);
stroka.plus(strsql);



stroka.ps_plus(gettext("Операция"));
stroka.plus(":");
stroka.plus(data->kodop.ravno());
stroka.plus("/");
stroka.plus(data->naim_kodop.ravno());

stroka.plus(" ");
if(data->tipz == 1)
 {
  stroka.plus("(");
  stroka.plus(gettext("Приход"));
  stroka.plus(")");
 }
if(data->tipz == 2)
 {
  stroka.plus("(");
  stroka.plus(gettext("Расход"));
  stroka.plus(")");
 }
stroka.ps_plus(gettext("Сумма выполненных проводок"));
stroka.plus(":");
stroka.plus(data->suma_deb);
stroka.plus("/");
stroka.plus(data->suma_kre);

stroka.ps_plus(gettext("Счета"));
stroka.plus(":");
stroka.plus(data->shetk.ravno());
stroka.plus(" - ");
stroka.plus(data->shetd.ravno());

gtk_label_set_text(GTK_LABEL(data->label_kolstr),stroka.ravno());

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  prov_kas_radio0(GtkWidget *widget,class prov_kas_data *data)
{

if(data->metka_r == 0)
  return;
data->metka_r=0;
prov_kas_create_list(data);

}
void  prov_kas_radio1(GtkWidget *widget,class prov_kas_data *data)
{
if(data->metka_r == 1)
  return;
data->metka_r=1;
prov_kas_create_list(data);
}
void  prov_kas_radio2(GtkWidget *widget,class prov_kas_data *data)
{
if(data->metka_r == 2)
  return;
data->metka_r=2;
prov_kas_create_list(data);
}
/**************************/
/*Проверка блокировки*/
/************************/

int prov_kas_prpvblok(class prov_kas_data *data)
{
if(iceb_pbpds(data->md,data->gd,data->window) != 0)
 return(1);

if(data->kolzap == 0)
 return(0);


short d1,m1,g1;

iceb_u_rsdat(&d1,&m1,&g1,data->datap_v.ravno(),1);

if(iceb_pvglkni(m1,g1,data->window) != 0)
 return(1);

return(0);
}

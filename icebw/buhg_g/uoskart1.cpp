/*$Id: uoskart1.c,v 1.20 2013/10/04 07:27:17 sasa Exp $*/
/*13.07.2015	12.11.2007	Белых А.И.	uoskart1.c
Ввод новой карточки на основное средство
*/

#include "buhg_g.h"
enum
{
 FK2,
 FK3,
 FK4,
 FK10,
 KOL_F_KL
};

enum
 {
  E_NAIM,
  E_ZAV_IZ,
  E_PASPORT,
  E_MODEL,
  E_GOD,
  E_ZAV_NOMER,
  E_DATAVV,
  E_SHETU,
  E_SHET_AMORT,
  E_SIFR_AU,
  E_GRUP_NU,
  E_KOEFIC_NU,
  E_KOEFIC_BU,
  E_SOST_OB,
  E_NOMER_ZNAK,
  E_GRUP_BU,
  E_BAL_ST_NU,
  E_IZ_NU,
  E_BAL_ST_BU,
  E_IZ_BU,
  KOLENTER  
 };

class uoskart1_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_naim[KOLENTER];
  GtkWidget *label_invnom;
  GtkWidget *opt;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  
  /*Реквизиты документа*/
  class iceb_u_str data_dok;
  class iceb_u_str nomdok;
  int innom;
  
  /*Реквизиты которые вводим*/  
  class iceb_u_str god; /*Год изготовления*/
  class iceb_u_str naim;
  class iceb_u_str zav_iz;
  class iceb_u_str pasport;
  class iceb_u_str model;
  class iceb_u_str zav_nomer;  
  class iceb_u_str datavv;  
  class iceb_u_str shetu;  
  class iceb_u_str shet_amort;  
  class iceb_u_str sifr_au;  
  class iceb_u_str grup_nu;
  class iceb_u_str koefic_nu;
  class iceb_u_str koefic_bu;
  class iceb_u_str nomer_znak;
  class iceb_u_str grup_bu;
  class iceb_u_str bal_st_nu;
  class iceb_u_str iz_nu;
  class iceb_u_str bal_st_bu;
  class iceb_u_str iz_bu;
  int sost_ob;  
  
  
  uoskart1_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    god.plus("");
    naim.plus("");
    zav_iz.plus("");
    pasport.plus("");
    model.plus("");
    zav_nomer.plus("");
    datavv.plus("");
    shetu.plus("");
    shet_amort.plus("");
    sifr_au.plus("");
    grup_nu.plus("");
    koefic_nu.plus("1");
    koefic_bu.plus("1");
    nomer_znak.plus("");
    grup_bu.plus("");
    bal_st_nu.plus("");
    iz_nu.plus("");
    bal_st_bu.plus("");
    iz_bu.plus("");
    sost_ob=0;
   }

  void read_rek()
   {
    god.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GOD])));
    naim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM])));
    zav_iz.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ZAV_IZ])));
    pasport.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PASPORT])));
    model.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MODEL])));
    zav_nomer.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MODEL])));
    datavv.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAVV])));
    shetu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHETU])));
    shet_amort.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_AMORT])));
    sifr_au.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SIFR_AU])));
    grup_nu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GRUP_NU])));
    koefic_nu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOEFIC_NU])));
    koefic_bu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOEFIC_BU])));
    nomer_znak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMER_ZNAK])));
    grup_bu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GRUP_BU])));
    bal_st_nu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_BAL_ST_NU])));
    iz_nu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_IZ_NU])));
    bal_st_bu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_BAL_ST_BU])));
    iz_bu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_IZ_BU])));
    sost_ob=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));

   }
 };

gboolean   uoskart1_v_key_press(GtkWidget *widget,GdkEventKey *event,class uoskart1_data *data);
void    uoskart1_v_vvod(GtkWidget *widget,class uoskart1_data *data);
void  uoskart1_v_knopka(GtkWidget *widget,class uoskart1_data *data);
void uosopp_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int uosopp_pk(char *kod,GtkWidget *wpredok);
void  uoskart1_e_knopka(GtkWidget *widget,class uoskart1_data *data);

void uoskart1_read(class uoskart1_data *data);
int uoskart1_zk(class uoskart1_data *data);

extern SQL_baza bd;
//extern double	okrcn;  /*Округление цены*/

int uoskart1(const char *datadok,const char *nomdok,
int innom, /*Инвентарный номер*/
double *bsnu,
double *iznu,
double *bsbu,
double *izbu,
GtkWidget *wpredok)
{
class uoskart1_data data;
char strsql[512];

data.innom=innom;
data.data_dok.new_plus(datadok);
data.nomdok.new_plus(nomdok);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой карточки основного средства"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uoskart1_v_key_press),&data);

iceb_u_str zagolov;
zagolov.new_plus(gettext("Ввод новой карточки основного средства"));
GtkWidget *label=gtk_label_new(zagolov.ravno());

sprintf(strsql,"%s:%d",gettext("Инвентарный номер"),innom);
data.label_invnom=gtk_label_new(strsql);

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox_knop = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox_knop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox_main = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox_main),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }


gtk_container_add (GTK_CONTAINER (data.window), hbox_main);
gtk_box_pack_start (GTK_BOX (hbox_main), vbox_knop, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (hbox_main), vbox, FALSE, FALSE, 1);

gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.label_invnom, FALSE, FALSE, 1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], FALSE, FALSE, 1);




sprintf(strsql,"%s",gettext("Наименование"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label,FALSE, FALSE, 1);

data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),79);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

sprintf(strsql,"%s",gettext("Завод изготовитель"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_ZAV_IZ]), label,FALSE, FALSE, 1);

data.entry[E_ZAV_IZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ZAV_IZ]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_ZAV_IZ]), data.entry[E_ZAV_IZ],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_ZAV_IZ], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZAV_IZ]),data.zav_iz.ravno());
gtk_widget_set_name(data.entry[E_ZAV_IZ],iceb_u_inttochar(E_ZAV_IZ));

sprintf(strsql,"%s",gettext("Паспорт/чертёж"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PASPORT]), label,FALSE, FALSE, 1);

data.entry[E_PASPORT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PASPORT]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_PASPORT]), data.entry[E_PASPORT],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_PASPORT], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PASPORT]),data.pasport.ravno());
gtk_widget_set_name(data.entry[E_PASPORT],iceb_u_inttochar(E_PASPORT));

sprintf(strsql,"%s",gettext("Модель"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_MODEL]), label,FALSE, FALSE, 1);

data.entry[E_MODEL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MODEL]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_MODEL]), data.entry[E_MODEL],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_MODEL], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MODEL]),data.model.ravno());
gtk_widget_set_name(data.entry[E_MODEL],iceb_u_inttochar(E_MODEL));


sprintf(strsql,"%s",gettext("Год изготовления"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE, 1);

data.entry[E_GOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GOD]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_GOD], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.god.ravno());
gtk_widget_set_name(data.entry[E_GOD],iceb_u_inttochar(E_GOD));




sprintf(strsql,"%s",gettext("Заводской номер"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_ZAV_NOMER]), label,FALSE, FALSE, 1);

data.entry[E_ZAV_NOMER] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ZAV_NOMER]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_ZAV_NOMER]), data.entry[E_ZAV_NOMER],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_ZAV_NOMER], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZAV_NOMER]),data.zav_nomer.ravno());
gtk_widget_set_name(data.entry[E_ZAV_NOMER],iceb_u_inttochar(E_ZAV_NOMER));


sprintf(strsql,"%s (%s)",gettext("Дата ввода в эксплуатацию"),gettext("д.м.г"));
data.knopka_enter[E_DATAVV]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAVV]), data.knopka_enter[E_DATAVV], FALSE, FALSE, 1);
g_signal_connect(data.knopka_enter[E_DATAVV],"clicked",G_CALLBACK(uoskart1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAVV],iceb_u_inttochar(E_DATAVV));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAVV],gettext("Выбор даты"));

data.entry[E_DATAVV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAVV]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAVV]), data.entry[E_DATAVV],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_DATAVV], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAVV]),data.datavv.ravno());
gtk_widget_set_name(data.entry[E_DATAVV],iceb_u_inttochar(E_DATAVV));


sprintf(strsql,"%s",gettext("Счёт учёта"));
data.knopka_enter[E_SHETU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE, 1);
g_signal_connect(data.knopka_enter[E_SHETU],"clicked",G_CALLBACK(uoskart1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETU],iceb_u_inttochar(E_SHETU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETU],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHETU] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHETU]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_SHETU], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.shetu.ravno());
gtk_widget_set_name(data.entry[E_SHETU],iceb_u_inttochar(E_SHETU));

data.label_naim[E_SHETU]=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.label_naim[E_SHETU],TRUE, TRUE, 1);

sprintf(strsql,"%s",gettext("Шифр затрат амортотчислений"));
data.knopka_enter[E_SHET_AMORT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_AMORT]), data.knopka_enter[E_SHET_AMORT], FALSE, FALSE, 1);
g_signal_connect(data.knopka_enter[E_SHET_AMORT],"clicked",G_CALLBACK(uoskart1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET_AMORT],iceb_u_inttochar(E_SHET_AMORT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET_AMORT],gettext("Выбор шифра"));

data.entry[E_SHET_AMORT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET_AMORT]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_AMORT]), data.entry[E_SHET_AMORT],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_SHET_AMORT], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_AMORT]),data.shet_amort.ravno());
gtk_widget_set_name(data.entry[E_SHET_AMORT],iceb_u_inttochar(E_SHET_AMORT));

data.label_naim[E_SHET_AMORT]=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_AMORT]), data.label_naim[E_SHET_AMORT],TRUE, TRUE, 1);


sprintf(strsql,"%s",gettext("Шифр аналитического учёта"));
data.knopka_enter[E_SIFR_AU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SIFR_AU]), data.knopka_enter[E_SIFR_AU], FALSE, FALSE, 1);
g_signal_connect(data.knopka_enter[E_SIFR_AU],"clicked",G_CALLBACK(uoskart1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SIFR_AU],iceb_u_inttochar(E_SIFR_AU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SIFR_AU],gettext("Выбор шифра"));

data.entry[E_SIFR_AU] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SIFR_AU]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_SIFR_AU]), data.entry[E_SIFR_AU],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_SIFR_AU], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SIFR_AU]),data.sifr_au.ravno());
gtk_widget_set_name(data.entry[E_SIFR_AU],iceb_u_inttochar(E_SIFR_AU));

data.label_naim[E_SIFR_AU]=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_SIFR_AU]), data.label_naim[E_SIFR_AU],TRUE, TRUE, 1);


sprintf(strsql,"%s",gettext("Группа налогового учёта"));
data.knopka_enter[E_GRUP_NU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_NU]), data.knopka_enter[E_GRUP_NU], FALSE, FALSE, 1);
g_signal_connect(data.knopka_enter[E_GRUP_NU],"clicked",G_CALLBACK(uoskart1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_GRUP_NU],iceb_u_inttochar(E_GRUP_NU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_GRUP_NU],gettext("Выбор шифра"));

data.entry[E_GRUP_NU] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GRUP_NU]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_NU]), data.entry[E_GRUP_NU],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_GRUP_NU], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP_NU]),data.grup_nu.ravno());
gtk_widget_set_name(data.entry[E_GRUP_NU],iceb_u_inttochar(E_GRUP_NU));

data.label_naim[E_GRUP_NU]=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_NU]), data.label_naim[E_GRUP_NU],TRUE, TRUE, 1);



sprintf(strsql,"%s",gettext("Поправочный коэффициент для налогового учёта"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOEFIC_NU]), label,FALSE, FALSE, 1);

data.entry[E_KOEFIC_NU] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOEFIC_NU]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOEFIC_NU]), data.entry[E_KOEFIC_NU],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_KOEFIC_NU], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOEFIC_NU]),data.koefic_nu.ravno());
gtk_widget_set_name(data.entry[E_KOEFIC_NU],iceb_u_inttochar(E_KOEFIC_NU));


sprintf(strsql,"%s",gettext("Поправочный коэффициент для бухгалтерского учёта"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOEFIC_BU]), label,FALSE, FALSE, 1);

data.entry[E_KOEFIC_BU] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOEFIC_BU]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOEFIC_BU]), data.entry[E_KOEFIC_BU],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_KOEFIC_BU], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOEFIC_BU]),data.koefic_bu.ravno());
gtk_widget_set_name(data.entry[E_KOEFIC_BU],iceb_u_inttochar(E_KOEFIC_BU));


sprintf(strsql,"%s",gettext("Государственный номерной знак автотранспортного средства"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_ZNAK]), label,FALSE, FALSE, 1);

data.entry[E_NOMER_ZNAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMER_ZNAK]),39);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_ZNAK]), data.entry[E_NOMER_ZNAK],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_NOMER_ZNAK], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_ZNAK]),data.nomer_znak.ravno());
gtk_widget_set_name(data.entry[E_NOMER_ZNAK],iceb_u_inttochar(E_NOMER_ZNAK));




sprintf(strsql,"%s",gettext("Группа бухгалтерского учёта"));
data.knopka_enter[E_GRUP_BU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_BU]), data.knopka_enter[E_GRUP_BU], FALSE, FALSE, 1);
g_signal_connect(data.knopka_enter[E_GRUP_BU],"clicked",G_CALLBACK(uoskart1_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_GRUP_BU],iceb_u_inttochar(E_GRUP_BU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_GRUP_BU],gettext("Выбор шифра"));

data.entry[E_GRUP_BU] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GRUP_BU]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_BU]), data.entry[E_GRUP_BU],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_GRUP_BU], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP_BU]),data.grup_bu.ravno());
gtk_widget_set_name(data.entry[E_GRUP_BU],iceb_u_inttochar(E_GRUP_BU));

data.label_naim[E_GRUP_BU]=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_BU]), data.label_naim[E_GRUP_BU],TRUE, TRUE, 1);



sprintf(strsql,"%s",gettext("Балансовая стоимость (налоговый учёт)"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_BAL_ST_NU]), label,FALSE, FALSE, 1);

data.entry[E_BAL_ST_NU] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_BAL_ST_NU]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_BAL_ST_NU]), data.entry[E_BAL_ST_NU],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_BAL_ST_NU], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_BAL_ST_NU]),data.bal_st_nu.ravno());
gtk_widget_set_name(data.entry[E_BAL_ST_NU],iceb_u_inttochar(E_BAL_ST_NU));


sprintf(strsql,"%s",gettext("Износ (налоговый учёт)"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_IZ_NU]), label,FALSE, FALSE, 1);

data.entry[E_IZ_NU] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_IZ_NU]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_IZ_NU]), data.entry[E_IZ_NU],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_IZ_NU], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IZ_NU]),data.iz_nu.ravno());
gtk_widget_set_name(data.entry[E_IZ_NU],iceb_u_inttochar(E_IZ_NU));


sprintf(strsql,"%s",gettext("Балансовая стоимость (бухгалтерский учёт)"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_BAL_ST_BU]), label,FALSE, FALSE, 1);

data.entry[E_BAL_ST_BU] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_BAL_ST_BU]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_BAL_ST_BU]), data.entry[E_BAL_ST_BU],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_BAL_ST_BU], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_BAL_ST_BU]),data.bal_st_bu.ravno());
gtk_widget_set_name(data.entry[E_BAL_ST_BU],iceb_u_inttochar(E_BAL_ST_BU));


sprintf(strsql,"%s",gettext("Износ (бухгалтерский учёт)"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_IZ_BU]), label,FALSE, FALSE, 1);

data.entry[E_IZ_BU] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_IZ_BU]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_IZ_BU]), data.entry[E_IZ_BU],TRUE, TRUE, 1);
g_signal_connect(data.entry[E_IZ_BU], "activate",G_CALLBACK(uoskart1_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IZ_BU]),data.iz_bu.ravno());
gtk_widget_set_name(data.entry[E_IZ_BU],iceb_u_inttochar(E_IZ_BU));


label=gtk_label_new(gettext("Состояние объекта"));
gtk_box_pack_start (GTK_BOX (hbox[E_SOST_OB]), label, FALSE, FALSE, 1);

/*Создаём путкт меню для выбора режима расчёта амортизации*/
class iceb_u_spisok sost_ob;
sost_ob.plus(gettext("Амортизация считается для бух. учёта и налогового учёта"));
sost_ob.plus(gettext("Амортизиция не считается для бух. учёта и налогового учёта"));
sost_ob.plus(gettext("Амортизация считается для бух. учёта и не считается для налогового учёта"));
sost_ob.plus(gettext("Амортизация не считается для бух. учёта и считается для налогового учёта"));


iceb_pm_vibor(&sost_ob,&data.opt,data.sost_ob);
gtk_box_pack_start (GTK_BOX (hbox[E_SOST_OB]), data.opt, FALSE, FALSE, 1);




sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(uoskart1_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK2], TRUE, TRUE, 1);


sprintf(strsql,"F3 %s",gettext("Копировать"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Копировать в меню все данные с карточки другого инвентарного номера"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(uoskart1_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK3], TRUE, TRUE, 1);

sprintf(strsql,"F4 %s",gettext("Инв.ном."));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Получить свободный инвентарный номер"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(uoskart1_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK4], TRUE, TRUE, 1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(uoskart1_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(vbox_knop), data.knopka[FK10], TRUE, TRUE, 1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 {
  *bsnu=data.bal_st_nu.ravno_atof();
  *iznu=data.iz_nu.ravno_atof();
  *bsbu=data.bal_st_bu.ravno_atof();
  *izbu=data.iz_bu.ravno_atof();
 }

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uoskart1_e_knopka(GtkWidget *widget,class uoskart1_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("uoskart1_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATAVV:
    iceb_calendar(&data->datavv,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAVV]),data->datavv.ravno());
      
    return;  

  case E_SHETU:
    if(iceb_vibrek(1,"Plansh",&data->shetu,&naim,data->window) == 0)
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHETU]),naim.ravno(20));
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->shetu.ravno());
    return;

  case E_SHET_AMORT:
    if(l_uoshz(1,&kod,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHET_AMORT]),naim.ravno(20));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_AMORT]),kod.ravno());
     }
    return;

  case E_SIFR_AU:
    if(l_uoshau(1,&kod,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SIFR_AU]),naim.ravno(20));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SIFR_AU]),kod.ravno());
     }
    return;

  case E_GRUP_NU:
    if(l_uosgrnu(1,&kod,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_NU]),naim.ravno(20));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP_NU]),kod.ravno());
     }
    return;

  case E_GRUP_BU:
    if(l_uosgrbu(1,&kod,&naim,data->window) == 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_BU]),naim.ravno(20));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP_BU]),kod.ravno());
     }
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uoskart1_v_key_press(GtkWidget *widget,GdkEventKey *event,class uoskart1_data *data)
{

//printf("uoskart1_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);


  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uoskart1_v_knopka(GtkWidget *widget,class uoskart1_data *data)
{
char strsql[512];
int knop=atoi(gtk_widget_get_name(widget));

/*g_print("uoskart1_v_knopka knop=%d\n",knop);*/

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(uoskart1_zk(data) != 0)
     return;

    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

  case FK3: /*Прочитать реквизиты из другой карточки*/
    uoskart1_read(data);
    return;

  case FK4: /*Получить свободный инвентарный номер*/
    data->innom=iceb_invnomer(data->window);
    sprintf(strsql,"%s:%d",gettext("Инвентарный номер"),data->innom);
    printf("uoskatrt1_v_knopka=%s\n",strsql);
    
    gtk_label_set_text(GTK_LABEL(data->label_invnom),strsql);
    return;
    
  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    uoskart1_v_vvod(GtkWidget *widget,class uoskart1_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int enter=atoi(gtk_widget_get_name(widget));

//g_print("uoskart1_v_vvod enter=%d\n",enter);
struct OPSHET rek_shet;
switch (enter)
 {
  case E_SHETU:
    data->shetu.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(iceb_prsh1(data->shetu.ravno(),&rek_shet,data->window) != 0)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHETU]),"");
      return;
     }

    gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHETU]),rek_shet.naim.ravno(20));
    break;

  case E_SHET_AMORT:
    data->shet_amort.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select naik from Uoshz where kod='%s'",data->shet_amort.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Ненайден шифр аморт-отчислений"),data->shet_amort.ravno());
      iceb_menu_soob(strsql,data->window);
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHET_AMORT]),"");
      return;
     }
    sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);    
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHET_AMORT]),strsql);

    break;

  case E_SIFR_AU:
    data->sifr_au.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select naik from Uoshau where kod='%s'",data->sifr_au.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Ненайден шифр аналитического учёта"),data->sifr_au.ravno());
      iceb_menu_soob(strsql,data->window);
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SIFR_AU]),"");
      return;
     }
    sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);    
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_SIFR_AU]),strsql);

    break;

  case E_GRUP_NU:
    data->grup_nu.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select naik from Uosgrup where kod='%s'",data->grup_nu.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Ненайденa группа налогового учёта"),data->grup_nu.ravno());
      iceb_menu_soob(strsql,data->window);
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_NU]),"");
      return;
     }
    sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);    
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_NU]),strsql);

    break;


  case E_GRUP_BU:
    data->grup_bu.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",data->grup_bu.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Ненайденa группа бухгалтерского учёта"),data->grup_bu.ravno());
      iceb_menu_soob(strsql,data->window);
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_BU]),"");
      return;
     }
    sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);    
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_BU]),strsql);

    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/******************************/
/*Чтение реквизитов заданного инвентарного номера*/
/***********************************************/

void uoskart1_read(class uoskart1_data *data)
{

class iceb_u_str innom(""); 
if(uosgetinn(&innom,1,data->window) != 0)
 return;

class iceb_gdite_data gdite;
iceb_gdite(&gdite,1,data->window);

SQL_str row;
class SQLCURSOR cur;

char strsql[512];
sprintf(strsql,"select * from Uosin where innom=%d",innom.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s:%d!",gettext("Ненайден инвентарный номер"),innom.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return;   
 }

iceb_refresh();


gtk_entry_set_text(GTK_ENTRY(data->entry[E_GOD]),row[1]);

gtk_entry_set_text(GTK_ENTRY(data->entry[E_NAIM]),row[2]);

gtk_entry_set_text(GTK_ENTRY(data->entry[E_ZAV_IZ]),row[3]);

gtk_entry_set_text(GTK_ENTRY(data->entry[E_PASPORT]),row[4]);

gtk_entry_set_text(GTK_ENTRY(data->entry[E_MODEL]),row[5]);

gtk_entry_set_text(GTK_ENTRY(data->entry[E_ZAV_NOMER]),row[6]);

gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAVV]),iceb_u_datzap(row[7]));
 
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);


class poiinpdw_data rekin;

if(poiinpdw(innom.ravno_atoi(),mt,gt,&rekin,data->window) != 0)
 return;

class iceb_u_str naim("");
sprintf(strsql,"select nais from Plansh where ns='%s'",rekin.shetu.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim.new_plus(row[0]);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),rekin.shetu.ravno());
gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHETU]),naim.ravno(20));


naim.new_plus("");
sprintf(strsql,"select naik from Uoshz where kod='%s'",rekin.hzt.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim.new_plus(row[0]);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_AMORT]),rekin.hzt.ravno());
gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHET_AMORT]),naim.ravno(20));


naim.new_plus("");
sprintf(strsql,"select naik from Uoshau where kod='%s'",rekin.hau.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim.new_plus(row[0]);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_SIFR_AU]),rekin.hau.ravno());
gtk_label_set_text(GTK_LABEL(data->label_naim[E_SIFR_AU]),naim.ravno(20));

naim.new_plus("");
sprintf(strsql,"select naik from Uosgrup where kod='%s'",rekin.hna.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim.new_plus(row[0]);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP_NU]),rekin.hna.ravno());
gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_NU]),naim.ravno(20));

naim.new_plus("");
sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",rekin.hnaby.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim.new_plus(row[0]);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP_BU]),rekin.hnaby.ravno());
gtk_label_set_text(GTK_LABEL(data->label_naim[E_GRUP_BU]),naim.ravno(20));

sprintf(strsql,"%.10g",rekin.popkf);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOEFIC_NU]),strsql);

sprintf(strsql,"%.10g",rekin.popkfby);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOEFIC_BU]),strsql);

data->sost_ob=rekin.soso;

gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMER_ZNAK]),rekin.nomz.ravno());

/*Узнаём в каком подразделении находится инвентарный номер для того, чтобы потом взять балансовую стоимость*/
int podr=0;
int kodotl=0;
if(poiinw(innom.ravno_atoi(),dt,mt,gt,&podr,&kodotl,data->window) != 0)
 return;

class bsizw_data bal_st;

if(bsizw(innom.ravno_atoi(),podr,dt,mt,gt,&bal_st,NULL,data->window) != 0)
 return;

sprintf(strsql,"%.10g",bal_st.sbs);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_BAL_ST_NU]),strsql);

sprintf(strsql,"%.10g",bal_st.siz);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_IZ_NU]),strsql);

sprintf(strsql,"%.10g",bal_st.sbsby);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_BAL_ST_BU]),strsql);

sprintf(strsql,"%.10g",bal_st.sizby);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_IZ_BU]),strsql);

}
/*********************************/
/*Запись новой карточки */
/*******************************/
/*Если вернули 0-записали*/
int uoskart1_zk(class uoskart1_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;


/*Проверяем дату*/
if(iceb_pbpds(data->data_dok.ravno(),data->window) != 0)
 {
  sprintf(strsql,gettext("Дата %s г. заблокирована!"),data->data_dok.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(data->naim.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введено наименование!"),data->window);
  return(1);
 }

if(data->koefic_nu.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Коэффициент для налогового учёта не может быть равным нолю!"),data->window);
  return(1);
 }

if(data->koefic_bu.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Коэффициент для бух. учёта не может быть равным нолю!"),data->window);
  return(1);
 }

struct OPSHET rek_shet;

if(iceb_prsh1(data->shetu.ravno(),&rek_shet,data->window) != 0)
  return(1);

sprintf(strsql,"select naik from Uoshz where kod='%s'",data->shet_amort.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Ненайден шифр аморт-отчислений"),data->shet_amort.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

sprintf(strsql,"select naik from Uoshau where kod='%s'",data->sifr_au.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Ненайден шифр аналитического учёта"),data->sifr_au.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

sprintf(strsql,"select naik from Uosgrup where kod='%s'",data->grup_nu.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Ненайденa группа налогового учёта"),data->grup_nu.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",data->grup_bu.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Ненайденa группа бухгалтерского учёта"),data->grup_bu.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

class iceb_lock_tables kkk("LOCK TABLE Uosin WRITE,icebuser READ"); /*Блокируем таблицу*/
/*Проверяем есть ли уже такой инвентарный номер*/
sprintf(strsql,"select naim from Uosin where innom=%d",data->innom);
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  sprintf(strsql,"%s\n%d %s",gettext("Такой инвентарный номер уже есть!"),
  data->innom,row[0]);
  iceb_menu_soob(strsql,data->window);
  return(1);   
 }

time_t vrem=time(NULL);
sprintf(strsql,"insert into Uosin values (%d,%d,'%s','%s','%s','%s','%s','%s',%d,%ld)",
data->innom,
data->god.ravno_atoi(),
data->naim.ravno_filtr(),
data->zav_iz.ravno_filtr(),
data->pasport.ravno_filtr(),
data->model.ravno_filtr(),
data->zav_nomer.ravno_filtr(),
data->datavv.ravno_sqldata(),
iceb_getuid(data->window),vrem);

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 return(1);

kkk.unlock();/*Разблокируем таблицу*/

short dd=0,md=0,gd=0;
iceb_u_rsdat(&dd,&md,&gd,data->data_dok.ravno(),1);

sprintf(strsql,"replace into Uosinp values (%d,%d,%d,'%s','%s','%s','%s',%.6g,%d,'%s',%d,%ld,'%s',%.10g)",
data->innom,
md,gd,
data->shetu.ravno_filtr(),
data->shet_amort.ravno_filtr(),
data->sifr_au.ravno_filtr(),
data->grup_nu.ravno_filtr(),
data->koefic_nu.ravno_atof(),
data->sost_ob,
data->nomer_znak.ravno_filtr(),
iceb_getuid(data->window),
vrem,
data->grup_bu.ravno_filtr(),
data->koefic_bu.ravno_atof());

iceb_sql_zapis(strsql,1,0,data->window);

return(0);
}







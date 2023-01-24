//! @file dish_change_form.cpp
//! @authors Алексеев А.В.
//! @note Ответственный: Полевой Д.В.
//! @brief Файл с определниями классов Dish_change_form и Extra_ingredient

#include "dish_change_form.h"
#include "ui_dish_change_form.h"

Dish_change_form::Dish_change_form(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dish_change_form)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./burger.db");
}

Dish_change_form::Dish_change_form(QString d_name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dish_change_form)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./burger.db");
    db.open();
    QSqlQuery query(db);
    QString request = "SELECT dish_id, price, description FROM dishes WHERE dish_name = '" + d_name + "';";
    query.exec(request);
    query.next();
    dish_name = query.value(0).toString();
    base_price = query.value(1).toDouble();
    ui->label->setText(query.value(2).toString());
    query.clear();
    db.close();
    ui->main_label->setText(d_name);
    setWindowTitle(d_name);
    dish_start();
    change_price();
}

Dish_change_form::Dish_change_form(Order ord, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dish_change_form)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/Coursework_3/field_kitchen/burger.db");
    db.open();
    QSqlQuery query(db);
    QString request = "SELECT dish_id, price FROM dishes WHERE dish_name = '" + ord.dish_name + "';";
    query.exec(request);
    query.next();
    dish_name = query.value(0).toString();
    base_price = query.value(1).toDouble();
    query.clear();
    db.close();
    ui->main_label->setText(ord.dish_name);
    dish_start();
    int order_index = 0, all_ingredients_index = 0;
    while(order_index < ord.deleted_ingredients.size())
    {
        if(ord.deleted_ingredients[order_index] == delete_ingredients[all_ingredients_index]->text())
        {
            delete_ingredients[all_ingredients_index]->setChecked(true);
            order_index++;
        }
        all_ingredients_index++;
    }
    order_index = 0;
    all_ingredients_index = 0;
    while(order_index < ord.extra_ingredients.size())
    {
        if(ord.extra_ingredients[order_index].first == extra_ingredients[all_ingredients_index]->name_lable->text())
        {
            extra_ingredients[all_ingredients_index]->spinbox->setValue(ord.extra_ingredients[order_index].second);
            order_index++;
        }
        all_ingredients_index++;
    }
    setWindowTitle(ord.dish_name);
    change_price();
}


Dish_change_form::~Dish_change_form()
{
    delete ui;
}

void Dish_change_form::dish_start(){
    db.open();
    QSqlQuery query(db);
    QString request;
    request = "SELECT ingredients.ingredient_name, ingredients.portion_mass, necessary_ingredients.portion_count FROM necessary_ingredients ";
    request += "JOIN ingredients ON necessary_ingredients.ingredient = ingredients.ingredient_id ";
    request += "WHERE dish = " + dish_name + " UNION ";
    request += "SELECT ingredients.ingredient_name, ingredients.portion_mass, optional_ingredients.portion_count FROM optional_ingredients ";
    request += "JOIN ingredients ON optional_ingredients.ingredient = ingredients.ingredient_id ";
    request += "WHERE dish = " + dish_name + ";";
    query.exec(request);
    ingredient_lay = new QVBoxLayout(ui->scrollAreaWidgetContents_5);
    ingredient_lay->setAlignment(Qt::AlignTop);
    ui->scrollAreaWidgetContents_5->setLayout(ingredient_lay);
    while(query.next())
    {
        ingredients.append(new QLabel(ui->scrollAreaWidgetContents_5));
        int par = query.value(1).toInt() * query.value(2).toInt();
        (*ingredients.back()).setText(QString::number(par) + " гр\t" + query.value(0).toString());
        (*ingredients.back()).setWordWrap(true);
        ingredient_lay->addWidget(ingredients.back());
    }
    query.clear();
    request = "SELECT ingredients.ingredient_name FROM optional_ingredients ";
    request += "JOIN ingredients ON optional_ingredients.ingredient = ingredients.ingredient_id ";
    request += "WHERE dish = " + dish_name + ";";
    query.exec(request);
    optional_lay = new QVBoxLayout(ui->scrollAreaWidgetContents_3);
    optional_lay->setAlignment(Qt::AlignTop);
    ui->scrollAreaWidgetContents_3->setLayout(optional_lay);
    while(query.next())
    {
        delete_ingredients.append(new QCheckBox(ui->scrollAreaWidgetContents_3));
        (*delete_ingredients.back()).setText("Убрать " + (query.value(0).toString()).toLower());
        (*delete_ingredients.back()).setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        optional_lay->addWidget(delete_ingredients.back());
    }
    if (optional_lay->isEmpty())
    {
        QLabel *empty_option_lay = new QLabel(ui->scrollAreaWidgetContents_3);
        empty_option_lay->setText("Все ингредиенты обязательны");
        optional_lay->addWidget(empty_option_lay);
    }
    query.clear();
    request = "SELECT ingredients.ingredient_name, ingredients.portion_price, max_portion FROM extra_ingredients ";
    request += "JOIN ingredients ON ingredients.ingredient_id = extra_ingredients.ingredient ";
    request += "WHERE dish = " + dish_name + ";";
    query.exec(request);
    extra_lay = new QVBoxLayout(ui->scrollAreaWidgetContents_4);
    ui->scrollAreaWidgetContents_4->setLayout(extra_lay);
    while(query.next())
    {
        Extra_ingredient* new_ingredient = new Extra_ingredient("Добавить " + query.value(0).toString().toLower(), query.value(1).toString(),
                                                                query.value(2).toDouble(), ui->scrollAreaWidgetContents_4);
        extra_ingredients.append(new_ingredient);
        extra_lay->addWidget(extra_ingredients.back());
        connect(new_ingredient->spinbox, SIGNAL(valueChanged(int)), this, SLOT(change_price()));
    }
    if (extra_lay->isEmpty())
    {
        QLabel *empty_extra_lay = new QLabel(ui->scrollAreaWidgetContents_4);
        empty_extra_lay->setText("Нельзя добавить новых ингредиентов");
        empty_extra_lay->setWordWrap(true);
        extra_lay->addWidget(empty_extra_lay);
    }
    extra_lay->setAlignment(Qt::AlignTop);
    db.close();
}


void Dish_change_form::on_Dish_change_form_rejected()
{
    db.removeDatabase(db.connectionName());
}


void Dish_change_form::on_select_Button_clicked()
{
    Order ord;
    db.open();
    QSqlQuery query(db);
    QString request = "SELECT dish_name FROM dishes WHERE dish_id = '" + dish_name + "';";
    query.exec(request);
    query.next();
    ord.dish_name = query.value(0).toString();
    query.clear();
    db.close();
    for (QCheckBox*& i:delete_ingredients)
    {
        if (i->isChecked())
        {
            ord.deleted_ingredients.append(i->text());
        }
    }
    for (Extra_ingredient* &i:extra_ingredients)
    {
        if (i->spinbox->value() > 0)
        {
            ord.extra_ingredients.append(qMakePair(i->name_lable->text() ,i->spinbox->value()));
        }
    }
    ord.price = base_price;
    for (Extra_ingredient* i:extra_ingredients)
    {
        ord.price += ((i->price_lable->text().toDouble()) * (i->spinbox->value()));
    };
    emit make_order(ord);
    accept();
}


void Dish_change_form::on_back_Button_clicked()
{
    reject();
}

void Dish_change_form::change_price()
{
    double new_price = base_price;
    for (Extra_ingredient* i:extra_ingredients)
    {
        new_price += ((i->price_lable->text().toDouble()) * (i->spinbox->value()));
    }
    ui->select_Button->setText("Добавить в корзину " + QString::number(new_price) + " руб");
}

Extra_ingredient::Extra_ingredient(QWidget *parent) :
    QWidget(parent)
{
    name_lable = new QLabel(this);
    price_lable = new QLabel(this);
    spinbox = new QSpinBox(this);
    h_lay = new QHBoxLayout(this);
}

Extra_ingredient::Extra_ingredient(QString ing_name, QString ing_price, int max_count ,QWidget *parent):
    QWidget(parent)
{
    name_lable = new QLabel(this);
    name_lable->setText(ing_name);
    name_lable->setWordWrap(true);
    price_lable = new QLabel(this);
    price_lable->setText(ing_price);
    price_lable->setWordWrap(true);
    spinbox = new QSpinBox(this);
    spinbox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    spinbox->setMaximum(max_count);
    h_lay = new QHBoxLayout(this);
    this->setLayout(h_lay);
    h_lay->addWidget(spinbox);
    h_lay->addWidget(name_lable);
    h_lay->addWidget(price_lable);
    h_lay->setAlignment(Qt::AlignTop);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
}

Extra_ingredient::~Extra_ingredient() = default;


void Dish_change_form::on_Dish_change_form_accepted()
{
    db.removeDatabase(db.connectionName());
}


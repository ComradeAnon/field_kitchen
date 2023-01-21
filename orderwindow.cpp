//! @file orderwindow.h
//! @authors Алексеев А.В.
//! @note Ответственный: Полевой Д.В.
//! @brief Файл определяет класс OrderWindow.
#include "orderwindow.h"
#include "./ui_orderwindow.h"
#include "log_in_accaunt.h"

OrderWindow::OrderWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::OrderWindow)
{
    ui->setupUi(this);
    database_connect();
    categories_show();
}

OrderWindow::~OrderWindow()
{
    delete ui;
}

void OrderWindow::database_connect() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./burger.db");
}

void OrderWindow::categories_show() {
    // Изменения в кнопках
    if (!cart.empty())
        ui->cart_finish_Button->setEnabled(true);
    else
        ui->cart_finish_Button->setEnabled(false);
    ui->select_Button->setText("Выбрать");
    ui->cart_finish_Button->setText("Корзина");
    ui->back_Button->setEnabled(false);
    // Подключение бд
    query = new QSqlQuery(db);
    db.open();
    // достаем информацию из бд
    ui->listWidget->clear();
    query->exec("SELECT * FROM categories;");
    while (query->next()) {
        QWidget* category_box = new QWidget(this);
        category_box->setObjectName(query->value(1).toString());
        QHBoxLayout* category_box_h = new QHBoxLayout(category_box);
        QLabel* first_lable = new QLabel(category_box);
        first_lable->setText(query->value(1).toString());
        category_box_h->addWidget(first_lable);
        QListWidgetItem* item_l = new QListWidgetItem(ui->listWidget);
        item_l->setSizeHint(category_box->sizeHint());
        ui->listWidget->addItem(item_l);
        ui->listWidget->setItemWidget(item_l, category_box);
    }
    // Закрытие бд
    query->clear();
    db.close();
    delete query;
}

void OrderWindow::dishes_show() {
    // Изменения в кнопках
    if (!cart.empty())
        ui->cart_finish_Button->setEnabled(true);
    else
        ui->cart_finish_Button->setEnabled(false);
    ui->select_Button->setText("Выбрать");
    ui->cart_finish_Button->setText("Корзина");
    ui->back_Button->setEnabled(true);
    // Подключение бд
    query = new QSqlQuery(db);
    db.open();
    // достаем информацию из бд
    ui->listWidget->clear();
    QString request = "SELECT * FROM categories WHERE category_name = '" + select_category + "';";
    query->exec(request);
    while (query->next())
        select_category = query->value(0).toString();
    request = "SELECT * FROM dishes WHERE category = " + select_category + ";";
    query->clear();
    query->exec(request);
    while (query->next())
    {
        QWidget* dish_box = new QWidget(this);
        dish_box->setObjectName(query->value(1).toString());
        QHBoxLayout* dish_box_h = new QHBoxLayout(dish_box);
        QLabel* first_lable = new QLabel(dish_box), * second_lable = new QLabel(dish_box);
        first_lable->setText(query->value(1).toString());
        second_lable->setText(query->value(3).toString());
        second_lable->setAlignment(Qt::AlignRight);
        dish_box_h->addWidget(first_lable);
        dish_box_h->addWidget(second_lable);
        QListWidgetItem* item_l = new QListWidgetItem(ui->listWidget);
        item_l->setSizeHint(dish_box->sizeHint());
        ui->listWidget->addItem(item_l);
        ui->listWidget->setItemWidget(item_l, dish_box);
    }
    // Закрытие бд
    query->clear();
    db.close();
    delete query;
}

void OrderWindow::cart_show() {
    // Изменения в кнопках
    ui->select_Button->setText("Изменить");
    ui->back_Button->setEnabled(true);
    // Заполнение листа
    ui->listWidget->clear();
    int num = 0;
    for (Order& i : cart)
    {
        QWidget* widget_order = new QWidget(ui->listWidget);
        QListWidgetItem* order_item = new QListWidgetItem(ui->listWidget);
        widget_order->setLayout(new QHBoxLayout(widget_order));
        QVBoxLayout* lables_lay = new QVBoxLayout(), * button_lay = new QVBoxLayout();
        widget_order->layout()->addItem(lables_lay);
        widget_order->layout()->addItem(button_lay);
        button_lay->setAlignment(Qt::AlignBottom);
        QPushButton* delete_button = new QPushButton(widget_order);
        delete_button->setObjectName(QString::number(num));
        delete_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        delete_button->setText("Удалить");
        button_lay->addWidget(delete_button);
        QLabel* label_ing = new QLabel(widget_order);
        label_ing->setStyleSheet(QString("font-size: %1px").arg(14));
        label_ing->setText(i.dish_name);
        lables_lay->addWidget(label_ing);
        for (QString& j : i.deleted_ingredients)
        {
            label_ing = new QLabel(widget_order);
            label_ing->setText("-" + j);
            lables_lay->addWidget(label_ing);
        }
        for (QPair<QString, int>& j : i.extra_ingredients)
        {
            label_ing = new QLabel(widget_order);
            label_ing->setText("-" + j.first + " * " + QString::number(j.second));
            lables_lay->addWidget(label_ing);
        }
        label_ing = new QLabel(widget_order);
        label_ing->setText("Стоимость: " + QString::number(i.price));
        lables_lay->addWidget(label_ing);
        order_item->setSizeHint(widget_order->sizeHint());
        ui->listWidget->addItem(order_item);
        ui->listWidget->setItemWidget(order_item, widget_order);
        num++;
        connect(delete_button, SIGNAL(clicked(bool)), this, SLOT(del_order_cart()));
    }
    double sum_price = 0;
    for (Order& i : cart)
    {
        sum_price += i.price;
    }
    ui->cart_finish_Button->setText("Оплатить " + QString::number(sum_price) + " руб");
    // Защита от пустой корзины
    if (cart.empty())
    {
        ui->back_Button->click();
        ui->cart_finish_Button->setEnabled(false);
    }
}

void OrderWindow::history_show() {
    // Изменения в кнопках
    ui->history_Button->setEnabled(false);
    ui->select_Button->setEnabled(true);
    ui->back_Button->setEnabled(true);
    // Подключение бд
    query = new QSqlQuery(db);
    db.open();
    // Получение данных из бд
    ui->listWidget->clear();
    query->exec("SELECT * FROM orders WHERE customer = " + client + ";");
    while (query->next()) {
        QWidget* order_box = new QWidget(this);
        order_box->setObjectName(query->value(0).toString());
        QHBoxLayout* order_box_h = new QHBoxLayout(order_box);
        QLabel* first_lable = new QLabel(order_box), * second_lable = new QLabel(order_box);
        int order_id_name = query->value(0).toInt();
        QString order_name = QChar(65 + (order_id_name / 1000)) + '-' + QString::number(order_id_name % 1000);
        first_lable->setText(order_name);
        second_lable->setText(query->value(2).toString());
        second_lable->setAlignment(Qt::AlignRight);
        order_box_h->addWidget(first_lable);
        order_box_h->addWidget(second_lable);
        QListWidgetItem* item_l = new QListWidgetItem(ui->listWidget);
        item_l->setSizeHint(order_box->sizeHint());
        ui->listWidget->addItem(item_l);
        ui->listWidget->setItemWidget(item_l, order_box);
    }
    // Закрытие бд
    query->clear();
    db.close();
    delete query;
}

void OrderWindow::history_dishes_show() {
    // Изменения в кнопках
    ui->select_Button->setEnabled(false);
    // Подключение бд
    query = new QSqlQuery(db);
    db.open();
    // Получение данных из бд
    ui->listWidget->clear();
    query->exec("SELECT * FROM orders_dish WHERE order_id = " + history_order + ";");
    while (query->next()) {
        QStringList str_list;
        QWidget* order_box = new QWidget(this);
        order_box->setObjectName(query->value(0).toString());
        QHBoxLayout* order_box_h = new QHBoxLayout(order_box);
        QLabel* first_lable = new QLabel(order_box), * second_lable = new QLabel(order_box);
        first_lable->setText(query->value(1).toString());
        QVBoxLayout* order_box_v_1 = new QVBoxLayout();
        order_box_v_1->addWidget(first_lable);
        QVBoxLayout* order_box_v_2 = new QVBoxLayout();
        str_list = query->value(2).toString().split("\n");
        for (QString& i : str_list)
        {
            QLabel* new_label = new QLabel(order_box);
            new_label->setText(i);
            order_box_v_2->addWidget(new_label);
        }
        str_list = query->value(3).toString().split("\n");
        QVBoxLayout* order_box_v_3 = new QVBoxLayout();
        for (QString& i : str_list)
        {
            QLabel* new_label = new QLabel(order_box);
            new_label->setText(i);
            order_box_v_3->addWidget(new_label);
        }
        second_lable->setText(query->value(4).toString() + " руб.");
        second_lable->setAlignment(Qt::AlignRight);
        QVBoxLayout* order_box_v_4 = new QVBoxLayout();
        order_box_v_4->addWidget(second_lable);
        order_box_v_1->setAlignment(Qt::AlignTop);
        order_box_v_2->setAlignment(Qt::AlignTop);
        order_box_v_3->setAlignment(Qt::AlignTop);
        order_box_v_4->setAlignment(Qt::AlignTop);
        order_box_h->addLayout(order_box_v_1);
        order_box_h->addLayout(order_box_v_2);
        order_box_h->addLayout(order_box_v_3);
        order_box_h->addLayout(order_box_v_4);
        QListWidgetItem* item_l = new QListWidgetItem(ui->listWidget);
        item_l->setSizeHint(order_box_h->sizeHint());
        ui->listWidget->addItem(item_l);
        ui->listWidget->setItemWidget(item_l, order_box);
    }
    // Закрытие бд
    query->clear();
    db.close();
    delete query;
}

void OrderWindow::cart_add(Order ord) {
    cart.append(ord);
    select_category.clear();
}

void OrderWindow::cart_update(Order const& new_order) {
    cart[ui->listWidget->currentRow()] = new_order;
}

void OrderWindow::del_order_cart() {
    QPushButton* button = (QPushButton*)sender();
    cart.remove(button->objectName().toInt());
    cart_show();
}

void OrderWindow::successful_login(QString client_id) {
    database_connect();
    client = client_id;
    //Изменение кнопок
    ui->account_Button->setText("Выйти из аккаунта");
    ui->history_Button->setEnabled(true);
    // Подключение бд
    db.open();
    query = new QSqlQuery(db);
    query->exec("SELECT login FROM clients WHERE client_id = " + client + ";");
    query->next();
    ui->client_label->setText(query->value(0).toString());
    ui->account_Button->setText("Выйти из аккаунта");
    ui->history_Button->setEnabled(true);
    //Закрытие бд
    query->clear();
    delete query;
    db.close();
}

void OrderWindow::on_listWidget_doubleClicked(const QModelIndex& index)
{
    if (ui->select_Button->isEnabled())
        ui->select_Button->click();
}

void OrderWindow::on_back_Button_clicked()
{
    if (is_history)
    {
        if(!history_order.isEmpty())
        {
            history_order.clear();
            history_show();
        }
        else
        {
            is_history = false;
            ui->history_Button->setEnabled(true);
            if (select_category.isEmpty())
            {
                categories_show();
            }
            else
            {
                dishes_show();
            }
        }
    }
    else
    {
        if (is_cart)
        {
            is_cart = false;
            if (select_category.isEmpty())
            {
                categories_show();
            }
            else
            {
                dishes_show();
            }
        }
        else{
            select_category.clear();
            categories_show();
        }
    }
}

void OrderWindow::on_select_Button_clicked()
{
    if (ui->listWidget->currentItem()) {
        if (is_history)
        {
            history_order = ui->listWidget->itemWidget((ui->listWidget->currentItem()))->objectName();
            history_dishes_show();
        }
        else
        {
            if(is_cart)
            {
                db.removeDatabase(db.connectionName());
                dish_select = new Dish_change_form(cart[ui->listWidget->currentRow()], this);
                connect(dish_select, SIGNAL(make_order(Order)), this, SLOT(cart_update(Order)));
                dish_select->exec();
                if(!db.isValid())
                {
                    database_connect();
                }
                cart_show();
            }
            else
            {
                if(select_category.isEmpty())
                {
                    select_category = ui->listWidget->itemWidget((ui->listWidget->currentItem()))->objectName();
                    dishes_show();
                }
                else
                {
                    db.removeDatabase(db.connectionName());
                    dish_select = new Dish_change_form(ui->listWidget->itemWidget((ui->listWidget->currentItem()))->objectName(), this);
                    connect(dish_select, SIGNAL(make_order(Order)), this, SLOT(cart_add(Order)));
                    if(dish_select->exec()){
                        database_connect();
                        categories_show();
                    }
                    else{
                        database_connect();
                        dishes_show();
                    }
                }
            }
        }
    }
}

void OrderWindow::on_history_Button_clicked()
{
    is_history = true;
    is_cart = false;
    history_show();
}

void OrderWindow::on_account_Button_clicked()
{
    if (client == "NULL")
    {
        db.removeDatabase(db.connectionName());
        Log_in_accaunt log_in(this);
        log_in.setFixedSize(350, 150);
        connect(&log_in, SIGNAL(successful_login(QString)), this, SLOT(successful_login(QString)));
        log_in.exec();
        if (!db.isValid())
        {
            database_connect();
        }
    }
    else
    {
        client = "NULL";
        while (is_history)
            ui->back_Button->click();
        ui->client_label->setText("Гость");
        ui->account_Button->setText("Войти в аккаунт");
        ui->history_Button->setEnabled(false);
    }
}

void OrderWindow::on_cart_finish_Button_clicked()
{
    if(is_cart){
        // Подключение бд
        query = new QSqlQuery(db);
        db.open();
        // Занесение заказа в бд
        QString request = "INSERT INTO orders (customer) VALUES (" + client + ");";
        query->exec(request);
        query->clear();
        request = "SELECT order_id FROM orders ORDER BY order_id DESC LIMIT 1;";
        query->exec(request);
        query->next();
        QString new_id = query->value(0).toString();
        int order_id_name = new_id.toInt() % 26000;
        query->clear();
        QString order_name = QChar(65 + (order_id_name / 1000)) + '-' + QString::number(order_id_name % 1000);
        QString order_delete_ingredients = "", order_extra_ingredients = "";
        for (Order& i : cart)
        {
            for (QString& j : i.deleted_ingredients)
            {
                order_delete_ingredients += j + "\n";
            }
            for (QPair<QString, int>& j : i.extra_ingredients)
            {
                order_extra_ingredients += j.first + " * " + QString::number(j.second) + "\n";
            }
            query->prepare("INSERT INTO orders_dish (order_id, dish_name, delete_ingredients, extra_ingredients, cost) "
                           "VALUES (:order_id, :dish_name, :delete_ingredients, :extra_ingredients, :cost);");
            query->bindValue(":order_id", new_id);
            query->bindValue(":dish_name", i.dish_name);
            query->bindValue(":delete_ingredients", order_delete_ingredients);
            query->bindValue(":extra_ingredients", order_extra_ingredients);
            query->bindValue(":cost", i.price);
            query->exec();
            order_delete_ingredients = "";
            order_extra_ingredients = "";
        }
        QMessageBox exit_box;
        exit_box.setText("Номер вашего заказа: " + order_name + " \nМожете забрать заказ через 10 минут" +
                         " \nХотите продолжить работать в приложении?");
        QPushButton* continue_button = exit_box.addButton(tr("Продолжить"), QMessageBox::AcceptRole);
        QPushButton* exit_button = exit_box.addButton(tr("Выйти"), QMessageBox::RejectRole);
        int result = exit_box.exec();
        if (result == 0)
        {
            // Закрытие бд
            query->clear();
            db.close();
            delete query;
            is_cart = false;
            select_category.clear();
            cart.clear();
            categories_show();
        }
        if (result == 1)
        {
            // Закрытие бд
            query->clear();
            db.close();
            delete query;
            close();
        }
    }
    else
    {
        is_history = false;
        history_order.clear();
        is_cart = true;
        cart_show();
    }
}

//! @file orderwindow.h
//! @authors Алексеев А.В.
//! @note Ответственный: Полевой Д.В.
//! @brief Файл объявляет класс OrderWindow.

#ifndef ORDERWINDOW_H
#define ORDERWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include "dish_change_form.h"
#include <QFont>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class OrderWindow; }
QT_END_NAMESPACE

/// @brief Класс, главную форму в которой происходит
//! заказ блюд, просмотр корзины, просмотр истории заказов с аккаунта.
class OrderWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// @brief Конструктор по умолчанию.
    /// @param parent указатель на родительский виджет.
    OrderWindow(QWidget *parent = nullptr);
    /// @brief Деструктор.
    ~OrderWindow();

private:
    /// @brief Функция показывающая экран выбора категории.
    void categories_show();
    /// @brief Функция показывающая экран выбора блюда.
    void dishes_show();
    /// @brief Функция показывающая экран корзины.
    void cart_show();
    /// @brief Функция показывающая экран истории заказов.
    void history_show();
    /// @brief Функция показывающая экран блюд в уже сделаном заказе.
    void history_dishes_show();

    /// @brief Функция подключения к бд.
    void database_connect();

private slots:
    // Обработка нажатий на кнопки
    /// @brief Слот обработки сигнала нажатия на кнопку выбора/изменения.
    void on_select_Button_clicked();
    /// @brief Слот обработки сигнала нажатия на кнопку назад.
    void on_back_Button_clicked();
    /// @brief Слот обработки сигнала нажатия на кнопку показать историю.
    void on_history_Button_clicked();
    /// @brief Слот обработки сигнала нажатия на кнопку зайти в аккаунт/ выйти из аккаунта.
    void on_account_Button_clicked();
    /// @brief Слот обработки сигнала нажатия на кнопку показать корзину/ оплатить заказ.
    void on_cart_finish_Button_clicked();

    // Обработка сигналов из сторонних форм
    /// @brief Слот обработки сигнала изменения ингредиентов в заказе из корзины.
    void cart_update(Order const& new_order);
    /// @brief Слот обработки сигнала добавления в корзину.
    void cart_add(Order ord);
    /// @brief Слот обработки сигнала удаления из корзины.
    void del_order_cart();
    /// @brief Слот обработки сигнала успешного входа в аккаунт.
    void successful_login(QString client_id);

    /// @brief Слот обработки сигнала двойного кнажатия на элемент listWidget,
    /// синоним нажатия на кнопку выбора/изменения.
    void on_listWidget_doubleClicked(const QModelIndex &index);

private:
    /// @brief Указатель на пользовательский интерфейс.
    Ui::OrderWindow *ui;
    /// @brief База данных от куда берутся данные.
    QSqlDatabase db;
    /// @brief Указатель на очередь.
    QSqlQuery* query;

    /// @brief Флаг показывающий открыта корзина или нет.
    bool is_cart = false;
    /// @brief Флаг показывающий открыта история или нет.
    bool is_history = false;
    /// @brief Переменная показывающая id выбранного заказа в истории.
    QString history_order = "";
    /// @brief Переменная показывающая id выбранной категории.
    QString select_category = "";

    /// @brief Указатель на форму изменения конфигурации блюда.
    Dish_change_form *dish_select;
    /// @brief Корзина.
    QList<Order> cart;
    /// @brief id клиента, если клиент не вошел в аккаунт или вышел из него, то NULL.
    QString client = "NULL";
};
#endif // ORDERWINDOW_H

//! @file dish_change_form.h
//! @authors Алексеев А.В.
//! @note Ответственный: Полевой Д.В.
//! @brief Файл с объявлениями классов Dish_change_form и Extra_ingredient и структурой Order

#ifndef DISH_CHANGE_FORM_H
#define DISH_CHANGE_FORM_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QSqlError>
#include <QHBoxLayout>
#include <QString>
#include <QList>

namespace Ui {
class Dish_change_form;
class Extra_ingredient;
}

/// @brief Структура, для хранения информации о заказанном блюде.
struct Order {
    /// @brief Имя блюда в заказе.
    QString dish_name;
    /// @brief Список удаленных ингредиентов.
    QList<QString> deleted_ingredients;
    /// @brief Список дополнительных ингредиентов ингредиентов.
    QList<QPair<QString, int>> extra_ingredients;
    double price;
    /// @brief Конструктор по умолчанию.
    Order() = default;
    /// @brief Деструктор по умолчанию.
    ~Order() = default;
};

/// @brief Класс виджета для получения информации о дополнительных ингредиентах.
class Extra_ingredient : public QWidget
{
    Q_OBJECT
public:
    /// @brief Конструктор по умолчанию.
    /// @param parent Указатель на родительский виджет.
    explicit Extra_ingredient(QWidget *parent = nullptr);
    /// @brief Конструктор по входным данным.
    /// @param ing_name Название ингредиента.
    /// @param ing_price Цена порции ингредиента.
    /// @param max_count Максимальное число порций ингредиента доступное для этого блюда.
    /// @param parent Указатель на родительский виджет.
    Extra_ingredient(QString ing_name, QString ing_price, int max_count ,QWidget *parent = nullptr);
    /// @brief Деструктор.
    ~Extra_ingredient();
    /// @brief Указатель на надпись с названием.
    QLabel* name_lable;
    /// @brief Указатель на надпись с ценой порции.
    QLabel* price_lable;
    /// @brief Указатель на Spinbox с помощью которого задается
    /// количество заказанных порций ингредиента.
    QSpinBox* spinbox;
    /// @brief Указатель на слой виджета.
    QHBoxLayout* h_lay;
};

/// @brief Класс, реализующий диалоговое окно, для изменения состава блюда
class Dish_change_form : public QDialog
{
    Q_OBJECT

public:
    /// @brief Конструктор по умолчанию.
    /// @param parent указатель на родительский виджет.
    explicit Dish_change_form(QWidget *parent = nullptr);
    /// @brief Конструктор по названию блюда.
    /// @param d_name Название выбраного блюда.
    Dish_change_form(QString d_name, QWidget *parent = nullptr);
    /// @brief Конструктор по готовому заказу.
    /// Используется для редактирования сделанных заказов из корзины
    /// @param ord Struct Order по данным из которой формируется форма.
    Dish_change_form(Order ord, QWidget *parent = nullptr);
    /// @brief Деструктор.
    ~Dish_change_form();


private slots:
    //Нажатия на кнопки
    /// @brief Слот обработки сигнала нажатия на кнопку выбора.
    void on_select_Button_clicked();
    /// @brief Слот обработки сигнала нажатия на кнопку назад.
    void on_back_Button_clicked();
    // Закрытия окна
    /// @brief Слот обработки сигнала успешного завершения работы окна.
    void on_Dish_change_form_accepted();
    /// @brief Слот обработки сигнала выхода из окна.
    void on_Dish_change_form_rejected();
    //Изменение цены
    /// @brief Выставляет правильную цену на кнопку успешного завершения работы формы.
    void change_price();


private:
    /// @brief Указатель на пользовательский интерфейс.
    Ui::Dish_change_form *ui;

    /// @brief Цена блюда по умолчанию.
    double base_price;
    /// @brief Название блюда.
    QString dish_name;
    /// @brief База данных от куда берутся данные.
    QSqlDatabase db;
    /// @brief Список надписей ингредиентов по умолчанию(обязательных и опциональных).
    QList<QLabel*> ingredients;
    /// @brief Список CheckBox показывающих опциональные игредиенты(которые можно удалить).
    QList<QCheckBox*> delete_ingredients;
    /// @brief Список Extra_ingredient какие дополнительные игредиенты, сколько их порций
    /// и по какой цене можно добавить в блюдо.
    QList<Extra_ingredient*> extra_ingredients;
    /// @brief Слой CheckBox опциональных ингредиентов.
    QVBoxLayout *optional_lay;
    /// @brief Слой Label ингредиентов по умолчанию.
    QVBoxLayout *ingredient_lay;
    /// @brief Слой Extra_ingredient дополнительных игредиентов.
    QVBoxLayout *extra_lay;
    /// @brief Функция использующаяся в конструкторе для заполнения
    /// получения данных из базы данных и заполнения формы виджетами.
    void dish_start();

 signals:
    /// @brief Сигнал испускаемый при успешном завершении диалогового окна
    /// @param ord Заказ передающийся в главную форму.
    void make_order(Order ord);
};


#endif // DISH_CHANGE_FORM_H

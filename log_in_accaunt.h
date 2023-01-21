//! @file log_in_accaunt.h
//! @authors Алексеев А.В.
//! @note Ответственный: Полевой Д.В.
//! @brief Файл с объявлением класса Log_in_accaunt

#ifndef LOG_IN_ACCAUNT_H
#define LOG_IN_ACCAUNT_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QKeyEvent>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

namespace Ui {
class Log_in_accaunt;
}

/// @brief Класс диалогового окна для входа в аккаунт.
class Log_in_accaunt : public QDialog
{
    Q_OBJECT

public:
    /// @brief Конструктор по умолчанию.
    /// @param parent указатель на родительский виджет.
    explicit Log_in_accaunt(QWidget *parent = nullptr);
    /// @brief Деструктор.
    ~Log_in_accaunt();

private slots:
    /// @brief Слот обработки сигнала нажатия на кнопку входа в аккаунт/ регистрации нового.
    void on_Accept_Button_clicked();

    /// @brief Слот обработки сигнала нажатия на кнопку создания нового аккаунта.
    void on_Log_in_accaunt_rejected();

    /// @brief Слот обработки сигнала нажатия на кнопку назад.
    void on_back_Button_clicked();

    /// @brief Слот обработки сигнала нажатия на кнопку создания нового аккаунта.
    void on_registration_Button_clicked();

private:
    /// @brief Указатель на пользовательский интерфейс.
    Ui::Log_in_accaunt *ui;
    /// @brief База данных от куда берутся данные.
    QSqlDatabase db;
    /// @brief Указатель на очередь.
    QSqlQuery* query;
    /// @brief Флаг состояния регистрации.
    bool is_registration = false;

 protected:
    /// @brief Обработка нажатия на клавиши во время выполнения формы.
    void keyPressEvent(QKeyEvent *event);

signals:
    /// @brief Сигнал успешной регистрации.
    /// @param client_id id аккаунта с которого осуществлен вход.
    void successful_login(QString client_id);
};

#endif // LOG_IN_ACCAUNT_H

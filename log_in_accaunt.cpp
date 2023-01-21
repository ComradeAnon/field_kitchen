//! @file log_in_accaunt.cpp
//! @authors Алексеев А.В.
//! @note Ответственный: Полевой Д.В.
//! @brief Файл с определением класса Log_in_accaunt

#include "log_in_accaunt.h"
#include "ui_log_in_accaunt.h"

Log_in_accaunt::Log_in_accaunt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Log_in_accaunt)
{
    ui->setupUi(this);
    QRegularExpression reg_ex("^[A-Za-z0-9_]{0,30}$");
    ui->lineEdit_login->setValidator(new QRegularExpressionValidator(reg_ex, this));
    ui->lineEdit_password->setValidator(new QRegularExpressionValidator(reg_ex, this));
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./burger.db");
    query = new QSqlQuery(db);
    ui->lineEdit_login->setFocus();
}

Log_in_accaunt::~Log_in_accaunt()
{
    delete query;
    delete ui;
}

void Log_in_accaunt::on_Accept_Button_clicked()
{
    if (is_registration)
    {
        if ((ui->lineEdit_login->text() == "") || (ui->lineEdit_password->text() == ""))
        {
            ui->help_label->setText("Нельзя оставлять пустым логин и/или пароль");
        }
        else
        {
            if ((ui->lineEdit_login->text() == "NULL") || (ui->lineEdit_password->text() == "NULL"))
            {
                ui->help_label->setText("Недопустимый логин и/или пароль");
            }
            else
            {
                db.open();
                QString login = ui->lineEdit_login->text(), password = ui->lineEdit_password->text();
                QString request = "SELECT client_id FROM clients WHERE login = '" + login + "';";
                query->exec(request);
                if (query->next())
                {
                    db.close();
                    ui->help_label->setText("Данный логин уже существует");
                }
                else
                {
                    query->prepare("INSERT INTO clients (login , password) VALUES (:login, :password);");
                    query->bindValue(":login", login);
                    query->bindValue(":password", password);
                    query->exec();
                    QString client_id = query->lastInsertId().toString();
                    query->clear();
                    db.close();
                    db.removeDatabase(db.connectionName());
                    emit successful_login(client_id);
                    accept();
                }
            }
        }
    }
    else
    {
        if ((ui->lineEdit_login->text() == "") || (ui->lineEdit_password->text() == ""))
        {
            ui->help_label->setText("Неверный логин и/или пароль");
        }
        else
        {
            db.open();
            QString login = ui->lineEdit_login->text(), password = ui->lineEdit_password->text();
            QString request = "SELECT client_id FROM clients WHERE login = '" + login +
                    "' AND password = '" + password + "';";
            query->exec(request);
            if (query->next())
            {
                QString client_id = query->value(0).toString();
                db.close();
                db.removeDatabase(db.connectionName());
                emit successful_login(client_id);
                accept();
            }
            else
            {
                db.close();
                ui->help_label->setText("Неверный логин и/или пароль");
            }
        }
    }
}

void Log_in_accaunt::keyPressEvent(QKeyEvent *event){
    if ((event->key() == 16777220) || (event->key() == Qt::Key_Enter))
    {
        ui->Accept_Button->click();
    }
    if (event->key() == Qt::Key_Escape)
    {
        ui->back_Button->click();
    }
}


void Log_in_accaunt::on_Log_in_accaunt_rejected()
{
    db.removeDatabase(db.connectionName());
}


void Log_in_accaunt::on_back_Button_clicked()
{
    if (is_registration)
    {
        is_registration = false;
        ui->Accept_Button->setText("Войти");
        ui->registration_Button->setEnabled(true);
        ui->help_label->setText("");
        ui->lineEdit_login->clear();
        ui->lineEdit_password->clear();
    }
    else
        reject();
}


void Log_in_accaunt::on_registration_Button_clicked()
{
    is_registration = true;
    ui->Accept_Button->setText("Создать аккаунт");
    ui->registration_Button->setEnabled(false);
    ui->help_label->setText("");
    ui->lineEdit_login->clear();
    ui->lineEdit_password->clear();
}


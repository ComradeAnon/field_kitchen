//! @file main.cpp
//! @authors Алексеев А.В.
//! @note Ответственный: Полевой Д.В.
//! @brief Файл с инициализацией приложения

#include "orderwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OrderWindow w;
    w.show();
    return a.exec();
}

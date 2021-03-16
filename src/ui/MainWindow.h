#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <qstandarditemmodel.h>
#include "../capturekey/maincontroller.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = Q_NULLPTR);

private slots:
    void alwaysOnTop(bool enabled);
    void startCaptureOCR(bool enable);
    void startCaptureText(bool enable);

public slots:
    void search();

private:

    MainController* MainControl;
    QPushButton* OCRBtn;
    QPushButton* textBtn;
    Ui::MainWindow ui;
    QLineEdit* textbox;
    QTableView* table;
    QStandardItemModel dictmodel;
};

#ifndef ACTIVEINFOWINDOW_H
#define ACTIVEINFOWINDOW_H

#include <QDialog>
#include <vector>
#include <utility>
#include <QStandardItemModel>
#include "stutimeinfo.h"
using namespace std;

namespace Ui {
class ActiveInfoWindow;
}

class ActiveInfoWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ActiveInfoWindow(QWidget *parent = nullptr, vector<StuTimeInfo> TimeInfo = vector<StuTimeInfo>());
    ~ActiveInfoWindow();

private slots:
    void on_btnOK_clicked();

private:
    Ui::ActiveInfoWindow *ui;
    QStandardItemModel* model;
};

#endif // ACTIVEINFOWINDOW_H

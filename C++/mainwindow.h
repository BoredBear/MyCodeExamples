#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbconnector.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void loadPlaneTypes();
    void updateInfoInTable(QString newInfo);
    void connectToDB(QString);
    ~MainWindow();

private slots:
    void on_bt_searchFlight_clicked();

    void on_bt_updateFlights_clicked();

    void on_dbConnectClicked();

    void on_dbDisconnectClicked();

    void on_bt_addNewFlight_clicked();

    void on_cb_searchDate_clicked();

    void on_cb_searchTime_clicked();

    void on_tv_flightsInfoTable_clicked(const QModelIndex &index);

    void on_bt_deleteFlight_clicked();

    void on_tv_flightsInfoTable_doubleClicked(const QModelIndex &index);

    void on_bt_export_clicked();

    void on_bt_import_clicked();

private:
    DBConnector *dbConnector;
    Ui::MainWindow *ui;

    int countNotNullFields();

    QModelIndex itemIndex;
    QString currentFlightId;
};

#endif // MAINWINDOW_H

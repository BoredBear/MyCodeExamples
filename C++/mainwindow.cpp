#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "passwidget.h"
#include "changefieldwidget.h"
#include <qfile.h>
#include <qtextstream.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->connectToDB, SIGNAL(triggered(bool)), this, SLOT(on_dbConnectClicked()));
    connect(ui->disconnectFromBD, SIGNAL(triggered(bool)), this, SLOT(on_dbDisconnectClicked()));
    ui->de_dateDeparture->setEnabled(false);
    ui->te_timeDeparture->setEnabled(false);
    ui->de_dateDeparture->setDate(QDate::currentDate());
    ui->te_timeDeparture->setTime(QTime::currentTime());
    dbConnector = new DBConnector();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectToDB(QString pass)
{
    if(dbConnector->initDB(pass))
    {
        ui->statusBar->showMessage(QString::fromUtf8("Соединение успешно установлено"));
    } else
    {
        ui->statusBar->showMessage(QString::fromUtf8("Неверный пароль"));
    }
}

void MainWindow::on_dbConnectClicked()
{
    passWidget *wdg = new passWidget();
    wdg->setMainWidget(this);
    wdg->show();
    this->hide();
}

void MainWindow::on_dbDisconnectClicked()
{
    dbConnector->disconnectFromDB();
    ui->statusBar->showMessage(QString::fromUtf8("Отключено от базы данных"));
}

void MainWindow::loadPlaneTypes()
{
    if(dbConnector->isOpen() && !dbConnector->getPlaneTypes(ui->cb_planeType))
    {
        ui->statusBar->showMessage(QString::fromUtf8("Ошибка загрузки типов самолётов"));
    }
}

void MainWindow::on_bt_searchFlight_clicked()
{
    if(ui->le_flightNumber->text().length() > 0)
    {
        if(dbConnector->findFlights(ui->le_flightNumber->text(), "", "", "", "", "", "", "",
                                    ui->tv_flightsInfoTable,
                                    dbConnector->FindMethod::ByFlightNumber))
        {
            ui->statusBar->showMessage(QString::fromUtf8("Совпадения найдены!"));
        }
        else
            ui->statusBar->showMessage((QString::fromUtf8("Ничего не найдено!")));
    }
    else
    {
        if(countNotNullFields() > 0)
        {
            QString date;
            QString time;
            if(!ui->cb_searchDate->isChecked())
                date = QString::fromLatin1("");
            else
                date = ui->de_dateDeparture->text();
            if(!ui->cb_searchTime->isChecked())
                time = QString::fromLatin1("");
            else
                time = ui->te_timeDeparture->text();

            if(dbConnector->findFlights("",
                                        ui->le_planeNumber->text(),
                                        ui->cb_planeType->currentText(),
                                        ui->le_airportDeparture->text(),
                                        ui->le_airportArrival->text(),
                                        date,
                                        time,
                                        ui->cb_status->currentText(),
                                        ui->tv_flightsInfoTable,
                                        dbConnector->FindMethod::ByRest))
            {
                ui->statusBar->showMessage(QString::fromUtf8("Совпадения найдены!"));
            }
            else
            {
                ui->statusBar->showMessage((QString::fromUtf8("Ничего не найдено!")));
            }
        }
    }
}

void MainWindow::on_bt_updateFlights_clicked()
{
    if(dbConnector->getFlightsList(ui->tv_flightsInfoTable))
        ui->statusBar->showMessage(QString::fromUtf8("Список рейсов успешно загружен"));
    else
        ui->statusBar->showMessage(QString::fromUtf8("Вы не подключены к БД"));
}

int MainWindow::countNotNullFields()
{
    int count = 0;
    ui->le_flightNumber->text().length() > 0 ? count++ : count;
    ui->le_planeNumber->text().length() > 0 ? count++ : count;
    ui->le_airportArrival->text().length() > 0 ? count++ : count;
    ui->le_airportDeparture->text().length() > 0 ? count++ : count;
    ui->cb_planeType->currentIndex() > 0 ? count++ : count;
    ui->te_timeDeparture->text().length() > 0 ? count++ : count;
    ui->de_dateDeparture->text().length() > 0 ? count++ : count;
    ui->cb_status->currentIndex() > 0 ? count++ : count;
    return count;
}

void MainWindow::on_bt_addNewFlight_clicked()
{
    if(countNotNullFields() == 8 )
    {
        if(dbConnector->createNewFlight(ui->le_flightNumber->text(),
                                        ui->le_planeNumber->text(),
                                        ui->cb_planeType->currentText(),
                                        ui->le_airportDeparture->text(),
                                        ui->le_airportArrival->text(),
                                        ui->de_dateDeparture->text(),
                                        ui->te_timeDeparture->text(),
                                        ui->cb_status->currentText()))
        {
            ui->statusBar->showMessage(QString::fromUtf8("Успешно добавлено"));
            on_bt_updateFlights_clicked();
        }
        else
            ui->statusBar->showMessage(QString::fromUtf8("Ошибка при добавлении в БД"));
    } else
    {
        ui->statusBar->showMessage(QString::fromUtf8("Не все поля заполнены"));
    }
}

void MainWindow::on_cb_searchDate_clicked()
{
    if(ui->cb_searchDate->isChecked())
    {
        ui->de_dateDeparture->setEnabled(true);

    }
    else
    {
        ui->de_dateDeparture->setEnabled(false);
    }
}

void MainWindow::on_cb_searchTime_clicked()
{
    if(ui->cb_searchTime->isChecked())
    {
        ui->te_timeDeparture->setEnabled(true);
    }
    else
    {
        ui->te_timeDeparture->setEnabled(false);
    }
}

void MainWindow::on_tv_flightsInfoTable_clicked(const QModelIndex &index)
{
    currentFlightId = ui->tv_flightsInfoTable->model()->index(index.row(), 0).data().toString();
}

void MainWindow::on_bt_deleteFlight_clicked()
{
    if(!(currentFlightId.isNull() || currentFlightId.isEmpty()))
    {
        if(dbConnector->deleteCurrentFlight(currentFlightId))
        {
            ui->statusBar->showMessage(QString::fromUtf8("Полёт успешно удалён из БД!"));
            on_bt_updateFlights_clicked();
        }
        else
        {
            ui->statusBar->showMessage(QString::fromUtf8("Ошибка при удалении полёта из БД!"));
        }
    } else
    {
        ui->statusBar->showMessage(QString::fromUtf8("Не выбрано поле для удаления!"));
    }
}

void MainWindow::on_tv_flightsInfoTable_doubleClicked(const QModelIndex &index)
{
    if(!index.column() == 0)
    {
        itemIndex = index;
        changeFieldWidget *wdg = new changeFieldWidget();
        wdg->setMainWidget(this);

        if(index.column() == 2)
        {
            wdg->setComboBoxField(ui->cb_planeType);
            wdg->setCurrentType(1);
        } else if(index.column() == 5)
        {
            wdg->setCurrentType(2);
        }
        else if(index.column() == 6)
        {
            wdg->setCurrentType(3);
        } else if(index.column() == 7)
        {
            wdg->setCurrentType(4);
        }
        else
        {
            wdg->setCurrentType(0);
        }
        wdg->show();
        this->hide();
    } else
    {
        ui->statusBar->showMessage(QString::fromUtf8("Ошибка! Нельзя изменить ключевое значение!"));
    }
}

void MainWindow::updateInfoInTable(QString newInfo)
{
    if(!(newInfo.isEmpty() || newInfo.isNull()))
    {
        QModelIndex flightNumber = itemIndex.model()->index(itemIndex.row(), 0);

        QString paramName;
        paramName = itemIndex.column() == 1 ? QString::fromLatin1("planeNumber") : paramName;
        paramName = itemIndex.column() == 2 ? QString::fromLatin1("planeType") : paramName;
        paramName = itemIndex.column() == 3 ? QString::fromLatin1("Arrival") : paramName;
        paramName = itemIndex.column() == 4 ? QString::fromLatin1("Destination") : paramName;
        paramName = itemIndex.column() == 5 ? QString::fromLatin1("flightDate") : paramName;
        paramName = itemIndex.column() == 6 ? QString::fromLatin1("flightTime") : paramName;
        paramName = itemIndex.column() == 7 ? QString::fromLatin1("status") : paramName;

        if(dbConnector->modifyCurrentFlight(flightNumber.data().toString(), paramName, newInfo))
        {
            ui->statusBar->showMessage(QString::fromUtf8("Значение успешно изменено!"));
            dbConnector->getFlightsList(ui->tv_flightsInfoTable);
        }
        else
            ui->statusBar->showMessage(QString::fromUtf8("Ошибка! Не удалось измененить значение в БД!"));
    } else
    {
        ui->statusBar->showMessage(QString::fromUtf8("Ошибка! Не было введено новое значение!"));
    }
}

void MainWindow::on_bt_export_clicked()
{
    QString path = QCoreApplication::applicationDirPath() + "/DataExport.csv";

    QFile f(path);

    if(f.open(QIODevice::WriteOnly))
    {
        QTextStream ts(&f);
        QStringList strList;

        strList << "\" \"";
        for(int c = 0; c < ui->tv_flightsInfoTable->horizontalHeader()->count(); ++c)
        {
            strList << "\"" + ui->tv_flightsInfoTable->model()->headerData(c, Qt::Horizontal).toString()+"\"";
        }
        ts << strList.join(";") + "\n";

        for(int r = 0; r < ui->tv_flightsInfoTable->verticalHeader()->count(); ++r)
        {
            strList.clear();
            strList << "\""+ui->tv_flightsInfoTable->model()->headerData(r, Qt::Horizontal).toString()+ "\"";
            for(int c = 0; c < ui->tv_flightsInfoTable->horizontalHeader()->count(); ++c)
            {
                strList << "\""+ui->tv_flightsInfoTable->model()->data(ui->tv_flightsInfoTable->model()->index(r,c), Qt::DisplayRole).toString()+"\"";
            }
            ts << strList.join(";")+"\n";
        }
        f.close();
        qDebug(QString::fromLatin1("Export completed").toStdString().c_str());
        qDebug(path.toStdString().c_str());
    }
}

void MainWindow::on_bt_import_clicked()
{
    QString path = QCoreApplication::applicationDirPath() + "/DataImport.csv";
    dbConnector->importFile(ui->tv_flightsInfoTable, path);
}

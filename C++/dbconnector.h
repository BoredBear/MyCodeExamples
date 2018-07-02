#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <qstring.h>
#include <qcombobox.h>
#include <qtableview.h>
#include <QSqlQueryModel>
#include <qsqlerror.h>

class DBConnector
{
public:
    enum FindMethod { ByFlightNumber, ByRest};
    DBConnector();
    bool isOpen();
    bool initDB(QString);
    void disconnectFromDB();
    bool getPlaneTypes(QComboBox*);
    bool findFlights(QString, QString, QString, QString, QString, QString, QString, QString, QTableView *, FindMethod);
    bool createNewFlight(QString, QString, QString, QString, QString, QString, QString, QString);
    bool modifyCurrentFlight(QString flightNumber, QString paramName, QString newInfo);
    bool deleteCurrentFlight(QString);
    bool getFlightsList(QTableView*);
    void importFile(QTableView *table, QString filename);
private:
    QString *pass;
    QSqlDatabase db;
};

#endif // DBCONNECTOR_H

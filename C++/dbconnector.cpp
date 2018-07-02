#include "dbconnector.h"
#include <qfile.h>

DBConnector::DBConnector()
{

}

bool DBConnector::initDB(QString pass)
{
    db = QSqlDatabase::addDatabase("QMYSQL", "afl_db");
    db.setDatabaseName("afl_db");
    db.setUserName("root");
    db.setPassword(pass);
    db.setHostName("localhost");

    if(!db.open())
        return false;
    return true;
}

bool DBConnector::isOpen()
{
    if(db.isOpen())
        return true;
    else return false;
}

void DBConnector::disconnectFromDB()
{
    if(db.isOpen())
    {
        db.close();
    }
}

bool DBConnector::getPlaneTypes(QComboBox *cb_planeTypes)
{
    bool isValid = false;
    QSqlQuery query(db);
    query.exec("SELECT * FROM planes;");
    cb_planeTypes->clear();
    cb_planeTypes->addItem(QString::fromUtf8("Выбрать"));
    while(query.next())
    {
        cb_planeTypes->addItem(query.value(0).toString());
        isValid = true;
    }
    if(isValid)
        return true;
    else return false;
}

void DBConnector::importFile(QTableView *table, QString filename)
{
    QFile f(filename);

    if(f.open(QIODevice::ReadOnly))
    {
        while(!f.atEnd())
        {
            QString temp = f.readLine();
            QVector<QString> v;
            int k = 0;
            QString s = "";
            for(int i = 0; i < temp.length(); i++)
            {
                if(i==temp.length()-1)
                {
                    v.insert(k,s);
                    k++;
                    s="";
                }
                if(temp[i]!=";")
                {
                    s+=temp[i];
                }
                else if(temp[i]==";")
                {
                    v.insert(k,s);
                    k++;
                    s="";
                }
            }
            createNewFlight(v.at(0), v.at(1), v.at(2), v.at(3), v.at(4), v.at(5), v.at(6), v.at(7));
        }
        getFlightsList(table);
    }
}

bool DBConnector::getFlightsList(QTableView *table)
{
    QSqlQueryModel *model = new QSqlQueryModel();

    model->setQuery("SELECT * FROM flights", db);
    model->setHeaderData(0, Qt::Horizontal, "Номер рейса");
    model->setHeaderData(1, Qt::Horizontal, "Бортовой номер");
    model->setHeaderData(2, Qt::Horizontal, "Тип самолёта");
    model->setHeaderData(3, Qt::Horizontal, "Аэропорт отправления");
    model->setHeaderData(4, Qt::Horizontal, "Аэропорт прибытия");
    model->setHeaderData(5, Qt::Horizontal, "Дата вылета");
    model->setHeaderData(6, Qt::Horizontal, "Время вылета");
    model->setHeaderData(7, Qt::Horizontal, "Статус");

    table->setModel(model);
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->setAlternatingRowColors(true);

    QSqlError err = model->lastError();
    if(err.type() != QSqlError::NoError)
        return false;
    return true;
}

bool DBConnector::createNewFlight(QString flightNumber,
                                  QString planeNumber,
                                  QString planeType,
                                  QString airportDeparture,
                                  QString airportArrival,
                                  QString departureDate,
                                  QString departureTime,
                                  QString status)
{
    QSqlQuery *query = new QSqlQuery(db);
    QString str = QString::fromLatin1("INSERT INTO flights (flightNumber, planeNumber, planeType,  Arrival,  Destination, flightDate, flightTime, status) VALUES ('")
            + flightNumber     + QString::fromLatin1("','")
            + planeNumber      + QString::fromLatin1("','")
            + planeType        + QString::fromLatin1("','")
            + airportDeparture + QString::fromLatin1("','")
            + airportArrival   + QString::fromLatin1("','")
            + departureDate    + QString::fromLatin1("','")
            + departureTime    + QString::fromLatin1("','")
            + status           + QString::fromLatin1("');");
    query->exec(str);

    QSqlError err = query->lastError();
    if(err.type() != QSqlError::NoError)
        return false;
    return true;
}

bool DBConnector::findFlights(QString flightNumber,
                              QString planeNumber,
                              QString planeType,
                              QString airportDeparture,
                              QString airportArrival,
                              QString departureDate,
                              QString departureTime,
                              QString status,
                              QTableView *table,
                              FindMethod method)
{
    QSqlQueryModel *model = new QSqlQueryModel();

    switch(method)
    {
        case FindMethod::ByFlightNumber:
            model->setQuery("SELECT * FROM flights WHERE flightNumber = '" + flightNumber + "';", db);
        break;
        case FindMethod::ByRest:
            bool isFirst = true;
            QString queryStr = QString::fromLatin1("SELECT * FROM flights WHERE ");
            if(planeNumber.length() > 0)
            {
                queryStr = queryStr + " planeNumber = '" + planeNumber + "' ";
                isFirst = false;
            }
            if(planeType != QString::fromUtf8("Выбрать"))
            {
                if(isFirst)
                {
                    queryStr = queryStr + "planeType = '" + planeType + "' ";
                    isFirst = false;
                }
                else
                    queryStr = queryStr + "AND planeType = '" + planeType + "' ";
            }
            if(airportDeparture.length() > 0)
            {
                if(isFirst)
                {
                    queryStr = queryStr + "Arrival = '" + airportDeparture + "' ";
                    isFirst = false;
                }
                else
                    queryStr = queryStr + "AND Arrival = '" + airportDeparture + "' ";
            }
            if(airportArrival.length() > 0)
            {
                if(isFirst)
                {
                    queryStr = queryStr + "Destination = '" + airportArrival + "' ";
                }
                else
                    queryStr = queryStr + "AND Destination = '" + airportArrival + "' ";
            }
            if(departureDate.length() > 0)
            {
                if(isFirst)
                {
                    queryStr = queryStr + "flightDate = '" + departureDate + "' ";
                }
                else
                    queryStr = queryStr + "AND flightDate = '" + departureDate + "' ";
            }
            if(departureTime.length() > 0)
            {
                if(isFirst)
                {
                    queryStr = queryStr + "flightTime = '" + departureTime + "' ";
                }
                else
                    queryStr = queryStr + "AND flightTime = '" + departureTime + "' ";
            }
            if(status.length() > 0 && status != QString::fromUtf8("Выбрать"))
            {
                if(isFirst)
                {
                    queryStr = queryStr + "status = '" + status + "' ";
                }
                else
                    queryStr = queryStr + "AND status = '" + status + "' ";
            }
            queryStr = queryStr + ";";

            model->setQuery(queryStr, db);
        break;
    }

    model->setHeaderData(0, Qt::Horizontal, "Номер рейса");
    model->setHeaderData(1, Qt::Horizontal, "Бортовой номер");
    model->setHeaderData(2, Qt::Horizontal, "Тип самолёта");
    model->setHeaderData(3, Qt::Horizontal, "Аэропорт отправления");
    model->setHeaderData(4, Qt::Horizontal, "Аэропорт прибытия");
    model->setHeaderData(5, Qt::Horizontal, "Дата вылета");
    model->setHeaderData(6, Qt::Horizontal, "Время вылета");
    model->setHeaderData(7, Qt::Horizontal, "Статус");

    table->setModel(model);
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->setAlternatingRowColors(true);

    QSqlError err = model->lastError();
    if(err.type() != QSqlError::NoError || model->rowCount() < 1)
        return false;
    return true;
}

bool DBConnector::modifyCurrentFlight(QString flightNumber, QString paramName, QString newInfo)
{
    QSqlQuery query(db);
    query.exec("UPDATE flights SET " + paramName + " = '" + newInfo + "' WHERE flightNumber = '" + flightNumber + "';");
    QSqlError err = query.lastError();
    if(err.type() != QSqlError::NoError)
        return false;
    return true;
}

bool DBConnector::deleteCurrentFlight(QString flightNumber)
{
    QSqlQuery query(db);
    query.exec("DELETE FROM flights WHERE flightNumber = '" + flightNumber + "';");
    QSqlError err = query.lastError();
    if(err.type() != QSqlError::NoError)
        return false;
    return true;
}

#include "pregame.h"
#include "ui_pregame.h"

pregame::pregame(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::pregame)
{
    ui->setupUi(this);
    ui->centralwidget->setStyleSheet("background-color: #effeff");

    gl = new GameLogic(this, ui->infoText);
    gl->createMap(35, 80, "Player", this);
    gl->createMap(250, 80, "Enemy", this);

    pregame::updateShipCount();

    //Initialize button's colours
    {
        ui->helpbt_1->btEMPTY;
        ui->helpbt_2->btSHIP;
        ui->helpbt_3->btDESTROYED;
        ui->helpbt_4->btSHOOTED;
        ui->clearButton->btEMPTY;
        ui->startGame->btEMPTY;
    }
}

pregame::~pregame()
{
    delete ui;
}

void pregame::btClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    if(gl->isPlayerMap(clickedButton))
    {
        if(!gl->isGameStarted)
        {
            gl->eventOnPlayerMap(clickedButton);
            pregame::updateShipCount();
        }
    }
    else
    {
        if(gl->isGameStarted)
        {
            gl->eventOnEnemyMap(clickedButton);
        }
    }
}

void pregame::updateShipCount()
{
    QString *qstr = new QString("Кораблей осталось: " + QString::number(GameLogic::MAX_SHIP_COUNT - gl->currentShipCount));
    const char *str = qstr->toStdString().c_str();
    ui->shipCount->setText(str);
}

void pregame::on_clearButton_clicked()
{
    if(gl->isClearBrush)
    {
        ui->clearButton->btEMPTY;
        gl->isClearBrush = false;
    } else
    {
        ui->clearButton->btSHIP;
        gl->isClearBrush = true;
    }
}

void pregame::on_startGame_clicked()
{
    ui->infoText->setText(*new QString("Расставьте все корабли..."));
    if(gl->currentShipCount == GameLogic::MAX_SHIP_COUNT)
    {
        ui->infoText->setText(*new QString("Инициализация ИИ"));
        gl->isGameStarted = true;
        ui->clearButton->deleteLater();
        ui->shipCount->deleteLater();
        ui->startGame->deleteLater();
        gl->initEnemyMap();
        ui->infoText->setText(*new QString(""));
    }
}

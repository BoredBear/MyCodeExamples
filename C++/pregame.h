#ifndef PREGAME_H
#define PREGAME_H

#include <QMainWindow>
#include "gamelogic.h"
#include "defsfile.h"

using namespace std;
namespace Ui {
class pregame;
}

class pregame : public QMainWindow
{
    Q_OBJECT

public:
    explicit pregame(QWidget *parent = 0);
    void updateShipCount();
    ~pregame();
public slots:
    void btClicked();
private slots:
    void on_clearButton_clicked();
    void on_startGame_clicked();

private:
    GameLogic *gl;
    Ui::pregame *ui;
};

#endif // PREGAME_H

#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#include "qwidget.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "defsfile.h"

class GameLogic
{
public:
    enum ButtonState { EMPTY, SHIP, DESTROYED, SHOOTED };
    ButtonState **playerMap = new ButtonState*[6];
    ButtonState **enemyMap = new ButtonState*[6];

    QWidget *widget;
    QLabel *infoText;
    const static int MAX_SHIP_COUNT = 5;
    int countEmptyCells = 36;
    bool isWin = false;
    bool isGameStarted;
    bool isClearBrush;
    int currentShipCount;

    void initEnemyMap();
    void createMap(int, int, std::string, QWidget*);
    bool isPlayerMap(QPushButton*);

    void getArrayOfEmptyCells(std::string*);
    void eventOnPlayerMap(QPushButton*);
    void eventOnEnemyMap(QPushButton*);
    void enemyShoot();
    void checkWinner(int);

    bool isNonShooted(std::string, ButtonState**);
    bool isCurrentPosEqual(std::string, ButtonState, ButtonState**);
    std::string getButtonIndex(QPushButton*);
    GameLogic(QWidget*, QLabel*);
private:
    void initMaps();
};

#endif // GAMELOGIC_H

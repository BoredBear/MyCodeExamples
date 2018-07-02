#include "gamelogic.h"
#include <ctime>

GameLogic::GameLogic(QWidget *widget, QLabel *lb)
{
    this->widget = widget;
    this->infoText = lb;
    isGameStarted = false;
    isClearBrush = false;
    currentShipCount = 0;
    initMaps();
}

void GameLogic::checkWinner(int a)
{
    bool win = true;
    if(a == 0)
    {
        if(!isWin)
        {
            for(int i = 0; i < 6; i++)
            {
                for(int j = 0; j < 6; j++)
                {
                    if(playerMap[i][j] == ButtonState::SHIP)
                    {
                        win = false;
                    }
                }
            }
            if(win)
            {
                QString *str = new QString("Вы проиграли!!!");
                infoText->setText(*str);
            }
        }
    } else
    {
        for(int i = 0; i < 6; i++)
        {
            for(int j = 0; j < 6; j++)
            {
                if(enemyMap[i][j] == ButtonState::SHIP)
                {
                    win = false;
                }
            }
        }
        if(win)
        {
            isWin = true;
            QString *str = new QString("Поздравляем, вы победили!!!");
            infoText->setText(*str);
        }
    }
}

void GameLogic::initMaps()
{
    for(int i = 0; i < 6; i++)
    {
        playerMap[i] = new ButtonState[6];
        enemyMap[i] = new ButtonState[6];
    }
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            playerMap[i][j] = ButtonState::EMPTY;
            enemyMap[i][j] = ButtonState::EMPTY;
        }
    }
}

void GameLogic::initEnemyMap()
{
    int shipsCount = 0;
    int posA;
    int posB;
    while(true)
    {
        if(shipsCount == GameLogic::MAX_SHIP_COUNT) { break; }
        srand(time(0));
        posA = std::rand() % 6;
        posB = std::rand() % 6;
        QString pos = QString::number(posA) + QString::number(posB);
        if(isCurrentPosEqual(pos.toStdString(), ButtonState::EMPTY, enemyMap))
        {
            shipsCount++;
            enemyMap[posA][posB] = ButtonState::SHIP;
        }
    }
}

void GameLogic::createMap(int Xcoord, int Ycoord, std::string buttonPrefix, QWidget *widget)
{
    QPushButton *bt;
    int startPos = Xcoord;
    buttonPrefix += "_";
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            bt = new QPushButton(widget);
            bt->setGeometry(Xcoord, Ycoord, 30, 30);
            bt->btEMPTY;
            std::string downSlash = "_";
            QString *btName = new QString(buttonPrefix.c_str() + QString::number(i) + downSlash.c_str() + QString::number(j));
            bt->setObjectName(*btName);
            widget->connect(bt, SIGNAL(pressed()), SLOT(btClicked()));
            Xcoord = Xcoord + 30;
        }
        Xcoord = startPos;
        Ycoord = Ycoord + 30;
    }
}

bool GameLogic::isPlayerMap(QPushButton *bt)
{
    std::string btName = bt->objectName().toStdString();
    std::string btPrefix = "";
    for(unsigned int i = 0; i < btName.length(); i++)
    {
        if(btName.at(i) == '_')
        {
            break;
        } else
        {
            btPrefix += btName.at(i);
        }
    }
    if(btPrefix == "Player")
    {
        return true;
    } else
    {
        return false;
    }
}

void GameLogic::eventOnPlayerMap(QPushButton *bt)
{
    std::string pos = getButtonIndex(bt);
    if(!isClearBrush)
    {
        if(((MAX_SHIP_COUNT - this->currentShipCount) > 0) && isCurrentPosEqual(pos, ButtonState::EMPTY, playerMap))
        {
            playerMap[(int)pos[0] - 48][(int)pos[1] - 48] = ButtonState::SHIP;
            bt->btSHIP;
            this->currentShipCount += 1;
        }
    } else
    {
       if(isCurrentPosEqual(pos, ButtonState::SHIP, playerMap))
       {
           playerMap[(int)pos[0] - 48][(int)pos[1] - 48] = ButtonState::EMPTY;
           this->currentShipCount -= 1;
           bt->btEMPTY;
       }
    }
}

void GameLogic::eventOnEnemyMap(QPushButton *bt)
{
    if(isGameStarted && isNonShooted(getButtonIndex(bt), enemyMap))
    {
        std::string pos = getButtonIndex(bt);
        if(isCurrentPosEqual(pos, ButtonState::SHIP, enemyMap))
        {
            enemyMap[(int)pos[0] - 48][(int)pos[1] - 48] = ButtonState::DESTROYED;
            bt->btDESTROYED;
        } else
        {
            enemyMap[(int)pos[0] - 48][(int)pos[1] - 48] = ButtonState::SHOOTED;
            bt->btSHOOTED;
        }
        checkWinner(1);
        enemyShoot();
    }
}

void GameLogic::getArrayOfEmptyCells(std::string* arr)
{
    int lastIndex = 0;
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            if(playerMap[i][j] == ButtonState::EMPTY || playerMap[i][j] == ButtonState::SHIP)
            {
                arr[lastIndex] = (QString::number(i) + QString::number(j)).toStdString();
                lastIndex++;
            }
        }
    }
}

void GameLogic::enemyShoot()
{
    std::string *emptyCells = new std::string[countEmptyCells];
    getArrayOfEmptyCells(emptyCells);
    while(true)
    {
        srand(time(0));
        int posIndex = std::rand() % countEmptyCells;
        std::string pos = emptyCells[posIndex];
        if(isNonShooted(pos, playerMap))
        {
            char i = pos[0];
            char j = pos[1];
            QString btName = QString::fromLatin1("Player_") + i + QString::fromLatin1("_") + j;
            QPushButton *bt = widget->findChild<QPushButton*>(btName.toStdString().c_str());
            if(isCurrentPosEqual(pos, ButtonState::SHIP, playerMap))
            {
                playerMap[(int)i - 48][(int)j - 48] = ButtonState::DESTROYED;
                bt->btDESTROYED;

            } else
            {
                playerMap[(int)i - 48][(int)j - 48] = ButtonState::SHOOTED;
                bt->btSHOOTED;
            }
            countEmptyCells--;
            break;
        }
    }
    checkWinner(0);
}

std::string GameLogic::getButtonIndex(QPushButton *bt)
{
    std::string currentId = "";
    std::string btName = bt->objectName().toStdString();
    for(unsigned int i = 0; i < btName.length(); i++)
    {
        if(btName[i] == '_')
        {
            currentId += QString::number(btName[i + 1] - 48).toStdString();
        }
    }
    return currentId;
}

bool GameLogic::isNonShooted(std::string pos, ButtonState** currentMap)
{
    if(isCurrentPosEqual(pos, ButtonState::DESTROYED, currentMap) || isCurrentPosEqual(pos, ButtonState::SHOOTED, currentMap))
    {
        return false;
    } else
    {
        return true;
    }
}

bool GameLogic::isCurrentPosEqual(std::string pos, ButtonState btState, ButtonState **currentMap)
{
    int posA = (int)pos[0] - 48;
    int posB = (int)pos[1] - 48;

    if(btState == currentMap[posA][posB])
    {
        return true;
    } else return false;
}

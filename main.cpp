#include "TXLib.h"
#include "constants.h"
#include <string>
#include <fstream>
#include <iostream>
#include <conio.h>
#include <time.h>

using namespace std;

int scrMiddleX;
int scrMiddleY;
int level = 1;
string car1, car2;

bool soundInGame =  true;
bool exitGame    = false;
HDC trackImage;

struct Star {
    int x;
    int y;
    bool visible;
};

Star stars[3] = {
    {700,  40, true},
    {670, 360, true},
    {50 , 440, true}
};

struct Car {
    int x;
    int y;
    double fuel;
    int speed;
    bool visibility;
    HDC carImage;
    HDC carRight;
    HDC carLeft;
    HDC carUp;
    HDC carDown;
    bool isWinner;
    int scores;
};

struct Enemy {
    int x;
    int y;
    int speed;
    bool visible;
};

void background(COLORREF color);
void gameLoop(string car1Adress, string car2Adress);
void drawMenu();
void drawEnemy(int x, int y);
void loadRectangles(int x, int y);
int changeSpeedCar1(int speed);
int changeSpeedCar2(int speed1);

void changeXCar(Car *car, bool letters_mgmt);
void changeYCar(Car *car, bool letters_mgmt);

void drawTrack();
void drawfuelCar(Car car, int fuelCoord);
void getStarsCoords();
bool click(int minX, int maxX, int minY, int maxY);
Car moveCar(Car car, bool letters_mgmt);
void pauseMenu(int x, int y, int width, int height);
bool enterCode();
void selectingCars(HDC* cars);

int main()
{
    if (enterCode()) {
        txCreateWindow(800, 600);

        scrMiddleX = txGetExtentX() / 2;
        scrMiddleY = txGetExtentY() / 2;

        HDC car1Image =  txLoadImage((carPicsDirectory + "gcarright.bmp").c_str());
        HDC car2Image = txLoadImage((carPicsDirectory + "gcar1right.bmp").c_str());
        HDC car3Image =  txLoadImage((carPicsDirectory + "rcarright.bmp").c_str());
        HDC car4Image = txLoadImage((carPicsDirectory + "rcar1right.bmp").c_str());

        HDC carsArray[4] = {car1Image, car2Image, car3Image, car4Image};

        getStarsCoords();

        while (!GetAsyncKeyState(VK_ESCAPE)) {

            txBegin();
            background(TX_WHITE);

            drawMenu();

            exitGame = false;

            if (click(scrMiddleX - 50, scrMiddleX + 50, scrMiddleY - 25, scrMiddleY + 25) ||
            GetAsyncKeyState(VK_RETURN))
            {

                if (level > 4) {
                    level = 1;
                }

                background(LoadColor);
                loadRectangles(100, 490);
                background(LoadColor);
                loadRectangles(100, 490);
                txSleep(800);

                selectingCars(carsArray);
            }

            txEnd();
            txSleep(50);
        }

        txDeleteDC(car1Image);
        txDeleteDC(car2Image);
        txDeleteDC(car3Image);
        txDeleteDC(car4Image);

        txDisableAutoPause();
    }
    else {
        cout << "Wrong code!" << endl;
    }

    return 0;
}

void background(COLORREF color)
{
    txSetFillColor(color);
    txSetColor(TX_BLACK, 5);
    txClear();
}

void gameLoop(string car1Adress, string car2Adress)
{
    int count_enemies = 8;
    Enemy enemies[count_enemies];

    for (int index = 0; index < count_enemies; index = index + 1)
    {
        enemies[index].visible = false;
    }

    if (level == 1) {
        enemies[0] = {450, 0  , 2, true};
        enemies[1] = {660, 332, 2, true};
        count_enemies = 2;
    }
    if (level == 2) {
        enemies[0] = {450, 0  , 3, true};
        enemies[1] = {660, 332, 3, true};
        enemies[2] = {610, 505, 3, true};
        count_enemies = 3;
    }
    if (level == 3) {
        enemies[0] = {450, 0  , 5, true};
        enemies[1] = {660, 332, 5, true};
        enemies[2] = {610, 505, 5, true};
        enemies[3] = {50 , 300, 5, true};
        enemies[4] = {450, 0  , 5, true};
        count_enemies = 5;
    }
    if (level == 4) {
        enemies[0] = {450, 0  , 5, true};
        enemies[1] = {660, 332, 5, true};
        enemies[2] = {610, 505, 5, true};
        enemies[3] = {50 , 300, 5, true};
        enemies[4] = {35 , 420, 5, true};
        enemies[5] = {490, 327, 5, true};
        enemies[6] = {568, 127, 5, true};
        enemies[7] = {30 , 178, 5, true};
        count_enemies = 8;
    }

    char timestr[20];

    HDC car1Right =  txLoadImage((carPicsDirectory + car1Adress + "right.bmp").c_str());
    HDC car2Right =  txLoadImage((carPicsDirectory + car2Adress + "right.bmp").c_str());

    HDC car1Left  =  txLoadImage((carPicsDirectory + car1Adress + "left.bmp").c_str());
    HDC car2Left  =  txLoadImage((carPicsDirectory + car2Adress + "left.bmp").c_str());

    HDC car1Up    =  txLoadImage((carPicsDirectory + car1Adress + "up.bmp").c_str());
    HDC car2Up    =  txLoadImage((carPicsDirectory + car2Adress + "up.bmp").c_str());

    HDC car1Down  =  txLoadImage((carPicsDirectory + car1Adress + "down.bmp").c_str());
    HDC car2Down  =  txLoadImage((carPicsDirectory + car2Adress + "down.bmp").c_str());

    HDC car1Image =  car1Right;
    HDC car2Image =  car2Right;

    HDC starImage = txLoadImage("pictures\\star.bmp");

    Car car1 = {12,  5, 150.0, 2, true, car1Image, car1Right, car1Left, car1Up, car1Down, false, 0};
    Car car2 = {12, 55, 150.0, 2, true, car2Image, car2Right, car2Left, car2Up, car2Down, false, 0};

    time_t t;
    struct tm *tm;

    t  = time(NULL);
    tm = localtime(&t);

    exitGame == false;

    if (soundInGame) {
        txPlaySound("music\\music.wav", SND_LOOP);
    }
    else {
        txPlaySound(NULL);
    }

    int startTime = 3600 * tm->tm_hour + 60 * tm->tm_min + tm->tm_sec;
    for (int numberStar = 0; numberStar < 3; numberStar++) {
        stars[numberStar].visible = true;
    }

    while (!GetAsyncKeyState(VK_SPACE)) {

        txBegin();

        t =  time(NULL);
        tm = localtime(&t);
        int curTime = 3600 * tm->tm_hour + 60 * tm->tm_min + tm->tm_sec;
        int curMins = (curTime - startTime) / 60;
        int curSecs = (curTime - startTime) - 60 * curMins;

        background(TX_WHITE);
        drawTrack();
        /*txRectangle(
            stars[0].x - 29,
            stars[0].y - 29,
            stars[0].x + 29,
            stars[0].y + 29
        );
        txRectangle(
            stars[1].x - 29,
            stars[1].y - 29,
            stars[1].x + 29,
            stars[1].y + 29
        );
        txRectangle(
            stars[2].x - 29,
            stars[2].y - 29,
            stars[2].x + 29,
            stars[2].y + 29
        ); */

        //Enemy moving
        for (int index = 0; index < count_enemies; index++)
        {
            enemies[index].x += enemies[index].speed;
            if (txGetPixel(enemies[index].x + 20, enemies[index].y + 15) == TX_BLACK)
            {
                enemies[index].speed *= -1;
            }
            enemies[index].y += random(-15,15);

            if (enemies[index].y < 0)
            {
                enemies[index].y = 15;
            }
        }

        //Drawing
        for (int index = 0; index < count_enemies; index++)
        {
            if (enemies[index].visible)
            {
                drawEnemy(enemies[index].x, enemies[index].y);
            }
        }

        for (int numberStar = 0; numberStar < 3; numberStar++)
        {
            if (stars[numberStar].visible) {
                txTransparentBlt(txDC(), stars[numberStar].x, stars[numberStar].y, 58, 60, starImage, 0, 0, TX_WHITE);
            }
        }

        drawfuelCar(car1, 10);
        drawfuelCar(car2, 160);
        sprintf(timestr, "%02d:%02d", curMins, curSecs);
        txSelectFont("Arial", 20, 10);
        txTextOut(txGetExtentX() - 100, 2, timestr);

        car1 = moveCar(car1, false);
        car2 = moveCar(car2, true);

        txSetColor(LoadColor, 3);

        if (GetAsyncKeyState('P')) {
            pauseMenu(scrMiddleX - 300, scrMiddleY - 200, 600, 400);
        }

        if (GetAsyncKeyState('I')) {
            while (GetAsyncKeyState('I')) {
                txSleep(10);
            }

            car1.visibility = !car1.visibility;
        }

        if (car1.visibility == true) {

            if (car1.carImage == car1.carRight || car1.carImage == car1.carLeft) {
                txTransparentBlt(txDC(), car1.x, car1.y, 57, 25, car1.carImage, 0, 0, TX_WHITE);
            } else {
                txTransparentBlt(txDC(), car1.x, car1.y, 25, 57, car1.carImage, 0, 0, TX_WHITE);
            }
        }

        if (GetAsyncKeyState('V')) {
            txEnd();
            while (GetAsyncKeyState('V')) {
                txSleep(10);
            }

            txBegin();
            car2.visibility = !car2.visibility;
        }

        if (car2.visibility == true) {

            if (car2.carImage == car2.carRight || car2.carImage == car2.carLeft) {
                txTransparentBlt(txDC(), car2.x, car2.y, 57, 25, car2.carImage, 0, 0, TX_WHITE);
            } else {
                txTransparentBlt(txDC(), car2.x, car2.y, 25, 57, car2.carImage, 0, 0, TX_WHITE);
            }
        }

        if (car1.isWinner || car2.isWinner ||
        (car1.fuel <= 0 && car2.fuel <= 0) || exitGame) {
            break;
        }

        txDrawText(car1.x, car1.y - 15, car1.x + 25, car1.y, "1", DT_CENTER);
        txDrawText(car2.x, car2.y - 15, car2.x + 25, car2.y, "2", DT_CENTER);

        txEnd();
        txSleep(50);
    }

    if      (car1.isWinner) {
        txSleep(850);
        car1.scores++;
        car1.isWinner = false;
    }
    else if (car2.isWinner) {
        txSleep(850);
        car2.scores++;
        car2.isWinner = false;
    }
    level += 1;

    if (level > 4) {
        if (car1.scores > car2.scores) {
            txSelectFont("Arial", 30);
            txTextOut(100, 100, "Player 1 is winner!");
        }
        if (car1.scores < car2.scores) {
            txSelectFont("Arial", 30);
            txTextOut(100, 100, "Player 2 is winner!");
        }
        if (car1.scores == car2.scores) {
            txSetColor(TX_BLACK);
            txTextOut(100, 100, "Err");
        }
    }

    if (soundInGame) {
        txPlaySound(NULL);
    }

    txSleep(1000);
    txDeleteDC(car1Image);
    txDeleteDC(car2Image);

    txDeleteDC(car1Right);
    txDeleteDC(car1Left);
    txDeleteDC(car1Up);
    txDeleteDC(car1Down);

    txDeleteDC(car2Right);
    txDeleteDC(car2Left);
    txDeleteDC(car2Up);
    txDeleteDC(car2Down);

    txDeleteDC(starImage);
}

void drawEnemy(int x, int y)
{
    txSetColor(TX_BLACK, 5);
    txSetFillColor(TX_BLACK);
    txRectangle(x, y, x + 40, y + 30);
}

void drawMenu()
{
    char lvlString[10];
    sprintf(lvlString, "”ровень %d", level);

    txSetColor(TX_BLACK, 3);
    txSetFillColor(BISQUECOLOR);
    txRectangle(scrMiddleX - 50, scrMiddleY - 25, scrMiddleX + 50, scrMiddleY + 25);
    txSelectFont("Comic Sans MS", 30, 10);
    txDrawText(scrMiddleX - 50, scrMiddleY - 25, scrMiddleX + 50, scrMiddleY + 25, "Play");

    POINT prev[3] = {
        {340, 350},
        {330, 360},
        {340, 370}
    };

    POINT next[3] = {
        {460, 350},
        {470, 360},
        {460, 370}
    };

    RECT prevArrow = {
        329, 349,
        339, 371
    };

    RECT nextArrow = {
        459, 349,
        471, 371
    };

    if (In(txMousePos(), prevArrow) && (txMouseButtons() & 1) && level > 1) {
        level -= 1;
        sprintf(lvlString, "”ровень %d", level);
    }
    if (In(txMousePos(), nextArrow) && (txMouseButtons() & 1) && level < 4) {
        level += 1;
        sprintf(lvlString, "”ровень %d", level);
    }

    txSelectFont("Calibri", 25, 10);
    txTextOut(355, 345, lvlString);

    txSetColor(TX_BLACK, 1.5);
    txSetFillColor(BISQUECOLOR);

    txPolygon(prev, 3);
    txPolygon(next, 3);
}

void loadRectangles(int x, int y)
{
    txSetFillColor(TX_WHITE);
    txSetColor(TX_WHITE);
    txSelectFont("Consolas", 145, 70, true);

    for (int line_x = 0; line_x < 600; line_x += 20) {

        txTextOut(60, 60, "Loading...");
        txRectangle(x + line_x - 15, y, x + line_x, y + 20);
        txSleep(50);
    }
}

void drawTrack()
{
    POINT trackPoints[13] = {
        {0,     0},
        {780,   0},
        {780, 380},
        {750, 380},
        {750, 580},
        {0,   580},
        {0,   140},
        {140, 140},
        {140, 440},
        {610, 440},
        {610, 140},
        {0,   140}
    };
    txPolygon(trackPoints, 13);
}

void drawfuelCar(Car car, int fuelCoord)
{
    txSetColor(TX_BLACK);
    txSetFillColor(TX_GREEN);

    if (car.fuel > 0)
    {
        txRectangle(fuelCoord, 589, fuelCoord + car.fuel, 599);
    }
    else
    {
        txRectangle(fuelCoord, 589, fuelCoord + 1, 599);
    }
}

void getStarsCoords()
{
    ifstream starsCoords("stars.conf");
    char buff[10];
    starsCoords.getline(buff, 10); //ignore word "stars"
    starsCoords.getline(buff, 10); //draw_stars?

    if (atoi(buff) == 1) {
        stars[0].visible = true;
        stars[1].visible = true;
        stars[2].visible = true;
    }
    if (atoi(buff) == 0) {
        stars[0].visible = false;
        stars[1].visible = false;
        stars[2].visible = false;
    }

    for (int numberStar = 0; numberStar < 3; numberStar++)
    {
        starsCoords.getline(buff, 10); //ignore word "star1"
        starsCoords.getline(buff, 10); //x
        stars[numberStar].x = atoi(buff);
        starsCoords.getline(buff, 10); //y
        stars[numberStar].y = atoi(buff);
    }
    starsCoords.close();
}

bool click(int minX, int maxX, int minY, int maxY)
{
    return (
        (txMouseX() > minX &&
         txMouseX() < maxX &&
         txMouseY() > minY &&
         txMouseY() < maxY &&
         txMouseButtons() & 1)
    );
}

Car moveCar(Car car, bool letters_mgmt)
{
    int x = car.x;
    int y = car.y;

    char key_for_right = VK_RIGHT;
    char key_for_left  = VK_LEFT;
    char key_for_up    = VK_UP;
    char key_for_down  = VK_DOWN;

    if (letters_mgmt) {
        key_for_right = 'D';
        key_for_left  = 'A';
        key_for_up    = 'W';
        key_for_down  = 'S';
    }

    if (car.x > 0 && car.x < 138 && car.y > 168 && car.y < 225) {
        //code (win)
        car.isWinner = true;
    }

    if (GetAsyncKeyState(key_for_right) && car.x < 800 && car.fuel >= 10) {
        car.x += car.speed;
        car.carImage = car.carRight;
    }
    else if (GetAsyncKeyState(key_for_left) && car.x > 0 && car.fuel >= 10) {
        car.x -= car.speed;
        car.carImage = car.carLeft;
    }

    if (GetAsyncKeyState(key_for_up) && car.y > 10 && car.fuel >= 10) {
        car.y -= car.speed;
        car.carImage = car.carUp;
    }
    else if (GetAsyncKeyState(key_for_down) && car.y < 550 && car.fuel >= 10) {
        car.y += car.speed;
        car.carImage = car.carDown;
    }

    if (letters_mgmt) {
        //Speed
        if (GetAsyncKeyState('J') && car.speed < 21) {
            car.speed++;
        }
        else if (GetAsyncKeyState('K') && car.speed > 1) {
            car.speed--;
        }

    } else {

        //Speed
        if (GetAsyncKeyState('H') && car.speed < 21) {
            car.speed++;
        }
        else if (GetAsyncKeyState('L') && car.speed > 1) {
            car.speed--;
        }

    }

    for (int x1_loop = car.x + 5; x1_loop < car.x + 50; x1_loop += 2)     {

        for (int y1_loop = car.y + 5; y1_loop < car.y + 50; y1_loop += 2)   {

            if (txGetPixel(x1_loop, y1_loop) == TX_BLACK) {
                car.x = x;
                car.y = y;
            }

            for (int numberStar = 0; numberStar < 3; numberStar++) {

                if (car.carImage == car.carLeft || car.carImage == car.carRight)
                {
                    //middleOfCar = х + 29, у + 13
                    if (car.x + 29 > stars[numberStar].x && car.x + 29 < stars[numberStar].x + 60 &&
                        car.y + 13 > stars[numberStar].y && car.y + 13 < stars[numberStar].y + 60 &&
                        stars[numberStar].visible) {
                            stars[numberStar].visible = false;
                            car.fuel += 15.0;
                    }
                }
                else
                {
                    //middleOfCar = х + 13, у + 29
                    if (car.x + 13 > stars[numberStar].x && car.x + 13 < stars[numberStar].x + 60 &&
                        car.y + 29 > stars[numberStar].y && car.y + 29 < stars[numberStar].y + 60 &&
                        stars[numberStar].visible) {
                            stars[numberStar].visible = false;
                            car.fuel += 15.0;
                    }
                }
            }
        }
    }

    car.fuel -= 0.20;

    return car;
}

void pauseMenu(int x, int y, int width, int height)
{
    int middleOfMenuX = (x + width)  / 2;
    int middleOfMenuY = (y + height) / 2;

    txSetColor(TX_BLACK, 5);
    txSetFillColor(BISQUECOLOR);
    txRectangle(x, y, x + width, y + height);

    //drawing the buttons
    txRectangle(
        middleOfMenuX     -     60,
        middleOfMenuY / 3 * 2 - 30,
        middleOfMenuX     +     60,
        middleOfMenuY / 3 * 2 + 30
    );

    txRectangle(
        middleOfMenuX     -     60,
        middleOfMenuY / 3 * 3 - 30,
        middleOfMenuX     +     60,
        middleOfMenuY / 3 * 3 + 30
    );

    txRectangle(
        middleOfMenuX     -     60,
        middleOfMenuY / 3 * 4 - 30,
        middleOfMenuX     +     60,
        middleOfMenuY / 3 * 4 + 30
    );

    txSelectFont("Arial", 20, -1, 1, false, false, false);

    txDrawText(
        middleOfMenuX     -     60,
        middleOfMenuY / 3 * 2 - 30,
        middleOfMenuX     +     60,
        middleOfMenuY / 3 * 2 + 30,
        "Continue"
    );

    if (!soundInGame) {
        txSelectFont("Arial", 20, -1, 1, false, false, true);
    }

    txDrawText(
        middleOfMenuX     -     60,
        middleOfMenuY / 3 * 3 - 30,
        middleOfMenuX     +     60,
        middleOfMenuY / 3 * 3 + 30,
        "Sound"
    );

    txSelectFont("Arial", 20, -1, 1, false, false, false);

    txDrawText(
        middleOfMenuX     -     60,
        middleOfMenuY / 3 * 4 - 30,
        middleOfMenuX     +     60,
        middleOfMenuY / 3 * 4 + 30,
        "Exit"
    );

    //arrays with the coordinates of the buttons
    RECT buttonPlay = {
        middleOfMenuX     -     60,
        middleOfMenuY / 3 * 2 - 30,
        middleOfMenuX     +     60,
        middleOfMenuY / 3 * 2 + 30
    };

    RECT buttonSound  = {
        middleOfMenuX     -     60,
        middleOfMenuY / 3 * 3 - 30,
        middleOfMenuX     +     60,
        middleOfMenuY / 3 * 3 + 30
    };

    RECT buttonExit = {
        middleOfMenuX     -     60,
        middleOfMenuY / 3 * 4 - 30,
        middleOfMenuX     +     60,
        middleOfMenuY / 3 * 4 + 30
    };

    while (!GetAsyncKeyState('B')) {
        if (In(txMousePos(), buttonPlay) && txMouseButtons() & 1) {

            break;
        }
        if (In(txMousePos(), buttonSound)  && txMouseButtons() & 1) {

            soundInGame = !soundInGame;
            if (!soundInGame) {
                txPlaySound(NULL);
            }
            else {
                txPlaySound("music\\music.wav");
            }
        }
        if (In(txMousePos(), buttonExit) && txMouseButtons() & 1) {

            exitGame = true;
            break;
        }

        txSleep(10);
    }
}

bool enterCode()
{
    ifstream fileWithCode("code.conf");
    char codeFromFile[25];
    char code[25];
    char buff[25];

    fileWithCode.getline(buff, 25);

    if (strcmp(buff, "code") == 0) {
        fileWithCode.getline(codeFromFile, 25);
    }
    //strcpy(codeFromFile, buff);

    fileWithCode.close();

    if (!(strcmp(codeFromFile, "") == 0)) {

        cout << "Enter the code" << endl;
        cin >> code;

        return (strcmp(code, codeFromFile) == 0);
    }
    else {
        return true;
    }
}

void selectingCars(HDC* cars)
{
    background(LoadColor);
    txSetColor(TX_WHITE, 2);
    txSelectFont("Arial", 30, 10);
    txDrawText(50, 50, 200, 120, "Select car for\nplayer1");
    Win32::TransparentBlt(txDC(), 50 + 164 * 0, 200, 114, 50, cars[0], 0, 0, 57, 25, TX_WHITE);
    Win32::TransparentBlt(txDC(), 50 + 164 * 1, 200, 114, 50, cars[1], 0, 0, 57, 25, TX_WHITE);
    Win32::TransparentBlt(txDC(), 50 + 164 * 2, 200, 114, 50, cars[2], 0, 0, 57, 25, TX_WHITE);
    Win32::TransparentBlt(txDC(), 50 + 164 * 3, 200, 114, 50, cars[3], 0, 0, 57, 25, TX_WHITE);

    car1 = "";
    while (car1 == "")
    {
        if        (click(50 + 164 * 0, 164 + 164 * 0, 200, 250) || GetAsyncKeyState('1')) {
            car1 = "gcar";

        } else if (click(50 + 164 * 1, 164 + 164 * 1, 200, 250) || GetAsyncKeyState('2')) {
            car1 = "gcar1";

        } else if (click(50 + 164 * 2, 164 + 164 * 2, 200, 250) || GetAsyncKeyState('3')) {
            car1 = "rcar";

        } else if (click(50 + 164 * 3, 164 + 164 * 3, 200, 250) || GetAsyncKeyState('4')) {
            car1 = "rcar1";
        }

        txSleep(10);
    }

    txSleep(500);

    background(LoadColor);
    txSetColor(TX_WHITE, 2);
    txSelectFont("Arial", 30, 10);
    txDrawText(50, 50, 200, 120, "Select car for\nplayer2");
    Win32::TransparentBlt(txDC(), 50 + 164 * 0, 200, 114, 50, cars[0], 0, 0, 57, 25, TX_WHITE);
    Win32::TransparentBlt(txDC(), 50 + 164 * 1, 200, 114, 50, cars[1], 0, 0, 57, 25, TX_WHITE);
    Win32::TransparentBlt(txDC(), 50 + 164 * 2, 200, 114, 50, cars[2], 0, 0, 57, 25, TX_WHITE);
    Win32::TransparentBlt(txDC(), 50 + 164 * 3, 200, 114, 50, cars[3], 0, 0, 57, 25, TX_WHITE);

    car2 = "";
    while (car2 == "")
    {
        if        (click(50 + 164 * 0, 164 + 164 * 0, 200, 250) || GetAsyncKeyState('1')) {
            car2 = "gcar";

        } else if (click(50 + 164 * 1, 164 + 164 * 1, 200, 250) || GetAsyncKeyState('2')) {
            car2 = "gcar1";

        } else if (click(50 + 164 * 2, 164 + 164 * 2, 200, 250) || GetAsyncKeyState('3')) {
            car2 = "rcar";

        } else if (click(50 + 164 * 3, 164 + 164 * 3, 200, 250) || GetAsyncKeyState('4')) {
            car2 = "rcar1";
        }

        txSleep(10);
    }

    txSleep(400);
    gameLoop(car1, car2);
}


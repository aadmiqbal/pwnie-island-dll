/*
- Game will be initiated through chat so overwrite the chat handling function
- will call this game logic tailored for pwnie island
*/

#include <iostream>
#include <windows.h>
#include <time.h>
#include <thread>

using namespace std;

int GAME_SPEED = 100;

bool endGame = false;
bool canRunTimer = true;

int randomNumber;
int enemyNumber;
int timeLeft;

char map[20][20] = {
    "###################",
    "#  @ @ @ @ @ @ @  #",
    "#   @ @ @ @ @ @   #",
    "#                 #",
    "#                 #",
    "#                 #",
    "#                 #",
    "#                 #",
    "#                 #",
    "#                 #",
    "#                 #",
    "#                 #",
    "#                 #",
    "#                 #",
    "#                 #",
    "#        W        #",
    "###################",
};

int countCharOnMap(char c)
{
    int occ = 0;

    for (int y = 0; y < 20; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            if (map[y][x] == '@')
                occ++;
        }
    }

    return occ;
}

void timer()
{
    timeLeft = 60;
    
    while (canRunTimer && timeLeft >= 1)
    {
        Sleep(1000);
        timeLeft--;
    }
}


int main()
{
    // start timer
    thread timer_thread(timer);

    while (!endGame)
    {
        srand(time(0));

        system("clear||cls");

        enemyNumber = countCharOnMap('@');

        // render the map
        for (int y = 0; y < 20; y++)
        {
            cout << map[y] << endl;
        }

        cout << "Remaining enemies = " << enemyNumber << "\n";
        cout << "Remaining time = " << timeLeft << "\n";

        // size of the map
        for (int y = 0; y < 20; y++)
        {
            for (int x = 0; x < 20; x++)
            {
                switch (map[y][x])
                {
                    case 'W':
                        // when left key pressed move W obj left (-1)
                        if (GetAsyncKeyState(VK_LEFT))
                        {
                            int newX = x - 1;

                            switch (map[y][newX])
                            {
                                // replace old coordinate with space and 
                                // insert W to new location
                                case ' ':
                                    map[y][x] = ' ';
                                    x--;
                                    map[y][newX] = 'W';

                                    break;
                            }
                        }

                        // when right key pressed move W obj right (+1)
                        if (GetAsyncKeyState(VK_RIGHT))
                        {
                            int newX = x + 1;

                            switch (map[y][newX])
                            {
                                case ' ':
                                    map[y][x] = ' ';
                                    x++;
                                    map[y][newX] = 'W';

                                    break;
                            }
                        }

                        // when space is pressed "shoot"
                        if (GetAsyncKeyState(VK_SPACE))
                        {
                            y--;
                            map[y][x] = '^';
                        }

                        break;

                    // what actually happens when shot
                    case '^':
                        // the bullet (^) travels - what's left behind
                        map[y][x] = ' ';
                        y--;

                        // the bullet (^) travels - what's ahead
                        if (map[y][x] != '#' && map[y][x] != '@')
                            map[y][x] = '^';
                        else if (map[y][x] == '@')
                            // enemy shot
                            map[y][x] = ' ';

                        break;

                    case '@':
                        randomNumber = rand() % 100 + 1;

                        if (randomNumber == 1)
                        {
                            y++;
                            map[y][x] = '@';
                        }

                        break;
                }
            }
        }

        if (enemyNumber == 0 && timeLeft >= 1)
        {
            system("clear||cls");
            cout << "You win!\n\nGAME OVER\n";
            endGame = true;
            canRunTimer = false;

            timer_thread.join();
        }

        if (timeLeft < 1)
        {
            system("clear||cls");
            cout << "You lost!\n\nGAME OVER\n";
            endGame = true;
            canRunTimer = false;

            timer_thread.join();
        }

        Sleep(GAME_SPEED);
    }

    return 0;
}

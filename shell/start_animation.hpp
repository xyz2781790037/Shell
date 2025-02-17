#include <iostream>
#include <unistd.h>
using namespace std;
bool start = false;
void print()
{
    string showName[9];
    showName[0] = "      __                                     _______               __                            ";
    showName[1] = "     /\\ \\                                   /\\____  \\             /\\ \\                           ";
    showName[2] = "  ___\\ \\ \\___         ___ ___   __  __      \\/__/\\/ /   __     ___\\ \\ \\___                       ";
    showName[3] = " / __`\\ \\  _ `\\     /' __` __`\\/\\ \\/\\ \\        /\\/ /  /'_ `\\  /',__\\ \\  _ `\\                     ";
    showName[4] = "/\\ \\L\\ \\ \\ \\ \\ \\    /\\ \\/\\ \\/\\ \\ \\ \\_\\ \\      /\\/ /__/\\ \\L\\ \\/\\__, `\\ \\ \\ \\ \\                    ";
    showName[5] = "\\ \\____/\\ \\_\\ \\_\\   \\ \\_\\ \\_\\ \\_\\/`____ \\    /\\/_____\\ \\____ \\/\\____/\\ \\_\\ \\_\\                   ";
    showName[6] = " \\/___/  \\/_/\\/_/    \\/_/\\/_/\\/_/`/___/> \\   \\/______/\\/___L\\ \\/___/  \\/_/\\/_/                   ";
    showName[7] = "                                    /\\___/              /\\____/                                  ";
    showName[8] = "                                    \\/__/               \\_/__/                                   ";
    for (int i = 0; i < 9; i++)
    {
        printf("\e[9%dm", i);
        cout << showName[i] << endl;
        usleep(110000);
    }
    cout << "输入任意键继续：";
    start = true;
    getchar();
    system("clear");
}
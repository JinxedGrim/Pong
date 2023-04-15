#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>

int sx = GetSystemMetrics(SM_CXSCREEN);
int sy = GetSystemMetrics(SM_CYSCREEN);

int Vx = 1;
int Vy = 1;
int BallWidth = 15;
int BallHeight = 15;
int Bounces = 0;
int MaxSpeed = 2;
const int BouncesTillIncrease = 5;

int BallX = sx / 2 - (BallWidth / 2);
int BallY = sy / 2 - (BallHeight / 2);

int PlayerHeight = 80;
int PlayerWidth = 20;
int PlayerVelocity = 2;

int Player1X = 0;
int Player1Y = sy / 2 - (PlayerHeight / 2);
int Player1Score = 0;

int Player2X = sx - 20;
int Player2Y = sy / 2 - (PlayerHeight / 2);
int Player2Score = 0;

const int MaxX = Player2X;

int R = 255;
int B = 255;
int G = 255;

bool Paused = false;

void Settings()
{
    while (true)
    {
        if (GetAsyncKeyState(0x31) & 0x8000)
        {
            if (Vx < 0)
                Vx -= 1;
            else
                Vx += 1;
            if (Vy < 0)
                Vy -= 1;
            else
                Vy += 1;
        }
        if (GetAsyncKeyState(0x32) & 0x8000)
        {
            if (Vx < 0)
                Vx += 1;
            else
                Vx -= 1;
            if (Vy <= 0)
                Vy += 1;
            else
                Vy -= 1;
        }
        if (GetAsyncKeyState(0x33) & 0x8000)
        {
            PlayerHeight += 10;
        }
        if (GetAsyncKeyState(0x34) & 0x8000)
        {
            if (PlayerHeight > 0)
                PlayerHeight -= 10;

            if (PlayerHeight <= 0)
                PlayerHeight += 10;
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            BallX = sx / 2 - (BallWidth / 2);
            BallY = sy / 2 - (BallHeight / 2);

            Player1X = 0;
            Player1Y = sy / 2 - (PlayerHeight / 2);

            Player2X = sx - 20;
            Player2Y = sy / 2 - (PlayerHeight / 2);

            Paused = !Paused;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
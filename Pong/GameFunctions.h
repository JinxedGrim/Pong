#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>

int sx = GetSystemMetrics(SM_CXSCREEN);
int sy = GetSystemMetrics(SM_CYSCREEN);
bool Fs = false;

int Vx = 300; // Velocity in pixels per second
int Vy = 300; // Velocity in pixels per second
int BallWidth = 15;
int BallHeight = 15;
int Bounces = 0;
int MaxSpeed = 900;
const int BouncesTillIncrease = 5;

int BallX = sx / 2 - (BallWidth / 2);
int BallY = sy / 2 - (BallHeight / 2);

float BallSubX = 0.0f;
float BallSubY = 0.0f;
float Player1SubY = 0.0f;
float Player2SubY = 0.0f;


int PlayerHeight = 100;
int PlayerWidth = 15;
int PlayerVelocity = 1100;

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

void CollisionDetection()
{
    // Out Of Bounds Y Detetcion
    if (BallY + BallHeight >= sy && Vy > 0)
    {
        Vy *= -1;
    }

    if (Bounces >= BouncesTillIncrease && !(MaxSpeed < abs(Vx)))
    {
        if (Vx <= 0 && Vx > -MaxSpeed)
            Vx -= 100;
        else if ((Vx >= 0 && Vx < MaxSpeed))
            Vx += 100;

        if (Vy <= 0 && Vy > -MaxSpeed)
            Vy -= 100;
        else if (Vy >= 0 && Vy < MaxSpeed)
            Vy += 100;

        Bounces = 0;
    }

    else if (BallY <= 0 && Vy < 0)
    {
        Vy *= -1;
    }
    if (((BallX <= PlayerWidth && BallY + BallHeight >= Player1Y && BallY <= Player1Y + PlayerHeight) || BallX <= 0) && Vx < 0)
    {
        if (BallY >= Player1Y && BallY <= Player1Y + PlayerHeight)
        {
            Player1Score++;
            BallX = PlayerWidth;
        }
        else
        {
            Player1Score--;
        }

        Bounces++;
        Vx *= -1;
    }
    if (((BallX >= Player2X - PlayerWidth && BallY + BallHeight >= Player2Y && BallY <= Player2Y + PlayerHeight) || BallX >= MaxX) && Vx > 0)
    {
        if (BallY >= Player2Y && BallY <= Player2Y + PlayerHeight)
        {
            Player2Score++;
            BallX = Player2X - PlayerWidth;
            Bounces++;
        }
        else
        {
            Player2Score--;
            Bounces++;
        }
        Vx *= -1;
        Bounces++;
    }
}

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
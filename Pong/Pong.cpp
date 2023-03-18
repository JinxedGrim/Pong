#include "GameFunctions.h"
#include "Graphics/GdiPP.hpp"
#include "Graphics/WndCreator.hpp"

void CustomErr(std::string Str)
{
    MessageBoxA(0, Str.c_str(), "Error!", MB_OK);
}

int main()
{
    WndCreator Cmd = GetConsoleWindow();
    Cmd.Hide();

    // Create Brush and Pen
    HBRUSH ClearBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

    // Create Overlay
    WndCreator Window = WndCreator(CS_CLASSDC, GetModuleHandle(NULL), L"Mouse ESP", ClearBrush, 0, WS_POPUP | WS_VISIBLE, 0, 0, sx, sy);

    // Create gdi Object
    GdiPP Gdi = GdiPP(Window.Wnd, true);
    Gdi.ErrorHandler = CustomErr;
    std::thread Setting(Settings);

    PenPP DashedPen = PenPP(PS_DASH, 1, RGB(R, G, B));
    PenPP CurrentPen = PenPP(PS_SOLID, 2, RGB(R, G, B));
    BrushPP CurrentBrush = BrushPP(RGB(R, G, B));

    Gdi.ChangePen(CurrentPen);
    Gdi.ChangeBrush(CurrentBrush);

    // Init Variables
    MSG msg = { 0 };
    POINT p;

    while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) && !GetAsyncKeyState(VK_RETURN))
    {
        // Translate and Dispatch message to WindowProc
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Check Msg
        if (msg.message == WM_QUIT || msg.message == WM_CLOSE || msg.message == WM_DESTROY)
        {
            break;
        }

        // Ball Movement
        if (!Paused)
        {
            BallX += Vx;
            BallY += Vy;
        }

        // Out Of Bounds Y Detetcion
        if (BallY >= sy || BallY <= 0)
        {
            Vy *= -1;
        }

        // Controls
        if (GetAsyncKeyState(VK_DOWN))
        {
            if (Player2Y + PlayerHeight < sy)
                Player2Y += 5;
        }
        if (GetAsyncKeyState(VK_UP))
        {
            if (Player2Y > 0)
                Player2Y -= 5;
        }
        if (GetAsyncKeyState(0x53))
        {
            if (Player1Y + PlayerHeight < sy)
                Player1Y += 5;
        }
        if (GetAsyncKeyState(0x57))
        {
            if (Player1Y > 0)
                Player1Y -= 5;
        }

        // Reset Score
        if (GetAsyncKeyState(VK_BACK))
        {
            Player1Score = 0;
            Player2Score = 0;
        }

        // Colors
        if (GetAsyncKeyState(0x35) & 0x8000)
        {
            R += 1;
            if (R > 255)
            {
                R = 0;
            }
            //Gdi.ChangePen(PS_SOLID, 2, RGB(R, G, B));
            //Gdi.ChangeBrush(RGB(R, G, B));
            CurrentBrush = CreateSolidBrush(RGB(R, G, B));
            CurrentPen = CreatePen(PS_SOLID, 2, RGB(R, G, B));
            Gdi.ChangePen(CurrentPen);
            Gdi.ChangeBrush(CurrentBrush);
        }
        if (GetAsyncKeyState(0x36) & 0x8000)
        {
            G += 1;
            if (G > 255)
            {
                G = 0;
            }

            CurrentBrush = CreateSolidBrush(RGB(R, G, B));
            CurrentPen = CreatePen(PS_SOLID, 2, RGB(R, G, B));
            Gdi.ChangePen(CurrentPen);
            Gdi.ChangeBrush(CurrentBrush);
        }
        if (GetAsyncKeyState(0x37) & 0x8000)
        {
            B += 1;
            if (B > 255)
            {
                B = 0;
            }
            CurrentBrush = CreateSolidBrush(RGB(R, G, B));
            CurrentPen = CreatePen(PS_SOLID, 2, RGB(R, G, B));
            Gdi.ChangePen(CurrentPen);
            Gdi.ChangeBrush(CurrentBrush);
        }

        // Collision Detetction && Scoring
        if (((BallX <= PlayerWidth) && BallY >= Player1Y && BallY <= Player1Y + PlayerHeight) || BallX <= 0)
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
        if (((BallX >= Player2X - PlayerWidth) && BallY >= Player2Y && BallY <= Player2Y + PlayerHeight) || BallX >= MaxX)
        {
            if (BallY >= Player2Y && BallY <= Player2Y + PlayerHeight)
            {
                Player2Score++;
                BallX = Player2X - PlayerWidth;
            }
            else
            {
                Player2Score--;
            }
            Vx *= -1;
            Bounces++;
        }

        if (Bounces == BouncesTillIncrease)
        {
            if (Vx <= 0 && (Vx < 3 && Vx > -3))
                Vx -= 1;
            else
                Vx += 1;

            if (Vy <= 0 && (Vy < 3 && Vy > -3))
                Vy -= 1;
            else
                Vy += 1;

            Bounces = 0;
        }

        // Init Strings
        std::string P1Str = std::to_string(Player1Score);
        std::string P2Str = std::to_string(Player2Score);

        // Clear Screen
        Gdi.Clear(GDIPP_FILLRECT, ClearBrush);

        // Draw Game
        Gdi.ChangePen(DashedPen);
        Gdi.DrawLine(sx / 2, 0, sx / 2, sy);
        Gdi.ChangePen(CurrentPen);
        Gdi.DrawStringA(sx / 2 - 50, 50, P1Str, RGB(R, G, B), TRANSPARENT);
        Gdi.DrawStringA(sx / 2 + 50, 50, P2Str, RGB(R, G, B), TRANSPARENT);
        Gdi.DrawRectangle(Player1X, Player1Y, PlayerWidth, PlayerHeight);
        Gdi.DrawRectangle(Player2X, Player2Y, PlayerWidth, PlayerHeight);
        Gdi.DrawEllipse(BallX, BallY, BallWidth, BallHeight);
        Gdi.DrawDoubleBuffer();
    }

    Setting.detach();

    return 0;
}
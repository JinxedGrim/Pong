#include "GameFunctions.h"
#include "resource.h"
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

    WndIconW ICO = WndIconW(IDI_ICON1, CurrHinst);

    // Create Brush and Pen
    HBRUSH ClearBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

    // Create Overlay
    WndCreator Window = WndCreator(CS_OWNDC, L"Pong", L"Pong By JinxedGrim", LoadCursor(NULL, IDC_ARROW), NULL, ClearBrush, WndExModes::BorderLessEx, WndModes::BorderLess, 0, 0, sx, sy);

    // Create gdi Object
    GdiPP Gdi = GdiPP(Window.Wnd, true);
    Gdi.ErrorHandler = CustomErr;
    Window.ErrorHandler = CustomErr;
    std::thread Setting(Settings);

    PenPP DashedPen = PenPP(PS_DASH, 1, RGB(R, G, B));
    PenPP CurrentPen = PenPP(PS_SOLID, 2, RGB(R, G, B));
    BrushPP CurrentBrush = BrushPP(RGB(R, G, B));

    Gdi.ChangePen(CurrentPen);
    Gdi.ChangeBrush(CurrentBrush);

    Gdi.UpdateClientRgn();
    sx = Window.GetClientArea().Width;
    sy = Window.GetClientArea().Height;

    auto LastTime = std::chrono::system_clock::now();
    double DeltaTime = 0.0f;

    // Init Variables
    MSG msg = { 0 };

    while (!GetAsyncKeyState(VK_RETURN))
    {
        LastTime = std::chrono::system_clock::now();
        // Translate and Dispatch message to WindowProc
        PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE);

        // Translate and Dispatch message to WindowProc
        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        // Check Msg
        if (msg.message == WM_QUIT || msg.message == WM_CLOSE || msg.message == WM_DESTROY)
        {
            break;
        }

        // Ball Movement
        if (!Paused)
        {
            BallSubX += Vx * DeltaTime; // New var used for sub pixel moving
            BallSubY += Vy * DeltaTime;

            int BallMoveX = static_cast<int>(BallSubX + 0.5f);
            int BallMoveY = static_cast<int>(BallSubY + 0.5f);

            BallX += BallMoveX;
            BallY += BallMoveY;

            BallSubX -= BallMoveX;
            BallSubY -= BallMoveY;
        }

        // Controls
        if (GetAsyncKeyState(VK_DOWN))
        {
            if (Player2Y + PlayerHeight < sy)
            {
                Player2SubY += PlayerVelocity * DeltaTime;

                int Player2MoveY = static_cast<int>(Player2SubY + 0.5f);

                Player2Y += Player2MoveY;

                Player2SubY -= Player2MoveY;
            }
        }
        if (GetAsyncKeyState(VK_UP))
        {
            if (Player2Y > 0)
            {
                Player2SubY += PlayerVelocity * DeltaTime;

                int Player2MoveY = static_cast<int>(Player2SubY + 0.5f);

                Player2Y -= Player2MoveY;

                Player2SubY -= Player2MoveY;
            }
        }
        if (GetAsyncKeyState(0x53))
        {
            if (Player1Y + PlayerHeight < sy)
            {
                Player1SubY += PlayerVelocity * DeltaTime;

                int Player1MoveY = static_cast<int>(Player1SubY + 0.5f);

                Player1Y += Player1MoveY;

                Player1SubY -= Player1MoveY;
            }
        }
        if (GetAsyncKeyState(0x57))
        {
            if (Player1Y > 0)
            {
                Player1SubY += PlayerVelocity * DeltaTime;

                int Player1MoveY = static_cast<int>(Player1SubY + 0.5f);

                Player1Y -= Player1MoveY;

                Player1SubY -= Player1MoveY;
            }
        }

        // Reset Score
        if (GetAsyncKeyState(VK_BACK))
        {
            Player1Score = 0;
            Player2Score = 0;
        }

        if (GetAsyncKeyState(VK_INSERT) & 0x8000)
        {
            if (Fs)
            {
                Window.ResetStyle(WndModes::Windowed);
                Window.ResetStyleEx(WndExModes::WindowedEx);
                Gdi.UpdateClientRgn();
                sx = Window.GetClientArea().Width;
                sy = Window.GetClientArea().Height;
            }
            else
            {
                Window.ResetStyle(WndModes::BorderLess);
                Window.ResetStyleEx(WndExModes::BorderLessEx);
                Gdi.UpdateClientRgn();
                sx = Window.GetClientArea().Width;
                sy = Window.GetClientArea().Height;
            }
            Fs = !Fs;
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

        // Collision Detetction && Scoring && Speed
        CollisionDetection();

        // Init Strings
        std::string P1ScoreStr = std::to_string(Player1Score);
        std::string P2ScoreStr = std::to_string(Player2Score);
        std::string P1NameStr = "Player 1";
        std::string P2NameStr = "Player 2";
        std::string sxstr = "SpeedX: " + std::to_string(Vx);
        std::string systr = "SpeedY: " + std::to_string(Vy);

        // Clear Screen
        Gdi.Clear(GDIPP_FILLRECT, ClearBrush);

        // Helpful debug lines
        //Gdi.DrawStringA(20, 20, sxstr, RGB(R, G, B), TRANSPARENT);
        //Gdi.DrawStringA(20, 40, systr, RGB(R, G, B), TRANSPARENT);

        // Draw Game
        Gdi.ChangePen(DashedPen);
        Gdi.DrawLine(sx / 2, 0, sx / 2, sy);
        Gdi.ChangePen(CurrentPen);
        Gdi.DrawStringA(sx / 2 - 150, 50, P1NameStr, RGB(R, G, B), TRANSPARENT);
        Gdi.DrawStringA(sx / 2 + 100, 50, P2NameStr, RGB(R, G, B), TRANSPARENT);
        Gdi.DrawStringA(sx / 2 - 50, 50, P1ScoreStr, RGB(R, G, B), TRANSPARENT);
        Gdi.DrawStringA(sx / 2 + 50, 50, P2ScoreStr, RGB(R, G, B), TRANSPARENT);
        Gdi.DrawRectangle(Player1X, Player1Y, PlayerWidth, PlayerHeight);
        Gdi.DrawRectangle(Player2X, Player2Y, PlayerWidth, PlayerHeight);
        Gdi.DrawEllipse(BallX, BallY, BallWidth, BallHeight);
        Gdi.DrawDoubleBuffer();

        DeltaTime = std::chrono::duration<double>(std::chrono::system_clock::now() - LastTime).count();
    }

    Setting.detach();

    return 0;
}
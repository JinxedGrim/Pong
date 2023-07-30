#include "Networking/NetPP.h"
#include "GameFunctions.h"
#include "resource.h"
#include "Graphics/GdiPP.hpp"
#include "Graphics/WndCreator.hpp"

#define Port 9909

void CustomErr(std::string Str)
{
    MessageBoxA(0, Str.c_str(), "Error!", MB_OK);
}

int main()
{
    WndCreator Cmd = GetConsoleWindow();

    int choice = 0;
    std::cout << "Would you like to (1) host a session or (2) connect to a host?" << std::endl;
    std::cin >> choice;

    if (choice == 1)
    {
        // start host
        std::cout << "Waiting for a client. Press escape to cancel and quit." << std::endl;

        NetPP Server = NetPP(true, false, Port);
        Server.StartListening();

        while (!GetAsyncKeyState(VK_ESCAPE))
        {
            if (Server.ConnectedClients.size() != 0)
            {
                break;
            }
        }

        Server.StopListening();

        //Cmd.Hide();

        WndIconW ICO = WndIconW(IDI_ICON1, CurrHinst);

        // Create Brush and Pen
        HBRUSH ClearBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

        // Create Overlay
        WndCreator Window = WndCreator(CS_OWNDC, L"PongMp", L"Pong Multiplayer (Server) By JinxedGrim", LoadCursor(NULL, IDC_ARROW), NULL, ClearBrush, WndExModes::BorderLessEx, WndModes::BorderLess, 0, 0, sx, sy);

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

        // Init Variables
        MSG msg = { 0 };

        while (!GetAsyncKeyState(VK_BACK))
        {
            // Translate and Dispatch message to WindowProc
            PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE);

            // Check Msg
            if (msg.message == WM_QUIT || msg.message == WM_CLOSE || msg.message == WM_DESTROY)
            {
                break;
            }

            // Translate and Dispatch message to WindowProc
            TranslateMessage(&msg);
            DispatchMessageW(&msg);

            int Buffer[6] = { Player1Y, Player2Y, BallX, BallY, Player1Score, Player2Score };
            Server.SendToClient(Server.ConnectedClients[0].Sock, Buffer, sizeof(Buffer));

            int RecvBuff[1];
            if (Server.RecvFromClient(Server.ConnectedClients[0].Sock, RecvBuff, sizeof(RecvBuff)) == NETPP_CONNECTION_RESET)
            {
                MessageBoxA(0, "The client has disconnected. after this dialog Pong will exit", "Disconnected", MB_OK);
                Server.Shutdown();
                return 0;
            }
            Player2Y = RecvBuff[0];

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

            // Collision Detetction && Scoring
            if (((BallX <= PlayerWidth) && BallY >= Player1Y && BallY <= Player1Y + PlayerHeight) || BallX <= 0)
            {
                if (BallY >= Player1Y && BallY <= Player1Y + PlayerHeight)
                {
                    Player1Score++;
                    BallX = PlayerWidth;
                    Bounces++;
                }
                else
                {
                    Player1Score--;
                    Bounces++;
                }
                Vx *= -1;
            }
            if (((BallX >= Player2X - PlayerWidth) && BallY >= Player2Y && BallY <= Player2Y + PlayerHeight) || BallX >= MaxX)
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

            if (Bounces == BouncesTillIncrease && !(MaxSpeed < abs(Vx)))
            {
                if (Vx < 0 && Vx > -3)
                    Vx -= 1;
                else if ((Vx > 0 && Vx < 3))
                    Vx += 1;

                if (Vy < 0 && Vy > -3)
                    Vy -= 1;
                else if (Vy > 0 && Vy < 3)
                    Vy += 1;

                Bounces = 0;
            }

            // Init Strings
            std::string P1Str = std::to_string(Player1Score);
            std::string P2Str = std::to_string(Player2Score);
            //std::string sxstr = "SpeedX: " + std::to_string(Vx);
            //std::string systr = "SpeedY: " + std::to_string(Vy);

            // Clear Screen
            Gdi.Clear(GDIPP_FILLRECT, ClearBrush);

            // Draw Game
            Gdi.ChangePen(DashedPen);
            Gdi.DrawLine(sx / 2, 0, sx / 2, sy);
            Gdi.ChangePen(CurrentPen);
            Gdi.DrawStringA(sx / 2 - 50, 50, P1Str, RGB(R, G, B), TRANSPARENT);
            Gdi.DrawStringA(sx / 2 + 50, 50, P2Str, RGB(R, G, B), TRANSPARENT);
            //Gdi.DrawStringA(20, 20, sxstr, RGB(R, G, B), TRANSPARENT);
            //Gdi.DrawStringA(20, 40, systr, RGB(R, G, B), TRANSPARENT);
            Gdi.DrawRectangle(Player1X, Player1Y, PlayerWidth, PlayerHeight);
            Gdi.DrawRectangle(Player2X, Player2Y, PlayerWidth, PlayerHeight);
            Gdi.DrawEllipse(BallX, BallY, BallWidth, BallHeight);
            Gdi.DrawDoubleBuffer();
        }

        Setting.detach();
        Server.Shutdown();
        return 0;
    }
    else
    {
        NetPP Net = NetPP(false, false, Port);

        Cmd.Hide();

        WndIconW ICO = WndIconW(IDI_ICON1, CurrHinst);

        // Create Brush and Pen
        HBRUSH ClearBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

        // Create Overlay
        WndCreator Window = WndCreator(CS_OWNDC, L"Pong", L"Pong Multiplayer (Client) By JinxedGrim", LoadCursor(NULL, IDC_ARROW), NULL, ClearBrush, WndExModes::BorderLessEx, WndModes::BorderLess, 0, 0, sx, sy);

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

        // Init Variables
        MSG msg = { 0 };

        while (!GetAsyncKeyState(VK_BACK))
        {
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

            int Buffer[6]; // Player2x, Player2y, Ballx, Bally

            if (Net.RecvFromClient(Net.ClientSocket, Buffer, sizeof(Buffer)) == NETPP_CONNECTION_RESET)
            {
                MessageBoxA(0, "You have been disconnected from the server. after this dialog Pong will exit", "Disconnected", MB_OK);
                Net.Shutdown();
                return 0;
            }

            Player1Y = Buffer[0];
            Player2Y = Buffer[1];
            BallX = Buffer[2];
            BallY = Buffer[3];
            Player1Score = Buffer[4];
            Player2Score = Buffer[5];

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

            int SendBuff[1] = { Player2Y };
            Net.SendToClient(Net.ClientSocket, SendBuff, sizeof(SendBuff));

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

            // Init Strings
            std::string P1Str = std::to_string(Player1Score);
            std::string P2Str = std::to_string(Player2Score);
            //std::string sxstr = "SpeedX: " + std::to_string(Vx);
            //std::string systr = "SpeedY: " + std::to_string(Vy);

            // Clear Screen
            Gdi.Clear(GDIPP_FILLRECT, ClearBrush);

            // Draw Game
            Gdi.ChangePen(DashedPen);
            Gdi.DrawLine(sx / 2, 0, sx / 2, sy);
            Gdi.ChangePen(CurrentPen);
            Gdi.DrawStringA(sx / 2 - 50, 50, P1Str, RGB(R, G, B), TRANSPARENT);
            Gdi.DrawStringA(sx / 2 + 50, 50, P2Str, RGB(R, G, B), TRANSPARENT);
            //Gdi.DrawStringA(20, 20, sxstr, RGB(R, G, B), TRANSPARENT);
            //Gdi.DrawStringA(20, 40, systr, RGB(R, G, B), TRANSPARENT);
            Gdi.DrawRectangle(Player1X, Player1Y, PlayerWidth, PlayerHeight);
            Gdi.DrawRectangle(Player2X, Player2Y, PlayerWidth, PlayerHeight);
            Gdi.DrawEllipse(BallX, BallY, BallWidth, BallHeight);
            Gdi.DrawDoubleBuffer();
        }

        Setting.detach();
        Net.Shutdown();
        return 0;
    }
    return 0;
}
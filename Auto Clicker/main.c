#undef UNICODE

#include <Windows.h>
#include <CommCtrl.h>

#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ID_EDIT_INTERVAL 1
#define ID_BUTTON_START_CLICKING 2
#define ID_BUTTON_STOP_CLICKING 3
#define ID_BUTTON_BEEP 4

#define TEXT_BUFFER_SIZE 16

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ClickingThread(void* data);
BOOL CALLBACK SetFont(HWND child, LPARAM font);

HANDLE clickingThread;
HWND editInterval;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    WNDCLASS wc = { 0 };

    wc.lpszClassName = "Auto Clicker";
    wc.hInstance = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    RegisterClass(&wc);
    CreateWindow(wc.lpszClassName, wc.lpszClassName, WS_SYSMENU | WS_VISIBLE, 150, 150, 320, 200, 0, 0, hInstance, 0);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        int margin = 16;

        CreateWindow("static", "Interval (ms): ", WS_CHILD | WS_VISIBLE, margin, margin, 100, 20, hwnd, NULL, NULL, NULL);
        editInterval = CreateWindow("edit", "1000", WS_CHILD | WS_VISIBLE | WS_BORDER, margin + 100 + margin, margin, 50, 20, hwnd, ID_EDIT_INTERVAL, NULL, NULL);
        CreateWindow("button", "Start clicking", WS_CHILD | WS_VISIBLE, margin, margin + 20 + margin, 100, 25, hwnd, ID_BUTTON_START_CLICKING, NULL, NULL);
        CreateWindow("button", "Stop clicking", WS_CHILD | WS_VISIBLE, margin + 100 + margin, margin + 20 + margin, 100, 25, hwnd, ID_BUTTON_STOP_CLICKING, NULL, NULL);
        CreateWindow("button", "Beep!", WS_CHILD | WS_VISIBLE, margin, margin + 20 + margin + 25 + margin, 320 - margin * 3, 50, hwnd, ID_BUTTON_BEEP, NULL, NULL);

        EnumChildWindows(hwnd, SetFont, CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial"));
    }
    break;
    case WM_COMMAND:
        switch (wParam)
        {
        case ID_BUTTON_START_CLICKING:
            clickingThread = CreateThread(NULL, 0, ClickingThread, NULL, 0, NULL);
            break;
        case ID_BUTTON_STOP_CLICKING:
            TerminateThread(clickingThread, 0);
            break;
        case ID_BUTTON_BEEP:
            MessageBeep(MB_OK);
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI ClickingThread(void* data)
{
    while (1)
    {
        char buffer[TEXT_BUFFER_SIZE];
        GetWindowText(editInterval, buffer, TEXT_BUFFER_SIZE);
        Sleep(atoi(buffer));

        INPUT ip[2] = { 0 };
        ip[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        ip[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &ip[0], sizeof(INPUT));
        SendInput(1, &ip[1], sizeof(INPUT));
    }
}

BOOL CALLBACK SetFont(HWND child, LPARAM font)
{
    SendMessage(child, WM_SETFONT, font, TRUE);
    return TRUE;
}

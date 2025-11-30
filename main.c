#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <process.h>
#include <stdbool.h>
#include <stdio.h>

#define ID_BTN_START 101
#define ID_BTN_STOP  102
#define ID_EDIT_TEXT 103

volatile bool g_isRunning = false;
HANDLE g_hWorkerThread = NULL;
char g_payload[256] = "L33T HAXXOR";

void SendStringInput(const char* str) {
    while (*str) {
        INPUT inputs[2] = {0};
        WORD vk = VkKeyScan(*str);

        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = LOBYTE(vk);
        if (HIBYTE(vk) & 1) { 
             INPUT shift[2] = {0};
             shift[0].type = INPUT_KEYBOARD;
             shift[0].ki.wVk = VK_SHIFT;
             shift[1].type = INPUT_KEYBOARD;
             shift[1].ki.wVk = VK_SHIFT;
             shift[1].ki.dwFlags = KEYEVENTF_KEYUP;
             SendInput(1, &shift[0], sizeof(INPUT));
             
             inputs[0].type = INPUT_KEYBOARD;
             inputs[0].ki.wVk = LOBYTE(vk);
             inputs[1].type = INPUT_KEYBOARD;
             inputs[1].ki.wVk = LOBYTE(vk);
             inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
             SendInput(2, inputs, sizeof(INPUT));
             
             SendInput(1, &shift[1], sizeof(INPUT));
        } else {
             inputs[1].type = INPUT_KEYBOARD;
             inputs[1].ki.wVk = LOBYTE(vk);
             inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
             SendInput(2, inputs, sizeof(INPUT));
        }
        Sleep(2);
        str++;
    }

    Sleep(10);

    INPUT enterDown = {0};
    enterDown.type = INPUT_KEYBOARD;
    enterDown.ki.wVk = VK_RETURN;
    SendInput(1, &enterDown, sizeof(INPUT));

    Sleep(5);

    INPUT enterUp = {0};
    enterUp.type = INPUT_KEYBOARD;
    enterUp.ki.wVk = VK_RETURN;
    enterUp.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &enterUp, sizeof(INPUT));
}

unsigned __stdcall WorkerProc(void* pArgs) {
    Sleep(3000);
    while (g_isRunning) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            g_isRunning = false;
            break;
        }
        SendStringInput(g_payload);
        Sleep(10); 
    }
    _endthreadex(0);
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hEdit;
    switch(msg) {
        case WM_CREATE:
            hEdit = CreateWindow("EDIT", "L33T HAXXOR",
                                 WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                 20, 20, 240, 25,
                                 hwnd, (HMENU)ID_EDIT_TEXT,
                                 (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            CreateWindow("BUTTON", "[ LAUNCH NUKE ]",
                         WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         20, 60, 240, 30,
                         hwnd, (HMENU)ID_BTN_START,
                         (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            CreateWindow("BUTTON", "[ CEASE FIRE ]",
                         WS_TABSTOP | WS_VISIBLE | WS_CHILD,
                         20, 100, 240, 30,
                         hwnd, (HMENU)ID_BTN_STOP,
                         (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BTN_START) {
                if (!g_isRunning) {
                    GetWindowText(hEdit, g_payload, 255);
                    g_isRunning = true;
                    g_hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, WorkerProc, NULL, 0, NULL);
                    SetFocus(hwnd);
                }
            }
            else if (LOWORD(wParam) == ID_BTN_STOP) {
                g_isRunning = false;
            }
            break;

        case WM_DESTROY:
            g_isRunning = false;
            if (g_hWorkerThread) {
                WaitForSingleObject(g_hWorkerThread, 1000);
                CloseHandle(g_hWorkerThread);
            }
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "ChatNukeClass";
    WNDCLASS wc = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    if (!RegisterClass(&wc)) return 0;

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, ":: Chat_Nuke_Elite :: [SYSTEM READY]",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
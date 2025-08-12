// Notepad By Wai.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Notepad By Wai.h"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include <commdlg.h>
#include <string>

#include <fstream>

#define MAX_LOADSTRING 100

#ifndef WM_FINDREPLACE
#define WM_FINDREPLACE 0x0400 + 0x0C
#endif

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hEdit;                                     // Global edit for the hedit 

static BOOL g_bFullScreen = FALSE;
static RECT g_rcNormal = { 0 };

WCHAR szCurrentFile[MAX_PATH] = L"";


FINDREPLACE fr = { 0 };                         // Find and replace 
WCHAR szFindWhat[80] = L"";                     // Findable character
WCHAR szReplaceWhat[80] = L"";                  // Replaceable character
HWND hFindDlg = NULL;                           // Window handler of finding Dialog box
HWND hReplaceDlg = NULL;                        // Window handler of replacing Dialog box 


// Forward declarations of functions included in this code module:

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


void SaveWindowRect(HWND hWnd) {
    GetWindowRect(hWnd, &g_rcNormal);
}
void RestoreWindow(HWND hwnd)
{
    // Restore window style to normal overlapped window (title bar, borders)
    SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);

    // Restore saved window size and position
    SetWindowPos(hwnd, HWND_TOP,
        g_rcNormal.left,
        g_rcNormal.top,
        g_rcNormal.right - g_rcNormal.left,
        g_rcNormal.bottom - g_rcNormal.top,
        SWP_FRAMECHANGED | SWP_SHOWWINDOW);
}
void ToggleFullScreen(HWND hwnd)
{
    if (!g_bFullScreen)
    {
        // Save the current window position/size
        GetWindowRect(hwnd, &g_rcNormal);

        // Remove border and title bar
        SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

        // Get screen dimensions
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // Resize to full screen
        SetWindowPos(hwnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED);

        g_bFullScreen = TRUE;
    }
    else
    {
        // Restore normal style
        SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);

        // Restore original position
        SetWindowPos(hwnd, HWND_TOP,
            g_rcNormal.left, g_rcNormal.top,
            g_rcNormal.right - g_rcNormal.left,
            g_rcNormal.bottom - g_rcNormal.top,
            SWP_FRAMECHANGED);

        g_bFullScreen = FALSE;
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NOTEPADBYWAI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NOTEPADBYWAI));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    //return the message of msg.wParam to int
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NOTEPADBYWAI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_NOTEPADBYWAI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_FINDREPLACE:
    {
        LPFINDREPLACE lpfr = (LPFINDREPLACE)lParam;

        // Handle dialog closure
        if (lpfr->Flags & FR_DIALOGTERM) {
            if (hFindDlg == lpfr->hwndOwner) hFindDlg = NULL;
            if (hReplaceDlg == lpfr->hwndOwner) hReplaceDlg = NULL;
            return 0;
        }

        // Update global find/replace strings
        if (lpfr->lpstrFindWhat) wcscpy_s(szFindWhat, lpfr->lpstrFindWhat);
        if (lpfr->lpstrReplaceWith) wcscpy_s(szReplaceWhat, lpfr->lpstrReplaceWith);

        int length = GetWindowTextLength(hEdit);
        if (length <= 0) return 0;

        // Get edit text content
        wchar_t* buffer = new wchar_t[length + 1];
        GetWindowText(hEdit, buffer, length + 1);
        int findLen = wcslen(szFindWhat);

        // Get current selection
        DWORD selStart, selEnd;
        SendMessage(hEdit, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);

        if (lpfr->Flags & FR_FINDNEXT)
        {
            BOOL found = FALSE;
            int searchStart = (lpfr->Flags & FR_DOWN) ? selEnd : 0;
            int searchEnd = (lpfr->Flags & FR_DOWN) ? length : selStart;

            // Case-sensitive search function
            auto Search = [&](const wchar_t* start, const wchar_t* end) -> wchar_t* {
                for (const wchar_t* p = start; p <= end - findLen; p++) {
                    if (lpfr->Flags & FR_MATCHCASE) {
                        if (wcsncmp(p, szFindWhat, findLen) == 0) return (wchar_t*)p;
                    }
                    else {
                        if (_wcsnicmp(p, szFindWhat, findLen) == 0) return (wchar_t*)p;
                    }
                }
                return nullptr;
                };

            wchar_t* foundPos = nullptr;
            if (lpfr->Flags & FR_DOWN) {
                // Forward search
                foundPos = Search(buffer + searchStart, buffer + length);
                if (!foundPos && (lpfr->Flags & FR_WRAPAROUND)) {
                    foundPos = Search(buffer, buffer + searchStart);
                }
            }
            else {
                // Backward search
                foundPos = Search(buffer, buffer + searchEnd);
                if (foundPos) {
                    // Find last occurrence
                    wchar_t* lastFound = foundPos;
                    while ((foundPos = Search(lastFound + 1, buffer + searchEnd)) != nullptr) {
                        lastFound = foundPos;
                    }
                    foundPos = lastFound;
                }
                else if (lpfr->Flags & FR_WRAPAROUND) {
                    foundPos = Search(buffer + searchEnd, buffer + length);
                }
            }

            if (foundPos) {
                int startPos = foundPos - buffer;
                SendMessage(hEdit, EM_SETSEL, startPos, startPos + findLen);
                SendMessage(hEdit, EM_SCROLLCARET, 0, 0);
            }
            else {
                MessageBox(hWnd, L"Text not found", L"Find", MB_OK | MB_ICONINFORMATION);
            }
        }
        else if (lpfr->Flags & FR_REPLACE)
        {
            // Verify selection matches find string
            if (selEnd - selStart == findLen) {
                wchar_t* selText = new wchar_t[findLen + 1];
                SendMessage(hEdit, EM_SETSEL, 0, (LPARAM)selText);

                BOOL match;
                if (lpfr->Flags & FR_MATCHCASE) {
                    match = (wcscmp(selText, szFindWhat) == 0);
                }
                else {
                    match = (_wcsicmp(selText, szFindWhat) == 0);
                }

                if (match) {
                    SendMessage(hEdit, EM_REPLACESEL, TRUE, (LPARAM)szReplaceWhat);
                }
                delete[] selText;
            }
        }
        else if (lpfr->Flags & FR_REPLACEALL)
        {
            int replaceCount = 0;
            std::wstring content(buffer);
            size_t pos = 0;
            int replaceLen = wcslen(szReplaceWhat);

            while ((pos = (lpfr->Flags & FR_MATCHCASE)
                ? content.find(szFindWhat, pos)
                : _wcsnicmp(content.c_str() + pos, szFindWhat, findLen) ?
                content.find(szFindWhat, pos + 1) : pos) != std::wstring::npos)
            {
                content.replace(pos, findLen, szReplaceWhat);
                pos += replaceLen;
                replaceCount++;
            }

            if (replaceCount > 0) {
                SetWindowText(hEdit, content.c_str());
                wchar_t msg[64];
                swprintf(msg, 64, L"Replaced %d occurrences", replaceCount);
                MessageBox(hWnd, msg, L"Replace All", MB_OK);
            }
            else {
                MessageBox(hWnd, L"No occurrences found", L"Replace All", MB_OK);
            }
        }
        delete[] buffer;
    }
    break;
    case WM_CREATE:
    {
        hEdit = CreateWindowEx(
            0, L"EDIT",
            NULL,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
            0, 0, 0, 0,
            hWnd,
            (HMENU)1,
            hInst,
            NULL
        );
        HFONT hFont = CreateFont(
            18, 0,
            0, 0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            FIXED_PITCH | FF_MODERN,
            L"Consolas"
        );
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    }
    break;
    case WM_SIZE:
        MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            OPENFILENAME ofn = { 0 };
            wchar_t szFileName[MAX_PATH] = L"";
            WCHAR szCurrentFile[MAX_PATH] = L"";
            // Common initialization
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = szFileName;
            ofn.nMaxFile = MAX_PATH;
            // Parse the menu selections:
            switch (wmId)
            {
                //copying the text
                case IDM_EDIT_COPY:
                    SendMessage(hEdit, WM_COPY, 0, 0);
                    //MessageBox(NULL, L"Hello world", L"hello world", MB_OK);
                    break;
                case IDM_EDIT_CUT:
                    SendMessage(hEdit, WM_CUT, 0, 0);
                    //MessageBox(NULL, L"Hello world", L"hello world", MB_OK);
                    break;
                case IDM_EDIT_SELECTALL:
                    SendMessage(hEdit, EM_SETSEL, 0, -1);
                    //MessageBox(NULL, L"Hello world", L"hello world", MB_OK);
				    break;
                case IDM_EDIT_PASTE:
                    SendMessage(hEdit, WM_PASTE, 0, 0);
                    //MessageBox(NULL, L"Hello world", L"hello world", MB_OK);
                    break;
                case IDM_VIEW_FULL:
                    //MessageBox(NULL, L"Full window", L"Full window", MB_OKCANCEL);
                    ToggleFullScreen(hWnd);
                    break;
                //find the text 
                case IDM_EDIT_FIND:
                {
                    fr.lStructSize = sizeof(fr);
                    fr.hwndOwner = hWnd;
                    fr.lpstrFindWhat = szFindWhat;
                    fr.wFindWhatLen = 80;
                    fr.Flags = FR_DOWN;
                    hFindDlg = FindText(&fr);
                }
                    break;
                //replace the text 
                case IDM_EDIT_REPLACE:
                {
                    fr.lStructSize = sizeof(fr);
                    fr.hwndOwner = hWnd;
                    fr.lpstrFindWhat = szFindWhat;
                    fr.wFindWhatLen = 80;
                    fr.lpstrReplaceWith = szReplaceWhat;
                    fr.wReplaceWithLen = 80;
                    fr.Flags = FR_DOWN;
                    hReplaceDlg = ReplaceText(&fr);
                }
                    break;
                case IDM_VIEW_RESTORE:
                    RestoreWindow(hWnd);
                    break;
                case IDM_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
                case IDM_FILE_OPEN:
                    ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
                    if (GetOpenFileName(&ofn)) {
                        std::wifstream file(szFileName);
                        if (file) {
                            std::wstring content((std::istreambuf_iterator<wchar_t>(file)),
                                std::istreambuf_iterator<wchar_t>());
                            SetWindowText(hEdit, content.c_str());
                        }
                    }
                    break;
                case IDM_FILE_SAVE:
                    if (szCurrentFile[0] != L'\0') {
                        int length = GetWindowTextLength(hEdit);
                        wchar_t* buffer = new wchar_t[length + 1];
                        GetWindowText(hEdit, buffer, length + 1);
                        std::wofstream file(szCurrentFile);
                        if (file) {
                            file.write(buffer, length);
                        }
                    }
                    break;
                case IDM_FILE_NEW:
                    SetWindowText(hEdit, L"");
                    szCurrentFile[0] = L'\0';
                    break;
                case IDM_SAVE_AS:
                    ofn.Flags = OFN_OVERWRITEPROMPT;
                    if (GetSaveFileName(&ofn)) {
                        int length = GetWindowTextLength(hEdit);
                        wchar_t *buffer = new wchar_t[length+1];
                        GetWindowText(hEdit, buffer, length + 1);
                        std::wofstream file(szFileName);
                        if (file) {
							file.write(buffer, length);
							wcscpy_s(szCurrentFile, szFileName);
                        }
						delete[] buffer;
                    }
                break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

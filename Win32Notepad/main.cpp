#include <windows.h>

#include "resource.h"

constexpr wchar_t kWindowClassName[] = L"Win32Notepad.MainWindow";
constexpr int kEditorControlId = 1001;

bool HandleCommand(
    HWND hwnd,
    WORD commandId)
{
    switch (commandId)
    {
    case ID_FILE_NEW:
        // Implemented with document state in Lesson 5
        return true;

    case ID_FILE_OPEN:
        // Implemented with file loading in Lesson 6
        return true;

    case ID_FILE_SAVE:
        // Implemented with file saving in Lesson 6
        return true;

    case ID_FILE_SAVE_AS:
        // Implemented with file saving in Lesson 6
        return true;

    case ID_FILE_EXIT:
        SendMessageW(
            hwnd,
            WM_CLOSE,            // Request normal window closure
            0,                   // No additional command data
            0);                  // No additional command data
        return true;

    case ID_EDIT_UNDO:
        // Implemented with editing commands in Lesson 7
        return true;

    case ID_EDIT_CUT:
        // Implemented with editing commands in Lesson 7
        return true;

    case ID_EDIT_COPY:
        // Implemented with editing commands in Lesson 7
        return true;

    case ID_EDIT_PASTE:
        // Implemented with editing commands in Lesson 7
        return true;

    case ID_EDIT_DELETE:
        // Implemented with editing commands in Lesson 7
        return true;

    case ID_EDIT_SELECT_ALL:
        // Implemented with editing commands in Lesson 7
        return true;

    default:
        return false;
    }
}

LRESULT CALLBACK MainWindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: // Main window is being created
    {
        const auto* creation =
            reinterpret_cast<LPCREATESTRUCTW>(lParam);

        HWND editor = CreateWindowExW(
            WS_EX_CLIENTEDGE,    // Recessed border around the editor
            L"EDIT",             // Predefined Windows edit-control class
            L"",                 // Initially empty text
            WS_CHILD |           // Owned by the main window
            WS_VISIBLE |     // Display with the parent
            WS_TABSTOP |     // Participate in keyboard focus
            WS_VSCROLL |     // Display a vertical scrollbar
            WS_HSCROLL |     // Display a horizontal scrollbar
            ES_LEFT |        // Left-align the text
            ES_MULTILINE |   // Accept multiple lines
            ES_AUTOVSCROLL | // Follow text vertically
            ES_AUTOHSCROLL | // Scroll rather than wrap long lines
            ES_NOHIDESEL,    // Preserve inactive selections
            0,                   // Initial x-coordinate
            0,                   // Initial y-coordinate
            0,                   // Width assigned by WM_SIZE
            0,                   // Height assigned by WM_SIZE
            hwnd,                // Parent window
            reinterpret_cast<HMENU>(
                static_cast<INT_PTR>(kEditorControlId)),
            // Child-control identifier
            creation->hInstance, // Module creating the control
            nullptr);            // No creation data

        if (!editor)
            return -1;           // Abort main-window creation

        SendMessageW(
            editor,
            WM_SETFONT,          // Assign the display font
            reinterpret_cast<WPARAM>(
                GetStockObject(DEFAULT_GUI_FONT)),
            // System-owned GUI font
            TRUE);               // Repaint immediately

        return 0;
    }

    case WM_COMMAND: // Menu, accelerator or child-control command
    {
        const WORD commandId =
            LOWORD(wParam);      // Command or control identifier

        if (HandleCommand(hwnd, commandId))
            return 0;

        break;
    }

    case WM_SIZE: // Main client area has changed size
    {
        HWND editor = GetDlgItem(
            hwnd,
            kEditorControlId);

        if (editor)
        {
            RECT clientRect{};

            GetClientRect(
                hwnd,
                &clientRect);

            MoveWindow(
                editor,
                0,                              // Client left edge
                0,                              // Client top edge
                clientRect.right -
                clientRect.left,             // Full client width
                clientRect.bottom -
                clientRect.top,              // Full client height
                TRUE);                           // Repaint after resizing
        }

        return 0;
    }

    case WM_SETFOCUS: // Main window received keyboard focus
    {
        HWND editor = GetDlgItem(
            hwnd,
            kEditorControlId);

        if (editor)
            SetFocus(editor);     // Return input to the editor

        return 0;
    }

    case WM_DESTROY:              // Main window is being destroyed
        PostQuitMessage(0);       // End the message loop
        return 0;
    }

    return DefWindowProcW(
        hwnd,
        message,
        wParam,
        lParam);
}

int WINAPI wWinMain(
    HINSTANCE hInstance,          // Current executable module
    HINSTANCE,                    // Reserved; always nullptr
    PWSTR,                        // Command line without executable name
    int nCmdShow)                 // Requested initial display state
{
    WNDCLASSW windowClass{};

    windowClass.lpfnWndProc = MainWindowProc;      // Handles class messages
    windowClass.hInstance = hInstance;             // Registering module
    windowClass.hCursor = LoadCursorW(
        nullptr,                                   // Use a system cursor
        IDC_ARROW);                                // Standard arrow cursor
    windowClass.hbrBackground =
        reinterpret_cast<HBRUSH>(
            COLOR_WINDOW + 1);                     // System window colour
    windowClass.lpszMenuName =
        MAKEINTRESOURCEW(IDR_MAIN_MENU);           // Menu resource for the class
    windowClass.lpszClassName = kWindowClassName;  // Registered class name

    if (RegisterClassW(&windowClass) == 0)
        return 1;

    HWND mainWindow = CreateWindowExW(
        0,                          // No extended window styles
        kWindowClassName,           // Registered main-window class
        L"Win32 Notepad",           // Initial title-bar text
        WS_OVERLAPPEDWINDOW |       // Standard resizable top-level window
        WS_CLIPCHILDREN,        // Exclude child area from parent painting
        CW_USEDEFAULT,              // Let Windows choose the x-coordinate
        CW_USEDEFAULT,              // Let Windows choose the y-coordinate
        800,                        // Initial outer width
        600,                        // Initial outer height
        nullptr,                    // No parent window
        nullptr,                    // Use the class menu resource
        hInstance,                  // Module creating the window
        nullptr);                   // No creation data

    if (!mainWindow)
        return 1;

    HACCEL accelerators = LoadAcceleratorsW(
        hInstance,                                  // Module containing the table
        MAKEINTRESOURCEW(
            IDR_MAIN_ACCELERATORS));                 // Accelerator resource

    if (!accelerators)
        return 1;

    ShowWindow(
        mainWindow,
        nCmdShow);                   // Honour the requested startup state

    UpdateWindow(mainWindow);        // Paint immediately if required

    MSG message{};

    for (;;)
    {
        const BOOL result = GetMessageW(
            &message,
            nullptr,                 // Messages for this thread
            0,                       // No lower message filter
            0);                      // No upper message filter

        if (result == 0)             // WM_QUIT was retrieved
            break;

        if (result == -1)            // Message retrieval failed
            return 1;

        if (TranslateAcceleratorW(
            mainWindow,
            accelerators,
            &message) != 0)
        {
            continue;                // Accelerator produced a command
        }

        TranslateMessage(&message);  // Generate character messages
        DispatchMessageW(&message);  // Call the target window procedure
    }

    return static_cast<int>(message.wParam);
}
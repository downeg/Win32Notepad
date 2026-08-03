// Copyright (c) 2026 downeg
// Licensed under the MIT License. See LICENSE in the project root.

#include <windows.h>

constexpr wchar_t kWindowClassName[] = L"Win32Notepad.MainWindow";
constexpr int kEditorControlId = 1001;

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
            WS_EX_CLIENTEDGE,   // Recessed border around the editor
            L"EDIT",            // Predefined Windows edit-control class
            L"",                // Initially empty text
            WS_CHILD |          // Owned by the main window
            WS_VISIBLE |    // Display immediately with the parent
            WS_TABSTOP |    // Accept keyboard focus
            WS_VSCROLL |    // Display a vertical scrollbar
            WS_HSCROLL |    // Display a horizontal scrollbar
            ES_LEFT |       // Left-align the text
            ES_MULTILINE |  // Accept multiple lines
            ES_AUTOVSCROLL | // Scroll vertically as text grows
            ES_AUTOHSCROLL | // Scroll horizontally instead of wrapping
            ES_NOHIDESEL,   // Preserve selection when focus changes
            0,                  // Initial x-coordinate
            0,                  // Initial y-coordinate
            0,                  // Width set by WM_SIZE
            0,                  // Height set by WM_SIZE
            hwnd,               // Parent window
            reinterpret_cast<HMENU>(
                static_cast<INT_PTR>(kEditorControlId)),
            // Child-control identifier
            creation->hInstance, // Module creating the control
            nullptr);           // No additional creation data

        if (!editor)
            return -1;          // Abort creation of the main window

        SendMessageW(
            editor,
            WM_SETFONT,         // Assign the control's display font
            reinterpret_cast<WPARAM>(
                GetStockObject(DEFAULT_GUI_FONT)),
            // System-owned default GUI font
            TRUE);              // Repaint using the new font

        return 0;
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
                0,                              // Align with client left edge
                0,                              // Align with client top edge
                clientRect.right -
                clientRect.left,             // Fill client width
                clientRect.bottom -
                clientRect.top,              // Fill client height
                TRUE);                           // Repaint after resizing
        }

        return 0;
    }

    case WM_SETFOCUS: // Main window has received keyboard focus
    {
        HWND editor = GetDlgItem(
            hwnd,
            kEditorControlId);

        if (editor)
            SetFocus(editor);    // Direct input to the editing surface

        return 0;
    }

    case WM_DESTROY:             // Main window is being destroyed
        PostQuitMessage(0);      // End the thread's message loop
        return 0;
    }

    return DefWindowProcW(
        hwnd,
        message,
        wParam,
        lParam);
}

int WINAPI wWinMain(
    HINSTANCE hInstance,         // Current executable module
    HINSTANCE,                   // Reserved; always nullptr
    PWSTR,                       // Command line without executable name
    int nCmdShow)                // Requested initial display state
{
    WNDCLASSW windowClass{};

    windowClass.lpfnWndProc = MainWindowProc;      // Handles class messages
    windowClass.hInstance = hInstance;             // Registering module
    windowClass.hCursor = LoadCursorW(
        nullptr,                                  // Use a system cursor
        IDC_ARROW);                               // Standard arrow cursor
    windowClass.hbrBackground =
        reinterpret_cast<HBRUSH>(
            COLOR_WINDOW + 1);                    // System window colour
    windowClass.lpszClassName = kWindowClassName; // Registered class name

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
        nullptr,                    // No menu yet
        hInstance,                  // Module creating the window
        nullptr);                   // No additional creation data

    if (!mainWindow)
        return 1;

    ShowWindow(
        mainWindow,
        nCmdShow);                  // Honour the requested startup state

    UpdateWindow(mainWindow);       // Paint immediately if necessary

    MSG message{};

    for (;;)
    {
        const BOOL result = GetMessageW(
            &message,
            nullptr,                // Messages for this thread
            0,                      // No lower message filter
            0);                     // No upper message filter

        if (result == 0)            // WM_QUIT was retrieved
            break;

        if (result == -1)           // Message retrieval failed
            return 1;

        TranslateMessage(&message); // Generate character messages
        DispatchMessageW(&message); // Call the target window procedure
    }

    return static_cast<int>(message.wParam);
}
#include <windows.h>

#include <filesystem>
#include <string>

#include "resource.h"

constexpr wchar_t kWindowClassName[] = L"Win32Notepad.MainWindow";
constexpr int kEditorControlId = 1001;

struct ApplicationState
{
    HINSTANCE instance{};               // Module containing the application
    HWND editor{};                      // Multiline editing control
    std::filesystem::path currentPath;  // File represented by the document
    bool modified{};                    // Document differs from saved state
    bool suppressEditorNotifications{}; // Ignore programmatic text changes
};

ApplicationState* GetApplicationState(HWND hwnd) noexcept
{
    return reinterpret_cast<ApplicationState*>(
        GetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA));            // Per-window application pointer
}

std::wstring GetDocumentDisplayName(
    const ApplicationState& state)
{
    if (state.currentPath.empty())
        return L"Untitled";

    std::wstring filename =
        state.currentPath.filename().wstring();

    if (!filename.empty())
        return filename;

    return state.currentPath.wstring();
}

void UpdateWindowTitle(
    HWND hwnd,
    const ApplicationState& state)
{
    std::wstring title =
        GetDocumentDisplayName(state);

    if (state.modified)
        title += L" *";

    title += L" - Win32 Notepad";

    SetWindowTextW(
        hwnd,
        title.c_str());                  // Replace the frame caption
}

void SetDocumentModified(
    HWND hwnd,
    ApplicationState& state,
    bool modified)
{
    if (state.modified == modified)
        return;

    state.modified = modified;
    UpdateWindowTitle(hwnd, state);
}

void CreateNewDocument(
    HWND hwnd,
    ApplicationState& state)
{
    state.suppressEditorNotifications = true;

    SetWindowTextW(
        state.editor,
        L"");                            // Replace the editor contents

    state.suppressEditorNotifications = false;

    SendMessageW(
        state.editor,
        EM_SETMODIFY,                    // Reset the control's internal flag
        FALSE,                           // Mark the contents as unmodified
        0);                              // No additional message data

    state.currentPath.clear();
    state.modified = false;

    UpdateWindowTitle(hwnd, state);
    SetFocus(state.editor);
}

bool HandleCommand(
    HWND hwnd,
    ApplicationState& state,
    WORD commandId)
{
    switch (commandId)
    {
    case ID_FILE_NEW:
        CreateNewDocument(hwnd, state);
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
            WM_CLOSE,                    // Request the normal closure path
            0,                           // No additional message data
            0);                          // No additional message data
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
    case WM_NCCREATE: // Non-client creation is beginning
    {
        const auto* creation =
            reinterpret_cast<LPCREATESTRUCTW>(lParam);

        auto* state =
            static_cast<ApplicationState*>(
                creation->lpCreateParams);

        if (!state)
            return FALSE;                // Reject creation without state

        SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(state));
        // Attach state to this window

        return TRUE;                     // Permit creation to continue
    }

    case WM_CREATE: // Main-window client creation is beginning
    {
        ApplicationState* state =
            GetApplicationState(hwnd);

        if (!state)
            return -1;                   // Abort an invalid creation path

        state->editor = CreateWindowExW(
            WS_EX_CLIENTEDGE,            // Recessed border around the editor
            L"EDIT",                     // Predefined edit-control class
            L"",                         // Initially empty text
            WS_CHILD |                   // Owned by the main window
            WS_VISIBLE |             // Display with the parent
            WS_TABSTOP |             // Accept keyboard focus
            WS_VSCROLL |             // Show a vertical scrollbar
            WS_HSCROLL |             // Show a horizontal scrollbar
            ES_LEFT |                // Left-align the text
            ES_MULTILINE |           // Accept multiple lines
            ES_AUTOVSCROLL |         // Follow text vertically
            ES_AUTOHSCROLL |         // Scroll rather than wrap
            ES_NOHIDESEL,            // Preserve inactive selections
            0,                           // Initial x-coordinate
            0,                           // Initial y-coordinate
            0,                           // Width assigned by WM_SIZE
            0,                           // Height assigned by WM_SIZE
            hwnd,                        // Parent window
            reinterpret_cast<HMENU>(
                static_cast<INT_PTR>(
                    kEditorControlId)),  // Child-control identifier
            state->instance,             // Module creating the control
            nullptr);                    // No additional creation data

        if (!state->editor)
            return -1;                   // Abort main-window creation

        SendMessageW(
            state->editor,
            WM_SETFONT,                  // Assign the editor's display font
            reinterpret_cast<WPARAM>(
                GetStockObject(
                    DEFAULT_GUI_FONT)),  // System-owned interface font
            TRUE);                       // Repaint with the new font

        UpdateWindowTitle(hwnd, *state);
        return 0;
    }

    case WM_COMMAND: // Command or child-control notification
    {
        ApplicationState* state =
            GetApplicationState(hwnd);

        if (!state)
            break;

        const WORD identifier =
            LOWORD(wParam);              // Command or control identifier

        const WORD notification =
            HIWORD(wParam);              // Command source or notification

        const HWND source =
            reinterpret_cast<HWND>(
                lParam);                 // Sending control, when applicable

        if (source == state->editor &&
            identifier == kEditorControlId)
        {
            if (notification == EN_CHANGE &&
                !state->suppressEditorNotifications)
            {
                SetDocumentModified(
                    hwnd,
                    *state,
                    true);
            }

            return 0;
        }

        if (HandleCommand(
            hwnd,
            *state,
            identifier))
        {
            return 0;
        }

        break;
    }

    case WM_SIZE: // Main client area has changed size
    {
        ApplicationState* state =
            GetApplicationState(hwnd);

        if (state && state->editor)
        {
            RECT clientRect{};

            GetClientRect(
                hwnd,
                &clientRect);

            MoveWindow(
                state->editor,
                0,                       // Align with the client left edge
                0,                       // Align with the client top edge
                clientRect.right -
                clientRect.left,      // Fill the client width
                clientRect.bottom -
                clientRect.top,       // Fill the client height
                TRUE);                    // Repaint after resizing
        }

        return 0;
    }

    case WM_SETFOCUS: // Main window received keyboard focus
    {
        ApplicationState* state =
            GetApplicationState(hwnd);

        if (state && state->editor)
            SetFocus(state->editor);      // Return input to the editor

        return 0;
    }

    case WM_DESTROY: // Main window is being destroyed
        PostQuitMessage(0);               // End the thread's message loop
        return 0;

    case WM_NCDESTROY: // Final destruction of the window
    {
        ApplicationState* state =
            GetApplicationState(hwnd);

        if (state)
            state->editor = nullptr;      // Child handle is no longer valid

        SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            0);                           // Detach application state

        break;
    }
    }

    return DefWindowProcW(
        hwnd,
        message,
        wParam,
        lParam);
}

int WINAPI wWinMain(
    HINSTANCE hInstance,                  // Current executable module
    HINSTANCE,                            // Reserved; always nullptr
    PWSTR,                                // Command line without executable
    int nCmdShow)                         // Requested initial display state
{
    ApplicationState state{};
    state.instance = hInstance;

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
        MAKEINTRESOURCEW(
            IDR_MAIN_MENU);                        // Main menu resource
    windowClass.lpszClassName =
        kWindowClassName;                          // Registered class name

    if (RegisterClassW(&windowClass) == 0)
        return 1;

    HWND mainWindow = CreateWindowExW(
        0,                          // No extended window styles
        kWindowClassName,           // Registered main-window class
        L"Win32 Notepad",           // Temporary creation title
        WS_OVERLAPPEDWINDOW |       // Standard resizable top-level window
        WS_CLIPCHILDREN,        // Exclude child area from parent painting
        CW_USEDEFAULT,              // Let Windows choose the x-coordinate
        CW_USEDEFAULT,              // Let Windows choose the y-coordinate
        800,                        // Initial outer width
        600,                        // Initial outer height
        nullptr,                    // No parent window
        nullptr,                    // Use the class menu resource
        hInstance,                  // Module creating the window
        &state);                     // State passed into WM_NCCREATE

    if (!mainWindow)
        return 1;

    HACCEL accelerators = LoadAcceleratorsW(
        hInstance,                                  // Resource-owning module
        MAKEINTRESOURCEW(
            IDR_MAIN_ACCELERATORS));                 // Accelerator table

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
        DispatchMessageW(&message);  // Invoke the target window procedure
    }

    return static_cast<int>(message.wParam);
}
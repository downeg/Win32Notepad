# Win32Notepad

Win32Notepad is a small native Windows text editor written in modern C++ using the Win32 API.

The project was developed as the first practical application in a broader native Windows programming course. Its purpose is not to reproduce every feature of Microsoft Notepad, but to establish the core engineering patterns used by traditional Win32 desktop applications before progressing to more specialised projects.

The application is built directly on the Windows API without a GUI framework.

## Features

* Native Win32 top-level window
* Standard multiline `EDIT` control
* File, Edit, and Help menus
* Keyboard accelerators
* New, Open, Save, and Save As
* UTF-8 file output
* UTF-8 and UTF-16 little-endian file input
* Document modification tracking
* Unsaved-change protection
* Undo, Cut, Copy, Paste, Delete, and Select All
* Native Windows file dialogs
* Application icon and version resources
* About dialog
* Win32 error reporting
* Debug and Release build configurations

## Course

The complete Win32 API with C++ course is available at:
https://downeg.ie/#win32-api-with-c-1-1

The Win32Notepad project course is available at:
https://downeg.ie/#win32-api-with-c-4-1

The project course develops the application incrementally while explaining the Windows application model, Visual Studio workflow, message handling, controls, resources, application state, file I/O, debugging, and executable construction.

## Project Structure

The project is organised around a conventional native Windows application:

* `main.cpp` contains the application entry point, message loop, window procedure, document state, command routing, file handling, and encoding logic.
* `Win32Notepad.rc` contains the menu, accelerator table, icon, About dialog, and version resources.
* `resource.h` contains the symbolic resource and command identifiers.
* `Win32Notepad.vcxproj` contains the Visual Studio C++ project configuration.
* `Win32Notepad.sln` contains the Visual Studio solution.

## Requirements

* Windows 10 or Windows 11
* Visual Studio Community 2022 or later
* Desktop development with C++ workload
* Windows SDK
* C++20 or later

The project targets 64-bit Windows and uses the Unicode Win32 API explicitly.

## Building

1. Clone the repository.
2. Open `Win32Notepad.sln` in Visual Studio.
3. Select either `Debug | x64` or `Release | x64`.
4. Build the solution with **Build → Build Solution** or `Ctrl+Shift+B`.
5. Run with `F5` for debugging or `Ctrl+F5` without the debugger.

## Technical Overview

Application execution begins in `wWinMain()`. The program registers a custom main-window class, creates the top-level application window, loads the accelerator table, and enters the thread message loop.

The editing surface is an instance of the predefined Windows `EDIT` control class. Windows supplies its text-entry, selection, scrolling, clipboard, and undo behaviour. The application coordinates the control through messages and receives change notifications through `WM_COMMAND`.

Per-window application state is attached to the main window through `GWLP_USERDATA`. The state records:

* The application instance handle
* The editor control handle
* The active document path
* The document modification state
* Whether editor notifications are temporarily suppressed

Menus and accelerator keys converge on the same command identifiers. File and editing operations are therefore independent of whether the user invoked them through the mouse, keyboard, or menu mnemonics.

## Text Encoding

The editor uses UTF-16 internally because it is implemented with the Unicode Win32 API.

When opening files, the application accepts:

* UTF-8 without a byte-order mark
* UTF-8 with a byte-order mark
* UTF-16 little-endian with a byte-order mark

Files are saved as UTF-8 without a byte-order mark.

Line endings are normalised to Windows `CRLF` form when text is loaded into the editor.

The application deliberately rejects malformed Unicode, UTF-16 big-endian files, embedded null characters, and files that appear to contain binary data.

## Error Handling

Win32 API failures are captured immediately with `GetLastError()` and translated into readable messages using `FormatMessageW()`.

Common-dialog failures are reported through `CommDlgExtendedError()`, which allows the application to distinguish between an actual dialog failure and a normal user cancellation.

File operations update the active document state only after the requested read or write has completed successfully.

## Scope

Win32Notepad is intentionally compact. It demonstrates the foundations required for larger native Windows projects without expanding into a complete text-editor product.

Features such as the following are outside the scope of this introductory project:

* Search and replace
* Printing
* Multiple documents
* Settings persistence
* Recent-file lists
* Automatic legacy-encoding detection
* Configurable word wrapping
* Rich text
* Advanced undo history
* Large-file virtualisation

These omissions keep the project focused on the Win32 application model and allow the course to progress to applications involving binary data, custom rendering, memory mapping, and Portable Executable analysis.

## Licence

This project is licensed under the MIT License.

Copyright © `downeg`.

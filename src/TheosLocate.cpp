/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

// TheosPDF fork addition: "Locate on Board" sends the current text selection
// to a running TheosBV (OpenBoardView fork) via WM_COPYDATA, so the board
// centres+highlights the reference designator under the selection.
//
// Protocol (single source of truth: tools/theosbv-locate-sender.md in TheosBV):
//   - target: top-level window whose title starts with "TheosBV"
//   - SendMessage WM_COPYDATA with COPYDATASTRUCT{
//       dwData = 0x54424C43 ('TBLC'), cbData = strlen(utf8), lpData = utf8 refdes }
//   - returns 1 if TheosBV accepted it. TheosBV tokenises the text and matches
//     the exact refdes, ignoring surrounding value/footprint text. No focus steal.
//     TheosBV ignores payloads >= 512 bytes, so this is for short selections.
//
// Dependency-free (pure Win32) on purpose, to keep the fork a small, isolated,
// rebase-cheap patch. SumatraPDF selection strings are already UTF-8, which is
// exactly what the protocol wants, so no encoding conversion is needed.

#include <windows.h>

#include "TheosLocate.h"

struct FindTheosBVCtx {
    HWND hwnd;
};

static BOOL CALLBACK FindTheosBVProc(HWND h, LPARAM lp) {
    wchar_t title[256];
    if (IsWindowVisible(h) && GetWindowTextW(h, title, 256) && wcsncmp(title, L"TheosBV", 7) == 0) {
        ((FindTheosBVCtx*)lp)->hwnd = h;
        return FALSE; // found it, stop enumerating
    }
    return TRUE;
}

// utf8Refdes: the current selection (UTF-8). Returns true if a TheosBV window
// was found and it accepted the locate request.
bool LocateOnBoardTheosBV(const char* utf8Refdes) {
    if (!utf8Refdes || !*utf8Refdes) {
        return false;
    }
    FindTheosBVCtx ctx{nullptr};
    EnumWindows(FindTheosBVProc, (LPARAM)&ctx);
    if (!ctx.hwnd) {
        return false;
    }
    COPYDATASTRUCT cds{};
    cds.dwData = 0x54424C43;                  // 'TBLC' = TheosBV Locate
    cds.cbData = (DWORD)lstrlenA(utf8Refdes); // no trailing NUL
    cds.lpData = (PVOID)utf8Refdes;
    return SendMessageW(ctx.hwnd, WM_COPYDATA, 0, (LPARAM)&cds) == 1;
}

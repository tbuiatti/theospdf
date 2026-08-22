/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

// TheosPDF fork addition. See TheosLocate.cpp for the protocol.
// Sends utf8Refdes to a running TheosBV via WM_COPYDATA 'TBLC'.
// Returns true if a TheosBV window accepted the locate request.
bool LocateOnBoardTheosBV(const char* utf8Refdes);

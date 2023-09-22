#include <windows.h>
#include <WinBase.h>
#include <tchar.h>
#include <string>
#include "resource.h"
#include <Richedit.h>
#include <fstream>
#include <iostream>
#include <sstream>


wchar_t WinName[] = L"MainFrame";
wchar_t gl_filename[50];
HINSTANCE mainInst;
HWND hEdit;

void confirmExit();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL HandleButtons(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, HWND hEdit);

void SetHEditColor(const HWND& hwnd);

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE Prev,
	LPTSTR lpCmdLine,
	int nCmdShow)
{
	LoadLibrary(TEXT("Msftedit.dll"));
	HWND hwnd;
	MSG msg;
	WNDCLASS w;
	static HWND createButton;
	static HWND editButton;
	static HWND deleteButton;
	static HWND exitButton;
	memset(&w, 0, sizeof(WNDCLASS));
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.lpfnWndProc = WndProc;
	w.hInstance = mainInst;
	w.hbrBackground = CreateSolidBrush(0x00FFFFFF);
	w.lpszClassName = L"MyClass";
	w.lpszMenuName = MAKEINTRESOURCE(IDC_LAB1OSSP);
	if (!RegisterClass(&w)) return 0;


	hwnd = CreateWindow(L"MyClass", L"Editor",
		WS_OVERLAPPEDWINDOW,
		500, 300, 500, 380,
		NULL, NULL, mainInst, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void confirmExit()
{
	int msgboxID = MessageBox(
		NULL,
		L"Do you want to exit?",
		L"Exit",
		MB_ICONEXCLAMATION | MB_YESNO
	);

	if (msgboxID == IDYES)
	{
		exit(0);
	}

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{


	switch (message)
	{
	case WM_CREATE:
		hEdit = CreateWindow(MSFTEDIT_CLASS, L"Edit me",
			WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 500, 600,
			hwnd, 0, mainInst, NULL);


	case WM_COMMAND:
		HandleButtons(hwnd, message, wParam, lParam, hEdit);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

BOOL HandleButtons(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, HWND hEdit)
{
	std::wstring str;
	std::wstring fileName(MAX_PATH, L'\0');
	std::wofstream ofile(fileName);
	std::wifstream ifile(fileName);
	std::wstringstream buf;
	OPENFILENAME ofn{};
	std::wstring file_content;
	int len;

	switch LOWORD(wParam) {
	case IDC_SAVE_BUTTON:
		len = GetWindowTextLength(hEdit);
		str.resize(len);
		GetWindowText(hEdit, &str[0], len+1);

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
		ofn.lpstrFile = &fileName[0];
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_OVERWRITEPROMPT;
		
		if (!GetSaveFileName(&ofn)) {
			MessageBox(hwnd, L"Cant get filename", L"Error", MB_ICONINFORMATION);
			return FALSE;
		}
			
		ofile.open(fileName);
		if (!ofile.is_open())
			throw std::domain_error("Failed to open the file");
		
		ofile << str;
		ofile.close();
		break;
	case IDC_OPEN_BUTTON:
		len = GetWindowTextLength(hEdit);
		str.resize(len);
		GetWindowText(hEdit, &str[0], len + 1);

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hEdit;
		ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
		ofn.lpstrFile = &fileName[0];
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

		if (!GetOpenFileName(&ofn)) {
			MessageBox(hwnd, L"Cant get filename", L"Error", MB_ICONINFORMATION);
			return FALSE;
		}
		
		ifile.open(fileName);

		buf << ifile.rdbuf();
		ifile.close();
		file_content = buf.str();

		SetWindowTextW(hEdit, file_content.c_str());

		break;
	case ID_STYLES_TEXT: {
		LOGFONT lf{};
		CHOOSEFONT cf{ 0 };

		cf.lStructSize = sizeof(cf);
		cf.hwndOwner = hwnd;
		cf.lpLogFont = &lf;
		cf.Flags = CF_EFFECTS | CF_SCREENFONTS;

		if (ChooseFont(&cf)) {
			HFONT hfont = CreateFontIndirect(cf.lpLogFont);
			SendMessage(hEdit, WM_SETFONT, (WPARAM)hfont, TRUE);

			CHARFORMAT cfm;
			cfm.cbSize = sizeof(cfm);
			SendMessage(hEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfm);
			cfm.dwEffects = CFE_BOLD;
			cfm.crTextColor = cf.rgbColors;
			cfm.dwMask = CFM_BOLD | CFM_COLOR;
			SendMessage(hEdit, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cfm);
		}
	}
		
		break;
	case ID_STYLES_BG:
		SetHEditColor(hwnd);
		break;
	case IDM_EXIT:
		confirmExit();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hwnd, message, wParam, lParam);

	}
	return TRUE;

}

void SetHEditColor(const HWND& hwnd)
{
	static COLORREF acrCustClr[16];
	CHOOSECOLOR cc{ 0 };
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hwnd;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	if (ChooseColor(&cc)) {
		SendMessage(hEdit, EM_SETBKGNDCOLOR, FALSE, (LPARAM)cc.rgbResult);
	}
}

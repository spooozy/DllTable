#include "pch.h"
#include "Cell.h"

Cell::Cell(std::wstring text, bool type) {
	this->text = text;
	this->type = type;
}

void Cell::draw(HDC hdc) {

	HBRUSH hBrush = CreateSolidBrush(color);
	HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);
	RECT rect = { x, y, x + width, y + height };
	SIZE textSize;
	std::wstring buffer(100, L'\0');
	int dif, letterSize;

	GetTextExtentPoint32(hdc, text.c_str(), text.length(), &textSize);
	letterSize = textSize.cx / text.length();
	dif = textSize.cx - width;
	if (dif > 0) buffer = text.substr(0, text.length() - dif / letterSize - 3) + L"...";
	else buffer = text;

	FillRect(hdc, &rect, hBrush);
	SetBkMode(hdc, color);

	if (type)
		TextOut(hdc, x + 1, y, buffer.c_str(), buffer.size());
	else
	{
		SetWindowText(textEdit, text.c_str());
		SetWindowPos(textEdit, NULL, x, y, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
		ShowWindow(textEdit, SW_SHOW);
	}

	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
}

void Cell::setColor(COLORREF newColor) {
	color = newColor;
}

void Cell::setCellType(bool newType) {
	this->type = newType;
}

bool Cell::getCellType() {
	return type;
}

void Cell::setupEditProc() {
	oldEditProc = (WNDPROC)SetWindowLongPtr(textEdit, GWLP_WNDPROC, (LONG_PTR)EditProc);
	SetWindowLongPtr(textEdit, GWLP_USERDATA, (LONG_PTR)this);
}

LRESULT CALLBACK Cell::EditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	Cell* cell = (Cell*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch (msg) {
	case WM_KEYDOWN: {
		if (GetKeyState(VK_CONTROL) & 0x8000 && GetKeyState(VK_SHIFT) & 0x8000 && wParam == 'T')
		{
			cell->type = true;
			int length = GetWindowTextLength(cell->textEdit);
			std::wstring buffer(length, L'\0');
			GetWindowText(cell->textEdit, &buffer[0], length + 1);
			cell->text = buffer;
			DestroyWindow(cell->textEdit);
		}
		break;
	}
	}
	return CallWindowProc(cell->oldEditProc, hWnd, msg, wParam, lParam);
}

Cell::~Cell()
{
	if (textEdit) {
		SetWindowLongPtr(textEdit, GWLP_WNDPROC, (LONG_PTR)oldEditProc);
		DestroyWindow(textEdit);
	}
}
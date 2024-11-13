#pragma once
#include <windows.h>
#include <string>

class Cell {
public:
    std::wstring text;
    HWND textEdit;
    int x, y, width, height;
    Cell(std::wstring text, bool type);
    ~Cell();
    void draw(HDC hdc);
    void setColor(COLORREF newColor);
    bool getCellType();
    void setCellType(bool newType);
    void setupEditProc();

private:

    COLORREF color;
    WNDPROC oldEditProc;
    bool type; //true = static, false = edit    
    static LRESULT CALLBACK EditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
#pragma once
#include <windows.h>
#include <string>
#include <vector>

class Header {
public:
    Header(int tableColsNum);
    void addHeaderItem(std::wstring newItem);
    void drawHeader(HDC hdc, int itemWidth, int visibleCols, int difX);
    int updateHeaderHeight(int tableHeight);
    int getHeaderHeight();
    int getHeaderWidth();
    ~Header();

private:
    HWND parentWindow;
    HDC hdc;
    int id, tableColsNum, minHeaderHeight = 30, minHeaderWidth = 80, height = 40, width = 0, visibleItems = 0;
    static int colNum;
    std::vector<std::wstring> headerItems;
};
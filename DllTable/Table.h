#pragma once
#include "Cell.h"
#include "Header.h"
#include <vector>

extern "C" __declspec(dllexport)  void Draw_Table(void* p, HDC hdc, int scrollPos);
extern "C" __declspec(dllexport)  void* Create_Table(HWND parentWindow, int tableWidth, int tableHeight, int rows, int cols);
extern "C" __declspec(dllexport)  void Destroy_Table(void* tableptr);
extern "C" __declspec(dllexport)  void Update_Table(void* tablePtr, int width, int height);
extern "C" __declspec(dllexport)  int Rows_Left(void* tableptr);
extern "C" __declspec(dllexport)  void Handle_Left_Click(void* tableptr, int clickPosX, int clickPosY, const std::wstring& time);
extern "C" __declspec(dllexport)  void Handle_Dbl_Left_Click(void* tableptr, int clickPosX, int clickPosY, const std::wstring& time);
extern "C" __declspec(dllexport)  void Handle_Right_Click(void* tableptr, int clickPosX, int clickPosY, const std::wstring& time);
extern "C" __declspec(dllexport)  void Handle_Dbl_Right_Click(void* tableptr, int clickPosX, int clickPosY, const std::wstring& time);
extern "C" __declspec(dllexport)  void Start_Click_Timer(void* tableptr);
extern "C" __declspec(dllexport)  void Stop_Click_Timer(void* tableptr);
extern "C" __declspec(dllexport)  void Start_R_Click_Timer(void* tableptr);
extern "C" __declspec(dllexport)  void Stop_R_Click_Timer(void* tableptr);
extern "C" __declspec(dllexport)  void Change_Cell_Type(void* tableptr);
extern "C" __declspec(dllexport)  int Get_Click_Timer_ID();
extern "C" __declspec(dllexport)  int Get_R_Click_Timer_ID();


class Table {
public:
	HWND parentWindow;	HDC hdc;
	int tableWidth, tableHeight;

	static const int CLICK_TIMER_ID = 1;
	static const int RCLICK_TIMER_ID = 2;
	bool singleClickPending = false, doubleLeftClicked = false;
	bool singleRClickPending = false, doubleRightClicked = false;

	Table(HWND parentWindow, int tableWidth, int tableHeight, int rows, int cols);
	~Table();

	void drawTable(HDC hdc, int scrollPos);
	int rowsLeft();
	void handleLeftClick(int clickPosX, int clickPosY, const std::wstring& time);
	void handleDblLeftClick(int clickPosX, int clickPosY, const std::wstring& time);
	void handleRightClick(int clickPosX, int clickPosY, const std::wstring& time);
	void handleDblRightClick(int clickPosX, int clickPosY, const std::wstring& time);
	void updateTableParams(int newTableWidth, int newTableHeight);
	void startClickTimer();
	void stopClickTimer();
	void startRClickTimer();
	void stopRClickTimer();
	void changeCellType();

private:
	std::vector<std::vector<Cell>>cells;
	Header* header;
	int rows, cols, cellWidth, cellHeight, scrollPos = 0, visibleRows, visibleCols, minCellWidth = 10, minCellHeight = 15,
		activeCellI = 0, activeCellJ, headerHeight = 40;
	bool type = true;
	void createCells();
	void initHeaderItems();
	void setVisibleCells();
	void updateCellsParams(int cols, int rows);
	void drawTableBorders(HDC hdc);
	void changeActiveCell(int row, int col, COLORREF color);
};


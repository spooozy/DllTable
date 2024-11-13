#include "pch.h"
#include "Table.h"

Table::Table(HWND parentWindow, int tableWidth, int tableHeight, int rows, int cols) {
	this->parentWindow = parentWindow;
	this->tableWidth = tableWidth;
	this->tableHeight = tableHeight;
	this->rows = rows;
	this->cols = cols;
	createCells();
	header = new Header(cols);
	initHeaderItems();
};

void Table::createCells() {
	cells.resize(rows, std::vector<Cell>(cols, Cell(L"", true)));
	for (int i = 0;i < rows;i++) {
		for (int j = 0;j < cols;j++) {
			cells[i][j] = Cell(L"Cell " + std::to_wstring(i) + L"," + std::to_wstring(j), type);
			cells[i][j].setColor(RGB(255, 255, 255));
			if (!type)
			{
				cells[i][j].textEdit = CreateWindowEx(0, L"EDIT", NULL, WS_VISIBLE | WS_CHILD | ES_MULTILINE, 0, 0, 1, 1, parentWindow, NULL, NULL, NULL);
				SetWindowText(cells[i][j].textEdit, cells[i][j].text.c_str());
				cells[i][j].setupEditProc();
				SendMessage(cells[i][j].textEdit, EM_SETLIMITTEXT, 0, 0);
			}
		}
	}
}

void Table::initHeaderItems() {
	for (int i = 0;i < cols;i++)
	{
		header->addHeaderItem(L"Header ¹ " + std::to_wstring(i));
	}
}

void Table::updateTableParams(int newTableWidth, int newTableHeight)
{
	tableWidth = newTableWidth;
	tableHeight = newTableHeight;
	updateCellsParams(cols, rows);
	setVisibleCells();
	updateCellsParams(visibleCols, visibleRows);
}

void Table::drawTable(HDC hdc, int newScrollPos)
{
	scrollPos = newScrollPos;
	headerHeight = header->updateHeaderHeight(tableHeight);
	int rightBorder = tableWidth, bottomBorder = tableHeight, difY = 0, dif = 0, difX;

	difY = tableHeight - 1 - visibleRows * (cellHeight + 1) - headerHeight;
	difX = tableWidth - 1 - visibleCols * (cellWidth + 1);

	for (int i = 0;i < visibleRows;i++)
		for (int j = 0;j < visibleCols;j++)
		{
			cells[i + scrollPos][j].x = 1 + j * (cellWidth + 1);
			cells[i + scrollPos][j].y = 1 + i * (cellHeight + 1) + headerHeight;
			cells[i + scrollPos][j].width = cellWidth;
			cells[i + scrollPos][j].height = cellHeight;
		}

	if (difX != 0)
	{
		dif = difX;
		for (int j = visibleCols - 1;j >= visibleCols - difX;j--) {
			for (int i = 0;i < visibleRows;i++)
			{
				cells[i + scrollPos][j].x += dif;
				cells[i + scrollPos][j].width++;
			}
			dif--;
		}
	}

	if (difY != 0)
	{
		dif = difY;
		for (int i = visibleRows - 1;i >= visibleRows - difY;i--) {
			for (int j = 0;j < visibleCols;j++)
			{
				cells[i + scrollPos][j].y += dif;
				cells[i + scrollPos][j].height++;
			}
			dif--;
		}
	}

	for (int i = 1;i < visibleCols - difX;i++)
	{
		MoveToEx(hdc, i * (cellWidth + 1), 0, nullptr);
		LineTo(hdc, i * (cellWidth + 1), tableHeight);
	}
	if (difX != 0)
	{
		dif = 1;
		for (int i = visibleCols - difX;i <= visibleCols;i++)
		{
			MoveToEx(hdc, i * (cellWidth + 1) + dif, 0, nullptr);
			LineTo(hdc, i * (cellWidth + 1) + dif, tableHeight);
			dif++;
		}
	}

	for (int i = 1;i < visibleRows - difY;i++)
	{
		MoveToEx(hdc, 0, i * (cellHeight + 1) + headerHeight, nullptr);
		LineTo(hdc, tableWidth, i * (cellHeight + 1) + headerHeight);
	}

	if (difY != 0)
	{
		dif = 1;
		for (int i = visibleRows - difY;i <= visibleRows;i++)
		{
			MoveToEx(hdc, 0, i * (cellHeight + 1) + headerHeight + dif, nullptr);
			LineTo(hdc, tableWidth, i * (cellHeight + 1) + headerHeight + dif);
			dif++;
		}
	}

	header->drawHeader(hdc, cellWidth, visibleCols, difX);

	for (int i = 0;i < scrollPos;i++)
		for (int j = 0;j < visibleCols;j++)
			ShowWindow(cells[i][j].textEdit, SW_HIDE);

	for (int i = 0;i < visibleRows;i++) {
		for (int j = 0;j < visibleCols;j++)
			cells[i + scrollPos][j].draw(hdc);
	}

	drawTableBorders(hdc);
}

void Table::setVisibleCells() {
	visibleRows = (tableHeight - 1 - headerHeight - 1) / (cellHeight + 1);
	if (visibleRows > rows) visibleRows = rows;
	visibleCols = (tableWidth - 1) / (cellWidth + 1);
	if (visibleCols > cols)visibleCols = cols;
}

void Table::updateCellsParams(int cols, int rows) {
	int tempWidth = (tableWidth - 1 - cols) / cols;
	if (tempWidth < minCellWidth) cellWidth = minCellWidth;
	else cellWidth = tempWidth;

	int tempHeight = (tableHeight - 1 - rows - headerHeight - 1) / rows;
	if (tempHeight < minCellHeight) cellHeight = minCellHeight;
	else cellHeight = tempHeight;
}

void Table::drawTableBorders(HDC hdc) {
	MoveToEx(hdc, 0, 0, nullptr);
	LineTo(hdc, tableWidth, 0);

	MoveToEx(hdc, 0, tableHeight - 1, nullptr);
	LineTo(hdc, tableWidth, tableHeight - 1);

	MoveToEx(hdc, 0, 0, nullptr);
	LineTo(hdc, 0, tableHeight);

	MoveToEx(hdc, 0, headerHeight, nullptr);
	LineTo(hdc, tableWidth, headerHeight);

	MoveToEx(hdc, tableWidth - 1, 0, nullptr);
	LineTo(hdc, tableWidth - 1, tableHeight);
}

void Table::handleLeftClick(int clickPosX, int clickPosY, const std::wstring& time) {
	if (clickPosY > headerHeight + 1)
	{
		/*int col = (clickPosX - 1) / (cellWidth + 1);
		int row = (clickPosY - 1 - headerHeight) / (cellHeight + 1);
		if (row >= 0 && row < visibleRows && col >= 0 && col < visibleCols) {
			changeActiveCell(row, col, RGB(248, 225, 255));
			std::wstring msg = L"Ñlicked on cell (" + std::to_wstring(row) + L", " + std::to_wstring(col) + L")\n"
				+ L"Time: " + time;
			MessageBox(NULL, msg.c_str(), L"WM_LBUTTONDOWN", MB_OK | MB_TASKMODAL);
		}*/
		for (int i = 0;i < visibleRows;i++)
			for (int j = 0;j < visibleCols;j++)
			{
				if (clickPosX >= cells[i + scrollPos][j].x && clickPosX <= cells[i + scrollPos][j].x + cells[i + scrollPos][j].width &&
					clickPosY >= cells[i + scrollPos][j].y && clickPosY <= cells[i + scrollPos][j].y + cells[i + scrollPos][j].height)
				{
					changeActiveCell(i + scrollPos, j, RGB(248, 225, 255));
					std::wstring msg = L"Ñlicked on cell (" + std::to_wstring(i + scrollPos) + L", " + std::to_wstring(j) + L")\n"
						+ L"Time: " + time;
					MessageBox(NULL, msg.c_str(), L"WM_LBUTTONDOWN", MB_OK | MB_TASKMODAL);
				}
			}
	}
}

void Table::handleDblLeftClick(int clickPosX, int clickPosY, const std::wstring& time) {
	if (clickPosY > headerHeight + 1)
	{
		/*int col = (clickPosX - 1) / (cellWidth + 1);
		int row = (clickPosY - 1 - headerHeight) / (cellHeight + 1);
		if (row >= 0 && row < visibleRows && col >= 0 && col < visibleCols) {
			changeActiveCell(row, col, RGB(212, 225, 213));
			std::wstring msg = L"Double clicked on cell (" + std::to_wstring(row) + L", " + std::to_wstring(col) + L")\n"
				+ L"Time: " + time;
			MessageBox(NULL, msg.c_str(), L"WM_LBUTTONDBLDOWN", MB_OK | MB_TASKMODAL);
		}*/
		for (int i = 0;i < visibleRows;i++)
			for (int j = 0;j < visibleCols;j++)
			{
				if (clickPosX >= cells[i + scrollPos][j].x && clickPosX <= cells[i + scrollPos][j].x + cells[i + scrollPos][j].width &&
					clickPosY >= cells[i + scrollPos][j].y && clickPosY <= cells[i + scrollPos][j].y + cells[i + scrollPos][j].height)
				{
					changeActiveCell(i + scrollPos, j, RGB(212, 225, 213));
					std::wstring msg = L"Double clicked on cell (" + std::to_wstring(i + scrollPos) + L", " + std::to_wstring(j) + L")\n"
						+ L"Time: " + time;
					MessageBox(NULL, msg.c_str(), L"WM_LBUTTONDBLDOWN", MB_OK | MB_TASKMODAL);
				}
			}
	}
}

void Table::handleRightClick(int clickPosX, int clickPosY, const std::wstring& time) {
	if (clickPosY < headerHeight + 1)
	{
		int col = (clickPosX - 1) / (cellWidth + 1);
		if (col >= 0 && col < visibleCols) {
			std::wstring msg = L"Clicked on header ¹" + std::to_wstring(col) + L"\n"
				+ L"Time: " + time;
			MessageBox(NULL, msg.c_str(), L"WM_RBUTTONDOWN", MB_OK | MB_TASKMODAL);
		}
	}
}

void Table::handleDblRightClick(int clickPosX, int clickPosY, const std::wstring& time) {
	if (clickPosY < headerHeight + 1)
	{
		int col = (clickPosX - 1) / (cellWidth + 1);
		if (col >= 0 && col < visibleCols) {
			std::wstring msg = L"Double clicked on header ¹" + std::to_wstring(col) + L"\n"
				+ L"Time: " + time;
			MessageBox(NULL, msg.c_str(), L"WM_LBUTTONDBLDOWN", MB_OK | MB_TASKMODAL);
		}
	}
}

void Table::changeActiveCell(int row, int col, COLORREF color) {
	RECT newActive = { cells[row][col].x, cells[row][col].y , cells[row][col].x + cells[row][col].width,  cells[row][col].y + cells[row][col].height };
	RECT oldActive = { cells[activeCellI][activeCellJ].x, cells[activeCellI][activeCellJ].y,  cells[activeCellI][activeCellJ].x + cells[activeCellI][activeCellJ].width , cells[activeCellI][activeCellJ].y + cells[activeCellI][activeCellJ].height };
	cells[activeCellI][activeCellJ].setColor(RGB(255, 255, 255));
	activeCellI = row + scrollPos;
	activeCellJ = col;
	cells[activeCellI][activeCellJ].setColor(color);
	InvalidateRect(parentWindow, &oldActive, true);
	InvalidateRect(parentWindow, &newActive, true);
}

void Table::startClickTimer() {
	SetTimer(parentWindow, CLICK_TIMER_ID, 200, NULL);
}

void Table::stopClickTimer() {
	KillTimer(parentWindow, CLICK_TIMER_ID);
	singleClickPending = false;
}

void Table::startRClickTimer() {
	SetTimer(parentWindow, RCLICK_TIMER_ID, 200, NULL);
}

void Table::stopRClickTimer() {
	KillTimer(parentWindow, RCLICK_TIMER_ID);
	singleRClickPending = false;
}

void Table::changeCellType()
{
	RECT rect = { cells[activeCellI][activeCellJ].x, cells[activeCellI][activeCellJ].y,  cells[activeCellI][activeCellJ].x + cells[activeCellI][activeCellJ].width , cells[activeCellI][activeCellJ].y + cells[activeCellI][activeCellJ].height };
	if (cells[activeCellI][activeCellJ].getCellType()) {
		cells[activeCellI][activeCellJ].setCellType(false);
		cells[activeCellI][activeCellJ].textEdit = CreateWindowEx(0, L"EDIT", NULL, WS_VISIBLE | WS_CHILD | ES_MULTILINE, 1 + activeCellJ * (cellWidth + 1), 1 + (activeCellI) * (cellHeight + 1) + headerHeight, cellWidth, cellHeight, parentWindow, NULL, NULL, NULL);
		SendMessage(cells[activeCellI][activeCellJ].textEdit, EM_SETLIMITTEXT, 0, 0);
		cells[activeCellI][activeCellJ].setupEditProc();
	}
	else {
		cells[activeCellI][activeCellJ].setCellType(true);
		DestroyWindow(cells[activeCellI][activeCellJ].textEdit);
	}
	InvalidateRect(parentWindow, &rect, true);
}

int Table::rowsLeft()
{
	return rows - visibleRows;
}

Table::~Table()
{
	delete(header);
	header = nullptr;
}

extern "C" __declspec(dllexport) void Draw_Table(void* tablePtr, HDC hdc, int scrollPos) {
	Table* table = static_cast<Table*>(tablePtr);
	table->drawTable(hdc, scrollPos);
}

extern "C" __declspec(dllexport) void* Create_Table(HWND parentWindow, int tableWidth, int tableHeight, int rows, int cols) {
	return new Table(parentWindow,tableWidth, tableHeight,rows, cols);
}

extern "C" __declspec(dllexport)  void Destroy_Table(void* tableptr) {
	delete static_cast<Table*>(tableptr);
}

extern "C" __declspec(dllexport)  void Update_Table(void* tablePtr, int width, int height) {
	Table* table = static_cast<Table*>(tablePtr);
	table->updateTableParams(width, height);
}

extern "C" __declspec(dllexport)  int Rows_Left(void* tableptr) {
	Table* table = static_cast<Table*>(tableptr);
	return table->rowsLeft();
}

extern "C" __declspec(dllexport)  void Handle_Left_Click(void* tableptr, int clickPosX, int clickPosY, const std::wstring & time){
	Table* table = static_cast<Table*>(tableptr);
	table->handleLeftClick(clickPosX,clickPosY, time);
}

extern "C" __declspec(dllexport)  void Handle_Dbl_Left_Click(void* tableptr, int clickPosX, int clickPosY, const std::wstring & time) {
	Table* table = static_cast<Table*>(tableptr);
	table->handleDblLeftClick(clickPosX, clickPosY, time);
}

extern "C" __declspec(dllexport)  void Handle_Right_Click(void* tableptr, int clickPosX, int clickPosY, const std::wstring & time) {
	Table* table = static_cast<Table*>(tableptr);
	table->handleRightClick(clickPosX, clickPosY, time);
}

extern "C" __declspec(dllexport)  void Handle_Dbl_Right_Click(void* tableptr, int clickPosX, int clickPosY, const std::wstring & time) {
	Table* table = static_cast<Table*>(tableptr);
	table->handleDblRightClick(clickPosX, clickPosY, time);
}

extern "C" __declspec(dllexport)  void Start_Click_Timer(void* tableptr) {
	Table* table = static_cast<Table*>(tableptr);
	table->startClickTimer();
}

extern "C" __declspec(dllexport)  void Stop_Click_Timer(void* tableptr) {
	Table* table = static_cast<Table*>(tableptr);
	table->stopClickTimer();
}

extern "C" __declspec(dllexport)  void Start_R_Click_Timer(void* tableptr) {
	Table* table = static_cast<Table*>(tableptr);
	table->startRClickTimer();
}

extern "C" __declspec(dllexport)  void Stop_R_Click_Timer(void* tableptr) {
	Table* table = static_cast<Table*>(tableptr);
	table->stopRClickTimer();
}

extern "C" __declspec(dllexport)  void Change_Cell_Type(void* tableptr) {
	Table* table = static_cast<Table*>(tableptr);
	table->changeCellType();
}

extern "C" __declspec(dllexport)  int Get_Click_Timer_ID() {
	return Table::CLICK_TIMER_ID;
}

extern "C" __declspec(dllexport)  int Get_R_Click_Timer_ID() {
	return Table::RCLICK_TIMER_ID;
}


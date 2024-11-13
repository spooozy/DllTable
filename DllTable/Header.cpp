#include "pch.h"
#include "Header.h"


Header::Header(int tableColsNum) {
	this->tableColsNum = tableColsNum;
};

void Header::addHeaderItem(std::wstring newItem) {
	if (headerItems.size() < tableColsNum)
		headerItems.push_back(newItem);
}

void Header::drawHeader(HDC hdc, int itemWidth, int visibleCols, int difX) {
	SIZE textSize;
	std::wstring buffer(100, L'\0');
	int hx, hy, dif, letterSize, diff;

	for (int i = 0;i < visibleCols - difX;i++)
	{
		GetTextExtentPoint32(hdc, headerItems[i].c_str(), headerItems[i].length(), &textSize);
		letterSize = textSize.cx / headerItems[i].length();
		dif = textSize.cx - itemWidth;
		if (dif > 0) buffer = headerItems[i].substr(0, headerItems[i].length() - dif / letterSize - 3) + L"...";
		else buffer = headerItems[i].c_str();
		GetTextExtentPoint32(hdc, buffer.c_str(), buffer.length(), &textSize);
		hx = (1 + i * (itemWidth + 1) + itemWidth / 2) - (textSize.cx / 2);
		hy = ((height - 1) / 2) - (textSize.cy / 2);
		TextOut(hdc, hx, hy, buffer.c_str(), buffer.length());
	}
	if (difX != 0) {
		diff = 0;
		for (int i = visibleCols - difX;i < visibleCols;i++)
		{
			GetTextExtentPoint32(hdc, headerItems[i].c_str(), headerItems[i].length(), &textSize);
			letterSize = textSize.cx / headerItems[i].length();
			dif = textSize.cx - itemWidth + 1;
			if (dif > 0) buffer = headerItems[i].substr(0, headerItems[i].length() - dif / letterSize - 3) + L"...";
			else buffer = headerItems[i].c_str();
			GetTextExtentPoint32(hdc, buffer.c_str(), buffer.length(), &textSize);
			hx = (1 + i * (itemWidth + 1) + (itemWidth) / 2) - (textSize.cx / 2) + diff;
			hy = ((height - 1) / 2) - (textSize.cy / 2);
			TextOut(hdc, hx, hy, buffer.c_str(), buffer.length());
			diff++;
		}
	}
}

int Header::getHeaderHeight() {
	return height;
}

int Header::getHeaderWidth() {
	return width;
}

int Header::updateHeaderHeight(int newTableHeght)
{
	int temp = 0.06 * newTableHeght;
	if (temp < minHeaderHeight) height = minHeaderHeight;
	else height = temp;
	return height;
}

Header::~Header() {

}
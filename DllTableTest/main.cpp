#include <windows.h>
#include <chrono>
#include <iomanip>
#include <sstream>

typedef void* (*Create_Table_t)(HWND, int, int, int, int);
typedef void (*Destroy_Table_t)(void*);
typedef void (*Draw_Table_t)(void*, HDC, int);
typedef void (*Update_Table_t)(void*, int, int);
typedef int (*Rows_Left_t)(void*);
typedef void (*Handle_Left_Click_t)(void*, int, int, const std::wstring&);
typedef void (*Handle_Dbl_Left_Click_t)(void*, int, int, const std::wstring&);
typedef void (*Handle_Right_Click_t)(void*, int, int, const std::wstring&);
typedef void (*Handle_Dbl_Right_Click_t)(void*, int, int, const std::wstring&);
typedef void (*Start_Click_Timer_t)(void*);
typedef void (*Stop_Click_Timer_t)(void*);
typedef void (*Start_R_Click_Timer_t)(void*);
typedef void (*Stop_R_Click_Timer_t)(void*);
typedef void (*Change_Cell_Type_t)(void*);
typedef int (*Get_Click_Timer_ID_t)();
typedef int (*Get_R_Click_Timer_ID_t)();



void* table;
int colNum = 10, rowNum = 30, clientWidth, clientHeight, scrollWidth = 0, scrollPos = 0, clickPosX = 0, clickPosY = 0;
HWND hWnd;
HINSTANCE load;
Create_Table_t Create_Table; Destroy_Table_t Destroy_Table; Draw_Table_t Draw_Table; Update_Table_t Update_Table;
Rows_Left_t Rows_Left; Handle_Left_Click_t Handle_Left_Click; Handle_Dbl_Left_Click_t Handle_Dbl_Left_Click;
Handle_Right_Click_t Handle_Right_Click; Handle_Dbl_Right_Click_t Handle_Dbl_Right_Click;
Start_Click_Timer_t Start_Click_Timer; Stop_Click_Timer_t Stop_Click_Timer;
Start_R_Click_Timer_t Start_R_Click_Timer; Stop_R_Click_Timer_t Stop_R_Click_Timer;
Change_Cell_Type_t Change_Cell_Type; Get_Click_Timer_ID_t Get_Click_Timer_ID; Get_R_Click_Timer_ID_t Get_R_Click_Timer_ID;
bool doubleLeftClicked = false, doubleRightClicked = false;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void updateClientSizeParams();
void loadFunctions();
std::wstring getFormattedTime(const std::chrono::system_clock::time_point& time);
std::chrono::system_clock::time_point clickTime;

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const wchar_t TableClass[] = L"Table";
    int x = 0;


    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TableClass;
    wc.style = CS_DBLCLKS;
    RegisterClass(&wc);

    hWnd = CreateWindowEx(0, TableClass, L"Table", WS_OVERLAPPEDWINDOW | WS_VSCROLL,
        0, 0, screenWidth - 40, screenHeight,
        NULL, NULL, hInstance, NULL);

    updateClientSizeParams();

    load = LoadLibraryA("DllTable.dll");
    if (!load) {
        MessageBox(hWnd, L"Не удалось загрузить библиотеку!", L"Error", MB_OK | MB_TASKMODAL);
        return -1;
    }
    
    loadFunctions();

    if (!Create_Table || !Destroy_Table || !Draw_Table || !Update_Table || !Rows_Left ||
        !Handle_Left_Click || !Handle_Dbl_Left_Click || !Handle_Right_Click || !Handle_Dbl_Right_Click ||
        !Start_Click_Timer || !Stop_Click_Timer || !Start_R_Click_Timer || !Stop_R_Click_Timer ||
        !Change_Cell_Type || !Get_Click_Timer_ID || !Get_R_Click_Timer_ID) {

        MessageBox(hWnd, L"Не удалось получить указатели на функции!", L"Error", MB_OK | MB_TASKMODAL);
        FreeLibrary(load);
        return -1;
    }
    table = Create_Table(hWnd, clientWidth - scrollWidth, clientHeight, rowNum, colNum);
   // table = new Table(hWnd, clientWidth - scrollWidth, clientHeight, rowNum, colNum);

    ShowWindow(hWnd, SW_SHOWMAXIMIZED);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT clearRect;
    HDC hdc;
    PAINTSTRUCT ps;
    switch (message)
    {
    case WM_CREATE:
    {
        break;
    }
    case WM_SIZE: {
        updateClientSizeParams();
        InvalidateRect(hWnd, NULL, true);
    }
    case WM_PAINT: {
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &clearRect);
        FillRect(hdc, &clearRect, (HBRUSH)(COLOR_WINDOW + 1));
        Update_Table(table, clientWidth - scrollWidth, clientHeight);
        //table->updateTableParams(clientWidth - scrollWidth, clientHeight);
        SetScrollRange(hWnd, SB_VERT, 0, Rows_Left(table), true);
        //SetScrollRange(hWnd, SB_VERT, 0, table->rowsLeft(), true);
        Draw_Table(table, hdc, scrollPos);
        //table->drawTable(hdc, scrollPos);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_COMMAND: {
        break;
    }
    case WM_VSCROLL: {
        int oldPos = scrollPos;
        switch (LOWORD(wParam)) {
        case SB_LINEUP:
            if (--scrollPos < 0) scrollPos = 0;
            break;
        case SB_LINEDOWN:
            if (++scrollPos > Rows_Left(table))
                scrollPos = Rows_Left(table);
            //if (++scrollPos > table->rowsLeft())scrollPos = table->rowsLeft();
            break;
        }
        if (oldPos != scrollPos)
        {
            SetScrollPos(hWnd, SB_VERT, scrollPos, true);
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    }
    case WM_DESTROY: {
        FreeLibrary(load);
        PostQuitMessage(0);
        break;
    }
    case WM_LBUTTONDOWN: {
        clickTime = std::chrono::system_clock::now();
        clickPosX = LOWORD(lParam);
        clickPosY = HIWORD(lParam);
        Start_Click_Timer(table);
        //table->startClickTimer();
        break;
    }
    case WM_LBUTTONDBLCLK: {
        clickTime = std::chrono::system_clock::now();
        Stop_Click_Timer(table);
        //table->stopClickTimer();        
        doubleLeftClicked = true;
        clickPosX = LOWORD(lParam);
        clickPosY = HIWORD(lParam);
        Handle_Dbl_Left_Click(table, clickPosX, clickPosY, getFormattedTime(clickTime));
        //table->handleDblLeftClick(clickPosX, clickPosY, getFormattedTime(clickTime));
        doubleLeftClicked = false;
        break;
    }
    case WM_RBUTTONDOWN: {
        clickTime = std::chrono::system_clock::now();
        clickPosX = LOWORD(lParam);
        clickPosY = HIWORD(lParam);
        Start_R_Click_Timer(table);
        //table->startRClickTimer();
        break;
    }
    case WM_RBUTTONDBLCLK: {
        clickTime = std::chrono::system_clock::now();
        Stop_R_Click_Timer(table);
        //table->stopRClickTimer();
        doubleRightClicked = true;
        clickPosX = LOWORD(lParam);
        clickPosY = HIWORD(lParam);
        Handle_Dbl_Right_Click(table, clickPosX, clickPosY, getFormattedTime(clickTime));
        //table->handleDblRightClick(clickPosX, clickPosY, getFormattedTime(clickTime));
        doubleRightClicked = false;
        break;
    }

    case WM_TIMER: {
        if (wParam == Get_Click_Timer_ID()) {
            Stop_Click_Timer(table);
            //table->stopClickTimer();
            if (!doubleLeftClicked) {
                Handle_Left_Click(table, clickPosX, clickPosY, getFormattedTime(clickTime));
               // table->handleLeftClick(clickPosX, clickPosY, getFormattedTime(clickTime));
            }
        }
        if (wParam == /*table->RCLICK_TIMER_ID*/ Get_R_Click_Timer_ID()) {
            Stop_R_Click_Timer(table);
            //table->stopRClickTimer();
            if (!doubleRightClicked) {
                Handle_Right_Click(table, clickPosX, clickPosY, getFormattedTime(clickTime));
                //table->handleRightClick(clickPosX, clickPosY, getFormattedTime(clickTime));
            }
        }
        break;
    }
    case WM_KEYDOWN: {
        if (GetKeyState(VK_CONTROL) & 0x8000 &&
            GetKeyState(VK_SHIFT) & 0x8000 &&
            wParam == 'T') {
            Change_Cell_Type(table);
            //table->changeCellType();
        }
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    return 0;
}

void updateClientSizeParams() {
    RECT rect;
    GetClientRect(hWnd, &rect);
    clientWidth = rect.right - rect.left;
    clientHeight = rect.bottom - rect.top;
}

std::wstring getFormattedTime(const std::chrono::system_clock::time_point& time) {
    std::time_t t = std::chrono::system_clock::to_time_t(time);
    std::tm localTime;
    localtime_s(&localTime, &t);

    std::wostringstream oss;
    oss.imbue(std::locale(""));
    oss << std::put_time(&localTime, L"%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void loadFunctions() {
    Create_Table = (Create_Table_t)GetProcAddress(load, "Create_Table");
    Destroy_Table = (Destroy_Table_t)GetProcAddress(load, "Destroy_Table");
    Draw_Table = (Draw_Table_t)GetProcAddress(load, "Draw_Table");
    Update_Table = (Update_Table_t)GetProcAddress(load, "Update_Table");
    Rows_Left = (Rows_Left_t)GetProcAddress(load, "Rows_Left");
    Handle_Left_Click = (Handle_Left_Click_t)GetProcAddress(load, "Handle_Left_Click");
    Handle_Dbl_Left_Click = (Handle_Dbl_Left_Click_t)GetProcAddress(load, "Handle_Dbl_Left_Click");
    Handle_Right_Click = (Handle_Right_Click_t)GetProcAddress(load, "Handle_Right_Click");
    Handle_Dbl_Right_Click = (Handle_Dbl_Right_Click_t)GetProcAddress(load, "Handle_Dbl_Right_Click");
    Start_Click_Timer = (Start_Click_Timer_t)GetProcAddress(load, "Start_Click_Timer");
    Stop_Click_Timer = (Stop_Click_Timer_t)GetProcAddress(load, "Stop_Click_Timer");
    Start_R_Click_Timer = (Start_R_Click_Timer_t)GetProcAddress(load, "Start_R_Click_Timer");
    Stop_R_Click_Timer = (Stop_R_Click_Timer_t)GetProcAddress(load, "Stop_R_Click_Timer");
    Change_Cell_Type = (Change_Cell_Type_t)GetProcAddress(load, "Change_Cell_Type");
    Get_Click_Timer_ID = (Get_Click_Timer_ID_t)GetProcAddress(load, "Get_Click_Timer_ID");
    Get_R_Click_Timer_ID = (Get_R_Click_Timer_ID_t)GetProcAddress(load, "Get_R_Click_Timer_ID");
}
#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <commctrl.h> 
#pragma comment(lib, "comctl32.lib")
#include "resource.h"

// Define colors like in the image
#define COLOR_PURPLE    RGB(207, 130, 255)
#define COLOR_CORAL     RGB(255, 131, 111)
#define COLOR_GREEN     RGB(151, 206, 47)
#define COLOR_BLUE      RGB(57, 175, 247)
#define COLOR_DARK      RGB(45, 50, 55)

#define MAX_EMPLOYEES 100
#define ID_ADD_EMPLOYEE 1
#define ID_UPDATE_EMPLOYEE 2
#define ID_DELETE_EMPLOYEE 3
#define ID_VIEW_EMPLOYEE 4
#define ID_SEARCH_EMPLOYEE 5
#define ID_EXIT 6

// Panel handles
HWND g_hPanel1 = NULL;
HWND g_hPanel2 = NULL;
HWND g_hPanel3 = NULL;
HWND g_hPanel4 = NULL;
HWND g_hHeader = NULL;

// Font handles
HFONT g_hFontHeader = NULL;
HFONT g_hFontButton = NULL;

typedef struct {
    wchar_t name[50];
    int id;
    wchar_t department[50];
    wchar_t position[50];
} Employee;

typedef struct {
    Employee employees[MAX_EMPLOYEES];
    int count;
} EmployeeList;

EmployeeList employeeList = { .count = 0 };

// Helper function to setup ListView for Employee data
void SetupEmployeeListView(HWND hwndList) {
    // Set ListView to report view (table-like appearance)
    SendMessage(hwndList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    // Add columns to the ListView
    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    // Column 1: Name
    lvc.iSubItem = 0;
    lvc.cx = 150;  // Width in pixels
    lvc.pszText = L"Name";
    SendMessage(hwndList, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

    // Column 2: ID
    lvc.iSubItem = 1;
    lvc.cx = 50;
    lvc.pszText = L"ID";
    SendMessage(hwndList, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc);

    // Column 3: Department
    lvc.iSubItem = 2;
    lvc.cx = 120;
    lvc.pszText = L"Department";
    SendMessage(hwndList, LVM_INSERTCOLUMN, 2, (LPARAM)&lvc);

    // Column 4: Position
    lvc.iSubItem = 3;
    lvc.cx = 120;
    lvc.pszText = L"Position";
    SendMessage(hwndList, LVM_INSERTCOLUMN, 3, (LPARAM)&lvc);
}

// Helper function to populate ListView with employee data
void PopulateEmployeeListView(HWND hwndList, EmployeeList* list) {
    // Clear existing items
    SendMessage(hwndList, LVM_DELETEALLITEMS, 0, 0);

    // Add items (rows) to the ListView
    LVITEM lvi;
    lvi.mask = LVIF_TEXT;

    for (int i = 0; i < list->count; i++) {
        // Insert the row (for the name column)
        lvi.iItem = i;
        lvi.iSubItem = 0;
        lvi.pszText = list->employees[i].name;
        SendMessage(hwndList, LVM_INSERTITEM, 0, (LPARAM)&lvi);

        // Add the ID text (convert int to string)
        wchar_t idStr[16];
        swprintf(idStr, 16, L"%d", list->employees[i].id);
        ListView_SetItemText(hwndList, i, 1, idStr);

        // Add the Department text
        ListView_SetItemText(hwndList, i, 2, list->employees[i].department);

        // Add the Position text
        ListView_SetItemText(hwndList, i, 3, list->employees[i].position);
    }
}

void saveEmployeesToFile(const EmployeeList* list) {
    FILE* file = _wfopen(L"employee_data.txt", L"w");
    if (file == NULL) {
        MessageBox(NULL, L"Error opening file for writing!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    for (int i = 0; i < list->count; i++) {
        fwprintf(file, L"%ls,%d,%ls,%ls\n", list->employees[i].name, list->employees[i].id, list->employees[i].department, list->employees[i].position);
    }
    fclose(file);
}

void loadEmployeesFromFile(EmployeeList* list) {
    FILE* file = _wfopen(L"employee_data.txt", L"r");
    if (file == NULL) {
        MessageBox(NULL, L"Error opening file for reading!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    list->count = 0;
    while (fwscanf(file, L"%49[^,],%d,%49[^,],%49[^\n]\n", list->employees[list->count].name, &list->employees[list->count].id, list->employees[list->count].department, list->employees[list->count].position) != EOF) {
        list->count++;
    }
    fclose(file);
}

LRESULT CALLBACK AddEmployeeDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static Employee employee;
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            GetDlgItemText(hwndDlg, IDC_EDIT1, employee.name, 50);
            employee.id = GetDlgItemInt(hwndDlg, IDC_EDIT2, NULL, FALSE);
            GetDlgItemText(hwndDlg, IDC_EDIT3, employee.department, 50);
            GetDlgItemText(hwndDlg, IDC_EDIT4, employee.position, 50);
            employeeList.employees[employeeList.count++] = employee;
            saveEmployeesToFile(&employeeList);
            MessageBox(hwndDlg, L"Employee added successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
            EndDialog(hwndDlg, IDOK);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hwndDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK UpdateEmployeeDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static Employee* employee = NULL;
    switch (message) {
    case WM_INITDIALOG:
        employee = (Employee*)lParam;
        if (employee != NULL) {
            SetDlgItemText(hwndDlg, IDC_EDIT2, employee->name);
            SetDlgItemText(hwndDlg, IDC_EDIT3, employee->department);
            SetDlgItemText(hwndDlg, IDC_EDIT4, employee->position);
        }
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            GetDlgItemText(hwndDlg, IDC_EDIT2, employee->name, 50);
            GetDlgItemText(hwndDlg, IDC_EDIT3, employee->department, 50);
            GetDlgItemText(hwndDlg, IDC_EDIT4, employee->position, 50);
            saveEmployeesToFile(&employeeList);
            MessageBox(hwndDlg, L"Employee updated successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
            EndDialog(hwndDlg, IDOK);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hwndDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK SelectEmployeeDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndList;
    static int action;

    switch (message) {
    case WM_INITDIALOG:
        hwndList = GetDlgItem(hwndDlg, IDC_LIST1);
        action = (int)lParam;

        SetupEmployeeListView(hwndList);
        PopulateEmployeeListView(hwndList, &employeeList);

        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            int index = SendMessage(hwndList, LVM_GETNEXTITEM, -1, LVNI_SELECTED);

            if (index != -1) {
                Employee* employee = &employeeList.employees[index];
                if (action == ID_UPDATE_EMPLOYEE) {
                    DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UPDATE_EMPLOYEE),
                        hwndDlg, UpdateEmployeeDlgProc, (LPARAM)employee);
                }
                else if (action == ID_DELETE_EMPLOYEE) {
                    int response = MessageBox(hwndDlg, L"Are you sure you want to delete this employee?", L"Confirm Delete", MB_YESNO | MB_ICONQUESTION);
                    if (response == IDYES) {
                        for (int i = index; i < employeeList.count - 1; i++) {
                            employeeList.employees[i] = employeeList.employees[i + 1];
                        }
                        employeeList.count--;
                        saveEmployeesToFile(&employeeList);
                        MessageBox(hwndDlg, L"Employee deleted successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
                        PopulateEmployeeListView(hwndList, &employeeList);
                    }
                }
            }
            else {
                MessageBox(hwndDlg, L"No employee selected!", L"Error", MB_OK | MB_ICONERROR);
            }
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hwndDlg, IDCANCEL);
            return TRUE;
        }
        break;
    case WM_NOTIFY:
        if (((LPNMHDR)lParam)->code == NM_DBLCLK) {
            int index = SendMessage(hwndList, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
            if (index != -1) {
                Employee* employee = &employeeList.employees[index];
                    DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UPDATE_EMPLOYEE),
                        hwndDlg, UpdateEmployeeDlgProc, (LPARAM)employee);
                }
                else if (action == ID_DELETE_EMPLOYEE) {
                    int response = MessageBox(hwndDlg, L"Are you sure you want to delete this employee?", L"Confirm Delete", MB_YESNO | MB_ICONQUESTION);
                    if (response == IDYES) {
                        for (int i = index; i < employeeList.count - 1; i++) {
                            employeeList.employees[i] = employeeList.employees[i + 1];
                        }
                        employeeList.count--;
                        saveEmployeesToFile(&employeeList);
                        MessageBox(hwndDlg, L"Employee deleted successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
                        PopulateEmployeeListView(hwndList, &employeeList);
                    }
            return TRUE;
        }
        break;
    }

    return FALSE;
}

// Add this to your WinMain or initialization function
void InitializeCommonControls() {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);
}

LRESULT CALLBACK ViewEmployeesDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndList;

    switch (message) {
    case WM_INITDIALOG:
        hwndList = GetDlgItem(hwndDlg, IDC_LIST1);

        SetupEmployeeListView(hwndList);
        PopulateEmployeeListView(hwndList, &employeeList);

        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hwndDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }

    return FALSE;
}

LRESULT CALLBACK SearchEmployeeDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static wchar_t criteria[50];
    static HWND hwndList;

    switch (message) {
    case WM_INITDIALOG:
        hwndList = GetDlgItem(hwndDlg, IDC_LIST1);
        SetupEmployeeListView(hwndList);
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            GetDlgItemText(hwndDlg, IDC_EDIT1, criteria, 50);
            PopulateEmployeeListView(hwndList, &employeeList);

            for (int i = 0; i < employeeList.count; i++) {
                Employee* employee = &employeeList.employees[i];
                if (!wcsstr(employee->name, criteria) && !wcsstr(employee->department, criteria) &&
                    !wcsstr(employee->position, criteria) && employee->id != _wtoi(criteria)) {
                    ListView_DeleteItem(hwndList, i);
                    i--; // Adjust index to account for deleted item
                }
            }
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hwndDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        // Initialize common controls
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_WIN95_CLASSES;
        InitCommonControlsEx(&icex);

        // Register panel window class
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = PanelProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"ColorPanel";
        RegisterClass(&wc);

        // Create fonts
        g_hFontHeader = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

        g_hFontButton = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

        // Create header
        g_hHeader = CreateWindow(L"STATIC", L"A Company Employee Record",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            10, 10, 380, 40, hwnd, NULL, GetModuleHandle(NULL), NULL);

        // Set header font
        SendMessage(g_hHeader, WM_SETFONT, (WPARAM)g_hFontHeader, TRUE);

        // Define panel parameters
        struct {
            COLORREF color;
            int id;
            WCHAR* text;
            WCHAR* icon;
        } panel1 = { COLOR_PURPLE, ID_ADD_EMPLOYEE, L"SERVICES", L"⏺" };

        struct {
            COLORREF color;
            int id;
            WCHAR* text;
            WCHAR* icon;
        } panel2 = { COLOR_CORAL, ID_UPDATE_EMPLOYEE, L"AUTO", L"⏺" };

        struct {
            COLORREF color;
            int id;
            WCHAR* text;
            WCHAR* icon;
        } panel3 = { COLOR_GREEN, ID_VIEW_EMPLOYEE, L"JOB", L"⏺" };

        struct {
            COLORREF color;
            int id;
            WCHAR* text;
            WCHAR* icon;
        } panel4 = { COLOR_BLUE, ID_SEARCH_EMPLOYEE, L"Search Employee", L"🔍" };

        // Create colored panels with icons and text
        g_hPanel1 = CreateWindow(L"ColorPanel", NULL,
            WS_VISIBLE | WS_CHILD,
            100, 60, 200, 80, hwnd, NULL, GetModuleHandle(NULL), &panel1);

        g_hPanel2 = CreateWindow(L"ColorPanel", NULL,
            WS_VISIBLE | WS_CHILD,
            100, 145, 200, 80, hwnd, NULL, GetModuleHandle(NULL), &panel2);

        g_hPanel3 = CreateWindow(L"ColorPanel", NULL,
            WS_VISIBLE | WS_CHILD,
            100, 230, 200, 80, hwnd, NULL, GetModuleHandle(NULL), &panel3);

        g_hPanel4 = CreateWindow(L"ColorPanel", NULL,
            WS_VISIBLE | WS_CHILD,
            100, 315, 200, 80, hwnd, NULL, GetModuleHandle(NULL), &panel4);

        // Create exit button at bottom
        CreateWindow(L"BUTTON", L"Exit",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            150, 405, 100, 30, hwnd, (HMENU)ID_EXIT, GetModuleHandle(NULL), NULL);

        break;
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        HWND hwndStatic = (HWND)lParam;

        if (hwndStatic == g_hHeader) {
            SetBkColor(hdcStatic, COLOR_DARK);
            SetTextColor(hdcStatic, RGB(255, 255, 255));
            return (INT_PTR)CreateSolidBrush(COLOR_DARK);
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_ADD_EMPLOYEE:
            MessageBox(hwnd, L"Add Employee Selected", L"Action", MB_OK | MB_ICONINFORMATION);
            // DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADD_EMPLOYEE), hwnd, AddEmployeeDlgProc);
            break;
        case ID_UPDATE_EMPLOYEE:
            MessageBox(hwnd, L"Update Employee Selected", L"Action", MB_OK | MB_ICONINFORMATION);
            // DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SELECT_EMPLOYEE), hwnd, SelectEmployeeDlgProc, ID_UPDATE_EMPLOYEE);
            break;
        case ID_VIEW_EMPLOYEE:
            MessageBox(hwnd, L"View Employees Selected", L"Action", MB_OK | MB_ICONINFORMATION);
            // DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_VIEW_EMPLOYEES), hwnd, ViewEmployeesDlgProc);
            break;
        case ID_SEARCH_EMPLOYEE:
            MessageBox(hwnd, L"Search Employee Selected", L"Action", MB_OK | MB_ICONINFORMATION);
            // DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SEARCH_EMPLOYEE), hwnd, SearchEmployeeDlgProc);
            break;
        case ID_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

// WinMain function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window class
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = CreateSolidBrush(COLOR_DARK);
    wc.lpszClassName = L"EmployeeRecordApp";
    RegisterClass(&wc);

    // Get screen dimensions for centering
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Window dimensions based on image proportion
    int windowWidth = 300;
    int windowHeight = 450;
    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;

    // Create main window - with fixed borders to match the image
    HWND hwnd = CreateWindow(
        L"EmployeeRecordApp", L"K Company Employee Record",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 500,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        return 1;
    }

    // Show window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
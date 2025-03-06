#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <commctrl.h> 
#pragma comment(lib, "comctl32.lib")
#include "resource.h"

#define MAX_EMPLOYEES 100
#define ID_ADD_EMPLOYEE 1
#define ID_UPDATE_EMPLOYEE 2
#define ID_DELETE_EMPLOYEE 3
#define ID_VIEW_EMPLOYEE 4
#define ID_SEARCH_EMPLOYEE 5
#define ID_EXIT 6

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

// Updated function for SelectEmployeeDlgProc
LRESULT CALLBACK SelectEmployeeDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndList;

    switch (message) {
    case WM_INITDIALOG:
        // Initialize the ListView control
        hwndList = GetDlgItem(hwndDlg, IDC_LIST1);

        // Setup ListView columns
        SetupEmployeeListView(hwndList);

        // Populate ListView with data
        PopulateEmployeeListView(hwndList, &employeeList);

        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            // Get selected item from ListView instead of ListBox
            int index = SendMessage(hwndList, LVM_GETNEXTITEM, -1, LVNI_SELECTED);

            if (index != -1) {
                Employee* employee = &employeeList.employees[index];
                DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UPDATE_EMPLOYEE),
                    hwndDlg, UpdateEmployeeDlgProc, (LPARAM)employee);

                // Refresh the list after update
                PopulateEmployeeListView(hwndList, &employeeList);
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
        // Handle double-click on ListView item
        if (((LPNMHDR)lParam)->code == NM_DBLCLK) {
            int index = SendMessage(hwndList, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
            if (index != -1) {
                Employee* employee = &employeeList.employees[index];
                DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UPDATE_EMPLOYEE),
                    hwndDlg, UpdateEmployeeDlgProc, (LPARAM)employee);

                // Refresh the list after update
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

LRESULT CALLBACK DeleteEmployeeDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static int id;
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            id = GetDlgItemInt(hwndDlg, IDC_EDIT1, NULL, FALSE);
            for (int i = 0; i < employeeList.count; i++) {
                if (employeeList.employees[i].id == id) {
                    for (int j = i; j < employeeList.count - 1; j++) {
                        employeeList.employees[j] = employeeList.employees[j + 1];
                    }
                    employeeList.count--;
                    saveEmployeesToFile(&employeeList);
                    MessageBox(hwndDlg, L"Employee deleted successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
                    EndDialog(hwndDlg, IDOK);
                    return TRUE;
                }
            }
            MessageBox(hwndDlg, L"Employee not found!", L"Error", MB_OK | MB_ICONERROR);
            EndDialog(hwndDlg, IDCANCEL);
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



// Updated function for ViewEmployeesDlgProc
LRESULT CALLBACK ViewEmployeesDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndList;

    switch (message) {
    case WM_INITDIALOG:
        // Initialize the ListView control
        hwndList = GetDlgItem(hwndDlg, IDC_LIST1);

        // Setup ListView columns
        SetupEmployeeListView(hwndList);

        // Populate ListView with data
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
    static Employee* employee;
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            GetDlgItemText(hwndDlg, IDC_EDIT1, criteria, 50);
            HWND hwndList = GetDlgItem(hwndDlg, IDC_LIST1);
            SendMessage(hwndList, LB_RESETCONTENT, 0, 0);
            for (int i = 0; i < employeeList.count; i++) {
                employee = &employeeList.employees[i];
                if (wcsstr(employee->name, criteria) || wcsstr(employee->department, criteria) || wcsstr(employee->position, criteria) || employee->id == _wtoi(criteria)) {
                    wchar_t buffer[200];
                    swprintf(buffer, 200, L"Name: %ls, ID: %d, Department: %ls, Position: %ls", employee->name, employee->id, employee->department, employee->position);
                    SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)buffer);
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
    case WM_CREATE:
        CreateWindow(L"BUTTON", L"Add Employee", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 20, 150, 30, hwnd, (HMENU)ID_ADD_EMPLOYEE, GetModuleHandle(NULL), NULL);
        CreateWindow(L"BUTTON", L"Update Employee", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 60, 150, 30, hwnd, (HMENU)ID_UPDATE_EMPLOYEE, GetModuleHandle(NULL), NULL);
        CreateWindow(L"BUTTON", L"Delete Employee", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 100, 150, 30, hwnd, (HMENU)ID_DELETE_EMPLOYEE, GetModuleHandle(NULL), NULL);
        CreateWindow(L"BUTTON", L"View Employees", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 140, 150, 30, hwnd, (HMENU)ID_VIEW_EMPLOYEE, GetModuleHandle(NULL), NULL);
        CreateWindow(L"BUTTON", L"Search Employee", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 180, 150, 30, hwnd, (HMENU)ID_SEARCH_EMPLOYEE, GetModuleHandle(NULL), NULL);
        CreateWindow(L"BUTTON", L"Exit", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 220, 150, 30, hwnd, (HMENU)ID_EXIT, GetModuleHandle(NULL), NULL);
        break;
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        SetBkColor(hdcStatic, RGB(0, 0, 0));
        SetTextColor(hdcStatic, RGB(255, 255, 255));
        return (INT_PTR)CreateSolidBrush(RGB(0, 0, 0));
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_ADD_EMPLOYEE:
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADD_EMPLOYEE), hwnd, AddEmployeeDlgProc);
            break;
        case ID_UPDATE_EMPLOYEE:
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SELECT_EMPLOYEE), hwnd, SelectEmployeeDlgProc);
            break;
        case ID_DELETE_EMPLOYEE:
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DELETE_EMPLOYEE), hwnd, DeleteEmployeeDlgProc);
            break;
        case ID_VIEW_EMPLOYEE:
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_VIEW_EMPLOYEES), hwnd, ViewEmployeesDlgProc);
            break;
        case ID_SEARCH_EMPLOYEE:
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SEARCH_EMPLOYEE), hwnd, SearchEmployeeDlgProc);
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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    loadEmployeesFromFile(&employeeList);

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"EmployeeRecordSystem";

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hwnd = CreateWindowEx(0, L"EmployeeRecordSystem", L"Employee Record System",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
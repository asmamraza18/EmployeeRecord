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

// Define colors similar to the image
#define COLOR_SERVICES RGB(200, 100, 240)  // Purple
#define COLOR_AUTO RGB(255, 100, 100)      // Coral/Red
#define COLOR_JOB RGB(130, 200, 50)        // Green
#define COLOR_REALTY RGB(50, 150, 255)     // Blue
#define COLOR_EXIT RGB(100, 100, 100)      // Gray
#define COLOR_SEARCH RGB(230, 190, 80)     // Yellow/Gold

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
HBRUSH hServicesBrush, hAutoBrush, hJobBrush, hRealtyBrush;
HFONT hFont;

// Define colors based on the image
#define COLOR_SERVICES RGB(203, 116, 214)  // Purple
#define COLOR_AUTO RGB(255, 118, 103)      // Coral/Orange
#define COLOR_JOB RGB(154, 203, 60)        // Green
#define COLOR_REALTY RGB(66, 172, 229)     // Blue


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

            // Save changes to file
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
            int index = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);

            if (index != -1) {
                Employee* employee = &employeeList.employees[index];
                if (action == ID_UPDATE_EMPLOYEE) {
                    // Make sure you're using the correct dialog resource ID
                    DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UPDATE_EMPLOYEE),
                        hwndDlg, UpdateEmployeeDlgProc, (LPARAM)employee);

                    // Refresh the list view after update
                    PopulateEmployeeListView(hwndList, &employeeList);
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
            int index = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);
            if (index != -1) {
                Employee* employee = &employeeList.employees[index];
                if (action == ID_UPDATE_EMPLOYEE) {
                    DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UPDATE_EMPLOYEE),
                        hwndDlg, UpdateEmployeeDlgProc, (LPARAM)employee);

                    // Refresh the list view after update
                    PopulateEmployeeListView(hwndList, &employeeList);
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

// Create functional buttons with colorful design
void CreateFunctionalButtons(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    int width = rect.right - rect.left;
    int height = (rect.bottom - rect.top) / 6;

    // Create add employee button (purple)
    CreateWindow(L"BUTTON", L"ADD EMPLOYEE", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        0, 0, width, height, hwnd, (HMENU)ID_ADD_EMPLOYEE, GetModuleHandle(NULL), NULL);

    // Create update employee button (coral/orange)
    CreateWindow(L"BUTTON", L"UPDATE EMPLOYEE", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        0, height, width, height, hwnd, (HMENU)ID_UPDATE_EMPLOYEE, GetModuleHandle(NULL), NULL);

    // Create delete employee button (green)
    CreateWindow(L"BUTTON", L"DELETE EMPLOYEE", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        0, height * 2, width, height, hwnd, (HMENU)ID_DELETE_EMPLOYEE, GetModuleHandle(NULL), NULL);

    // Create view employees button (blue)
    CreateWindow(L"BUTTON", L"VIEW EMPLOYEES", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        0, height * 3, width, height, hwnd, (HMENU)ID_VIEW_EMPLOYEE, GetModuleHandle(NULL), NULL);

    // Create search employee button (yellow/gold)
    CreateWindow(L"BUTTON", L"SEARCH EMPLOYEE", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        0, height * 4, width, height, hwnd, (HMENU)ID_SEARCH_EMPLOYEE, GetModuleHandle(NULL), NULL);

    // Create exit button (gray)
    CreateWindow(L"BUTTON", L"EXIT", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        0, height * 5, width, height, hwnd, (HMENU)ID_EXIT, GetModuleHandle(NULL), NULL);
}

// Draw the custom buttons
void DrawFunctionalButton(LPDRAWITEMSTRUCT lpDIS) {
    HDC hdc = lpDIS->hDC;
    RECT rect = lpDIS->rcItem;

    // Set the color based on button ID
    COLORREF bgColor;
    switch (lpDIS->CtlID) {
    case ID_ADD_EMPLOYEE:
        bgColor = COLOR_SERVICES; // Purple
        break;
    case ID_UPDATE_EMPLOYEE:
        bgColor = COLOR_AUTO; // Coral/Orange
        break;
    case ID_DELETE_EMPLOYEE:
        bgColor = COLOR_JOB; // Green
        break;
    case ID_VIEW_EMPLOYEE:
        bgColor = COLOR_REALTY; // Blue
        break;
    case ID_SEARCH_EMPLOYEE:
        bgColor = COLOR_SEARCH; // Yellow/Gold
        break;
    case ID_EXIT:
        bgColor = COLOR_EXIT; // Gray
        break;
    default:
        bgColor = RGB(200, 200, 200);
    }

    // Fill background
    HBRUSH hBrush = CreateSolidBrush(bgColor);
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);

    // Get button text
    wchar_t text[50];
    GetWindowText(lpDIS->hwndItem, text, 50);

    // Set up text properties
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    // Save the original font
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    // Fixed icon size of 32x32
    const int iconSize = 32;
    const int spacing = 8;

    // Create icon font (32px height)
    HFONT iconFont = CreateFont(
        iconSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI Symbol"
    );

    // Calculate positions
    int totalHeight = iconSize + spacing + 20; // icon + spacing + text
    int startY = (rect.bottom - rect.top - totalHeight) / 2;

    // Set up icon and text rects
    RECT iconRect = rect;
    iconRect.top = startY;
    iconRect.bottom = iconRect.top + iconSize;

    RECT textRect = rect;
    textRect.top = iconRect.bottom + spacing;
    textRect.bottom = rect.bottom;

    // Select icon font
    SelectObject(hdc, iconFont);

    // Draw icon
    wchar_t* iconText;
    switch (lpDIS->CtlID) {
    case ID_ADD_EMPLOYEE: iconText = L"➕"; break;
    case ID_UPDATE_EMPLOYEE: iconText = L"➡️"; break;
    case ID_DELETE_EMPLOYEE: iconText = L"🗑️"; break;
    case ID_VIEW_EMPLOYEE: iconText = L"👁️"; break;
    case ID_SEARCH_EMPLOYEE: iconText = L"🔍"; break;
    case ID_EXIT: iconText = L"❌"; break;
    default: iconText = L"";
    }
    DrawText(hdc, iconText, -1, &iconRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Draw text
    SelectObject(hdc, hFont);
    DrawText(hdc, text, -1, &textRect, DT_CENTER | DT_TOP | DT_SINGLELINE);

    // Draw focus rect if needed
    if (lpDIS->itemState & ODS_FOCUS) {
        DrawFocusRect(hdc, &rect);
    }

    // Clean up
    SelectObject(hdc, oldFont);
    DeleteObject(iconFont);
}


// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        // Create font for buttons
        hFont = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

        // Initialize Common Controls
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_LISTVIEW_CLASSES;
        InitCommonControlsEx(&icex);

        // Create functional buttons
        CreateFunctionalButtons(hwnd);
        break;

    case WM_SIZE:
        // Resize buttons when window size changes
        DestroyWindow(GetDlgItem(hwnd, ID_ADD_EMPLOYEE));
        DestroyWindow(GetDlgItem(hwnd, ID_UPDATE_EMPLOYEE));
        DestroyWindow(GetDlgItem(hwnd, ID_DELETE_EMPLOYEE));
        DestroyWindow(GetDlgItem(hwnd, ID_VIEW_EMPLOYEE));
        DestroyWindow(GetDlgItem(hwnd, ID_SEARCH_EMPLOYEE));
        DestroyWindow(GetDlgItem(hwnd, ID_EXIT));
        CreateFunctionalButtons(hwnd);
        break;

    case WM_DRAWITEM:
        // Custom draw buttons
        switch (wParam) {
        case ID_ADD_EMPLOYEE:
        case ID_UPDATE_EMPLOYEE:
        case ID_DELETE_EMPLOYEE:
        case ID_VIEW_EMPLOYEE:
        case ID_SEARCH_EMPLOYEE:
        case ID_EXIT:
            DrawFunctionalButton((LPDRAWITEMSTRUCT)lParam);
            return TRUE;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_ADD_EMPLOYEE:
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADD_EMPLOYEE), hwnd, AddEmployeeDlgProc);
            break;
        case ID_UPDATE_EMPLOYEE:
            DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SELECT_EMPLOYEE), hwnd, SelectEmployeeDlgProc, ID_UPDATE_EMPLOYEE);
            break;
        case ID_DELETE_EMPLOYEE:
            DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SELECT_EMPLOYEE), hwnd, SelectEmployeeDlgProc, ID_DELETE_EMPLOYEE);
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
        // Clean up resources
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    InitializeCommonControls();
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
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 320, 600,
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
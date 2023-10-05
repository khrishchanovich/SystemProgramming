#pragma comment(lib, "comctl32.lib") // for Common Controls
#pragma comment(lib, "comdlg32.lib") // for Common Dialogs

#include <Windows.h>
#include <commctrl.h>
#include <cstring>
#include <string>
#include <shlwapi.h>

using namespace std;

#define BTNS_AUTOSIZE 16

#define TB_SETIMAGELIST    (WM_USER+48) // для установки изображения в панель инструментов
#define TB_LOADIMAGES       (WM_USER+50) // для загрузки изображений в панель инструментов

#define IDM_COPY      1001
#define IDM_DEL       1003
#define IDM_MOVE       1004
#define IDM_CREATE 1005

#define ID_LISTVIEW_1   3001
#define ID_LISTVIEW_2   3002

#define ID_COMBOBOX_1   4001
#define ID_COMBOBOX_2   4002

#define ID_LABEL_3  5003
#define ID_LABEL_4  5004

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Обработчик соозений для главного окна приложения

char nameMC[] = "FileManager";

char dir[MAX_PATH] = "D:\\*"; // Начальные пути к директориям
char dir1[MAX_PATH] = "D:\\*"; // -//-

char copy_buf1[MAX_PATH];

// Элементы хранения
HINSTANCE hInstance; // экземпляр текущего окна
HIMAGELIST g_hImageList = NULL; // изображения

// Храним уникальные идентификаторы
HWND hWndChild = NULL;  // оконный
static HWND hListView_1, hListView_2;
static HWND hComboBox_1, hComboBox_2;
static HWND hLabel_1, hLabel_2, hLabel_3, hLabel_4, hToolBar;

// Создание панели инстурментов
const int ImageListID = 0;
const int numButtons = 4;
const DWORD buttonStyles = BTNS_AUTOSIZE;

const int bitmapSize = 16;

OPENFILENAME ofn;
char szFileName[MAX_PATH];

TBBUTTON tbButtons[numButtons] =
        {
                {MAKELONG(STD_COPY, ImageListID),    IDM_COPY,   TBSTATE_ENABLED,
                        BTNS_AUTOSIZE, {0}, 0, (INT_PTR) "copy"},
                {MAKELONG(STD_DELETE, ImageListID),  IDM_DEL,    TBSTATE_ENABLED,
                        buttonStyles,  {0}, 0, (INT_PTR) "delete"},
                {MAKELONG(STD_REPLACE, ImageListID), IDM_MOVE,   TBSTATE_ENABLED,
                        buttonStyles,  {0}, 0, (INT_PTR) "move"},
                {MAKELONG(STD_FILENEW, ImageListID), IDM_CREATE, TBSTATE_ENABLED,
                        buttonStyles,  {0}, 0, (INT_PTR) "create"},
        };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, int nCmdShow) {
    MSG Msg;
    HWND hWnd;
    WNDCLASS WndClass;

    WndClass.style = CS_VREDRAW | CS_HREDRAW;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.lpfnWndProc = WndProc;
    WndClass.hInstance = hInstance;
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(230, 255, 0));
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = nameMC;

    if (!RegisterClass(&WndClass))
        return FALSE;


    hWnd = CreateWindow(nameMC,
                        "FileManager", WS_BORDER | WS_SYSMENU | WS_VISIBLE,
                        100, 100, 910, 620,
                        NULL, NULL, hInstance, NULL);

    InitCommonControls();

    while (GetMessage(&Msg, NULL, 0, 0)) {
        DispatchMessage(&Msg);
        TranslateMessage(&Msg);
    }
    return Msg.wParam;
}

HWND CreateSimpleToolbar(HWND hWndParent) {
    HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
                                      WS_CHILD | TBSTYLE_WRAPABLE,
                                      0, 0, 0, 0,
                                      hWndParent, NULL, hInstance, NULL);
    if (hWndToolbar == NULL) { return NULL; }

    g_hImageList = ImageList_Create(
            bitmapSize, bitmapSize,
            ILC_COLOR16 | ILC_MASK,
            numButtons, 0);

    SendMessage(hWndToolbar, TB_SETIMAGELIST, (WPARAM) ImageListID,
                (LPARAM) g_hImageList);

    SendMessage(hWndToolbar, TB_LOADIMAGES, (WPARAM) IDB_STD_SMALL_COLOR,
                (LPARAM) HINST_COMMCTRL);

    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);
    SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM) numButtons, (LPARAM) &tbButtons);
    SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);

    ShowWindow(hWndToolbar, TRUE);
    return hWndToolbar;
}

void AddColToListView(const char *st, int sub, int size) {
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.iSubItem = sub;
    lvc.pszText = const_cast<LPSTR>(st);
    lvc.cx = size;
    lvc.fmt = LVCFMT_LEFT;

    ListView_InsertColumn(hListView_1, sub, &lvc);
    ListView_InsertColumn(hListView_2, sub, &lvc);
}

int i;

BOOL InitListViewImageLists(HWND hWndListView, int size, char c_dir[MAX_PATH]) {
    HIMAGELIST hSmall;
    SHFILEINFO lp;
    hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_MASK | ILC_COLOR32,
                              size + 2, 1);

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    hFind = FindFirstFile(c_dir, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        MessageBox(0, "Error", "Not found", MB_OK | MB_ICONWARNING);
    } else {
        do {
            if (strcmp(FindFileData.cFileName, ".") == 0) {
                char buf1[MAX_PATH];
                strcpy(buf1, c_dir);
                strcat(buf1, FindFileData.cFileName);
                SHGetFileInfo("", FILE_ATTRIBUTE_DEVICE, &lp, sizeof(lp),
                              SHGFI_ICONLOCATION | SHGFI_ICON | SHGFI_SMALLICON);

                ImageList_AddIcon(hSmall, lp.hIcon);
                DestroyIcon(lp.hIcon);

            }
            if (strcmp(FindFileData.cFileName, "..") == 0) {//если фаилы,папки
                char buf1[MAX_PATH];
                strcpy(buf1, c_dir);
                strcat(buf1, FindFileData.cFileName);
                SHGetFileInfo("", FILE_ATTRIBUTE_DIRECTORY, &lp, sizeof(lp),
                              SHGFI_ICONLOCATION | SHGFI_ICON | SHGFI_SMALLICON);

                ImageList_AddIcon(hSmall, lp.hIcon);
                DestroyIcon(lp.hIcon);
            }
            char buf1[MAX_PATH];
            strcpy(buf1, c_dir);
            buf1[strlen(buf1) - 1] = 0;

            strcat(buf1, FindFileData.cFileName);
            DWORD num = GetFileAttributes(buf1);
            SHGetFileInfo(buf1, num, &lp, sizeof(lp), SHGFI_ICONLOCATION | SHGFI_ICON | SHGFI_SMALLICON);

            ImageList_AddIcon(hSmall, lp.hIcon);
            DestroyIcon(lp.hIcon);

        } while (FindNextFile(hFind, &FindFileData) != 0);

        FindClose(hFind);
    }

    ListView_SetImageList(hWndListView, hSmall, LVSIL_SMALL);

    return TRUE;
}

void ListView_GetSelectedItems(HWND hWndListView, int *selectedItems) {
    int index = -1;
    int count = 0;
    while ((index = ListView_GetNextItem(hWndListView, index, LVNI_SELECTED)) != -1) {
        selectedItems[count] = index;
        count++;
    }
}

void Viev_List(const char *buf, HWND hList, int i, int j) {
    LVITEM lvItem;
    lvItem.mask = LVIF_IMAGE | LVIF_TEXT;
    lvItem.state = 0;
    lvItem.stateMask = 0;
    lvItem.iItem = i;
    lvItem.iImage = i;
    lvItem.iSubItem = j;
    lvItem.pszText = const_cast<LPSTR>(buf);
    lvItem.cchTextMax = sizeof(buf);
    ListView_InsertItem(hList, &lvItem);
}

void FindFile(HWND hList, char c_dir[MAX_PATH]) {

    SendMessage(hList, LVM_DELETEALLITEMS, (WPARAM) 0, (LPARAM) 0);
    i = 0;

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    hFind = FindFirstFile(c_dir, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        MessageBox(0, "Error", "Not found", MB_OK | MB_ICONWARNING);
    } else {
        do {
            if ((FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(FindFileData.cFileName)) != FILE_ATTRIBUTE_DIRECTORY)
                Viev_List("File", hList, i, 1);
            else Viev_List("Directory", hList, i, 1);

            Viev_List(FindFileData.cFileName, hList, i,
                      0);
            ++i;
        } while (FindNextFile(hFind, &FindFileData) != 0);

        FindClose(hFind);
        InitListViewImageLists(hList, i, c_dir);
    }
}

bool ShowCreateFileDialog(HWND hWnd) {
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Create Text File";
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    return GetSaveFileName(&ofn);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    DWORD Drivers;

    int i;
    char buf[] = " ";
    char *ls;
    string s;
    int y = 9;
    char buf1[MAX_PATH];

    LPNMHDR lpnmHdr;
    lpnmHdr = (LPNMHDR) lParam;
    LPNMLISTVIEW pnmLV = (LPNMLISTVIEW) lParam;

    GetLogicalDrives();
    GetLogicalDriveStrings(128, (LPTSTR) buf);
    switch (msg) {
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case IDM_CREATE: {
                    if (ShowCreateFileDialog(hwnd)) {
                        if (PathFindExtension(szFileName) == NULL ||
                            _stricmp(PathFindExtension(szFileName), ".txt") != 0) {
                            strcat(szFileName, ".txt");
                        }
                        HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL,
                                                  NULL);
                        if (hFile != INVALID_HANDLE_VALUE) {
                            CloseHandle(hFile);
                            HWND activeListView = (GetFocus() == hListView_1) ? hListView_1 : hListView_2;
                            char *targetDir = (activeListView == hListView_1) ? dir : dir1;
                            FindFile(activeListView, targetDir);
                        } else {
                            MessageBox(hwnd, "Failed to create the file.", "Error", MB_ICONERROR | MB_OK);
                        }
                    }
                    return 0;
                }
                case IDM_COPY: {
                    HWND activeListView = (GetFocus() == hListView_1) ? hListView_1 : hListView_2;
                    int itemCount = ListView_GetSelectedCount(activeListView);

                    if (itemCount > 0) {
                        int *selectedItems = new int[itemCount];
                        ListView_GetSelectedItems(activeListView, selectedItems);
                        char *targetDir = (activeListView == hListView_1) ? dir : dir1;

                        for (int i = 0; i < itemCount; i++) {
                            LVITEM lvItem;
                            lvItem.iItem = selectedItems[i];
                            lvItem.iSubItem = 0;
                            lvItem.mask = LVIF_TEXT;
                            lvItem.pszText = copy_buf1;
                            lvItem.cchTextMax = sizeof(copy_buf1);

                            if (ListView_GetItem(activeListView, &lvItem)) {
                                char cm_dir_from[MAX_PATH];
                                char cm_dir_to[MAX_PATH], cm_dir_to_[MAX_PATH];
                                strcpy(cm_dir_from, targetDir);
                                cm_dir_from[strlen(cm_dir_from) - 1] = 0;
                                strcat(cm_dir_from, copy_buf1);

                                char *otherTargetDir = (activeListView == hListView_1) ? dir1 : dir;

                                strcpy(cm_dir_to, otherTargetDir);
                                strcpy(cm_dir_to_, otherTargetDir);
                                cm_dir_to[strlen(cm_dir_to) - 1] = 0;
                                strcat(cm_dir_to, copy_buf1);

                                CopyFile(cm_dir_from, cm_dir_to, FALSE);
                            }
                        }

                        delete[] selectedItems;

                        FindFile(activeListView, targetDir);

                        HWND otherListView = (activeListView == hListView_1) ? hListView_2 : hListView_1;
                        char *otherTargetDir = (activeListView == hListView_1) ? dir1 : dir;
                        FindFile(otherListView, otherTargetDir);
                    }
                    return 0;
                }
                case IDM_DEL: {
                    HWND activeListView = (GetFocus() == hListView_1) ? hListView_1 : hListView_2;
                    int itemCount = ListView_GetSelectedCount(activeListView);

                    if (itemCount > 0) {
                        int *selectedItems = new int[itemCount];
                        ListView_GetSelectedItems(activeListView, selectedItems);

                        for (int i = 0; i < itemCount; i++) {
                            LVITEM lvItem;
                            lvItem.iItem = selectedItems[i];
                            lvItem.iSubItem = 0;
                            lvItem.mask = LVIF_TEXT;
                            lvItem.pszText = copy_buf1;
                            lvItem.cchTextMax = sizeof(copy_buf1);

                            if (ListView_GetItem(activeListView, &lvItem)) {
                                char cm_dir_from[MAX_PATH];
                                char cm_dir_to[MAX_PATH], cm_dir_to_[MAX_PATH];
                                strcpy(cm_dir_from, dir);
                                cm_dir_from[strlen(cm_dir_from) - 1] = 0;
                                strcat(cm_dir_from, copy_buf1);

                                strcpy(cm_dir_to, dir1);
                                strcpy(cm_dir_to_, dir1);
                                cm_dir_to[strlen(cm_dir_to) - 1] = 0;
                                strcat(cm_dir_to, copy_buf1);

                                DeleteFile(cm_dir_from);
                            }
                        }

                        delete[] selectedItems;

                        FindFile(hListView_1, dir);
                        FindFile(hListView_2, dir1);
                    }
                    return 0;
                }
                case IDM_MOVE: {
                    HWND activeListView = (GetFocus() == hListView_1) ? hListView_1 : hListView_2;

                    int itemCount = ListView_GetSelectedCount(activeListView);

                    if (itemCount > 0) {
                        int *selectedItems = new int[itemCount];
                        ListView_GetSelectedItems(activeListView, selectedItems);

                        char *targetDir = (activeListView == hListView_1) ? dir : dir1;

                        for (int i = 0; i < itemCount; i++) {
                            LVITEM lvItem;
                            lvItem.iItem = selectedItems[i];
                            lvItem.iSubItem = 0;
                            lvItem.mask = LVIF_TEXT;
                            lvItem.pszText = copy_buf1;
                            lvItem.cchTextMax = sizeof(copy_buf1);

                            if (ListView_GetItem(activeListView, &lvItem)) {
                                char cm_dir_from[MAX_PATH];
                                char cm_dir_to[MAX_PATH], cm_dir_to_[MAX_PATH];
                                strcpy(cm_dir_from, targetDir);
                                cm_dir_from[strlen(cm_dir_from) - 1] = 0;
                                strcat(cm_dir_from, copy_buf1);

                                char *otherTargetDir = (activeListView == hListView_1) ? dir1 : dir;

                                strcpy(cm_dir_to, otherTargetDir);
                                strcpy(cm_dir_to_, otherTargetDir);
                                cm_dir_to[strlen(cm_dir_to) - 1] = 0;
                                strcat(cm_dir_to, copy_buf1);

                                if (CopyFile(cm_dir_from, cm_dir_to, FALSE)) {
                                    DeleteFile(cm_dir_from);
                                }
                            }
                        }

                        delete[] selectedItems;

                        FindFile(hListView_1, dir);
                        FindFile(hListView_2, dir1);
                    }
                    return 0;
                }
                case ID_COMBOBOX_1: {
                    switch (HIWORD(wParam)) {
                        case CBN_SELENDOK: {
                            char path[80];
                            int sel;
                            strcpy(path, "");
                            sel = SendMessage(hComboBox_1, CB_GETCURSEL, 0, 0);
                            SendMessage(hComboBox_1, CB_GETLBTEXT, sel, (LPARAM) path);
                            strcat(path, "\*");
                            strcpy(dir, path);
                            SetWindowText(hLabel_3, dir);
                            FindFile(hListView_1, dir);
                            return 0;
                        }
                        default:
                            return 0;
                    }
                }
                case ID_COMBOBOX_2: {
                    switch (HIWORD(wParam)) {
                        case CBN_SELENDOK: {
                            char path[80];
                            int sel;
                            strcpy(path, "");
                            sel = SendMessage(hComboBox_2, CB_GETCURSEL, 0, 0);
                            SendMessage(hComboBox_2, CB_GETLBTEXT, sel, (LPARAM) path);
                            strcat(path, "\*");
                            strcpy(dir1, path);
                            SetWindowText(hLabel_4, dir1);
                            FindFile(hListView_2, dir1);
                            return 0;
                        }
                        default:
                            return 0;
                    }
                }
                    return 0;
            }
        }
        case WM_NOTIFY: {
            if ((lpnmHdr->idFrom == ID_LISTVIEW_1) &&
                (lpnmHdr->code == NM_CLICK)) {
                ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, pnmLV->iSubItem, buf1, MAX_PATH);
                strcpy(copy_buf1, buf1);

                char _dir[MAX_PATH];
                strcpy(_dir, dir);
                _dir[strlen(_dir) - 1] = 0;
                strcat(_dir, buf1);
            }
            if (((lpnmHdr->idFrom == ID_LISTVIEW_1) || (lpnmHdr->idFrom == ID_LISTVIEW_2)) &&
                (lpnmHdr->code == NM_DBLCLK)) {
                ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, pnmLV->iSubItem, buf1, MAX_PATH);

                if (lpnmHdr->idFrom == ID_LISTVIEW_1) {
                    char _dir[MAX_PATH];
                    int k = 0;
                    strcpy(_dir, dir);
                    _dir[strlen(_dir) - 1] = 0;
                    strcat(_dir, buf1);

                    for (int i = 0; i < strlen(buf1); i++) {
                        string s;
                        s = buf1[i];
                        if (s == ".") k = i;
                    }

                    if ((k != 0) && (k != 1)) {
                        ShellExecute(0, "open", _dir, NULL, NULL, SW_SHOWNORMAL);
                    } else {
                        if (strcmp(buf1, "..") == 0) {
                            int k;
                            dir[strlen(dir) - 2] = 0;
                            for (i = 0; i < strlen(dir); i++) {
                                string s;
                                s = dir[i];
                                if (s == "\\") k = i;
                            }
                            dir[k + 1] = 0;
                            strcat(dir, "*");
                        } else if (strcmp(buf1, ".") == 0) {
                            dir[3] = 0;
                            strcat(dir, "*");
                        } else {
                            strcat(buf1, "\\*");
                            dir[strlen(dir) - 1] = 0;
                            strcat(dir, buf1);
                        }
                        SetWindowText(hLabel_3, dir);
                        FindFile(hListView_1, dir);
                    }
                } else if (lpnmHdr->idFrom == ID_LISTVIEW_2) {
                    char _dir1[MAX_PATH];
                    int k = 0;
                    strcpy(_dir1, dir1);
                    _dir1[strlen(_dir1) - 1] = 0;
                    strcat(_dir1, buf1);

                    for (int i = 0; i < strlen(buf1); i++) {
                        string s;
                        s = buf1[i];
                        if (s == ".") k = i;
                    }
                    if ((k != 0) && (k != 1)) {
                        ShellExecute(0, "open", _dir1, NULL, NULL, SW_SHOWNORMAL);
                    } else {
                        if (strcmp(buf1, "..") == 0) {
                            int k;
                            dir1[strlen(dir1) - 2] = 0;
                            for (i = 0; i < strlen(dir1); i++) {
                                string s;
                                s = dir1[i];
                                if (s == "\\") k = i;
                            }
                            dir1[k + 1] = 0;
                            strcat(dir1, "*");
                        } else if (strcmp(buf1, ".") == 0) {
                            dir1[3] = 0;
                            strcat(dir1, "*");
                        } else {
                            strcat(buf1, "\\*");
                            dir1[strlen(dir1) - 1] = 0;
                            strcat(dir1, buf1);
                        }
                        SetWindowText(hLabel_4, dir1);
                        FindFile(hListView_2, dir1);
                    }
                }
            }
            break;
        }
        case WM_CREATE: {
            hToolBar = CreateSimpleToolbar(hwnd);
            hLabel_3 = CreateWindow("static", "path_left",
                                    WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                                    3, 57 + y, 400, 16,
                                    hwnd, (HMENU) ID_LABEL_3, hInstance, NULL);
            hLabel_4 = CreateWindow("static", "path_right",
                                    WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                                    453, 57 + y, 400, 16,
                                    hwnd, (HMENU) ID_LABEL_4, hInstance, NULL);
            hComboBox_1 = CreateWindow("ComboBox", NULL,
                                       WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | CBS_SORT,
                                       3, 33 + y, 100, 110, hwnd,
                                       (HMENU) ID_COMBOBOX_1, hInstance, NULL);
            hComboBox_2 = CreateWindow("ComboBox", NULL,
                                       WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | CBS_SORT,
                                       453, 33 + y, 100, 110, hwnd,
                                       (HMENU) ID_COMBOBOX_2, hInstance, NULL);
            hListView_1 = CreateWindow(WC_LISTVIEW, NULL,
                                       LVS_REPORT | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | LVS_AUTOARRANGE,
                                       0, 60 + y + 15, 450, 500, hwnd,
                                       (HMENU) ID_LISTVIEW_1, hInstance, NULL);
            hListView_2 = CreateWindow(WC_LISTVIEW, NULL,
                                       LVS_REPORT | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | LVS_AUTOARRANGE,
                                       451, 60 + y + 15, 450, 500, hwnd,
                                       (HMENU) ID_LISTVIEW_2, hInstance, NULL);

            ls = buf;
            while (*ls) {
                SendMessage(hComboBox_1, CB_ADDSTRING, 0, (LPARAM) ls);
                SendMessage(hComboBox_2, CB_ADDSTRING, 0, (LPARAM) ls);
                ls += strlen(ls) + 1;
            }
            SendMessage(hComboBox_1, CB_SETCURSEL, 1, 0);
            SendMessage(hComboBox_2, CB_SETCURSEL, 1, 0);

            AddColToListView("Name", 1, 100);

            SetWindowText(hLabel_3, dir);
            FindFile(hListView_1, dir);
            SetWindowText(hLabel_3, dir);

            FindFile(hListView_2, dir);
            SetWindowText(hLabel_4, dir1);

            return 0;
        }
        case WM_DESTROY: {
            DestroyWindow(hListView_1);
            DestroyWindow(hListView_2);
            DestroyWindow(hComboBox_1);
            DestroyWindow(hComboBox_2);
            DestroyWindow(hLabel_1);
            DestroyWindow(hLabel_2);
            DestroyWindow(hLabel_3);
            DestroyWindow(hLabel_4);
            DestroyWindow(hToolBar);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#pragma comment(lib, "comctl32.lib") // for Common Controls
#pragma comment(lib, "comdlg32.lib") // for Common Dialogs

#include <Windows.h>
#include <commctrl.h>
#include <cstring>
#include <string>

using namespace std;

#define BTNS_AUTOSIZE 16

#define TB_SETIMAGELIST    (WM_USER+48) // для установки изображения в панель инструментов
#define TB_LOADIMAGES       (WM_USER+50) // для загрузки изображений в панель инструментов

#define IDM_COPY      1001
#define IDM_DEL       1003
#define IDM_CRC       1004

#define ID_LISTVIEW_1   3001
#define ID_LISTVIEW_2   3002

#define ID_COMBOBOX_1    4001
#define ID_COMBOBOX_2    4002

#define ID_LABEL_3      5003
#define ID_LABEL_4      5004

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
const int numButtons = 3; // копировать - удалить / переместить
const DWORD buttonStyles = BTNS_AUTOSIZE;

const int bitmapSize = 16; // !!!!

TBBUTTON tbButtons[numButtons] =
        {
                {MAKELONG(STD_COPY, ImageListID), IDM_COPY,  TBSTATE_ENABLED,
                        BTNS_AUTOSIZE, {0}, 0, (INT_PTR)"copy"},
                {MAKELONG(STD_DELETE, ImageListID), IDM_DEL, TBSTATE_ENABLED,
                        buttonStyles,  {0}, 0, (INT_PTR)"delete"},
                {MAKELONG(STD_REPLACE, ImageListID), IDM_CRC,   TBSTATE_ENABLED,
                        buttonStyles,  {0}, 0, (INT_PTR)"move"}
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
    WndClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(230, 255, 0));
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
    //создание тулбара
    HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
                                      WS_CHILD | TBSTYLE_WRAPABLE,
                                      0, 0, 0, 0,
                                      hWndParent, NULL, hInstance, NULL);
    if (hWndToolbar == NULL) {
        return NULL;
    }

    //создание списка картинок, для кнопок
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
        do {//присваеваем атрибуты
            if (strcmp(FindFileData.cFileName, ".") == 0) {//если диск
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
            //присваеваем иконки
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
                      0);//выз. ф-ция Viev_List передаем туда наиденый фаил ,и HWND ListBox и итератор i++,
            ++i;
        } while (FindNextFile(hFind, &FindFileData) != 0);

        FindClose(hFind); //закрываем работу поиска фаилов
        InitListViewImageLists(hList, i, c_dir);//тут уже передаем HWND ListBox, и кол-во фаилов
    }
}

DWORD Crc8(DWORD *pcBlock, unsigned int len) {
    DWORD crc = 0xFF;
    unsigned int i;

    while (len--) {
        crc ^= *pcBlock++;

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }

    return crc;
}

DWORD GetCheckSum(char filename[MAX_PATH]) {
    HANDLE f;
    DWORD fsize, freal, res_crc;
    DWORD buf[500];
    void *p;

    f = CreateFile(filename, GENERIC_READ | GENERIC_WRITE,
            /*FILE_SHARE_READ*/0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                   0);

    long long nFileLen = 0;
    if (f != INVALID_HANDLE_VALUE) {

        DWORD dwSizeHigh = 0, dwSizeLow = 0;
        dwSizeLow = GetFileSize(f, &dwSizeHigh);
        nFileLen = (dwSizeHigh * (MAXDWORD + 1)) + dwSizeLow;

        SetFilePointer(f, nFileLen / 2, NULL, FILE_BEGIN);
        fsize = nFileLen - 1 - SetFilePointer(f, 0, 0, FILE_CURRENT);

        if (fsize > 500) fsize = 500;
        ReadFile(f, buf, fsize, &freal, NULL);
        CloseHandle(f);
        p = &buf;
        res_crc = Crc8(buf, fsize);
    }
    return res_crc;
}

LPCTSTR SumCrcFile(char c_dir[MAX_PATH]) {
    i = 0;
    char dir1[MAX_PATH];
    DWORD sum;

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    hFind = FindFirstFile(c_dir, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        MessageBox(0, "Error", "Not found", MB_OK | MB_ICONWARNING);
    } else {
        do {
            int k = 0;
            strcpy(dir1, c_dir);
            dir1[strlen(dir1) - 1] = 0;
            strcat(dir1, FindFileData.cFileName);
            //MessageBox(0, dir1, "",0 );

            if ((FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(dir1)) != FILE_ATTRIBUTE_DIRECTORY) {
                break;
            }

            ++i;
        } while (FindNextFile(hFind, &FindFileData) != 0);

        FindClose(hFind);
    }
    return dir1;
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
                case IDM_COPY: {
                    char cm_dir_from[MAX_PATH];
                    char cm_dir_to[MAX_PATH], cm_dir_to_[MAX_PATH];
                    strcpy(cm_dir_from, dir);
                    cm_dir_from[strlen(cm_dir_from) - 1] = 0;
                    strcat(cm_dir_from, copy_buf1);
                    //MessageBox(0, cm_dir_from, "sg",0);
                    strcpy(cm_dir_to, dir1);
                    strcpy(cm_dir_to_, dir1);
                    cm_dir_to[strlen(cm_dir_to) - 1] = 0;
                    //MessageBox(0, cm_dir_to, "sg",0);
                    strcat(cm_dir_to, copy_buf1);

                    CopyFile(cm_dir_from, cm_dir_to, FALSE);
                    MessageBox(0, "Copy is completed.", "", 0);
                    FindFile(hListView_2, cm_dir_to_);
                    return 0;
                }
                case IDM_DEL: {
                    char cm_dir_from[MAX_PATH], cm_dir_from_[MAX_PATH];
                    strcpy(cm_dir_from, dir);
                    strcpy(cm_dir_from_, dir);
                    cm_dir_from[strlen(cm_dir_from) - 1] = 0;
                    strcat(cm_dir_from, copy_buf1);
                    DeleteFile(cm_dir_from);
                    MessageBox(0, "File is deleted.", "", 0);
                    FindFile(hListView_1, cm_dir_from_);
                    return 0;
                }
                case IDM_CRC: {
                    char cm_dir_from[MAX_PATH];
                    char cm_dir_to[MAX_PATH], cm_dir_to_[MAX_PATH];
                    strcpy(cm_dir_from, dir);
                    cm_dir_from[strlen(cm_dir_from) - 1] = 0;
                    strcat(cm_dir_from, copy_buf1);

                    // Create the destination folder path
                    strcpy(cm_dir_to, dir1);
                    strcpy(cm_dir_to_, dir1);
                    cm_dir_to[strlen(cm_dir_to) - 1] = 0;
                    strcat(cm_dir_to, copy_buf1);

                    // Perform the file copy operation
                    if (CopyFile(cm_dir_from, cm_dir_to, FALSE)) {
                        // File copied successfully, now delete it from the source folder
                        if (DeleteFile(cm_dir_from)) {
                            MessageBox(0, "File is moved.", "", 0);
                            FindFile(hListView_1, cm_dir_from);
                            FindFile(hListView_2, cm_dir_to_);
                        } else {
                            MessageBox(0, "Failed to delete the file from the source folder.", "Error", MB_OK | MB_ICONERROR);
                        }
                    } else {
                        MessageBox(0, "Failed to copy the file to the destination folder.", "Error", MB_OK | MB_ICONERROR);
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

            if (((lpnmHdr->idFrom == ID_LISTVIEW_1) /*|| (lpnmHdr->idFrom == ID_LISTVIEW_2)*/) &&
                (lpnmHdr->code == NM_CLICK)) {
                ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, pnmLV->iSubItem, buf1, MAX_PATH);
                strcpy(copy_buf1, buf1);

                char _dir[MAX_PATH];
                strcpy(_dir, dir);
                _dir[strlen(_dir) - 1] = 0;
                strcat(_dir, buf1);

                if ((FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(_dir)) == FILE_ATTRIBUTE_DIRECTORY) {
                    SendMessage(hToolBar, TB_SETSTATE, (WPARAM) IDM_COPY, (LPARAM) MAKELONG(0, 0));
                    SendMessage(hToolBar, TB_SETSTATE, (WPARAM) IDM_DEL, (LPARAM) MAKELONG(0, 0));
                } else {
                    SendMessage(hToolBar, TB_SETSTATE, (WPARAM) IDM_COPY, (LPARAM) MAKELONG(TBSTATE_ENABLED, 0));
                    SendMessage(hToolBar, TB_SETSTATE, (WPARAM) IDM_DEL, (LPARAM) MAKELONG(TBSTATE_ENABLED, 0));
                }
            }
            if (((lpnmHdr->idFrom == ID_LISTVIEW_1) || (lpnmHdr->idFrom == ID_LISTVIEW_2)) &&
                (lpnmHdr->code == NM_DBLCLK)) {
                // Копируем строку в буфер из ячейки ListView (pnmLV->iItem - номер строки;
                // pnmLV->iSubItem - номер столбца)
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


            hLabel_3 = CreateWindow("static", "way1",
                                    WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                                    3, 57 + y, 400, 16,
                                    hwnd, (HMENU) ID_LABEL_3, hInstance, NULL);
            hLabel_4 = CreateWindow("static", "way2",
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
            /*AddColToListView("Тип", 2, 50);
            AddColToListView("Размер", 3, 50);*/

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
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

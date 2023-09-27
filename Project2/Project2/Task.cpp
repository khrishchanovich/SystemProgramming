#include <Windows.h>
#include <cmath>

void DrawObject(HDC hdc);
void UpdateObjectPosition();
void OnPaint(HDC hdc);
void OnTimer(HWND hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND g_hWnd; // �������� ����
HBRUSH g_hBrush = NULL; // �������� 

HMENU hMenu = CreateMenu();
HMENU hSubMenuShape = CreatePopupMenu();
HMENU hSubMenuMove = CreatePopupMenu();
HMENU hSubMenuColor = CreatePopupMenu();

COLORREF g_shapeColor = RGB(255, 0 , 0);

int g_startX = 400; // ��������� ���������� X �������
int g_startY = 300; // ��������� ���������� Y �������
int g_angle = 45; // ���� ��� ���������� ����������
int g_speed = 5; // �������� �������� �������

int g_x = g_startX; // ������� ���������� �������
int g_y = g_startY;

bool g_isMoving = false;
int g_direction = 0; // ����������� �������� (0 - ������, 1 - ����, 2 - �����, 3 - �����)
int g_selectedTrajectory = 0; // 0 - ������, 1 - ����������

int g_selectedShape = 0; // 0 - ����, 1 - �������
bool g_clockwise = true; // true - �� ������� �������, false - ������ ������� �������

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 1. ��������� ��������� WNDCLASSEX
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"Animation";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    // 2. ������������ ����� ����
    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 3. ������� ����
    g_hWnd = CreateWindow(L"Animation", L"Moving Object", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    if (g_hWnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 4. ���������� ����
    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    // 5. ��������� �������� ���� ���������
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

void DrawObject(HDC hdc)
{
    HBRUSH hBrush = CreateSolidBrush(g_shapeColor);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    if (g_selectedShape == 0) // ����
    {
        Ellipse(hdc, g_x, g_y, g_x + 50, g_y + 50);
    }
    else // �������
    {
        Rectangle(hdc, g_x, g_y, g_x + 50, g_y + 50);
    }

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}


void UpdateObjectPosition()
{
    if (g_selectedTrajectory == 0) // ������
    {
        if (g_clockwise)
        {
            // � ����������� �� �������� ����������� �������� �������� ���������� �������
            if (g_direction == 0) // ������
            {
                g_x += g_speed;
                if (g_x >= 600) // �������� ������� ����
                {
                    g_x = 600;
                    g_direction = 1; // ������ ����������� �� ����
                }
            }
            else if (g_direction == 1) // ����
            {
                g_y += g_speed;
                if (g_y >= 450) // �������� ������� ����
                {
                    g_y = 450;
                    g_direction = 2; // ������ ����������� �� �����
                }
            }
            else if (g_direction == 2) // �����
            {
                g_x -= g_speed;
                if (g_x <= 100) // �������� ������ ����
                {
                    g_x = 100;
                    g_direction = 3; // ������ ����������� �� �����
                }
            }
            else if (g_direction == 3) // �����
            {
                g_y -= g_speed;
                if (g_y <= 50) // �������� �������� ����
                {
                    g_y = 50;
                    g_direction = 0; // ������ ����������� �� ������
                }
            }
        }
        else // ������ ������� �������
        {
            // � ����������� �� �������� ����������� �������� �������� ���������� �������
            if (g_direction == 0) // ������
            {
                g_x -= g_speed;
                if (g_x <= 100) // �������� ������ ����
                {
                    g_x = 100;
                    g_direction = 3; // ������ ����������� �� �����
                }
            }
            else if (g_direction == 1) // ����
            {
                g_y -= g_speed;
                if (g_y <= 50) // �������� �������� ����
                {
                    g_y = 50;
                    g_direction = 0; // ������ ����������� �� ������
                }
            }
            else if (g_direction == 2) // �����
            {
                g_x += g_speed;
                if (g_x >= 600) // �������� ������� ����
                {
                    g_x = 600;
                    g_direction = 1; // ������ ����������� �� ����
                }
            }
            else if (g_direction == 3) // �����
            {
                g_y += g_speed;
                if (g_y >= 450) // �������� ������� ����
                {
                    g_y = 450;
                    g_direction = 2; // ������ ����������� �� �����
                }
            }
        }
    }
    else if (g_selectedTrajectory == 1) // ����������
    {
        // �������� ���� ��������, ����� ������ �������� �� ����������
        if (g_clockwise)
        {
            g_angle += g_speed;
            if (g_angle >= 360)
            {
                g_angle -= 360;
            }
        }
        else
        {
            g_angle -= g_speed;
            if (g_angle < 0)
            {
                g_angle += 360;
            }
        }

        double radians = g_angle * 3.14159265358979323846 / 180.0;
        // ��������� ����� ���������� �� ����������
        g_x = g_startX - static_cast<int>(100 * cos(radians)); // �������� �����, ����� ��������� �������
        g_y = g_startY - static_cast<int>(100 * sin(radians));
    }

    // �������� �� ����� �� ������� ����
    if (g_x < 100) g_x = 100;
    if (g_x > 600) g_x = 600;
    if (g_y < 50) g_y = 50;
    if (g_y > 450) g_y = 450;
}


void OnPaint(HDC hdc)
{
    if (g_isMoving)
    {
        UpdateObjectPosition();
    }

    SelectObject(hdc, g_hBrush); // �������� ����� ��� �������
    DrawObject(hdc);
}

void OnTimer(HWND hWnd)
{
    if (g_isMoving)
    {
        UpdateObjectPosition();
        // ��������� ����
        InvalidateRect(hWnd, NULL, TRUE); // �����������
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        // ������� �������� ���������� ��� ������ ���������� � ��������
        CreateWindowA("button", "Straight", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 10, 100, 30, hWnd, (HMENU)101, NULL, NULL);
        CreateWindowA("button", "Circle", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 40, 100, 30, hWnd, (HMENU)102, NULL, NULL);
        CreateWindowA("button", "Start/Stop", WS_VISIBLE | WS_CHILD, 10, 120, 100, 30, hWnd, (HMENU)104, NULL, NULL);
        CreateWindowA("edit", "5", WS_CHILD | WS_VISIBLE | ES_NUMBER, 10, 170, 50, 20, hWnd, (HMENU)105, NULL, NULL);

        // ������� ���� ��� ������ ���� ������, ����������� ��������, ��������� �����
        AppendMenu(hSubMenuShape, MF_STRING, 201, L"����");
        AppendMenu(hSubMenuShape, MF_STRING, 202, L"�������");
        AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenuShape, L"��� ������");

        AppendMenu(hSubMenuMove, MF_STRING, 203, L"�� ������� �������");
        AppendMenu(hSubMenuMove, MF_STRING, 204, L"������ ������� �������");
        AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenuMove, L"����������� ��������");

        AppendMenu(hSubMenuColor, MF_STRING, 301, L"�������");
        AppendMenu(hSubMenuColor, MF_STRING, 302, L"�������");
        AppendMenu(hSubMenuColor, MF_STRING, 303, L"�����");
        AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenuColor, L"���� ������");

        SetMenu(hWnd, hMenu);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 101: // ������� ������ ����������
            if (HIWORD(wParam) == BN_CLICKED)
            {
                g_selectedTrajectory = 0;
            }
            break;
        case 102: // ������� ���������� ����������
            if (HIWORD(wParam) == BN_CLICKED)
            {
                g_selectedTrajectory = 1;
            }
            break;
        case 104: // ������ Start/Stop
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                g_isMoving = !g_isMoving; // ����������� ��������
                if (g_isMoving)
                {
                    SetTimer(hWnd, 1, 1000 / 60, NULL); // ������������� ������ ��� �������� (60 FPS)
                }
                else
                {
                    KillTimer(hWnd, 1); // ������������� ������
                    // ��������� ����
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
        }
        break;
        case 105: // ���� ����� ��������
            if (HIWORD(wParam) == EN_CHANGE)
            {
                // ��������� ��������� �������� ��������
                WCHAR buffer[10];
                GetWindowText(GetDlgItem(hWnd, 105), buffer, 10);
                g_speed = _wtoi(buffer);
            }
            break;
        case 201: // ����
            g_selectedShape = 0;
            break;
        case 202: // �������
            g_selectedShape = 1;
            break;
        case 203: // �� ������� �������
            g_clockwise = true;
            break;
        case 204: // ������ ������� �������
            g_clockwise = false;
            break;
        case 301:
            g_shapeColor = RGB(255, 0, 0); // ������� ����
            break;
        case 302:
            g_shapeColor = RGB(0, 255, 0); // ������� ����
            break;
        case 303:
            g_shapeColor = RGB(0, 0, 255); // ����� ����
            break;
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_TIMER:
    {
        if (wParam == 1)
        {
            OnTimer(hWnd);
        }
    }
    break;
    case WM_DESTROY:
        if (hMenu != NULL)
        {
            DestroyMenu(hMenu);
            hMenu = NULL;
        }
        if (hSubMenuShape != NULL)
        {
            DestroyMenu(hSubMenuShape);
            hSubMenuShape = NULL;
        }
        if (hSubMenuMove != NULL)
        {
            DestroyMenu(hSubMenuMove);
            hSubMenuMove = NULL;
        }
        if (hSubMenuColor != NULL)
        {
            DestroyMenu(hSubMenuColor);
            hSubMenuColor = NULL;
        }
        if (g_hBrush != NULL)
        {
            DeleteObject(g_hBrush);
            g_hBrush = NULL;
        }
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

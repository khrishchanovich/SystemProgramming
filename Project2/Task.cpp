#include <Windows.h>
#include <cmath>

void DrawObject(HDC hdc);
void UpdateObjectPosition();
void OnPaint(HDC hdc);
void OnTimer(HWND hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND g_hWnd; // Хранение окна
HBRUSH g_hBrush = NULL; // Хранение 

HMENU hMenu = CreateMenu();
HMENU hSubMenuShape = CreatePopupMenu();
HMENU hSubMenuMove = CreatePopupMenu();
HMENU hSubMenuColor = CreatePopupMenu();

COLORREF g_shapeColor = RGB(255, 0 , 0);

int g_startX = 400; // Начальные координаты X объекта
int g_startY = 300; // Начальные координаты Y объекта
int g_angle = 45; // Угол для траектории окружности
int g_speed = 5; // Скорость движения объекта

int g_x = g_startX; // Текущие координаты объекта
int g_y = g_startY;

bool g_isMoving = false;
int g_direction = 0; // Направление движения (0 - вправо, 1 - вниз, 2 - влево, 3 - вверх)
int g_selectedTrajectory = 0; // 0 - Прямая, 1 - Окружность

int g_selectedShape = 0; // 0 - Круг, 1 - Квадрат
bool g_clockwise = true; // true - по часовой стрелке, false - против часовой стрелки

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 1. Заполняем структуру WNDCLASSEX
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

    // 2. Регистрируем класс окна
    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 3. Создаем окно
    g_hWnd = CreateWindow(L"Animation", L"Moving Object", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    if (g_hWnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 4. Отображаем окно
    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    // 5. Запускаем основной цикл сообщений
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

    if (g_selectedShape == 0) // Круг
    {
        Ellipse(hdc, g_x, g_y, g_x + 50, g_y + 50);
    }
    else // Квадрат
    {
        Rectangle(hdc, g_x, g_y, g_x + 50, g_y + 50);
    }

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}


void UpdateObjectPosition()
{
    if (g_selectedTrajectory == 0) // Прямая
    {
        if (g_clockwise)
        {
            // В зависимости от текущего направления движения изменяем координаты объекта
            if (g_direction == 0) // Вправо
            {
                g_x += g_speed;
                if (g_x >= 600) // Достигли правого края
                {
                    g_x = 600;
                    g_direction = 1; // Меняем направление на вниз
                }
            }
            else if (g_direction == 1) // Вниз
            {
                g_y += g_speed;
                if (g_y >= 450) // Достигли нижнего края
                {
                    g_y = 450;
                    g_direction = 2; // Меняем направление на влево
                }
            }
            else if (g_direction == 2) // Влево
            {
                g_x -= g_speed;
                if (g_x <= 100) // Достигли левого края
                {
                    g_x = 100;
                    g_direction = 3; // Меняем направление на вверх
                }
            }
            else if (g_direction == 3) // Вверх
            {
                g_y -= g_speed;
                if (g_y <= 50) // Достигли верхнего края
                {
                    g_y = 50;
                    g_direction = 0; // Меняем направление на вправо
                }
            }
        }
        else // Против часовой стрелки
        {
            // В зависимости от текущего направления движения изменяем координаты объекта
            if (g_direction == 0) // Вправо
            {
                g_x -= g_speed;
                if (g_x <= 100) // Достигли левого края
                {
                    g_x = 100;
                    g_direction = 3; // Меняем направление на вверх
                }
            }
            else if (g_direction == 1) // Вниз
            {
                g_y -= g_speed;
                if (g_y <= 50) // Достигли верхнего края
                {
                    g_y = 50;
                    g_direction = 0; // Меняем направление на вправо
                }
            }
            else if (g_direction == 2) // Влево
            {
                g_x += g_speed;
                if (g_x >= 600) // Достигли правого края
                {
                    g_x = 600;
                    g_direction = 1; // Меняем направление на вниз
                }
            }
            else if (g_direction == 3) // Вверх
            {
                g_y += g_speed;
                if (g_y >= 450) // Достигли нижнего края
                {
                    g_y = 450;
                    g_direction = 2; // Меняем направление на влево
                }
            }
        }
    }
    else if (g_selectedTrajectory == 1) // Окружность
    {
        // Изменяем угол движения, чтобы объект двигался по окружности
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
        // Вычисляем новые координаты на окружности
        g_x = g_startX - static_cast<int>(100 * cos(radians)); // Изменить число, чтобы увеличить диаметр
        g_y = g_startY - static_cast<int>(100 * sin(radians));
    }

    // Проверка на выход за границы окна
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

    SelectObject(hdc, g_hBrush); // Выбираем кисть для заливки
    DrawObject(hdc);
}

void OnTimer(HWND hWnd)
{
    if (g_isMoving)
    {
        UpdateObjectPosition();
        // Обновляем окно
        InvalidateRect(hWnd, NULL, TRUE); // Перерисовка
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        // Создаем элементы управления для выбора траектории и скорости
        CreateWindowA("button", "Straight", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 10, 100, 30, hWnd, (HMENU)101, NULL, NULL);
        CreateWindowA("button", "Circle", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 10, 40, 100, 30, hWnd, (HMENU)102, NULL, NULL);
        CreateWindowA("button", "Start/Stop", WS_VISIBLE | WS_CHILD, 10, 120, 100, 30, hWnd, (HMENU)104, NULL, NULL);
        CreateWindowA("edit", "5", WS_CHILD | WS_VISIBLE | ES_NUMBER, 10, 170, 50, 20, hWnd, (HMENU)105, NULL, NULL);

        // Создаем меню для выбора типа фигуры, направления движения, изменения цвета
        AppendMenu(hSubMenuShape, MF_STRING, 201, L"Круг");
        AppendMenu(hSubMenuShape, MF_STRING, 202, L"Квадрат");
        AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenuShape, L"Тип фигуры");

        AppendMenu(hSubMenuMove, MF_STRING, 203, L"По часовой стрелке");
        AppendMenu(hSubMenuMove, MF_STRING, 204, L"Против часовой стрелки");
        AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenuMove, L"Направление движения");

        AppendMenu(hSubMenuColor, MF_STRING, 301, L"Красный");
        AppendMenu(hSubMenuColor, MF_STRING, 302, L"Зеленый");
        AppendMenu(hSubMenuColor, MF_STRING, 303, L"Синий");
        AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenuColor, L"Цвет фигуры");

        SetMenu(hWnd, hMenu);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 101: // Выбрана Прямая траектория
            if (HIWORD(wParam) == BN_CLICKED)
            {
                g_selectedTrajectory = 0;
            }
            break;
        case 102: // Выбрана Окружность траектория
            if (HIWORD(wParam) == BN_CLICKED)
            {
                g_selectedTrajectory = 1;
            }
            break;
        case 104: // Кнопка Start/Stop
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                g_isMoving = !g_isMoving; // Переключаем анимацию
                if (g_isMoving)
                {
                    SetTimer(hWnd, 1, 1000 / 60, NULL); // Устанавливаем таймер для анимации (60 FPS)
                }
                else
                {
                    KillTimer(hWnd, 1); // Останавливаем таймер
                    // Обновляем окно
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
        }
        break;
        case 105: // Поле ввода скорости
            if (HIWORD(wParam) == EN_CHANGE)
            {
                // Обработка изменения значения скорости
                WCHAR buffer[10];
                GetWindowText(GetDlgItem(hWnd, 105), buffer, 10);
                g_speed = _wtoi(buffer);
            }
            break;
        case 201: // Круг
            g_selectedShape = 0;
            break;
        case 202: // Квадрат
            g_selectedShape = 1;
            break;
        case 203: // По часовой стрелке
            g_clockwise = true;
            break;
        case 204: // Против часовой стрелки
            g_clockwise = false;
            break;
        case 301:
            g_shapeColor = RGB(255, 0, 0); // Красный цвет
            break;
        case 302:
            g_shapeColor = RGB(0, 255, 0); // Зеленый цвет
            break;
        case 303:
            g_shapeColor = RGB(0, 0, 255); // Синий цвет
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

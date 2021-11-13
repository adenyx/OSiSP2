#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <wingdi.h>
#include <vector>
#include <string>
#include <fstream>
#include <regex>

using namespace std;

vector<vector<string>> table{};
int colCount;
int rowCount;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void readData();


int WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
    readData();

    const wchar_t CLASS_NAME[] = L"Window Class";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);


    HWND hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            L"OSiSP_lab2",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 900, 300,
            nullptr,
            nullptr,
            hInstance,
            nullptr
    );

    if (hwnd == nullptr) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}


LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
            TEXTMETRIC textMetric;
            GetTextMetrics(hdc, &textMetric);
            int cellHeight = 2 * textMetric.tmHeight;
            RECT rcClient;
            GetClientRect(hwnd, &rcClient);
            int cellWidth = (rcClient.right - rcClient.left) / colCount;
            int totalHeight = cellHeight / 2;
            for (int i = 0; i < rowCount; i++) {
                int maxRowHeight = cellHeight;
                for (int j = 0; j < colCount; ++j) {
                    RECT cell;
                    cell.left = j * cellWidth;
                    cell.right = cell.left + cellWidth;
                    cell.top = totalHeight;
                    cell.bottom = rcClient.bottom;
                    wstring line = wstring(table[i][j].begin(), table[i][j].end());
                    int currentCellHeight = DrawText(hdc, line.c_str(),
                                                     -1, &cell, DT_WORDBREAK | DT_CENTER | DT_END_ELLIPSIS);
                    maxRowHeight = currentCellHeight > maxRowHeight ? currentCellHeight : maxRowHeight;
                }
                MoveToEx(hdc, 0, totalHeight - cellHeight / 2, nullptr);
                LineTo(hdc, rcClient.right, totalHeight - cellHeight / 2);
                totalHeight += maxRowHeight + cellHeight / 2;
            }

            MoveToEx(hdc, 0, totalHeight - cellHeight / 2, nullptr);
            LineTo(hdc, rcClient.right, totalHeight - cellHeight / 2);

            for (int i = 0; i <= colCount; i++) {
                MoveToEx(hdc, cellWidth * i, 0, nullptr);
                LineTo(hdc, cellWidth * i, totalHeight - cellHeight / 2);
            }
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_SIZE: {
            InvalidateRect(hwnd, nullptr, TRUE);
            break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void readData() {
    std::ifstream infile("../table.txt");
    string line;
    while (std::getline(infile, line)) {
        std::regex regex{R"([;][\s]+)"};
        std::sregex_token_iterator it{line.begin(), line.end(), regex, -1};
        table.emplace_back(vector<string>{it, {}});
    }
    rowCount = table.size();
    colCount = table[0].size();
}
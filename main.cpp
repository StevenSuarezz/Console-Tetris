#include <iostream>
#include <Windows.h>
using namespace std;

wstring tetronimo[7];

int nFieldWidth = 12;
int nFieldHeight = 18;
int nScreenWidth = 80;
int nScreenHeight = 30;
unsigned char* pField = nullptr;

// Returns index in tetronimo array for a rotated piece based on r rotation
int rotate(int x, int y, int r)
{
    switch (r % 4)
    {
        case 0: return y * 4 + x; // 0 degree rotation
        case 1: return 12 + y - (x * 4); // 90 degree rotation
        case 2: return 15 - (y * 4) - x; // 180 degree rotation
        case 3: return 3 - y + (x * 4); // 270 degree rotation
    }

    return 0;
}

int main()
{
    // Create tetronimo assets
    tetronimo[0].append(L"..x.");
    tetronimo[0].append(L"..x.");
    tetronimo[0].append(L"..x.");
    tetronimo[0].append(L"..x.");

    tetronimo[1].append(L"..x.");
    tetronimo[1].append(L".xx.");
    tetronimo[1].append(L".x..");
    tetronimo[1].append(L"....");

    tetronimo[2].append(L".x..");
    tetronimo[2].append(L".xx.");
    tetronimo[2].append(L"..x.");
    tetronimo[2].append(L"....");

    tetronimo[3].append(L"....");
    tetronimo[3].append(L".xx.");
    tetronimo[3].append(L".xx.");
    tetronimo[3].append(L"....");

    tetronimo[4].append(L"..x.");
    tetronimo[4].append(L".xx.");
    tetronimo[4].append(L"..x.");
    tetronimo[4].append(L"....");

    tetronimo[5].append(L"....");
    tetronimo[5].append(L".xx.");
    tetronimo[5].append(L"..x.");
    tetronimo[5].append(L"..x.");

    tetronimo[6].append(L"....");
    tetronimo[6].append(L".xx.");
    tetronimo[6].append(L".x..");
    tetronimo[6].append(L".x..");

    // Initialize playing field
    pField = new unsigned char[nFieldWidth * nFieldHeight];

    // Set left, right, and bottom borders to character '9' to indicate the tetris playing field
    for (int x = 0; x < nFieldWidth; x++)
        for (int y = 0; y < nFieldHeight; y++)
            pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

    // Create screen array and initialize to empty spaces
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
    for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
        screen[i] = L' ';

    // Create a screen buffer and set it as the active buffer
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    // Force console window to correct size
    HWND hwnd = GetConsoleWindow();
    if (hwnd != NULL)
    {
        MoveWindow(hwnd, 200, 200, 680, 400, TRUE);
    }

    // Game loop
    bool bGameOver = false;

    while (!bGameOver)
    {
        // Draw field into the screen
        for (int x = 0; x < nFieldWidth; x++)
        {
            for (int y = 0; y < nFieldHeight; y++) 
            {
                // Create a string with all of the characters used in the game
                wstring fieldChars = L" ABCDEFG=#";
                
                // Draw the field onto the screen by indexing the fieldChars string by the corresponding value in the pField array
                // Offset the x and y positions by 2 to place the tetris field away from the edges of the console
                screen[(y + 2) * nScreenWidth + (x + 2)] = fieldChars[pField[y * nFieldWidth + x]];
            }
                
        }

        // Display frame
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
    }

    return 0;
}
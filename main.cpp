#include <iostream>
#include <Windows.h>
using namespace std;

const int SCREEN_OFFSET = 2;

wstring tetronimo[7];

int nFieldWidth = 12;
int nFieldHeight = 18;
int nScreenWidth = 80;
int nScreenHeight = 30;

// Playing field which contains all information to be rendered
unsigned char* pPlayingField = nullptr;

// Returns index in tetronimo string (0 - 15) for a piece (x, y) depending on rotation r
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

bool doesPieceFit(int nTetronimo, int nRotation, int posX, int posY)
{
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            // Get index of the piece of a tetronimo given the rotation
            int pieceIndex = rotate(x, y, nRotation);

            int fieldIndex = (posY + y) * nFieldWidth + (posX + x);

            // Ensure no out of bound checks are made
            if (posX + x >= 0 && posX + x < nFieldWidth)
            {
                if (posY + y >= 0 && posY + y < nFieldHeight)
                {
                    // Collision detection
                    if (tetronimo[nTetronimo][pieceIndex] == L'X' && pPlayingField[fieldIndex] != 0)
                        return false;
                }
            }
        }
    }

    return true;
}

int main()
{
    // Create tetronimo assets
    tetronimo[0].append(L"..X.");
    tetronimo[0].append(L"..X.");
    tetronimo[0].append(L"..X.");
    tetronimo[0].append(L"..X.");

    tetronimo[1].append(L"..X.");
    tetronimo[1].append(L".XX.");
    tetronimo[1].append(L".X..");
    tetronimo[1].append(L"....");

    tetronimo[2].append(L".X..");
    tetronimo[2].append(L".XX.");
    tetronimo[2].append(L"..X.");
    tetronimo[2].append(L"....");

    tetronimo[3].append(L"....");
    tetronimo[3].append(L".XX.");
    tetronimo[3].append(L".XX.");
    tetronimo[3].append(L"....");

    tetronimo[4].append(L"..X.");
    tetronimo[4].append(L".XX.");
    tetronimo[4].append(L"..X.");
    tetronimo[4].append(L"....");

    tetronimo[5].append(L"....");
    tetronimo[5].append(L".XX.");
    tetronimo[5].append(L"..X.");
    tetronimo[5].append(L"..X.");

    tetronimo[6].append(L"....");
    tetronimo[6].append(L".XX.");
    tetronimo[6].append(L".X..");
    tetronimo[6].append(L".X..");

    // Initialize playing field
    pPlayingField = new unsigned char[nFieldWidth * nFieldHeight];

    // Set left, right, and bottom borders to character '9' to indicate the tetris playing field
    for (int x = 0; x < nFieldWidth; x++)
        for (int y = 0; y < nFieldHeight; y++)
            pPlayingField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

    // Create screen array and initialize to empty spaces
    wchar_t* pScreen = new wchar_t[nScreenWidth * nScreenHeight];
    for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
        pScreen[i] = L' ';

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

    // Temp debug/testing variables to test piece renderingg
    int nCurrentPiece = 1;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2;
    int nCurrentY = 0;


    // Boolean array which keeps track of state of keys
    bool bKey[4];

    // Game loop
    bool bGameOver = false;

    while (!bGameOver)
    {
        // Game timing
        this_thread::sleep_for(50ms);
        for (int k = 0; k < 4; k++)                             // R    L   D Z
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

        // Input
        if (bKey[1])
        {
            if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
            {
                nCurrentX = nCurrentX - 1
            }
        }

        // Game logic
    
        // Render output

        // Draw field into the screen
        for (int x = 0; x < nFieldWidth; x++)
        {
            for (int y = 0; y < nFieldHeight; y++) 
            {
                // Create a string with all of the characters used in the game
                wstring fieldChars = L" ABCDEFG=#";
                
                // Draw the field onto the screen by indexing the fieldChars string by the corresponding value in the pPlayingField array
                // Offset the x and y positions by SCREEN_OFFSET to place the tetris field away from the edges of the console
                pScreen[(y + SCREEN_OFFSET) * nScreenWidth + (x + SCREEN_OFFSET)] = fieldChars[pPlayingField[y * nFieldWidth + x]];
            }
                
        }

        // Draw current piece
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                if (tetronimo[nCurrentPiece][rotate(x, y, nCurrentRotation)] == L'X') {
                    pScreen[(nCurrentY + y + SCREEN_OFFSET) * nScreenWidth + (nCurrentX + x + SCREEN_OFFSET)] = nCurrentPiece + 'A';
                }
            }
        }

        // Display frame
        WriteConsoleOutputCharacter(hConsole, pScreen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
    }

    return 0;
}
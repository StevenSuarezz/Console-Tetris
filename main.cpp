#include <iostream>
#include <Windows.h>
#include <thread>
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

    // Initial piece values
    int nCurrentPiece = rand() % 7;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2;
    int nCurrentY = 0;

    // Boolean array which keeps track of state of keys
    bool bKey[4];
    // Rotate flag to clamp rotations
    bool bRotateHold = false;

    int nSpeed = 20;
    int nSpeedCounter = 0;
    bool bForceDown = false;

    // Game loop
    bool bGameOver = false;

    while (!bGameOver)
    {
        // Game timing
        this_thread::sleep_for(50ms);
        nSpeedCounter++;
        bForceDown = (nSpeedCounter == nSpeed);

        // Input - Read key states then check keys pressed and collision
        for (int k = 0; k < 4; k++)                             // R    L   D Z
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

        // Right arrow key
        nCurrentX += (bKey[0] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
        // Left arrow key
        nCurrentX -= (bKey[1] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;  
        // Down arrow key
        nCurrentY += (bKey[2] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;
        
        // Rotate key 'Z'
        if (bKey[3])
        {
            nCurrentRotation += (!bRotateHold && doesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
            bRotateHold = true;
        }
        else
        {
            bRotateHold = false;
        }

        // Game logic
        if (bForceDown)
        {
            // Check downwards collision detection and move piece down if it fits
            if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
            {
                nCurrentY++;
            }
            else
            {
                // Lock the current piece in the field
                for (int x = 0; x < 4; x++)
                    for (int y = 0; y < 4; y++)
                        if (tetronimo[nCurrentPiece][rotate(x, y, nCurrentRotation)] == L'X')
                            pPlayingField[(nCurrentY + y) * nFieldWidth + (nCurrentX + x)] = nCurrentPiece + 1;
                
                // Check if there are any horizontal lines
                for (int y = 0; y < 4; y++)
                {
                    // Only need to check the rows of the current tetronimo position
                    if (nCurrentY + y < nFieldHeight - 1)
                    {
                        // Loop through the row and set flag to false if empty space is found
                        bool bLine = true;
                        for (int x = 1; x < nFieldWidth - 1; x++)
                            if (pPlayingField[(nCurrentY + y) * nFieldWidth + x] == 0)
                            {
                                bLine = false;
                                break;
                            }

                        // If there is a horizontal line, set it to '='
                        if (bLine)
                        {
                            for (int x = 1; x < nFieldWidth - 1; x++)
                                pPlayingField[(nCurrentY + y) * nFieldWidth + x] = 8;
                        }
                    }
                }

                // Choose next piece
                nCurrentX = nFieldWidth / 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = rand() % 7;

                // End game if next piece does not fit
                bGameOver = !doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
            }
            nSpeedCounter = 0;
        }
    
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
                if (tetronimo[nCurrentPiece][rotate(x, y, nCurrentRotation)] == L'X') 
                {
                    pScreen[(nCurrentY + y + SCREEN_OFFSET) * nScreenWidth + (nCurrentX + x + SCREEN_OFFSET)] = nCurrentPiece + 'A';
                }
            }
        }

        // Display frame
        WriteConsoleOutputCharacter(hConsole, pScreen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
    }

    return 0;
}

//Left off on 30:30
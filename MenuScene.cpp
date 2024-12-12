#include "MenuScene.h"
#include "GameFramework.h"

HANDLE	hSelectEvent;


CMenuScene::CMenuScene(HWND _hWnd, HINSTANCE _hInst, CGameFramework* GameFramework) : CScene(_hWnd, _hInst, GameFramework)
{
}

CMenuScene::~CMenuScene()
{
}

void CMenuScene::Initialize()
{
    drawTutorial = false;

    backgroundImage = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MENU));


    // 버튼 설정
    Button_Help.pos = { rc.right / 4 - 100, 700, 200, 100 };
    Button_Help.size = { 0, 0, 200, 100 };
    Button_Help.ButtonImage = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_HELPBUTTON));


    Button_Play = { rc.right / 2 - 100, 700, 200, 100 };
    Button_Play.size = { 0, 0, 200, 100 };
    Button_Play.ButtonImage = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PLAYBUTTON));


    Button_Quit = { rc.right / 2 + 200, 700, 200, 100 };
    Button_Quit.size = { 0, 0, 200, 100 };
    Button_Quit.ButtonImage = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_QUITBUTTON));


    tutoralBtm = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TUTO));

    tutoExitButton.pos = { 535, 560, 135, 40 };
    tutoExitButton.size = { 0, 0, 135, 40 };
    tutoExitButton.ButtonImage = NULL;


    hSelectEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}
    



void CMenuScene::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	GetKeyboardState(pKeysBuffer);
    if (pKeysBuffer[VK_UP] & 0xF0) exit(0);
}


void CMenuScene::Update(float fTimeElapsed)
{
    // TODO : Menu Scene Update

}



void CMenuScene::Render()
{
    HDC hdc = GetDC(hWnd);

    HDC MemDC = CreateCompatibleDC(hdc);
    HDC MemDCImage = CreateCompatibleDC(hdc);

    hBit = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
    OldBit[0] = (HBITMAP)SelectObject(MemDC, hBit);
    OldBit[1] = (HBITMAP)SelectObject(MemDCImage, backgroundImage); //--- 배경 이미지	
    StretchBlt(MemDC, 0, 0, rc.right, rc.bottom, MemDCImage, 0, 0, 1220, 950, SRCCOPY);

    // 버튼 세개
    OldBit[1] = (HBITMAP)SelectObject(MemDCImage, Button_Help.ButtonImage);
    DrawImage(MemDC, Button_Help.pos, MemDCImage, Button_Help.size, RGB(255, 0, 255));


    OldBit[1] = (HBITMAP)SelectObject(MemDCImage, Button_Play.ButtonImage);
    DrawImage(MemDC, Button_Play.pos, MemDCImage, Button_Play.size, RGB(255, 0, 255));


    OldBit[1] = (HBITMAP)SelectObject(MemDCImage, Button_Quit.ButtonImage);
    DrawImage(MemDC, Button_Quit.pos, MemDCImage, Button_Quit.size, RGB(255, 0, 255));


    if (drawTutorial) {
        OldBit[2] = (HBITMAP)SelectObject(MemDCImage, tutoralBtm);
        RECT tutoPos{ rc.right / 2 - 210 , rc.bottom / 2 - 176 , 420, 352 };
        RECT tutoSize{ 0, 0, 320, 252 };
        DrawImage(MemDC, tutoPos, MemDCImage, tutoSize, RGB(255, 0, 255));
    }
    


    BitBlt(hdc, 0, 0, rc.right, rc.bottom, MemDC, 0, 0, SRCCOPY);


    // 자원 해제
    SelectObject(MemDC, OldBit[0]);
    DeleteObject(hBit);
    DeleteDC(MemDC);
    DeleteDC(MemDCImage);
    ReleaseDC(hWnd, hdc);
}


bool IsCursorInButton(SButton button, POINT cursorPos)
{
    if (cursorPos.x > button.pos.left && cursorPos.x < button.pos.left + button.size.right&&
        cursorPos.y > button.pos.top && cursorPos.y < button.pos.top + button.size.bottom)
        return true;
    return false;
}


void CMenuScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
    switch (nMessageID)
    {
    case WM_LBUTTONDOWN:
        break;
    case WM_RBUTTONDOWN:
        break;
    case WM_LBUTTONUP:
        SetCapture(hWnd);
        GetCursorPos(&cursorPos);
        ScreenToClient(hWnd, &cursorPos);

        if (IsCursorInButton(Button_Help, cursorPos)) {
            drawTutorial = true;
        }
        else if (IsCursorInButton(Button_Play, cursorPos)) {
            if (Login()) {  // Login 성공 시에만 다음 씬으로 이동
                GetFramework()->SetCurScene(LOBBYSCENE);
            }
        }
        else if (IsCursorInButton(Button_Quit, cursorPos)) {
            exit(0);
        }

        if (drawTutorial && IsCursorInButton(tutoExitButton, cursorPos)) {
            drawTutorial = false;
        }
        break;
    case WM_RBUTTONUP:
        ::ReleaseCapture();
        break;
    case WM_MOUSEMOVE:
        break;
    default:
        break;
    }
}

INT_PTR CALLBACK IpInputDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

char ipAddress[20] = "172.30.1.86";

bool CMenuScene::Login()
{
    // 윈속 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup 실패\n");
        return false;
    }

    // 소켓 생성
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("소켓 생성 실패\n");
        WSACleanup();
        return false;
    }

    //// 다이얼로그 생성 및 응답 확인
    //int response = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, IpInputDialogProc);

    //// 이벤트 대기
    //WaitForSingleObject(hSelectEvent, INFINITE);

    //if (response == IDCANCEL) {
    //    ResetEvent(hSelectEvent);
    //    return false;
    //}

    // 서버 연결
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, ipAddress, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(SERVERPORT);

    retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        printf("connect() 실패\n");
        closesocket(sock);
        WSACleanup();
        ResetEvent(hSelectEvent);
        return false;
    }

    recv(sock, (char*)&m_ID, sizeof(m_ID), 0);

    ResetEvent(hSelectEvent); // 이벤트 초기화
    return true;
}



INT_PTR CALLBACK IpInputDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    //switch (message)
    //{
    //case WM_INITDIALOG:
    //    SetDlgItemTextA(hDlg, IDC_EDIT1, ipAddress);
    //    return (INT_PTR)TRUE;

    //case WM_COMMAND:
    //    if (LOWORD(wParam) == IDOK) {
    //        GetDlgItemTextA(hDlg, IDC_EDIT1, ipAddress, 20);
    //        SetEvent(hSelectEvent); // 이벤트 설정
    //        EndDialog(hDlg, IDOK);
    //        return (INT_PTR)TRUE;
    //    }
    //    else if (LOWORD(wParam) == IDCANCEL) {
    //        SetEvent(hSelectEvent); // 이벤트 설정
    //        EndDialog(hDlg, IDCANCEL);
    //        return (INT_PTR)TRUE;
    //    }
    //    break;
    //}
    return (INT_PTR)FALSE;
}









#include "Map.h"


int ObstacleBreak[13][15];


CMap::CMap()
{
	xPos = 0;
	xPosF = 0.0f;

	Item_xPos = 0;
	Item_xPosF = 0.0f;
}

void CMap::Initialize(HINSTANCE _hInst, SC_PlayersInfoPacket receivedPacket, int _ClientID)		// 매개변수로 패킷 구조체를 받는다.
{
	hInst = _hInst;
	ClientID = _ClientID;
	for (int i = 0; i < 13; i++)			
	{
		for (int j = 0; j < 15; j++)
		{
			Board[i][j].SetPosition((j * 60 + 30), (i * 60 + 65));
		}
	}
	player[0] =  new CPlayer(_hInst, receivedPacket, 0);					// TODO : 여기서 플레이어의 초기화 데이터를 서버로부터 받은 후 playerData 객체를 생성한다.
	player[1] = new CPlayer(_hInst, receivedPacket, 1);					// TODO : 여기서 플레이어의 초기화 데이터를 서버로부터 받은 후 playerData 객체를 생성한다.

	// Resource
	BallonBitmap[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Bubble));
	BallonBitmap[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Explosion));
	ItemBitmap		= LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ITEM));
	WinBitmap[0]	= LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Win1));
	WinBitmap[1]	= LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Win2));

}

void CMap::Render(HDC MemDC, HDC MemDCImage)
{
	for (int i = 0; i < 2; i++) player[i]->Render(MemDC, MemDCImage, this);


	(HBITMAP)SelectObject(MemDCImage, BallonBitmap[0]);      // 물풍선
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (Board[i][j].GetState() == 4 || Board[i][j].GetState() == 40)
			{
				//printf("풍선 생성!\n");
				TransparentBlt(MemDC, j * 60 + 30, i * 60 + 65, 60, 60, MemDCImage, xPos, 0, 44, 41, RGB(255, 0, 255));   // 물풍선 설치
			}
		}
	}

	(HBITMAP)SelectObject(MemDCImage, BallonBitmap[1]);      // 물풍선 폭발
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (Board[i][j].GetState() == 5)
			{
				TransparentBlt(MemDC, j * 60 + 30, i * 60 + 65, 60, 60, MemDCImage, 0, 0, 40, 40, RGB(255, 0, 255));   // 물풍선 설치
			}
		}
	}

	(HBITMAP)SelectObject(MemDCImage, ItemBitmap);			// Item
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (Board[i][j].GetState() == 6)
			{
				TransparentBlt(MemDC, Board[i][j].x + 10, Board[i][j].y + 10, 40, 40, MemDCImage, Item_xPos, 45, 42, 45, RGB(255, 0, 255));		// 이동속도
			}
			else if (Board[i][j].GetState() == 7)
			{
				TransparentBlt(MemDC, Board[i][j].x + 10, Board[i][j].y + 10, 40, 40, MemDCImage, Item_xPos, 90, 42, 45, RGB(255, 0, 255));		// 물풍선 길이
			}
			else if (Board[i][j].GetState() == 8)
			{
				TransparentBlt(MemDC, Board[i][j].x + 10, Board[i][j].y + 10, 40, 40, MemDCImage, Item_xPos, 0, 42, 45, RGB(255, 0, 255));		// 물풍선 갯수
			}
			else if (Board[i][j].GetState() == 9)
			{
				TransparentBlt(MemDC, Board[i][j].x + 10, Board[i][j].y + 10, 40, 40, MemDCImage, Item_xPos, 270, 42, 45, RGB(255, 0, 255));		// 바늘
			}
		}
	}
	if(player[ClientID]->needle) TransparentBlt(MemDC, 1000, 735, 80, 80, MemDCImage, 0, 270, 42, 45, RGB(255, 0, 255));			// TODO : 클라이언트 ID를 인덱스로 받아서 해당 플레이어가 바늘을 가지고 있다면 바늘 아이템을 보유하고 있다고 아이템 창에 그린다

	if (player[0]->state == DEAD) {
		(HBITMAP)SelectObject(MemDCImage, WinBitmap[1]);		
		TransparentBlt(MemDC, 350, 400, 258, 88, MemDCImage, 0, 0, 258, 88, RGB(255, 0, 255));

	}
	else if (player[1]->state == DEAD) {
		(HBITMAP)SelectObject(MemDCImage, WinBitmap[0]);
		TransparentBlt(MemDC, 350, 400, 258, 88, MemDCImage, 0, 0, 258, 88, RGB(255, 0, 255));

	}
}

void CMap::Update(SC_PlayersInfoPacket receivedPacket, float fTimeElapsed)
{
	xPosF += 44 * 5 * fTimeElapsed;
	while (xPosF >= 44.0f) {
		xPosF = 0;
		xPos += 44;
		if (xPos >= 132)
			xPos = 0;
	}

	Item_xPosF += 42 * 10 * fTimeElapsed;
	while (Item_xPosF >= 42.0f) {
		Item_xPosF = 0;
		Item_xPos += 42;
		if (Item_xPos >= 126)
			Item_xPos = 0;
	}
}

/// <summary>
/// Village Map
/// </summary>

void CVillage::Initialize(HINSTANCE _hInst, SC_PlayersInfoPacket receivedPacket, int _ClientID)
{
	// TODO : Map이 Village일 때 초기화
	CMap::Initialize(_hInst, receivedPacket, _ClientID);

	for (int i = 0; i < 13; i++)		// TODO : 보드의 State를 가져온다.
	{
		for (int j = 0; j < 15; j++)
		{
			Board[i][j].SetState(receivedPacket.mapData.boardData[i][j].state);
		}
	}
	printf("block stat: %d\n", Board[12][3].GetState());
	// Resource
	TileBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Village_Tile));
	Block[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Red_Block));
	Block[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Orange_Block)); 
	Block[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Bush));
	Block[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Redhouse));
	Block[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Bluehouse));
	Block[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Yellowhouse));
	Block[6] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Tree));



}
void CVillage::Render(HDC MemDC, HDC MemDCImage)
{	
	(HBITMAP)SelectObject(MemDCImage, TileBitmap); //--- 배경 이미지
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			StretchBlt(MemDC, Board[i][j].x, Board[i][j].y, 60, 60, MemDCImage, 0, 0, 40, 40, SRCCOPY);                  // Tile
		}
	}

	(HBITMAP)SelectObject(MemDCImage, Block[0]); // 장애물
	for (int i = 2; i < 13; i += 2)
	{
		if (Board[0][i].GetState() == 2)
			TransparentBlt(MemDC, Board[0][i].x, Board[0][i].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
		if (Board[12][i].GetState() == 2)
			TransparentBlt(MemDC, Board[12][i].x, Board[12][i].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	}
	for (int i = 4; i < 11; i += 2)
	{
		if (Board[1][i].GetState() == 2)
			TransparentBlt(MemDC, Board[1][i].x, Board[1][i].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
		if (Board[11][i].GetState() == 2)
			TransparentBlt(MemDC, Board[11][i].x, Board[11][i].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	}
	if (Board[2][1].GetState() == 2) TransparentBlt(MemDC, Board[2][1].x, Board[2][1].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[2][3].GetState() == 2) TransparentBlt(MemDC, Board[2][3].x, Board[2][3].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[2][11].GetState() == 2) TransparentBlt(MemDC, Board[2][11].x, Board[2][11].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[2][13].GetState() == 2) TransparentBlt(MemDC, Board[2][13].x, Board[2][13].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[3][0].GetState() == 2) TransparentBlt(MemDC, Board[3][0].x, Board[3][0].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[3][5].GetState() == 2) TransparentBlt(MemDC, Board[3][5].x, Board[3][5].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[3][9].GetState() == 2) TransparentBlt(MemDC, Board[3][9].x, Board[3][9].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[3][14].GetState() == 2) TransparentBlt(MemDC, Board[3][14].x, Board[3][14].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	for (int i = 5; i < 10; i++)
	{
		if (Board[5][i].GetState() == 2)
			TransparentBlt(MemDC, Board[5][i].x, Board[5][i].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	}
	for (int i = 5; i < 10; i++)
	{
		if (Board[7][i].GetState() == 2)
			TransparentBlt(MemDC, Board[7][i].x, Board[7][i].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	}
	if (Board[8][1].GetState() == 2) TransparentBlt(MemDC, Board[8][1].x, Board[8][1].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[8][3].GetState() == 2) TransparentBlt(MemDC, Board[8][3].x, Board[8][3].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[8][11].GetState() == 2) TransparentBlt(MemDC, Board[8][11].x, Board[8][11].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[8][13].GetState() == 2) TransparentBlt(MemDC, Board[8][13].x, Board[8][13].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[9][0].GetState() == 2) TransparentBlt(MemDC, Board[9][0].x, Board[9][0].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[9][5].GetState() == 2) TransparentBlt(MemDC, Board[9][5].x, Board[9][5].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[9][9].GetState() == 2) TransparentBlt(MemDC, Board[9][9].x, Board[9][9].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[9][14].GetState() == 2) TransparentBlt(MemDC, Board[9][14].x, Board[9][14].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));


	(HBITMAP)SelectObject(MemDCImage, Block[1]); // 장애물
	for (int i = 3; i < 12; i += 2)
	{
		if (Board[0][i].GetState() == 2)
			TransparentBlt(MemDC, Board[0][i].x, Board[0][i].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
		if (Board[12][i].GetState() == 2)
			TransparentBlt(MemDC, Board[12][i].x, Board[12][i].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	}
	for (int i = 5; i < 11; i += 2)
	{
		if (Board[1][i].GetState() == 2)
			TransparentBlt(MemDC, Board[1][i].x, Board[1][i].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
		if (Board[11][i].GetState() == 2)
			TransparentBlt(MemDC, Board[11][i].x, Board[11][i].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	}
	if (Board[2][0].GetState() == 2) TransparentBlt(MemDC, Board[2][0].x, Board[2][0].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[2][2].GetState() == 2) TransparentBlt(MemDC, Board[2][2].x, Board[2][2].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[2][4].GetState() == 2) TransparentBlt(MemDC, Board[2][4].x, Board[2][4].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[2][10].GetState() == 2) TransparentBlt(MemDC, Board[2][10].x, Board[2][10].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[2][12].GetState() == 2) TransparentBlt(MemDC, Board[2][12].x, Board[2][12].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[2][14].GetState() == 2) TransparentBlt(MemDC, Board[2][14].x, Board[2][14].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	for (int i = 0; i < 5; i++)
	{
		if (Board[4][i].GetState() == 2)
			TransparentBlt(MemDC, Board[4][i].x, Board[4][i].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	}
	for (int i = 10; i < 15; i++)
	{
		if (Board[4][i].GetState() == 2)
			TransparentBlt(MemDC, Board[4][i].x, Board[4][i].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	}
	if (Board[8][0].GetState() == 2) TransparentBlt(MemDC, Board[8][0].x, Board[8][0].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[8][2].GetState() == 2) TransparentBlt(MemDC, Board[8][2].x, Board[8][2].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[8][4].GetState() == 2) TransparentBlt(MemDC, Board[8][4].x, Board[8][4].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[8][10].GetState() == 2) TransparentBlt(MemDC, Board[8][10].x, Board[8][10].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[8][12].GetState() == 2) TransparentBlt(MemDC, Board[8][12].x, Board[8][12].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[8][14].GetState() == 2) TransparentBlt(MemDC, Board[8][14].x, Board[8][14].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[10][0].GetState() == 2) TransparentBlt(MemDC, Board[10][0].x, Board[10][0].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[10][1].GetState() == 2) TransparentBlt(MemDC, Board[10][1].x, Board[10][1].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[10][3].GetState() == 2) TransparentBlt(MemDC, Board[10][3].x, Board[10][3].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[10][4].GetState() == 2) TransparentBlt(MemDC, Board[10][4].x, Board[10][4].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[10][10].GetState() == 2) TransparentBlt(MemDC, Board[10][10].x, Board[10][10].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[10][11].GetState() == 2) TransparentBlt(MemDC, Board[10][11].x, Board[10][11].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[10][13].GetState() == 2) TransparentBlt(MemDC, Board[10][13].x, Board[10][13].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));
	if (Board[10][14].GetState() == 2) TransparentBlt(MemDC, Board[10][14].x, Board[10][14].y, 60, 60, MemDCImage, 0, 0, 40, 44, RGB(255, 0, 255));

	(HBITMAP)SelectObject(MemDCImage, Block[3]); // 파란 집
	if (Board[3][1].GetState() == 3) TransparentBlt(MemDC, Board[3][1].x, Board[3][1].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[3][3].GetState() == 3) TransparentBlt(MemDC, Board[3][3].x, Board[3][3].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[3][11].GetState() == 3) TransparentBlt(MemDC, Board[3][11].x, Board[3][11].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[3][13].GetState() == 3)TransparentBlt(MemDC, Board[3][13].x, Board[3][13].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[5][1].GetState() == 3) TransparentBlt(MemDC, Board[5][1].x, Board[5][1].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[5][3].GetState() == 3) TransparentBlt(MemDC, Board[5][3].x, Board[5][3].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[5][11].GetState() == 3) TransparentBlt(MemDC, Board[5][11].x, Board[5][11].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[5][13].GetState() == 3) TransparentBlt(MemDC, Board[5][13].x, Board[5][13].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[6][5].GetState() == 3) TransparentBlt(MemDC, Board[6][5].x, Board[6][5].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[6][7].GetState() == 3) TransparentBlt(MemDC, Board[6][7].x, Board[6][7].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[6][9].GetState() == 3) TransparentBlt(MemDC, Board[6][9].x, Board[6][9].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[7][1].GetState() == 3) TransparentBlt(MemDC, Board[7][1].x, Board[7][1].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[7][3].GetState() == 3) TransparentBlt(MemDC, Board[7][3].x, Board[7][3].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[7][11].GetState() == 3) TransparentBlt(MemDC, Board[7][11].x, Board[7][11].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[7][13].GetState() == 3) TransparentBlt(MemDC, Board[7][13].x, Board[7][13].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));

	(HBITMAP)SelectObject(MemDCImage, Block[4]); // 빨간 집
	if (Board[1][1].GetState() == 3) TransparentBlt(MemDC, Board[1][1].x, Board[1][1].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[1][3].GetState() == 3) TransparentBlt(MemDC, Board[1][3].x, Board[1][3].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[1][11].GetState() == 3) TransparentBlt(MemDC, Board[1][11].x, Board[1][11].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[1][13].GetState() == 3) TransparentBlt(MemDC, Board[1][13].x, Board[1][13].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[9][1].GetState() == 3) TransparentBlt(MemDC, Board[9][1].x, Board[9][1].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[9][3].GetState() == 3) TransparentBlt(MemDC, Board[9][3].x, Board[9][3].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[9][11].GetState() == 3) TransparentBlt(MemDC, Board[9][11].x, Board[9][11].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[9][13].GetState() == 3) TransparentBlt(MemDC, Board[9][13].x, Board[9][13].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[10][7].GetState() == 3) TransparentBlt(MemDC, Board[10][7].x, Board[10][7].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));

	(HBITMAP)SelectObject(MemDCImage, Block[5]); // 나무
	if (Board[4][6].GetState() == 3) TransparentBlt(MemDC, Board[4][6].x, Board[4][6].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[4][8].GetState() == 3) TransparentBlt(MemDC, Board[4][8].x, Board[4][8].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[8][6].GetState() == 3) TransparentBlt(MemDC, Board[8][6].x, Board[8][6].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[8][8].GetState() == 3) TransparentBlt(MemDC, Board[8][8].x, Board[8][8].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	

	(HBITMAP)SelectObject(MemDCImage, Block[2]); // 부쉬
	if (Board[2][6].GetState() == 1) TransparentBlt(MemDC, Board[2][6].x, Board[2][6].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[2][8].GetState() == 1) TransparentBlt(MemDC, Board[2][8].x, Board[2][8].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[6][6].GetState() == 1) TransparentBlt(MemDC, Board[6][6].x, Board[6][6].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[6][8].GetState() == 1) TransparentBlt(MemDC, Board[6][8].x, Board[6][8].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[9][6].GetState() == 1) TransparentBlt(MemDC, Board[9][6].x, Board[9][6].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[9][8].GetState() == 1) TransparentBlt(MemDC, Board[9][8].x, Board[9][8].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[10][2].GetState() == 1) TransparentBlt(MemDC, Board[10][2].x, Board[10][2].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[10][5].GetState() == 1) TransparentBlt(MemDC, Board[10][5].x, Board[10][5].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[10][9].GetState() == 1) TransparentBlt(MemDC, Board[10][9].x, Board[10][9].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));
	if (Board[10][12].GetState() == 1) TransparentBlt(MemDC, Board[10][12].x, Board[10][12].y - 13, 60, 73, MemDCImage, 0, 0, 40, 57, RGB(255, 0, 255));

	CMap::Render(MemDC, MemDCImage);

}

void CVillage::Update(SC_PlayersInfoPacket receivedPacket, float fTimeElapsed)
{
	CMap::Update(receivedPacket, fTimeElapsed);

	for (int i = 0; i < 13; i++)		// TODO : 보드의 State를 가져온다.
	{
		for (int j = 0; j < 15; j++)
		{
			Board[i][j].SetState(receivedPacket.mapData.boardData[i][j].state);
		}
	}
	for (int i = 0; i < 2; i++) player[i]->Update(receivedPacket, fTimeElapsed);
}


/// <summary>
/// Pirate Map
/// </summary>

void CPirate::Initialize(HINSTANCE _hInst, SC_PlayersInfoPacket receivedPacket, int _ClientID)
{
	// TODO : Map이 Pirate일 때 초기화
	CMap::Initialize(_hInst, receivedPacket, _ClientID);

	for (int i = 0; i < 13; i++)		// TODO : 보드의 State를 가져온다.
	{
		for (int j = 0; j < 15; j++)
		{
			Board[i][j].SetState(receivedPacket.mapData.boardData[i][j].state);
		}
	}

	// Resource
	TileBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Pirate_Tile));
	block_pirate = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Pirate_Block));
	obstacle_pirate = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_Pirate_Block2));
}
void CPirate::Render(HDC MemDC, HDC MemDCImage)
{
	(HBITMAP)SelectObject(MemDCImage, TileBitmap); //--- 배경 이미지
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			StretchBlt(MemDC, Board[i][j].x, Board[i][j].y, 60, 60, MemDCImage, 0, 0, 40, 40, SRCCOPY);                  // Tile
		}
	}

	(HBITMAP)SelectObject(MemDCImage, block_pirate); // 장애물
	// 0줄
	for (int i = 0; i < 4; i++)
	{
		if (Board[0][i].GetState() == 2)
			TransparentBlt(MemDC, Board[0][i].x, Board[0][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	if (Board[0][7].GetState() == 2) TransparentBlt(MemDC, Board[0][7].x, Board[0][7].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	for (int i = 11; i < 15; i++)
	{
		if (Board[0][i].GetState() == 2) TransparentBlt(MemDC, Board[0][i].x, Board[0][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	// 1줄
	if (Board[1][0].GetState() == 2) TransparentBlt(MemDC, Board[1][0].x, Board[1][0].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	if (Board[1][1].GetState() == 3)TransparentBlt(MemDC, Board[1][1].x, Board[1][1].y - 60, 60, 120, MemDCImage, 120, 0, 40, 80, RGB(255, 0, 255));
	if (Board[1][2].GetState() == 2) TransparentBlt(MemDC, Board[1][2].x, Board[1][2].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	for (int i = 4; i < 7; i++)
	{
		if (Board[1][i].GetState() == 2) TransparentBlt(MemDC, Board[1][i].x, Board[1][i].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 8; i < 11; i++)
	{
		if (Board[1][i].GetState() == 2) TransparentBlt(MemDC, Board[1][i].x, Board[1][i].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	}
	if (Board[1][12].GetState() == 2) TransparentBlt(MemDC, Board[1][12].x, Board[1][12].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	if (Board[1][13].GetState() == 3) TransparentBlt(MemDC, Board[1][13].x, Board[1][13].y - 60, 60, 120, MemDCImage, 120, 0, 40, 80, RGB(255, 0, 255));
	if (Board[1][14].GetState() == 2) TransparentBlt(MemDC, Board[1][14].x, Board[1][14].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	// 2줄
	if (Board[2][0].GetState() == 2) TransparentBlt(MemDC, Board[2][0].x, Board[2][0].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	if (Board[2][1].GetState() == 2) TransparentBlt(MemDC, Board[2][1].x, Board[2][1].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	if (Board[2][3].GetState() == 2) TransparentBlt(MemDC, Board[2][3].x, Board[2][3].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	if (Board[2][5].GetState() == 2) TransparentBlt(MemDC, Board[2][5].x, Board[2][5].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	if (Board[2][7].GetState() == 2) TransparentBlt(MemDC, Board[2][7].x, Board[2][7].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	if (Board[2][9].GetState() == 2) TransparentBlt(MemDC, Board[2][9].x, Board[2][9].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	if (Board[2][11].GetState() == 2) TransparentBlt(MemDC, Board[2][11].x, Board[2][11].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	if (Board[2][13].GetState() == 2) TransparentBlt(MemDC, Board[2][13].x, Board[2][13].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	if (Board[2][14].GetState() == 2) TransparentBlt(MemDC, Board[2][14].x, Board[2][14].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	// 3줄
	for (int i = 3; i < 13; i++)
	{
		if (Board[i][0].GetState() == 2) TransparentBlt(MemDC, Board[i][0].x, Board[i][0].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 7; i < 11; i++)
	{
		if (Board[i][1].GetState() == 2) TransparentBlt(MemDC, Board[i][1].x, Board[i][1].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 8; i < 10; i++)
	{
		if (Board[i][2].GetState() == 2) TransparentBlt(MemDC, Board[i][2].x, Board[i][2].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 9; i < 13; i++)
	{
		if (Board[i][3].GetState() == 2) TransparentBlt(MemDC, Board[i][3].x, Board[i][3].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 10; i < 13; i++)
	{
		if (Board[i][4].GetState() == 2) TransparentBlt(MemDC, Board[i][4].x, Board[i][4].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 11; i < 13; i++)
	{
		if (Board[i][5].GetState() == 2) TransparentBlt(MemDC, Board[i][5].x, Board[i][5].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 6; i < 12; i++)
	{
		if (Board[12][i].GetState() == 2) TransparentBlt(MemDC, Board[12][i].x, Board[12][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 9; i < 12; i++)
	{
		if (Board[11][i].GetState() == 2) TransparentBlt(MemDC, Board[11][i].x, Board[11][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 3; i < 13; i++)
	{
		if (Board[i][14].GetState() == 2) TransparentBlt(MemDC, Board[i][14].x, Board[i][14].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}

	for (int i = 10; i < 12; i++)
	{
		if (Board[10][i].GetState() == 2) TransparentBlt(MemDC, Board[10][i].x, Board[10][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 11; i < 14; i++)
	{
		if (Board[9][i].GetState() == 2) TransparentBlt(MemDC, Board[9][i].x, Board[9][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 12; i < 14; i++)
	{
		if (Board[8][i].GetState() == 2) TransparentBlt(MemDC, Board[8][i].x, Board[8][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	if (Board[7][13].GetState() == 2) TransparentBlt(MemDC, Board[7][13].x, Board[7][13].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	for (int i = 4; i < 11; i++)
	{
		if (Board[3][i].GetState() == 2) TransparentBlt(MemDC, Board[3][i].x, Board[3][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 3; i < 12; i++)
	{
		if (Board[4][i].GetState() == 2) TransparentBlt(MemDC, Board[4][i].x, Board[4][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 4; i < 11; i++)
	{
		if (Board[5][i].GetState() == 2) TransparentBlt(MemDC, Board[5][i].x, Board[5][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 3; i < 6; i++)
	{
		if (Board[6][i].GetState() == 2) TransparentBlt(MemDC, Board[6][i].x, Board[6][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	for (int i = 9; i < 12; i++)
	{
		if (Board[6][i].GetState() == 2) TransparentBlt(MemDC, Board[6][i].x, Board[6][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));  /////////////
	}
	for (int i = 5; i < 10; i++)
	{
		if (Board[7][i].GetState() == 2) TransparentBlt(MemDC, Board[7][i].x, Board[7][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
		if (Board[8][i].GetState() == 2) TransparentBlt(MemDC, Board[8][i].x, Board[8][i].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	}
	if (Board[9][7].GetState() == 2) TransparentBlt(MemDC, Board[9][7].x, Board[9][7].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));
	for (int i = 3; i < 7; i++)
	{
		if (Board[i][2].GetState() == 2) TransparentBlt(MemDC, Board[i][2].x, Board[i][2].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
		if (Board[i][12].GetState() == 2) TransparentBlt(MemDC, Board[i][12].x, Board[i][12].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	}
	if (Board[7][3].GetState() == 2) TransparentBlt(MemDC, Board[7][3].x, Board[7][3].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	if (Board[7][11].GetState() == 2) TransparentBlt(MemDC, Board[7][11].x, Board[7][11].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	if (Board[8][4].GetState() == 2) TransparentBlt(MemDC, Board[8][4].x, Board[8][4].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	if (Board[8][10].GetState() == 2) TransparentBlt(MemDC, Board[8][10].x, Board[8][10].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	if (Board[9][5].GetState() == 2) TransparentBlt(MemDC, Board[9][5].x, Board[9][5].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	if (Board[9][9].GetState() == 2) TransparentBlt(MemDC, Board[9][9].x, Board[9][9].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	for (int i = 6; i < 9; i++)
	{
		if (Board[10][i].GetState() == 2) TransparentBlt(MemDC, Board[10][i].x, Board[10][i].y, 60, 60, MemDCImage, 40, 37, 40, 40, RGB(255, 0, 255));
	}
	if (Board[10][13].GetState() == 2) TransparentBlt(MemDC, Board[10][13].x, Board[10][13].y, 60, 60, MemDCImage, 0, 37, 40, 40, RGB(255, 0, 255));


	if (Board[11][1].GetState() == 3) TransparentBlt(MemDC, Board[11][1].x, Board[11][1].y - 60, 60, 120, MemDCImage, 120, 0, 40, 80, RGB(255, 0, 255));
	if (Board[11][13].GetState() == 3) TransparentBlt(MemDC, Board[11][13].x, Board[11][13].y - 60, 60, 120, MemDCImage, 120, 0, 40, 80, RGB(255, 0, 255));

	(HBITMAP)SelectObject(MemDCImage, obstacle_pirate); // 장애물
	if (Board[6][6].GetState() == 3 && Board[6][7].GetState() == 3 && Board[6][8].GetState() == 3) TransparentBlt(MemDC, Board[6][6].x, Board[6][6].y - 180, 180, 240, MemDCImage, 160, 0, 120, 160, RGB(255, 255, 255));

	CMap::Render(MemDC, MemDCImage);

}

void CPirate::Update(SC_PlayersInfoPacket receivedPacket, float fTimeElapsed)
{
	CMap::Update(receivedPacket, fTimeElapsed);

	for (int i = 0; i < 13; i++)		// TODO : 보드의 State를 가져온다.
	{
		for (int j = 0; j < 15; j++)
		{
			Board[i][j].SetState(receivedPacket.mapData.boardData[i][j].state);
		}
	}
	for (int i = 0; i < 2; i++) player[i]->Update(receivedPacket, fTimeElapsed);
}


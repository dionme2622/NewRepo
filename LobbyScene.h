#pragma once
#include "Map.h"
#include "Scene.h"



class CLobbyScene : public CScene
{
public:
	CLobbyScene(HWND _hWnd, HINSTANCE _hInst, CGameFramework* GameFramework);
	~CLobbyScene();

	virtual void Initialize();

	virtual void ProcessInput();

	virtual void Update(float fTimeElapsed);
	virtual void Render();

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void SelectMap();

	virtual void SendData(SOCKET _sock);
	virtual void ReceiveData(SOCKET _sock);


private:
	HBITMAP				 backgroundImage;

	HBITMAP				 selecMapImage;
	bool				 showSelectMap = false;

	HBITMAP				 mapImage;
	HBITMAP*			 mapImages;

	SSendLobbyPacket	 sendLobbyPacket;
	SRecvLobbyPacket     recvLobbyPacket;

	int pastData = -1;

};





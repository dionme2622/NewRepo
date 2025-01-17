#include "Common.h"
#define SERVERPORT 9000
#define BUFSIZE    50

// 클라이언트의 방향키 값 처리 함수
DWORD WINAPI ProcessClient(LPVOID arg)
{
    int retval;
    SOCKET client_sock = (SOCKET)arg;
    struct sockaddr_in clientaddr;
    char addr[INET_ADDRSTRLEN];
    int addrlen = sizeof(clientaddr);

    // 클라이언트 정보 얻기
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    printf("[TCP/%s:%d] 클라이언트 처리 시작\n", addr, ntohs(clientaddr.sin_port));

    // 방향키 값 수신
    char buf[BUFSIZE];
    int dir;
    while (1) {
        retval = recv(client_sock, (char*)&dir, sizeof(int), MSG_WAITALL); // 방향키 데이터 수신
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0) {
            break; // 연결 종료
        }

        buf[retval] = '\0'; // 수신한 문자열 종료 처리
        printf("[TCP/%s:%d] 방향키 입력값: %d\r", addr, ntohs(clientaddr.sin_port), dir);

        // "exit" 입력 시 연결 종료
        if (strcmp(buf, "exit") == 0) {
            printf("[TCP/%s:%d] 클라이언트 연결 종료 요청\n", addr, ntohs(clientaddr.sin_port));
            break;
        }
    }

    // 소켓 닫기
    closesocket(client_sock);
    printf("[TCP/%s:%d] 클라이언트 처리 종료\n", addr, ntohs(clientaddr.sin_port));
    return 0;
}

int main(int argc, char* argv[])
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // 소켓 생성
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    // bind()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    // 데이터 통신에 사용할 변수
    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;
    HANDLE hThread;

    printf("서버가 방향키 값 수신 대기 중입니다...\n");

    while (1) {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        // 접속한 클라이언트 정보 출력
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));

        // 스레드 생성하여 클라이언트 처리
        hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) {
            closesocket(client_sock);
        }
        else {
            CloseHandle(hThread);
        }
    }

    // 소켓 닫기
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}
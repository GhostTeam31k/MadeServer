#include <iostream>
#include <string>
#include <WS2tcpip.h>  // Windows용 Winsock 라이브러리
#pragma comment (lib, "ws2_32.lib")

int main() {
    // Winsock 초기화
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    if (WSAStartup(ver, &wsData) != 0) {
        std::cerr << "Winsock 초기화 실패" << std::endl;
        return 1;
    }

    // 서버 소켓 생성
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        std::cerr << "소켓 생성 실패" << std::endl;
        return 1;
    }

    // 주소 설정
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);  // 포트 54000을 사용
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    // 소켓을 주소에 바인딩
    if (bind(listening, (sockaddr*)&hint, sizeof(hint) == SOCKET_ERROR)) {
        std::cerr << "바인딩 실패" << std::endl;
        return 1;
    }

    // 연결 대기
    if (listen(listening, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "대기 실패" << std::endl;
        return 1;
    }

    std::cout << "서버가 클라이언트 연결을 대기 중..." << std::endl;

    // 클라이언트 연결 처리
    sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "클라이언트 연결 실패" << std::endl;
    }
    else {
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        ZeroMemory(host, NI_MAXHOST);
        ZeroMemory(service, NI_MAXSERV);

        if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
            std::cout << host << "가 " << service << " 포트로 연결되었습니다." << std::endl;
        }
        else {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            std::cout << host << "가 " << ntohs(client.sin_port) << " 포트로 연결되었습니다." << std::endl;
        }

        closesocket(listening);

        char buf[4096];
        while (true) {
            ZeroMemory(buf, 4096);

            int bytesReceived = recv(clientSocket, buf, 4096, 0);
            if (bytesReceived == SOCKET_ERROR) {
                std::cerr << "클라이언트로부터 메시지 수신 실패" << std::endl;
                break;
            }

            if (bytesReceived == 0) {
                std::cout << "클라이언트가 연결을 종료했습니다." << std::endl;
                break;
            }

            // 클라이언트에게 메시지 다시 보내기
            send(clientSocket, buf, bytesReceived, 0);
        }

        // 소켓 닫기
        closesocket(clientSocket);
    }

    // Winsock 종료
    WSACleanup();

    return 0;
}

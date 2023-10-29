#include <iostream>
#include <string>
#include <WS2tcpip.h>  // Windows�� Winsock ���̺귯��
#pragma comment (lib, "ws2_32.lib")

int main() {
    // Winsock �ʱ�ȭ
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    if (WSAStartup(ver, &wsData) != 0) {
        std::cerr << "Winsock �ʱ�ȭ ����" << std::endl;
        return 1;
    }

    // ���� ���� ����
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        std::cerr << "���� ���� ����" << std::endl;
        return 1;
    }

    // �ּ� ����
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);  // ��Ʈ 54000�� ���
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    // ������ �ּҿ� ���ε�
    if (bind(listening, (sockaddr*)&hint, sizeof(hint) == SOCKET_ERROR)) {
        std::cerr << "���ε� ����" << std::endl;
        return 1;
    }

    // ���� ���
    if (listen(listening, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "��� ����" << std::endl;
        return 1;
    }

    std::cout << "������ Ŭ���̾�Ʈ ������ ��� ��..." << std::endl;

    // Ŭ���̾�Ʈ ���� ó��
    sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Ŭ���̾�Ʈ ���� ����" << std::endl;
    }
    else {
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        ZeroMemory(host, NI_MAXHOST);
        ZeroMemory(service, NI_MAXSERV);

        if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
            std::cout << host << "�� " << service << " ��Ʈ�� ����Ǿ����ϴ�." << std::endl;
        }
        else {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            std::cout << host << "�� " << ntohs(client.sin_port) << " ��Ʈ�� ����Ǿ����ϴ�." << std::endl;
        }

        closesocket(listening);

        char buf[4096];
        while (true) {
            ZeroMemory(buf, 4096);

            int bytesReceived = recv(clientSocket, buf, 4096, 0);
            if (bytesReceived == SOCKET_ERROR) {
                std::cerr << "Ŭ���̾�Ʈ�κ��� �޽��� ���� ����" << std::endl;
                break;
            }

            if (bytesReceived == 0) {
                std::cout << "Ŭ���̾�Ʈ�� ������ �����߽��ϴ�." << std::endl;
                break;
            }

            // Ŭ���̾�Ʈ���� �޽��� �ٽ� ������
            send(clientSocket, buf, bytesReceived, 0);
        }

        // ���� �ݱ�
        closesocket(clientSocket);
    }

    // Winsock ����
    WSACleanup();

    return 0;
}

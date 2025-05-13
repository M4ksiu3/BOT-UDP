#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <stdint.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 1024

#pragma pack(push, 1)
typedef struct {
    uint8_t id;
    uint32_t timestamp;
    uint16_t temperature;
    uint8_t power_status;  // enum 0 siec 1 bateria
    uint8_t checksum;
} MeasurementPacket;
#pragma pack(pop)

uint8_t calculate_checksum(const MeasurementPacket* packet) {
    uint8_t sum = 0;
    const uint8_t* data = (const uint8_t*)packet;
    for (size_t i = 0; i < sizeof(MeasurementPacket) - 1; ++i) {
        sum += data[i];
    }
    return sum;
}

void display_packet(const MeasurementPacket* pkt) {
    time_t t = pkt->timestamp;
    struct tm* tm_info = localtime(&t);
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%d.%m.%Y %H:%M:%S", tm_info);

    printf("\n==== DANE POMIAROWE KLIENTA ====\n");
    printf("ID: %u\n", pkt->id);
    printf("Czas dokonanego pomiaru: %s\n", time_buf);
    printf("Temperatura: %.1f stopni Celsjusza\n", pkt->temperature / 10.0);
    printf("zasilanie: %s\n", pkt->power_status ? "bateria" : "siec");
    printf("=================================\n");
}

int main() {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    int client_len = sizeof(client_addr);
    int result;
    int port = 1;  // domyœlny port
    int choice;

    // MENU
    do {
        printf("\n===== KONFIGURACJA SERWERA =====\n");
        printf("Aktualny port: %d\n", port);
        printf("1. Dokonaj zmiany portu\n");
        printf("2. Uruchom serwer\n");
        printf("Wybierz opcje: ");
        scanf("%d", &choice);

        if (choice == 1) {
            printf("Podaj nowy port (1-65535): ");
            scanf("%d", &port);
            if (port < 1 || port > 65535) {
                printf("nie poprawny port ustawiono port na 1.\n");
                port = 1;
            }
        }

    } while (choice != 2);

    //winsock od windows
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("wsastartup blad uruchomienia: %d\n", result);
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("blad tworzenia gniazda: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("blad laczenia: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    while (1) {
        int recv_len = recvfrom(sockfd, buffer, sizeof(MeasurementPacket), 0,
            (struct sockaddr*)&client_addr, &client_len);

        if (recv_len == SOCKET_ERROR) {
            printf("blad podczas dbierania danych: %d\n", WSAGetLastError());
            continue;
        }

        if (recv_len < sizeof(MeasurementPacket)) {
            printf("blad odbierania danych (%d bajt)\n", recv_len);
            continue;
        }

        MeasurementPacket pkt;
        memcpy(&pkt, buffer, sizeof(MeasurementPacket));

        uint8_t expected_checksum = calculate_checksum(&pkt);
        if (pkt.checksum != expected_checksum) {
            printf("\nbledna suma kontrolna!\n");
            printf("ID: %d oczekiwano sumy kontrolnej: %02X, odebrano od klienta: %02X\n",
                pkt.id, expected_checksum, pkt.checksum);
            continue;
        }

        display_packet(&pkt);
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>
 
#define DEFAULT_SERVER_IP "192.168.1.1"
#define DEFAULT_PORT 1
#define DEFAULT_SEND_TIME 1
#define DEFAULT_LOG_TIME 1
#define DEFAULT_LOG_FILE "log.txt"
 
int sockfd;
FILE *log_fp = NULL;
uint8_t measurement_id = 1;
 // struktura danych 
typedef struct __attribute__((__packed__)) {
    uint8_t id;
    uint32_t timestamp;
    uint16_t temperature;  // *10
    uint8_t power_status;  // 0 = siec 1 = bateria forma enuma
    uint8_t checksum;       // suma kontrolna suma bajt
} MeasurementPacket;
 
uint8_t calculate_checksum(const MeasurementPacket *packet) {
    const uint8_t *data = (const uint8_t *)packet;
    uint8_t sum = 0;
    for (size_t i = 0; i < sizeof(MeasurementPacket) - 1; i++) {
        sum += data[i];
    }
    return sum;
}
 
void handle_sigint(int sig) {
    printf("\nzamykanie aplikacji klienta\n");
    if (log_fp) fclose(log_fp);
    close(sockfd);
    exit(0);
}
 
void menu_config(char *ip, int *port, int *send_interval, int *log_interval, char *log_path) {
    int choice;
    while (1) {
        printf("\n===== MENU KONFIGURACJI =====\n");
        printf("1. IP SERWERA        [%s]\n", ip);
        printf("2. PORT UDP        [%d]\n", *port);
        printf("3. Czas cyklu wysylania do serwera    [%d s]\n", *send_interval);
        printf("4. Czas cyklu logowania w konsoli klienta    [%d s]\n", *log_interval);
        printf("5. Sciezka pliku [%s]\n", log_path);
        printf("6. Rozpocznij program\n");
        printf("Prosze wybrac numer: ");
        scanf("%d", &choice);
        getchar();  //'\n' z bufora
 
        switch (choice) {
            case 1:
                printf("ustawiono adres IP: ");
                fgets(ip, 64, stdin); ip[strcspn(ip, "\n")] = 0;
                break;
            case 2:
                printf("ustawiono port: ");
                scanf("%d", port); getchar();
                break;
            case 3:
                printf("ustawiono czas wysylania (s): ");
                scanf("%d", send_interval); getchar();
                break;
            case 4:
                printf("ustawiono czas logowania (s): ");
                scanf("%d", log_interval); getchar();
                break;
            case 5:
                printf("ustawiono sciezke do pliku logu: ");
                fgets(log_path, 256, stdin); log_path[strcspn(log_path, "\n")] = 0;
                break;
            case 6:
                return;
            default:
                printf("Niepoprawna opcja!\n");
        }
    }
}
 
void write_log(const MeasurementPacket *packet) {
    if (!log_fp) return;
 
    char time_str[64];
    time_t ts = packet->timestamp;  // kopiowanie zmiennej
    struct tm *tm_info = localtime(&ts); 
 
    strftime(time_str, sizeof(time_str), "%d.%m.%Y %H:%M:%S", tm_info); //zmiana timestamp na format daty 
 
    fprintf(log_fp, "Pomiar ID: %d\n", packet->id);
    fprintf(log_fp, "Czas pomiaru: %s\n", time_str);
    fprintf(log_fp, "Temperatura: %.1f °C\n", packet->temperature / 10.0);
    fprintf(log_fp, "Zasilanie: %s\n", packet->power_status ? "bateria" : "siec");
    fprintf(log_fp, "__________________________\n");
    fflush(log_fp);
}
int main() {
    char server_ip[64] = DEFAULT_SERVER_IP;
    int server_port = DEFAULT_PORT;
    int send_time = DEFAULT_SEND_TIME;
    int log_time = DEFAULT_LOG_TIME;
    char log_path[256] = DEFAULT_LOG_FILE;
 
    menu_config(server_ip, &server_port, &send_time, &log_time, log_path);
 
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("tworzenia gniazda nie powiodlo sie");
        return 1;
    }
 
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Nie ma takiego adresu IP");
        return 1;
    }
 
    log_fp = fopen(log_path, "a");
    if (!log_fp) {
        log_fp = fopen(log_path, "w");
        if (!log_fp) {
            perror("otwieranie pliku logu nie powiodlo sie");
            return 1;
        }
    }
 
    signal(SIGINT, handle_sigint);
    srand(time(NULL));
 
    time_t last_log = 0;
 
    while (1) {
        MeasurementPacket packet;
        packet.id = measurement_id++;
        packet.timestamp = (uint32_t)time(NULL);
        packet.temperature = (rand() % 1001) + 200;  // 200–1200 wartosc losowa przeniesiona o 10 aby nie wysylac liczb zmienno przecinkowych
        packet.power_status = rand() % 2;
        packet.checksum = calculate_checksum(&packet);
 
        //wysyl pakietu
        ssize_t sent = sendto(sockfd, &packet, sizeof(packet), 0,
                              (struct sockaddr*)&server_addr, sizeof(server_addr));
 
        if (sent < 0) {
            perror("Blad wysylania pakietu");
        } else {
            printf("Poprawnie wyslano pomiar o ID: %d do %s:%d\n", packet.id, server_ip, server_port);
        }
 
        if (difftime(time(NULL), last_log) >= log_time) {
            write_log(&packet);
            last_log = time(NULL);
        }
 
        sleep(send_time);
    }
 
    close(sockfd);
    if (log_fp) fclose(log_fp);
    return 0;
}
 
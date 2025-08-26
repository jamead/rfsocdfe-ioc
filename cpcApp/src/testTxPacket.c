#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP   "10.0.142.116"
#define SERVER_PORT 3000
#define PACKET_SIZE 100000

int main(void) {
    uint8_t packet[PACKET_SIZE];
    memset(packet, 0, sizeof(packet));

    // Header
    packet[0] = 0x50; // 'P'
    packet[1] = 0x53; // 'S'

    uint16_t msgID = 11;
    packet[2] = (msgID >> 8) & 0xFF; // High byte
    packet[3] = msgID & 0xFF;        // Low byte

    uint32_t msgLen = htonl(PACKET_SIZE-8);
    memcpy(&packet[4], &msgLen, sizeof(msgLen));

    // Value (fill with dummy data, 0xAA)
    memset(&packet[8], 0xAA, PACKET_SIZE - 8);

    // Create TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return EXIT_FAILURE;
    }

    // Connect
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return EXIT_FAILURE;
    }

    // Send packet
    ssize_t sent = send(sock, packet, PACKET_SIZE, 0);
    if (sent != PACKET_SIZE) {
        perror("send");
        close(sock);
        return EXIT_FAILURE;
    }

    printf("Sent %zd bytes to %s:%d\n", sent, SERVER_IP, SERVER_PORT);

    sleep(2);
    close(sock);
    return EXIT_SUCCESS;
}


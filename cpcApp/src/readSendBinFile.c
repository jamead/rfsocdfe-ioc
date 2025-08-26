#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <epicsExport.h>
#include <aSubRecord.h>
#include <registryFunction.h>

#define HEADER_SIZE 20  //PSC Header=8, Data Offset=4, Total Len=4, Chunk Len=4
#define DATA_SIZE 100000
#define PACKET_SIZE DATA_SIZE + HEADER_SIZE
#define MSG_ID 11
#define SERVER_IP   "10.0.142.116"
#define SERVER_PORT 3000

long readSendBinFile(aSubRecord *prec) {
    const char *path      = (const char *)prec->a;
    const char *filename  = (const char *)prec->b;
    const char *ip_addr   = (const char *)prec->c;
    long daughterbd       = *((long *)prec->d);  // Get value from INPD
 

    int32_t *outBuffer    = (int32_t *)prec->vala;
    long *numElementsRead = (long *)prec->valb;
    
    uint32_t file_size;

    printf("Hello ReadSend Bin File\n");
    if (!path || !filename || !outBuffer || !ip_addr) {
        printf("Error: Missing input(s)\n");
        return -1;
    }

    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", path, filename);
    printf("Reading binary file: %s\n", fullPath);
    printf("Sending data to IP: %s, port %d\n", ip_addr, SERVER_PORT);

    FILE *fp = fopen(fullPath, "rb");
    if (!fp) {
        printf("Error: cannot open file %s\n", fullPath);
        return -1;
    }
    
    // Find total file size
    if (fseek(fp, 0, SEEK_END) != 0) {
        perror("Error seeking to end of file");
        fclose(fp);
        return 1;
    }
    file_size = ftell(fp);
    if (file_size < 0) {
        perror("Error getting file size");
        fclose(fp);
        return 1;
    }
    rewind(fp);   
    printf("File Size = %d bytes\n",file_size); 
    
    

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        fclose(fp);
        return -1;
    }

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
        printf("Invalid IP address format: %s\n", ip_addr);
        close(sockfd);
        fclose(fp);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connect failed");
        close(sockfd);
        fclose(fp);
        return -1;
    }

    uint32_t totalBytesSent = 0;



    uint8_t packet[PACKET_SIZE];
    memset(packet, 0, sizeof(packet));

    uint16_t msgID;
    if (daughterbd == 0) {
        printf("Programming DaughterBoard 0\n");
        msgID = 11;
    }
    else if (daughterbd == 1) {
        printf("Programming DaughterBoard 1\n");
        msgID = 12;
    }
    else {
        printf("Invalid Daughterboard Selection\n");
        return -1;
    }
        
        

    
    while (1) {
        // Header
        packet[0] = 0x50; // 'P'
        packet[1] = 0x53; // 'S'
        packet[2] = (msgID >> 8) & 0xFF; // High byte
        packet[3] = msgID & 0xFF;        // Low byte

        uint32_t msgLen = htonl(PACKET_SIZE-8);
        memcpy(&packet[4], &msgLen, sizeof(msgLen));
        
        uint32_t msgOffset = htonl(totalBytesSent);
        memcpy(&packet[8], &msgOffset, sizeof(msgOffset));

        uint32_t msgTotSize = htonl(file_size);
        memcpy(&packet[12], &msgTotSize, sizeof(msgTotSize));

        uint32_t count = fread(&packet[HEADER_SIZE], sizeof(int8_t), DATA_SIZE, fp);
        printf("Read %d bytes from file\n",count);
    
        uint32_t msgChunkSize = htonl(count);
        memcpy(&packet[16], &msgChunkSize, sizeof(msgChunkSize));
        
        // Send packet
        uint32_t sent = send(sockfd, packet, PACKET_SIZE, 0);
        if (sent != PACKET_SIZE) {
            printf("Only sent %d bytes out of %d bytes\n",sent, PACKET_SIZE);
            close(sockfd);
            return EXIT_FAILURE;
        }
        sleep(0.1);
        totalBytesSent += count;
        printf("Sent %d bytes.   Total %d bytes sent\n", sent, totalBytesSent);
        if (totalBytesSent >= file_size)
           break;


    }
    
    sleep(1);
    close(sockfd);



    //*numElementsRead = (long)totalBytesSent;
    //prec->neva = (unsigned long)totalBytesSent;

    return 0;
}

epicsRegisterFunction(readSendBinFile);


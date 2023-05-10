// client 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// 브리지 설정 후 IP 세팅
#define SERVER_IP "192.168.0.29"
#define SERVER_PORT 9001
#define BUF_SIZE 1024

int main(int argc, char const *argv[])
{
    int sockfd;
    char *filename = "test.png";
    char buffer[BUF_SIZE];
    struct sockaddr_in server_addr;
    FILE *fp;
    int packet_size = BUF_SIZE;

    // \uc18c\ucf13 \uc0dd\uc131
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // \uc11c\ubc84 \uc8fc\uc18c, \ud3ec\ud2b8 \uc124\uc815
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }

    // \uc774\ubbf8\uc9c0 \ud30c\uc77c \ub85c\ub4dc
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        perror("file open failed");
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *image_data = malloc(file_size + 1);
    fread(image_data, file_size, 1, fp);
    image_data[file_size] = '\0';
    fclose(fp);

    // \uc774\ubbf8\uc9c0 \ub370\uc774\ud130\ub97c \ud328\ud0b7\uc73c\ub85c \ub098\ub220\uc11c send
    int num_packets = (file_size + packet_size - 1) / packet_size;
    int bytes_sent = 0;
    for (int i = 0; i < num_packets; i++)
    {
        int packet_start = i * packet_size;
        int packet_end = packet_start + packet_size;
        if (packet_end > file_size)
            packet_end = file_size;

        int packet_size = packet_end - packet_start;
        memcpy(buffer, &image_data[packet_start], packet_size);
        if (sendto(sockfd, buffer, packet_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            perror("sendto failed");
            exit(EXIT_FAILURE);
        }
        bytes_sent += packet_size;
        printf("Sent packet %d/%d\n", i + 1, num_packets);
    }

    free(image_data);
    close(sockfd);
    printf("Sent %d bytes to server\n", bytes_sent);
    return 0;
}
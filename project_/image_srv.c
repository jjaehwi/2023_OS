#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 1024

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUF_SIZE] = {0};
    char *filename = "testIMG.jpg";
    FILE *fp;

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Receive packets and write to file
    int total_size = 0;
    int expected_packet_size = BUF_SIZE;
    while (total_size < expected_packet_size)
    {
        if ((valread = recvfrom(server_fd, buffer, expected_packet_size, 0, (struct sockaddr *)&address, &addrlen)) < 0)
        {
            perror("recvfrom failed");
            exit(EXIT_FAILURE);
        }

        total_size += valread;
    }

    fp = fopen(filename, "wb");
    fwrite(buffer, sizeof(char), total_size, fp);

    while (total_size < 60000)
    {
        if ((valread = recvfrom(server_fd, buffer, BUF_SIZE, 0, (struct sockaddr *)&address, &addrlen)) < 0)
        {
            perror("recvfrom failed");
            exit(EXIT_FAILURE);
        }

        total_size += valread;
        fwrite(buffer, sizeof(char), valread, fp);
    }

    fclose(fp);
    printf("Image received and written to %s\n", filename);

    close(server_fd);
    return 0;
}

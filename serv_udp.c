/*
clientから送られたデータを標準出力に書き出す。
./serv_udp  50000 | play  -t raw -b 16 -c 1 -e s -r 44100 -

*/
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#define N 1024
int main(int argc, char **argv)
{

    if (argc == 2)
    {
        int s = socket(PF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(atoi(argv[1]));
        addr.sin_addr.s_addr = INADDR_ANY;
        bind(s, (struct sockaddr *)&addr, sizeof(addr));
        socklen_t len = sizeof(addr);
        short data1[N];
        while (1)
        {
            memset(data1, 0, sizeof(data1));
            int n = recvfrom(s, data1, sizeof(data1), 0, (struct sockaddr *)&addr, &len); //読み込んだバイト数が返り値
                                                                                          // int n = read(0, data1, 2 * N);
            if (n == -1)
            {

                //  perror("recv");
                exit(1);
            }

            write(1, data1, n);
        }
        close(s);
    }
}
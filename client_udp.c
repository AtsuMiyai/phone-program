/*
データを標準入力から受け取ってサーバー側に送る。
rec  -t raw -b 16 -c 1 -e s -r 44100 - | ./client_udp 192.168.100.123 50000
 ./client_udp 192.168.100.123 50000
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
    if (argc == 3)
    {
        int s = socket(PF_INET, SOCK_DGRAM, 0);
        if (s == -1)
        {
            perror("socket");
            exit(1);
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        inet_aton(argv[1], &addr.sin_addr);
        addr.sin_port = htons(atoi(argv[2]));

        short store1[N];
        socklen_t len = sizeof(addr);
        FILE *rec;
        /* char *cmd1 = "rec  -t raw -b 16 -c 1 -e s -r 44100 -";
        rec = popen(cmd1, "r"); //クライアントからの接続を受け付けてから録音を始める。
                                //ここまでがサーバーだけの役割
        if (rec == NULL)
        {
            perror("open");
            exit(1);
        }*/

        while (1)
        {
            memset(store1, 0, sizeof(store1));
            int n = read(0, store1, 2 * N); //読み込んだバイト数
            //int n = fread(store1, 2, N, rec); //文字数を読み込んでいる
            if (n == 0)
            {
                break;
            }
            if (n == -1)
            {
                perror("read");
                exit(1);
            }
            //int m = sendto(s, store1, 2 * n, 0, (struct sockaddr *)&addr, sizeof(addr));
            int m = sendto(s, store1, n, 0, (struct sockaddr *)&addr, sizeof(addr)); //
            if (m == -1)
            {
                perror("sendto");
                exit(1);
            }
        }
        close(s);
    }
}

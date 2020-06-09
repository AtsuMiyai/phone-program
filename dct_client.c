/*
サーバー側から送られてきたデータに対し、コサイン逆変換を適用し、時間領域の音声データに変換し、そのデータを標準出力に出す。

 ./dct_client_recv [IPアドレス] [ポート番号] | play -t raw -b 16 -c 1 -e s -r 44100 - 
  
*/
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#define N 1024
short max(int n, int m)
{
    if (n > m)
    {
        return n;
    }
    return m;
}
short DCT(double F[], short f[], int n)
{ /*コサイン変換を行う。*/
    int max1 = 0;

    for (int i = 0; i < n; i++)
    {
        double c = 1;
        if (i == 0)
        {
            c = 0.5;
        }
        for (int j = 0; j < n; j++)
        {
            F[i] += sqrt(2 / (double)n * c) * f[j] * cos((2 * j + 1) * i * M_PI / (double)(2 * n));
            max1 = max(max1, fabs(F[i]));
        }
    }
    //最大の周波数成分の大きさmax1の1/10以下の大きさを持つ音はなくす。
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (fabs(F[i]) <= max1 / 10.0)
        {
            F[i] = 0;
        } //最後に0じゃない数字が出た場所を記録しておく
        if (F[i] != 0)
        {
            count = i;
        }
    }
    return count;
}
void IDCT(double f1[], short F[], int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            double c = 1;
            if (j == 0)
            {
                c = 0.5;
            }
            f1[i] += sqrt(2 / (double)n * c) * F[j] * cos((2 * i + 1) * j * M_PI / (double)(2 * n));
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("~ [ipアドレス] [ポート番号]");
        exit(1);
    }
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1)
    {
        perror("open");
        exit(1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;                 /* 􏱔􏱅􏰊IPv4 􏰣􏶒􏵩􏴇􏱼􏰓􏰆 */
    addr.sin_addr.s_addr = inet_addr(argv[1]); /* IP􏶒􏵩􏴇􏱼􏰊...􏰓􏰆 */
    addr.sin_port = htons(atoi(argv[2]));      /* 􏴈􏳃􏱽􏰊...􏰓􏰆 */
    int ret = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect");
        exit(1);
    }

    short data[N];
    double f1[N]; //逆コサイン変換した値を格納
    short f2[N];
    memset(data, 0, sizeof(data));
    memset(f1, 0, sizeof(f1));
    memset(f2, 0, sizeof(f2));

    while (1)
    {
        int n = read(s, data, 2 * N);
        if (n == -1)
        {
            perror("read");
            exit(1);
        }
        if (n == 0) //EOF処理
        {
            break;
        }
        int n_ = N;
        data[n / 2 - 1] = 0;
        IDCT(f1, data, N);
        for (int i = 0; i < n_; i++)
        {
            f2[i] = (short)f1[i];
        }
        int m = write(1, f2, 2 * n_);
        if (m == 0)
        {
            break;
        }
        if (m == -1)
        {
            perror("write");
            exit(1);
        }
        memset(data, 0, sizeof(data));
        memset(f1, 0, sizeof(f1));
        memset(f2, 0, sizeof(f2));
    }
    close(s);
}

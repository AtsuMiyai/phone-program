/*

./serv [ポート番号]

*/
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
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
    //最大の周波数成分の大きさmax1の1/200以下の大きさを持つ音はなくす。
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (fabs(F[i]) <= max1 / 200.0)
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
    if (argc != 2)
    {
        printf("~ [ポート番号]");
        exit(1);
    }
    int ss = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(ss, (struct sockaddr *)&addr, sizeof(addr));
    listen(ss, 10);
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    int s = accept(ss, (struct sockaddr *)&client_addr, &len);
    close(ss);
    FILE *fp;
    FILE *rec;
    char *cmd1 = "rec  -t raw -b 16 -c 1 -e s -r 44100 -";
    rec = popen(cmd1, "r"); //クライアントからの接続を受け付けてから録音を始める。
    //ここまでがサーバーだけの役割
    if (rec == NULL)
    {
        perror("open");
        exit(1);
    }
    short store[N];
    double F[N]; //周波数領域の大きさを格納する。
    short F2[N];

    while (1)
    {

        int n = fread(store, 2, N, rec); //標準出力を読み取っている。
        if (n == -1)
        {
            perror("send");
            exit(1);
        }
        if (n == 0)
        {
            break;
        }

        int count = DCT(F, store, n); //0ではない最後のindex

        F[count + 1] = n;
        //もともと何個数字があったのかの情報を格納する。

        for (int i = 0; i < count + 2; i++)
        {
            F2[i] = (short)F[i];
        }
        int m = write(s, F2, 2 * (count + 2)); //周波数成分を送った。

        if (m == -1)
        {
            perror("write");
            exit(1);
        }
        memset(store, 0, sizeof(store));
        memset(F, 0, sizeof(F));
        memset(F2, 0, sizeof(F2));
    }

    pclose(rec);
    close(s);
}

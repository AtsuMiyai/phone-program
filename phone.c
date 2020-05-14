//./phone [ポート番号]  :サーバー側　　./phone [IPアドレス] [ポート番号] :クライアント側
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define N 1024

int main(int argc, char **argv)
{

    if ((argc != 2) && (argc != 3))
    {
        printf("コマンドラインが間違っています");
        exit(1);
    }
    /*ここからはサーバー側の役割*/
    if (argc == 2)
    {

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
        FILE *rec;
        //char buf[1024];
        char *cmd1 = "rec -t raw -b 16 -c 1 -e s -r 44100 -";
        rec = popen(cmd1, "r"); //クライアントからの接続を受け付けてから録音を始める。
        //ここまでがサーバーだけの役割
        if (rec == NULL)
        {
            perror("open");
            exit(1);
        }

        char *cmd2 = "play -t raw -b 16 -c 1 -e s -r 44100 - ";
        FILE *play;
        play = popen(cmd2, "w");
        if (play == NULL)
        {
            perror("open");
            exit(1);
        }
        //char store[1];
        // char data1[1];
        short store[N];
        short data1[N];
        while (1) //fpは録音を読んでる。
        {

            int n = fread(store, 2, N, rec); //標準出力を読み取っている。

            if (n == -1)
            {
                perror("send");
                exit(1);
            }
            if (n==0){
                break;
            }

            write(s, store, 2 * N);
            int o = read(s, data1, 2 * N); //クライアント側からの書き込みを標準入力に書き出す。
            if (o == -1)
            {

                perror("recv");
                exit(1);
            }
            fwrite(data1, 2, N, play);
        }

        pclose(rec);
        pclose(play);
        close(s);
    }
    if (argc == 3)
    {
        /*ここからはクライアントの役割*/
        int s = socket(PF_INET, SOCK_STREAM, 0);
        //FILE *fp;
        //gets()とか？何かしらの形で読み取っては、渡すようにすれば良い、これは簡単にできるはず。
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
        if (ret == 0)
        {
            perror("coct");
            //exit(1);
        }
        char *cmdline_r = "rec -t raw -b 16 -c 1 -e s -r 44100 - ";
        FILE *rec;
        rec = popen(cmdline_r, "r");
        if (rec == NULL)
        {
            perror("open_rec");
            exit(1);
        }
        char *cmdline_p = "play -t raw -b 16 -c 1 -e s -r 44100 - ";
        FILE *play;
        play = popen(cmdline_p, "w");
        if (play == NULL)
        {
            perror("open_play");
            exit(1);
        }
        int n;
        int i = 0;

        short data1[N];
        short store1[N];
        while (1)
        {

           
            int n = read(s, data1, 2 * N);
            if (n == -1)
            {
                perror("read");
                exit(1);
            }
            fwrite(data1, 2, N, play);
            //freadから読み取るべき
            int n_ = fread(store1, 2, N, rec);
            if (n_ == -1)
            {
                perror("fread");
                exit(1);
            }
            if (n==0){
                break;
            }

            write(s, store1, 2 * N);
            memset(data1, 0, sizeof(data1));
            memset(store1, 0, sizeof(store1)); 
        }
        pclose(rec);
        pclose(play);

        close(s);
    }
}

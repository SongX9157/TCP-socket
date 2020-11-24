#include<iostream>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>

using namespace std;

class Operation
{
public:
    int sendFile(const char *path, int packetsize, int sock)
    {
        FILE *fp;
        long long psize = packetsize*1024;
        char temp[psize+1];
        memset(temp,0,psize+1);
        fp = fopen(path,"rb");
        fseek(fp, 0, SEEK_END);
        //get the size of file
        long long siz = ftell(fp);
        //send the size of file
        send(sock, (char*)&siz, sizeof(siz),0);
        fseek(fp, 0, SEEK_SET);
        //send the packet size
        send(sock, (char*)&packetsize, sizeof(packetsize),0);
        //siz = siz / psize;
        long long index=0;
        int num;
        char key[50];
        cout << "Enter encryption key: ";
        cin>>key;
        int length=strlen(key);
        //cout << length << endl;

        int y=0;
        while (1)
        {
            num=fread(temp, 1, psize, fp);
            if(num==0)
                break;
            //encyption
            //cout<<temp<<endl;
            int len = strlen(temp);
            //cout << len <<"//"<<temp<<"//"<<endl;

            for (int i = 0; i < num; i++)
            {
                temp[i] = temp[i] ^ key[y % length];
                y++;
            }

            send(sock, temp, num, 0);
            index++;
            if(index<5)
            {
                cout <<"Sent encrypted Packet# "<<index<<":"<< num <<"-encrypted as ";
                if(num>=2){
                    printf("%0x",(unsigned char)temp[0]);
                    printf("%0x",(unsigned char)temp[1]);
                    printf("....");
                    printf("%0x",(unsigned char)temp[num-2]);
                    printf("%0x\n",(unsigned char)temp[num-1]);
                }

            }
            else if(index==5)
            {
                cout <<"."<<endl;
                cout <<"."<<endl;
                cout <<"."<<endl;
                cout <<"."<<endl;
            }

        }
        fclose(fp);
        cout << "Send Success!" << endl;

        //check md5
        char const *a="md5sum";
        char b[255];
        sprintf(b,"%s %s",a,path);
        cout<<"MD5:"<<endl;
        system(b);
        return 0;
    }

    int recieveFile(const char *path, int client)
    {
        FILE *fp;
        long long siz;
        long long siz2;
        int packetsize;
        //receive the size of file
        recv(client, (char*)&siz, sizeof(siz), 0);
        cout << "File size: " <<siz<<endl;
        //receive the size of packet
        recv(client, (char*)&packetsize, sizeof(packetsize), 0);
        cout << "Packet size: " <<packetsize<<endl;
        long long psize = packetsize*1024;
        char buff[psize+1];
        char buff2[psize+1];
        memset(buff,0,psize+1);
        memset(buff2,0,psize+1);
        siz2 = siz;
        //siz = siz / psize;
        fp = fopen(path, "wb+");
        long long int index = 0;
        int num;
        int num2;
        int sum=0;
        int x=0;
        int y=0;
        //int len=0;
        char key[50];
        cout << "Enter encryption key: ";
        cin>>key;
        int length=strlen(key);
        //cout << length << endl;

        while (sum<=siz2)
        {
            num = recv(client, buff, psize, 0);
            if (num == 0)
            {
                //cout << "num<0 "<<endl;
                break;
            }
            while(num < psize)
            {
                num2 = recv(client,buff+num,psize-num,0);
                num = num+num2;
                if(num2==0)
                    break;
                x++;
            }
            //encyption
            //len = strlen(buff);
            for (int i = 0; i < num; i++)
            {
                buff2[i] = buff[i];
                buff[i] = buff[i] ^ key[y % length];
                y++;
            }
            fwrite(buff, (int)num, 1, fp);

            sum=sum+num;

            index++;
            if(index<5)
            {
                cout <<"Rec encryption Packet# "<<(int)index<<":"<< num<<"-encrypted as ";
                if(num>=2){
                    printf("%0x",(unsigned char)buff2[0]);
                    printf("%0x",(unsigned char)buff2[1]);
                    printf("....");
                    printf("%0x",(unsigned char)buff2[num-2]);
                    printf("%0x\n",(unsigned char)buff2[num-1]);
                }

            }
            else if(index==5)
            {
                cout <<"."<<endl;
                cout <<"."<<endl;
                cout <<"."<<endl;
                cout <<"."<<endl;
            }

        }
        fclose(fp);
        cout << "Receive Success! " <<endl;
        //cout<<x<<" "<<siz2<<"&"<<sum<<endl;

        //check md5
        char const *a="md5sum";
        char b[255];

        sprintf(b,"%s %s",a,path);
        cout << "MD5:"<<endl;
        system(b);
        return 0;
    }


    void server()
    {
        short SERVERPORT;
        const int BACKLOG = 5;
        int sock, conn, recvBytes;
        sockaddr_in myAddr;
        sockaddr_in remoteAddr;

        //create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);

        if( sock == -1)
        {
            cerr<<"socket create fail!"<<endl;
            exit(1);
        }

        //cout<<"sock :"<<sock<<endl;

        cout<<"Connect to IP address:"<<endl;
        system("hostname -I| cut -d' ' -f1");
        cout<<"Port#:";
        cin>>SERVERPORT;

        //bind
        myAddr.sin_family = AF_INET;
        myAddr.sin_port = htons(SERVERPORT);
        myAddr.sin_addr.s_addr = INADDR_ANY;
        bzero( &(myAddr.sin_zero), 8);
        if(bind(sock, (sockaddr*)(&myAddr), sizeof(sockaddr)) ==-1 )
        {
            cerr<<"bind error!"<<endl;
            exit(1);
        }



        //listen
        if(listen(sock, BACKLOG) == -1)
        {
            cerr<<"listen error"<<endl;
            exit(1);
        }

        //connect
        unsigned int sin_size = sizeof(sockaddr_in);
        conn = accept(sock, (sockaddr*)(&remoteAddr), &sin_size);
        if(conn ==-1 )
        {
            cerr<<"connect error!"<<endl;
            exit(1);
        }

        char name[50];
        cerr<<"Save file to:";
        cin>>name;

        //send&receive
        while(1)
        {
            //receive
            recieveFile(name,conn);
            close(conn);
            exit(0);
        }

    }

    void client()
    {
        short SERVERPORT;
        //const char* SERVER_IP = "10.34.40.33";
        //const char* SERVER_IP = "134.209.126.236";
        char SERVER_IP[50];

        cerr<<"Connect to IP address:";
        cin>>SERVER_IP;
        cerr<<"Port#:";
        cin>>SERVERPORT;

        int sock, recvBytes;
        sockaddr_in serv_addr;

        //create socket
        if( (sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            cerr<<"socket create fail!"<<endl;
            exit(1);
        }
        bzero( &serv_addr, sizeof(serv_addr) );
        serv_addr.sin_family =  AF_INET;
        serv_addr.sin_port = htons(SERVERPORT);
        serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

        //connect
        if( connect(sock, (sockaddr*)&serv_addr, sizeof(sockaddr)) == -1)
        {
            cerr<<"connect error"<<endl;
            exit(1);
        }

        //cout<<"Connect to IP address: 134.209.126.236"<<endl;
        //cout<<"Port#"<<SERVERPORT<<endl;

        char name[50];
        cerr<<"File to send:";
        cin>>name;

        int pkt;
        cerr<<"Pkt size:";
        cin>>pkt;

        sendFile(name,pkt,sock);

        close(sock);
    }

};

int main(int argc, char** argv)
{
    Operation op;

    //cout << "Running..." << endl;
    //cout << argc << endl;
    int i;
    for(i=0; i<argc; i++)
    {
        //cout << argv[i] << endl;
        if(strcmp(argv[i],"server")==0)
        {
            cout<<"Running server..."<<endl;
            op.server();
            break;
        }
        else if(strcmp(argv[i],"client")==0)
        {
            cout<<"Running client..."<<endl;
            op.client();
            break;
        }
    }
    return 0;
}

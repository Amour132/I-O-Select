#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

using std::cout;
using std::endl;
using std::cerr;

void Usage(int argc)
{
  if(argc != 3)
  {
    cout << "Usage./diect [ip][port]" << endl;
    exit(1);
  }
}

int main(int argc,char* argv[])
{
  Usage(argc);

  int lst_fd;
  int cli_fd;
  socklen_t len;
  sockaddr_in lst_addr;
  sockaddr_in cli_addr;
  int fd_list[1024] = {0};
  fd_set readfds;

  lst_fd = socket(AF_INET,SOCK_STREAM,0);
  if(lst_fd < 0)
  {
    cerr << "socket error" << endl;
    exit(2);
  }

  lst_addr.sin_family = AF_INET;
  lst_addr.sin_port = htons(atoi(argv[2]));
  lst_addr.sin_addr.s_addr = inet_addr(argv[1]);
  len = sizeof(lst_addr);

  int ret = bind(lst_fd,(sockaddr*)&lst_addr,len);
  if(ret < 0)
  {
    cerr << "bind error" << endl;
    exit(2);
  }

  if(listen(lst_fd,5) < 0)
  {
    cerr << "listen error" << endl;
    exit(2);
  }
  for(int i = 0; i < 1024;i++)
  {
    fd_list[i] = -1;
  }
  fd_list[0] = lst_fd;

  int max_fd = lst_fd;
  timeval tv;
  for(;;)
  {
    FD_ZERO(&readfds);
    max_fd = lst_fd;
    for(int i = 0; i < 1024; i++)
    {
      if(fd_list[i] != -1)
        FD_SET(fd_list[i],&readfds);
      if(fd_list[i] > max_fd)
        max_fd = fd_list[i];
    }
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int ret = select(max_fd+1,&readfds,NULL,NULL,&tv);
    if(ret < 0)
    {
      cerr << "select error" << endl;
      continue;
    }
    else if(ret == 0)
    {
      cout << "time out" << endl;
      continue;
    }
    for(int i = 0; i < 1024; i++)
    {
      if(fd_list[i] < 0)
        continue;
      if(FD_ISSET(fd_list[i],&readfds))//如果就绪的是监听描述符，则要建立新的连接请求
      {
        if(fd_list[i] == lst_fd)
        {
          cli_fd = accept(lst_fd,(sockaddr*)&cli_addr,&len);
          if(cli_fd < 0)
          {
            cerr << "accept error" << endl;
            continue;
          }
          cout << "Get a new ..." << endl;
          for(int i = 0; i < 1024; i++)
          {
            if(fd_list[i] == -1)
              fd_list[i] = cli_fd;
          }
        }
        else//意味着客户端有数据
        {
          char buff[1024] = {0};
          int ret = recv(fd_list[i],buff,1024,0);
          if(ret <= 0)
          {
            close(fd_list[i]);
            fd_list[i] = -1;
          }
          cout << "client buff#" << buff << endl;
          send(fd_list[i],"what is your name",20,0);
        }
      }
    }
  }
  return 0;
}

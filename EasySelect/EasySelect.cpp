#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

int main()
{
  timeval tv;
  fd_set readfds;
  int max_fd = 0;
  for(;;)
  {
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(0,&readfds);
    int ret = select(max_fd+1,&readfds,NULL,NULL,&tv);
    if(ret < 0)
    {
      std::cerr << "select error" << std::endl;
      return -1;
    }
    else if(ret == 0)
    {
      std::cout << "time out" << std::endl;
    }
    char buff[1024] = {0};
    read(0,buff,1024);
    std::cout << buff << std::endl;
  }
  return 0;

}

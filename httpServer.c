#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

const char* HTTP_200_STRING = "OK";
const char* HTTP_404_STRING = "Not Found";
const char* HTTP_501_STRING = "Not Implemented";
const char* chtml = "Content-Type: text/html\r\n\r\n";
const char* ccss = "Content-Type: text/css\r\n\r\n";
const char* cjpg = "Content-Type: image/jpeg\r\n\r\n";
const char* cpng = "Content-Type: image/png\r\n\r\n";
const char* cgif = "Content-Type: image/gif\r\n\r\n";
const char* cplain = "Content-Type: text/plain\r\n\r\n";
const char* HTTP_404_CONTENT = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1>The requested resource could not be found but may be available again in the future.<div style=\"color: #eeeeee; font-size: 8pt;\">Actually, it probably won't ever be available unless this is showing up because of a bug in your program. :(</div></html>";
const char* HTTP_501_CONTENT = "<html><head><title>501 Not Implemented</title></head><body><h1>501 Not Implemented</h1>The server either does not recognise the request method, or it lacks the ability to fulfill the request.</body></html>";

struct sockaddr_in addr, clientAddr;
int socketfd, connectfd;

void * connectSocket(void* tid);

int main(int argc, char* argv[]){
  long portNumber;

  if(argc == 2) portNumber = strtol(argv[1],NULL,10);
  else return 1;
  memset(&addr, 0 ,sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(portNumber);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(&(addr.sin_zero), 0, 8);

  if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("Not able to socket");
  return 1;
  }
  if((bind(socketfd, (struct sockaddr*)&addr, sizeof(struct sockaddr))) == -1){
    perror("Not able to bind");
    return 1;
  }
  if((listen(socketfd, 10)) == -1){
    perror("Not able to listen");
    return 1;
  }

  int tid;
  pthread_t threads[128];
  int num =0;

  while(1){
    int sin_size = sizeof(struct sockaddr_in);
    if((connectfd = accept(socketfd, (struct sockaddr*)&clientAddr, &sin_size)) == -1){
      perror("Not able to accpet");
      continue;
    }
    if(num < 128)
    if((tid = pthread_create(&threads[num], NULL, connectSocket, &tid)) != 0){
      perror("Not able to create a thread");
      return 1;
    }
        num+=1;
    sleep(1);
  }

  for(int i = 0; i < num; ++i){
    if((tid = pthread_join(threads[i], NULL)) != 0){
      perror("Not able to join threads");
      return 1;
    }
  }
  return 0;
}

void * connectSocket(void* tid){
  int keepAlive = 0;
  do{

    int httpStatusCode;
char url[127], buf[BUFSIZ];
    if((recv(connectfd, buf, BUFSIZ, 0)) == -1)  perror("ERROR: recv");

    char* httpHeader= strtok(buf, "\r\n\r\n");
    char* headerToken = strtok(httpHeader, " \r\n");
    strcpy(url, "web");

    headerToken = strtok(NULL, " \r\n");
    if(headerToken !=NULL)
    if(headerToken[0] != '/') httpStatusCode = 501;

    strcat(url, headerToken);
    if(headerToken[strlen(headerToken)-1] == '/') strcat(url, "index.html");

    if(httpStatusCode != 501)
      while((headerToken = strtok(NULL, " \r\n")) != 0){

              if(!strcmp(headerToken, "Connection:")){
          headerToken = strtok(NULL, " \r\n");
          if(!strcasecmp(headerToken, "Keep-Alive")) keepAlive = 1;
          break;
        }
      }
    char content[BUFSIZ], contentSize[16];
    strcpy(content, "HTTP/1.1 ");
    int fd, fileSize;
    if(httpStatusCode != 501){
      if((fd = open(url, O_RDONLY)) == -1){
        httpStatusCode = 404;
        keepAlive = 0;
        strcat(content, "404 ");
        strcat(content, HTTP_404_STRING);
        strcat(content, "\r\n Connection: close\r\n Content-Length: ");
        sprintf(contentSize, "%ld", strlen(HTTP_404_CONTENT));
        strcat(contentSize, contentSize);
        strcat(contentSize, "\r\n");
        strcat(content, chtml);
        strcat(content, HTTP_404_CONTENT);
      }
      else{
 
        if(sizeof(content) > 0){
        char* contentType = strtok(url, ".");
        contentType = strtok(NULL, ".");
        httpStatusCode = 200;
        struct stat fileStat;
        fstat(fd, &fileStat);
        fileSize = fileStat.st_size;
        sprintf(contentSize, "%d", fileSize);
        strcat(content, "200 ");
        strcat(content, HTTP_200_STRING);
        strcat(content, "\r\n");
        if(keepAlive) strcat(content, "Connection: Keep-Alive\r\n");
        else strcat(content, "Connection: close\r\n");
        strcat(content, "Content-Length: ");
        strcat(content, contentSize);
        strcat(content, "\r\n");
        if(!strcmp(contentType, "html")) strcat(content, chtml);
        else if(!strcmp(contentType, "css")) strcat(content, ccss);
        else if(!strcmp(contentType, "jpg")) strcat(content, cjpg);
        else if(!strcmp(contentType, "png")) strcat(content, cpng);
        else if(!strcmp(contentType, "gif")) strcat(content, cgif);
        else strcat(content, cplain);
        }
      }
    }
        else{
         keepAlive = 0;
         strcat(content, "501 ");
         strcat(content, HTTP_501_STRING);
         strcat(content, "\r\n");
         strcat(content, "Connection: close\r\n");
         strcat(content, "Content-Length: ");
         sprintf(contentSize, "%ld", strlen(HTTP_501_CONTENT));
         strcat(content, contentSize);
         strcat(content, "\r\n");
         strcat(content, chtml);
         strcat(content, HTTP_501_CONTENT);
      }

 send(connectfd, content, strlen(content), MSG_NOSIGNAL);
    int chRead;
    char chBuff[2048];
    int ch;
    if(httpStatusCode == 200){
      while(chRead < fileSize){
        if((ch = read(fd, &chBuff, sizeof(chBuff))) == -1){
          perror("Not able to read");
          close(connectfd);
        }
        send(connectfd, chBuff, ch, MSG_NOSIGNAL);
        chRead += ch;
      }
      close(fd);
    }

  }while(keepAlive);
         close(connectfd);
        }


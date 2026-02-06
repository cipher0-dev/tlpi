#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  int optval;

  optval = 1;
  setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &optval, sizeof(optval));

  // write(sockfd, ...);
  // sendfile(sockfd, ...);

  optval = 0; // ERRATA: semicolon is needed here
  setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &optval, sizeof(optval));

  return EXIT_SUCCESS;
}

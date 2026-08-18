#include "protocol.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  struct sockaddr_in si_server = {0};
  char buf[RESP_MAX] = {0};

  int recv_len, si_server_len = sizeof(si_server);

  int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s == -1) {
    perror("Error while opening socket");
    return 1;
  }

  si_server.sin_family = AF_INET;
  si_server.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  si_server.sin_port = htons(BCAST_PORT);

  if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &(int){1}, sizeof(int)) < 0)
    perror("setsockopt(SO_REUSEADDR, SO_BROADCAST) failed");

  if (sendto(s, REQ_MAGIC, REQ_MAGIC_LEN, 0,
             (const struct sockaddr *)&si_server, si_server_len) == -1) {
    perror("Error while sending data");
    return 1;
  }

  while (1) {
    if ((recv_len = recvfrom(s, buf, RESP_MAX, 0, (struct sockaddr *)&si_server,
                             (socklen_t *)&si_server_len)) == -1) {
      perror("Error while receiving data");
      return 1;
    }

    printf("ip=%s %s\n", inet_ntoa(si_server.sin_addr), buf);
  }

  close(s);

  return 0;
}

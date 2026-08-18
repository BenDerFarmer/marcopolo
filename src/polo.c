#include "protocol.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <unistd.h>

int build_info_string(char *buf, size_t buflen) {
  struct utsname u;
  char host[256];

  if (gethostname(host, sizeof(host)) != 0)
    return -1;

  if (uname(&u) != 0)
    return -1;

  return snprintf(buf, buflen, "host=%s sys=%s release=%s arch=%s", host,
                  u.sysname, u.release, u.machine);
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  struct sockaddr_in si = {0};
  struct sockaddr_in si_client = {0};

  int recv_len, si_client_len = sizeof(si_client);
  char buf[RESP_MAX] = {0};

  char msg[512];
  if (build_info_string(msg, sizeof(msg)) < 0) {
    perror("Error while build info_string");
    return 1;
  }

  int msg_len = strlen(msg);

  int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s == -1) {
    perror("Error while opening socket");
    return 1;
  }

  si.sin_family = AF_INET;
  si.sin_port = htons(BCAST_PORT);
  si.sin_addr.s_addr = htonl(INADDR_ANY);

  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");

  if (bind(s, (const struct sockaddr *)&si, sizeof(si)) == -1) {
    perror("Error while binding socket");
    return 1;
  }

  while (1) {
    if ((recv_len = recvfrom(s, buf, RESP_MAX, 0, (struct sockaddr *)&si_client,
                             (socklen_t *)&si_client_len)) == -1) {
      perror("Error while receiving data");
      return 1;
    }

    if (recv_len != REQ_MAGIC_LEN)
      continue;

    if (strncmp(REQ_MAGIC, buf, recv_len) != 0)
      continue;

    printf("Received packet from %s:%d\n", inet_ntoa(si_client.sin_addr),
           ntohs(si_client.sin_port));

    if (sendto(s, msg, msg_len, 0, (const struct sockaddr *)&si_client,
               si_client_len) == -1) {
      perror("Error while sending data");
      return 1;
    }
  }

  close(s);

  return 0;
}

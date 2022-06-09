#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <sys/time.h>
#include <time.h>

#define BUFSIZE 512
#define UDP_PORT 2222

void usage(const char *cmd)
{
  fprintf(stderr, "usage: %s [-p udp-port]\n"
          "Listens and display log packets sent by network-logging Bob++ clients\n"
          "\nOptions:\n"
          "  -h, --help : display this help message\n"
          "  -p, --port VALUE : choose UDP listening port (default=%d)\n", cmd,
          UDP_PORT);
  exit(1);
}

int main(int argc, char **argv)
{
  struct timeval tv;
  struct sockaddr_in adr;
  char in_buffer[BUFSIZE];
  char out_buffer[BUFSIZE];
  int udp_in;
  int udp_port = UDP_PORT;
  double t0, t;
  ssize_t s;

  argv++;
  while (*argv)
  {
    if ((!strcmp(*argv, "-h")) || (!strcmp(*argv, "--help")))
      usage(argv[0]);
    else if ((!strcmp(*argv, "-p")) || (!strcmp(*argv, "--port")))
    {
      if (argv[1])
        udp_port = atoi(*++argv);
      else
        usage(argv[0]);
    }
    argv++;
  }

  udp_in = socket(PF_INET, SOCK_DGRAM, 0);
  if (udp_in == -1)
    perror("socket(udp)"), exit(1);
  memset(&adr, 0, sizeof(adr));
  adr.sin_port = htons(udp_port);
  adr.sin_addr.s_addr = INADDR_ANY;
  adr.sin_family = AF_INET;
  if (bind(udp_in, (struct sockaddr *) &adr, sizeof(adr)) == -1)
    perror("bind(udp)"), exit(1);

  s = recv(udp_in, in_buffer, BUFSIZE, 0);
  gettimeofday(&tv, NULL);
  t0 = tv.tv_sec + 1e-6 * tv.tv_usec;
  t = t0;
  while (1)
  {
    in_buffer[s < BUFSIZE ? s : BUFSIZE - 1] = '\0';
    sprintf(out_buffer, "%.06f %s", t - t0, in_buffer);
    write(1, out_buffer, strlen(out_buffer));
    s = recv(udp_in, in_buffer, BUFSIZE, 0);
    gettimeofday(&tv, NULL);
    t = tv.tv_sec + 1e-6 * tv.tv_usec;
  }
}

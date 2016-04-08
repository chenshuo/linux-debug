/*
 * sudo tunctl -t tap1
 * sudo ifconfig tap1 up
 * sudo gdb --args ./vmlinux mem=512M ubda=../Debian-Wheezy-AMD64-root_fs eth0=tuntap,tap1,fe:f0:00:00:00:01,192.168.1.254 con0=fd:0,fd:1
 * sudo ./a.out tap0 x
 * sudo brctl addbr br0
 * sudo brctl addif br0 tap0 tap1
 * sudo ifconfig br0 up
 */
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <libnet.h>
#include <arpa/inet.h>  // inet_ntop
#include <linux/if_ether.h>
#include <linux/if_tun.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>

#include <string>

using std::string;

class FakeTCP
{
};

extern const uint8_t ether_dest[];
extern const uint8_t ether_source[];
extern const uint32_t ip_dest;
extern const uint32_t ip_source;

int sethostaddr(const char* dev)
{
  struct ifreq ifr;
  bzero(&ifr, sizeof(ifr));
  strcpy(ifr.ifr_name, dev);

  struct sockaddr_in addr;
  bzero(&addr, sizeof addr);
  addr.sin_family = AF_INET;
  // inet_pton(AF_INET, "192.168.0.1", &addr.sin_addr);
  addr.sin_addr.s_addr = ip_dest;

  struct sockaddr hw_addr = {
    .sa_family = ARPHRD_ETHER
  };
  bcopy(ether_dest, hw_addr.sa_data, IFHWADDRLEN);

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
    return sockfd;
  int err = 0;
  // ifconfig tap0 192.168.0.1
  bcopy(&addr, &ifr.ifr_addr, sizeof addr);
  if ((err = ioctl(sockfd, SIOCSIFADDR, (void *) &ifr)) < 0)
  {
    perror("ioctl SIOCSIFADDR");
    goto done;
  }

  // ifconfig tap0 hw ether f6:00:00:00:00:01
  bcopy(&hw_addr, &ifr.ifr_hwaddr, sizeof hw_addr);
  if ((err = ioctl(sockfd, SIOCSIFHWADDR, (void *) &ifr)) < 0)
  {
    perror("ioctl SIOCGIFFLAGS");
    goto done;
  }
  // ifup tap0
  if ((err = ioctl(sockfd, SIOCGIFFLAGS, (void *) &ifr)) < 0)
  {
    perror("ioctl SIOCGIFFLAGS");
    goto done;
  }
  ifr.ifr_flags |= IFF_UP;
  if ((err = ioctl(sockfd, SIOCSIFFLAGS, (void *) &ifr)) < 0)
  {
    perror("ioctl SIOCSIFFLAGS");
    goto done;
  }
  // ifconfig tap0 192.168.0.1/24
  inet_pton(AF_INET, "255.255.255.0", &addr.sin_addr);
  bcopy(&addr, &ifr.ifr_netmask, sizeof addr);
  if ((err = ioctl(sockfd, SIOCSIFNETMASK, (void *) &ifr)) < 0)
  {
    perror("ioctl SIOCSIFNETMASK");
    goto done;
  }
done:
  close(sockfd);
  return err;
}

int tap_alloc(char dev[IFNAMSIZ])
{
  struct ifreq ifr;
  int fd, err;

  if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
  {
    perror("open");
    return -1;
  }

  bzero(&ifr, sizeof(ifr));
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

  if (*dev)
  {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0)
  {
    perror("ioctl TUNSETIFF");
    goto error;
  }
  strcpy(dev, ifr.ifr_name);

  if ((err = ioctl(fd, TUNSETPERSIST, 1)) < 0)
  {
    perror("enabling TUNSETPERSIST");
    goto error;
  }
  /*
  if ((err = sethostaddr(dev)) < 0)
  {
    goto error;
  }
  */

  return fd;
error:
  close(fd);
  return err;
}

void dump(const uint8_t* buf, int len)
{
  int i;
  for (i = 0; i < len; ++i)
  {
    printf("%02x ", buf[i]);
    if (i % 16 == 7)
      printf("  ");
    if (i % 16 == 15)
      printf("\n");
  }
  puts("");
}

struct pcap_header {
        uint32_t magic_number;   /* magic number */
        uint16_t version_major;  /* major version number */
        uint16_t version_minor;  /* minor version number */
        int32_t  thiszone;       /* GMT to local correction */
        uint32_t sigfigs;        /* accuracy of timestamps */
        uint32_t snaplen;        /* max length of captured packets, in octets */
        uint32_t network;        /* data link type */
};

struct pcap_record_header {
        uint32_t ts_sec;         /* timestamp seconds */
        uint32_t ts_usec;        /* timestamp microseconds */
        uint32_t incl_len;       /* number of octets of packet saved in file */
        uint32_t orig_len;       /* actual length of packet */
};

int g_count = 0;
void save(const uint8_t* buf, int len)
{
  char name[32];
  snprintf(name, sizeof name, "tap%d.pcap", g_count++);
  FILE* fp = fopen(name, "w");
  pcap_header header = {
    .magic_number = 0xa1b2c3d4,
    .version_major = 2,
    .version_minor = 4,
    .thiszone = 0,
    .sigfigs = 0,
    .snaplen = 65536,
    .network = 1
  };
  pcap_record_header record = {
    .ts_sec = time(NULL),
    .ts_usec = 0,
    .incl_len = len,
    .orig_len = len
  };
  int nw = fwrite(&header, 1, sizeof header, fp);
  assert(nw == sizeof header);
  nw = fwrite(&record, 1, sizeof record, fp);
  assert(nw == sizeof record);
  nw = fwrite(buf, 1, len, fp);
  assert(nw == len);
  fclose(fp);
}

string build_syn(bool ether);

void send_arp(int fd, const void* msg)
{
  char errbuf[LIBNET_ERRBUF_SIZE] = "";
  libnet_t* net = libnet_init(LIBNET_NONE, NULL, errbuf);
  // FIXME
  net->link_type = DLT_EN10MB;
  libnet_diag_dump_context(net);
  if (libnet_autobuild_arp(
      ARPOP_REPLY, // op
      ether_source,
      reinterpret_cast<const uint8_t*>(&ip_source),
      ether_dest,
      const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&ip_dest)),
      net) != 1)
  {
    printf("libnet: %s\n", net->err_buf);
  }
  libnet_build_ethernet(
      ether_dest,
      ether_source,
      ETHERTYPE_ARP,
      NULL, 0,
      net, 0);

  uint8_t* buf = NULL;
  uint32_t size = 0;
  if (libnet_pblock_coalesce(net, &buf, &size) == 1)
  {
    int nw = write(fd, buf, size);
    printf("wrote %d\n", nw);
  }
  dump(buf, size);
  // save(buf, size);
  free(buf);
  libnet_destroy(net);
}

void send_ack(int fd, const void* input)
{
  const struct iphdr* iphdr = static_cast<const struct iphdr*>(input);
  const int iphdr_len = iphdr->ihl*4;
  printf("iphdr_len %d\n", iphdr_len);
  const struct tcphdr* tcphdr = static_cast<const struct tcphdr*>(input + iphdr_len);

  char errbuf[LIBNET_ERRBUF_SIZE];
  libnet_t* net = libnet_init(LIBNET_NONE, NULL, errbuf);
  if (!net)
  {
    printf("libnet: %s\n", errbuf);
    return;
  }
  // printf("seq %u\n", ntohl
  libnet_ptag_t tcp = libnet_build_tcp(
      1025,  // source port
      2222,    // dest port
      1234567 + 1,  // seq
      ntohl(tcphdr->seq)+1,        // ack
      TH_ACK,   // flags
      65535,    // window
      0,        // checksum
      0,        // urg
      LIBNET_TCP_H, // length
      NULL, 0,      // payload
      net, 0);      // ptag, build a new one
  libnet_ptag_t ip = libnet_build_ipv4(
      LIBNET_TCP_H + LIBNET_IPV4_H,
      0, // tos
      2, // id
      0, // frag (off)
      64, // ttl
      IPPROTO_TCP,
      0, // checksum
      ip_source,
      ip_dest,
      NULL, 0, // payload
      net, 0);  // ptag,
  libnet_ptag_t ether = libnet_build_ethernet(
      ether_dest,
      ether_source,
      ETHERTYPE_IP,
      NULL, 0,
      net, 0);
  // printf("%d\n", ether);
  uint8_t* buf = NULL;
  uint32_t size = 0;
  if (libnet_pblock_coalesce(net, &buf, &size) == 1)
  {
    int nw = write(fd, buf, size);
    printf("wrote %d\n", nw);
  }
  dump(buf, size);
  save(buf, size);
  free(buf);
  libnet_destroy(net);
}

int main(int argc, char* argv[])
{
  char dev[IFNAMSIZ] = "";
  if (argc > 1)
    strcpy(dev, argv[1]);

  string message = build_syn(true);
  printf("%zd\n", message.size());
  int fd = tap_alloc(dev);

  if (fd >= 0)
  {
    printf("%s\n", dev);

    if (argc > 2)
    {
      getchar();
      int nw = write(fd, message.data(), message.size());
      printf("wrote %d\n", nw);
    }

    uint8_t frame[2048];
    while (true)
    {
      int nr = read(fd, frame, sizeof frame);
      const struct ether_header* eh = static_cast<const ether_header*>(static_cast<void*>(frame));
      uint16_t ether_type = ntohs(eh->ether_type);
      printf("recv %d type %04x\n", nr, ether_type);
      if (ether_type != 0x86dd)
        dump(frame, nr);
      if (ether_type == ETHERTYPE_ARP)
      {
        send_arp(fd, frame + ETH_HLEN);
      }
      else if (ether_type == ETHERTYPE_IP)
      {
        send_ack(fd, frame + ETH_HLEN);
      }
    }
  }
}

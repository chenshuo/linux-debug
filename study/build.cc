#include <libnet.h>
#include <string>

using std::string;

extern const uint8_t ether_dest[] = "\xfe\xf0\0\0\0\1";
extern const uint8_t ether_source[] = "\xf6\0\0\0\0\2";
extern const uint32_t ip_dest = htonl(0xc0a80064);  // dst 192.168.0.100
extern const uint32_t ip_source = htonl(0xc0a80003);  // src 192.168.0.3

string build_syn(bool ether)
{
  char errbuf[LIBNET_ERRBUF_SIZE];
  string result;
  libnet_t* net = libnet_init(LIBNET_NONE, NULL, errbuf);
  if (!net)
  {
    printf("libnet: %s\n", errbuf);
    return result;
  }
  libnet_build_tcp(
      1025,  // source port
      2222,    // dest port
      1234567,  // seq
      0,        // ack
      TH_SYN,   // flags
      65535,    // window
      0,        // checksum
      0,        // urg
      LIBNET_TCP_H, // length
      NULL, 0,      // payload
      net, 0);      // ptag, build a new one
  libnet_build_ipv4(
      LIBNET_TCP_H + LIBNET_IPV4_H,
      0, // tos
      1, // id
      0, // frag (off)
      64, // ttl
      IPPROTO_TCP,
      0, // checksum
      ip_source,
      ip_dest,
      NULL, 0, // payload
      net, 0);  // ptag,
  if (ether)
  {
    libnet_build_ethernet(
        ether_dest,
        ether_source,
        ETHERTYPE_IP,
        NULL, 0,
        net, 0);
  }
  uint8_t* buf = NULL;
  uint32_t size = 0;
  if (libnet_pblock_coalesce(net, &buf, &size) == 1)
  {
    result.assign(reinterpret_cast<const char*>(buf), size);
  }
  free(buf);
  libnet_destroy(net);
  return result;
}

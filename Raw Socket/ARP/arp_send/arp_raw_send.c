#include <sys/socket.h>        // needed for socket()
#include <linux/if_packet.h>   // struct sockaddr_ll (see man 7 packet)
#include <linux/if_ether.h>    // ETH_P_ARP = 0x0806
#include <linux/if_arp.h>
#include <netinet/in.h>        // IPPROTO_RAW, INET_ADDRSTRLEN
#include <netinet/ip.h>        // IP_MAXPACKET (which is 65535)
//#include <linux/ip.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>

// Define a struct for ARP header
typedef struct _arp_hdr arp_hdr;
struct _arp_hdr {
  uint16_t htype;
  uint16_t ptype;
  uint8_t hlen;
  uint8_t plen;
  uint16_t opcode;
  uint8_t sender_mac[6];
  uint8_t sender_ip[4];
  uint8_t target_mac[6];
  uint32_t target_ip;
};

main()
{
int sock_raw;
struct ifreq ifreq_i;
struct ifreq ifreq_c;
struct ifreq ifreq_ip;
char sendbuff[65535];
int total_len = 0;
int send_len;
struct ethhdr *eth = (struct ethhdr *)(sendbuff);
uint8_t *parphdr = (uint8_t *)(sendbuff + sizeof(struct ethhdr));
struct sockaddr_ll sadr_ll;
arp_hdr arphdr = {0};

sock_raw=socket(AF_PACKET,SOCK_RAW,IPPROTO_RAW);
if(sock_raw == -1)
printf("error in socket");

memset(&ifreq_i,0,sizeof(ifreq_i));
strncpy(ifreq_i.ifr_name,"eth0",IFNAMSIZ-1); //giving name of Interface

if((ioctl(sock_raw,SIOCGIFINDEX,&ifreq_i))<0)
printf("error in index ioctl reading");//getting Index Name
 
printf("index=%d\n",ifreq_i.ifr_ifindex);

memset(&ifreq_c,0,sizeof(ifreq_c));
strncpy(ifreq_c.ifr_name,"eth0",IFNAMSIZ-1);//giving name of Interface

if((ioctl(sock_raw,SIOCGIFHWADDR,&ifreq_c))<0) //getting MAC Address
printf("error in SIOCGIFHWADDR ioctl reading");

memset(&ifreq_ip,0,sizeof(ifreq_ip));
strncpy(ifreq_ip.ifr_name,"eth0",IFNAMSIZ-1);//giving name of Interface
if(ioctl(sock_raw,SIOCGIFADDR,&ifreq_ip)<0) //getting IP Address
{
printf("error in SIOCGIFADDR \n");
perror("SIOCGIFADDR");
}

//ETHERNET HEADER
eth->h_source[0] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[0]);
eth->h_source[1] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[1]);
eth->h_source[2] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[2]);
eth->h_source[3] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[3]);
eth->h_source[4] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[4]);
eth->h_source[5] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[5]);

/* filling destination mac. DESTMAC0 to DESTMAC5 are macro having octets of mac address. */
eth->h_dest[0] = 0x00;//DESTMAC0;
eth->h_dest[1] = 0xe0;//DESTMAC1;
eth->h_dest[2] = 0x4c;//DESTMAC2;
eth->h_dest[3] = 0x19;//DESTMAC3;
eth->h_dest[4] = 0x37;//DESTMAC4;
eth->h_dest[5] = 0x46;//DESTMAC5;
 
eth->h_proto = htons(ETH_P_ARP); //means next header will be ARP header

/* end of ethernet header */
total_len+=sizeof(struct ethhdr);

// Forming ARP header
arphdr.htype = htons (1);              // Hardware type (16 bits): 1 for ethernet
arphdr.ptype = htons (ETH_P_IP);       // Protocol type (16 bits): 2048 for IP
arphdr.hlen = 6;                       // 6 bytes for MAC address
arphdr.plen = 4;                       // 4 bytes for IP address
arphdr.opcode = htons (ARPOP_REQUEST); // OpCode
  
memcpy (&arphdr.sender_mac, &eth->h_source[0], 6);
memset (&arphdr.target_mac, 0, 6);  // MACC addr to be found

//arphdr.sender_ip = inet_addr(inet_ntoa((((struct sockaddr_in *)&(ifreq_ip.ifr_addr))->sin_addr)));
inet_pton (AF_INET, "192.168.17.2", &arphdr.sender_ip);
arphdr.target_ip = inet_addr("192.168.17.3"); // put destination IP address

memcpy(parphdr, &arphdr, sizeof(arphdr));

total_len+=sizeof(arphdr);

sadr_ll.sll_ifindex = ifreq_i.ifr_ifindex; // index of interface
sadr_ll.sll_halen = ETH_ALEN; // length of destination mac address
sadr_ll.sll_addr[0] = 0x00;//DESTMAC0;
sadr_ll.sll_addr[1] = 0xe0;//DESTMAC1;
sadr_ll.sll_addr[2] = 0x4c;//DESTMAC2;
sadr_ll.sll_addr[3] = 0x19;//DESTMAC3;
sadr_ll.sll_addr[4] = 0x37;//DESTMAC4;
sadr_ll.sll_addr[5] = 0x46;//DESTMAC5;

send_len = sendto(sock_raw,sendbuff,64,0,(const struct sockaddr*)&sadr_ll,sizeof(struct sockaddr_ll));
if(send_len<0)
{
printf("error in sending....sendlen=%d....errno=%d\n",send_len,errno);
return -1;
}

}

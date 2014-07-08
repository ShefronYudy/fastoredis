#include "common/net.h"

#include "common/logger.h"

#ifdef OS_WIN
#include <windows.h>
#include <iphlpapi.h>
#elif defined OS_POSIX
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <net/if.h>
#endif

#ifdef OS_WIN
namespace common
{
    namespace net
    {
        namespace details
        {
            std::string get_mac_address_host_impl(IPAddr destination, IPAddr source, std::string &out_mac_address)
            {
                std::string result;
                ULONG MacAddr[2]; /* for 6-byte hardware addresses */
                ULONG PhysAddrLen = 6; /* default to length of six bytes */

                DWORD dwRetVal = SendARP(destination, source, reinterpret_cast<PULONG>(&MacAddr), &PhysAddrLen);

                if (dwRetVal == NO_ERROR) {
                    BYTE *bPhysAddr = (BYTE *)&MacAddr;
                    for (int i = 0; i < PhysAddrLen; i++) {
                        char tmp[4] = {0};
                        if (i == (PhysAddrLen - 1))
                            sprintf(tmp, "%.2X", bPhysAddr[i]);
                        else
                            sprintf(tmp, "%.2X-", bPhysAddr[i]);

                        out_mac_address += tmp;
                    }
                }
                else if(dwRetVal == ERROR_BAD_NET_NAME){

                }
                else if(dwRetVal == ERROR_BUFFER_OVERFLOW){

                }
                else if(dwRetVal == ERROR_GEN_FAILURE){

                }
                else if(dwRetVal == ERROR_INVALID_PARAMETER){

                }
                else if(dwRetVal == ERROR_INVALID_USER_BUFFER){

                }
                else if(dwRetVal == ERROR_NOT_FOUND){

                }
                else if(dwRetVal == ERROR_NOT_SUPPORTED){

                }

                return true;
            }
        }

        bool get_mac_address_host(const std::string &host, std::string &out_mac_address)
        {
            hostent * record = gethostbyname(host.c_str());
            if(record == NULL)
            {
                return false;
            }
            in_addr * address = (in_addr * )record->h_addr;
            const char* ip_address = inet_ntoa(* address);
            IPAddr DestIp = inet_addr(ip_address);
            return details::get_mac_address_host_impl(DestIp, INADDR_ANY, out_mac_address);
        }
    }
}
#elif defined OS_POSIX
namespace common
{
    namespace net
    {
        bool getRemoteMacAddress(const unicode_string& host, unicode_string& out_mac_address)
        {
            // Socket to send ARP packet
            int list_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);//socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));

            if( list_sock == ERROR_RESULT_VALUE){
                DEBUG_MSG_PERROR("socket");
                return false;
            }

            struct sockaddr_in udp_sin;
            bzero (&udp_sin, sizeof(udp_sin));
            udp_sin.sin_family = AF_INET;
            udp_sin.sin_addr.s_addr = inet_addr(out_mac_address.c_str());
            udp_sin.sin_port = htons(5232);

            int i = sendto(list_sock, "TEST", 5, 0, (struct sockaddr *)&udp_sin, sizeof(udp_sin));
            if(i == ERROR_RESULT_VALUE){
                DEBUG_MSG_PERROR("sendto");
            }
            return true;
        }
    }
}
#endif

namespace common
{
    unicode_string convert2string(const net::hostAndPort& host)
    {
        unicode_char buff[512] = {0};
        unicode_sprintf(buff, UTEXT("%s:%u"), host.first, host.second);
        return buff;
    }

    template<>
    net::hostAndPort convertfromString(const unicode_string& host)
    {
        net::hostAndPort res;
        size_t del = host.find_first_of(':');
        if(del != std::string::npos){
            res.first = host.substr(0, del);
            res.second = convertfromString<uint16_type>(host.substr(del + 1));
        }

        return res;
    }
}

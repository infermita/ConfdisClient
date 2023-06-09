#include "ntpdate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

NtpDate::NtpDate()
{

}
bool NtpDate::SetDate(char *ip){

    bool execute = false;

    int sockfd, n; // Socket file descriptor and the n return result from writing/reading from the socket.
    int portno = 123; // NTP UDP port number.
    char* host_name = ip; // NTP server host-name.
    typedef struct
    {

        unsigned li   : 2;       // Only two bits. Leap indicator.
        unsigned vn   : 3;       // Only three bits. Version number of the protocol.
        unsigned mode : 3;       // Only three bits. Mode. Client will pick mode 3 for client.

        uint8_t stratum;         // Eight bits. Stratum level of the local clock.
        uint8_t poll;            // Eight bits. Maximum interval between successive messages.
        uint8_t precision;       // Eight bits. Precision of the local clock.

        uint32_t rootDelay;      // 32 bits. Total round trip delay time.
        uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
        uint32_t refId;          // 32 bits. Reference clock identifier.

        uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
        uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

        uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
        uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

        uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
        uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

        uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
        uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

    } ntp_packet;                 // Total: 384 bits or 48 bytes.
    ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    memset( &packet, 0, sizeof( ntp_packet ) );
    *( ( char * ) &packet + 0 ) = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.
    struct sockaddr_in serv_addr; // Server address data structure.
    struct hostent *server;	     // Server data structure.
    sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ); // Create a UDP socket.
    server = gethostbyname( host_name ); // Convert URL to IP.
    bzero( ( char* ) &serv_addr, sizeof( serv_addr ) );
    serv_addr.sin_family = AF_INET;
    bcopy( ( char* )server->h_addr, ( char* ) &serv_addr.sin_addr.s_addr, server->h_length );
    serv_addr.sin_port = htons( portno );

    //fcntl(sockfd, F_SETFL, O_NONBLOCK);

    if ( connect( sockfd, ( struct sockaddr * ) &serv_addr, sizeof( serv_addr) ) >=0 ){
        n = write( sockfd, ( char* ) &packet, sizeof( ntp_packet ) );

        if(n>0){

            n = read( sockfd, ( char* ) &packet, sizeof( ntp_packet ) );
            if(n>0){

                packet.txTm_s = ntohl( packet.txTm_s ); // Time-stamp seconds.
                packet.txTm_f = ntohl( packet.txTm_f ); // Time-stamp fraction of a second.

                time_t txTm = ( time_t ) ( packet.txTm_s - NTP_TIMESTAMP_DELTA );

                //stime(&txTm);
                execute = true;
            }
        }
    }
    return execute;

}

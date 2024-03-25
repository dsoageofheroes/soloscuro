#ifndef SOL_CONNECT_H
#define SOL_CONNECT_H

#include "dso.pb-c.h"

extern int sol_connect_to_server();
extern int sol_send_request(Soloscuro__Packet *sp);
extern Soloscuro__Packet* sol_get_packet_response(uint8_t *buf, size_t len);

#endif

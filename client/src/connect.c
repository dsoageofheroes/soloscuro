#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

#include "connect.h"

static SSL *ssl = NULL;

static int bind_addr(const char *hostname, int port) {
    // bind address to socket
    struct hostent *host;
    struct sockaddr_in addr;
    if ((host = gethostbyname(hostname)) == NULL) {
        return EXIT_FAILURE;
        //exitm(CONNECT_ERR); // get host by name
    }
    int sd = socket(PF_INET, SOCK_STREAM, 0); // create client descriptor
    bzero(&addr, sizeof(addr)); // memset address with 0
    addr.sin_family = AF_INET; // IPv4 address family
    addr.sin_port = htons(port); // convert to network short byte order
    addr.sin_addr.s_addr = *(long*)(host->h_addr); // set the IP of the socket; sin_addr is an union
    if (connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        //exitm(CONNECT_ERR); // connect to host
        return EXIT_FAILURE;
    }
    return sd;
}

static SSL_CTX* init_client_ctx(void) {
    // create server ssl context
    OpenSSL_add_all_algorithms(); // set cryptos
    SSL_load_error_strings(); // set error messages
    const SSL_METHOD *method = TLS_client_method(); // create client method
    SSL_CTX *ctx = SSL_CTX_new(method); // create client context
    if (ctx == NULL) {
        ERR_print_errors_fp(stderr); // print error
    }
    return ctx;
    //exitm(CONNECT_ERR);
}

int get_response(uint8_t *buf, int len) {
    /*In: ssl, buffer, buffer size
    Wait to data and copy to a buffer*/
    if (buf == NULL) return -1;
    int bytes = SSL_read(ssl, buf, len); // wait for data
    buf[bytes] = '\0'; // set the null terminator
    return bytes;
}

int send_request(uint8_t *data, size_t size) {
    SSL_write(ssl, data, size); // send the request
    return EXIT_SUCCESS;
}

extern int sol_connect_to_server() {
    SSL_CTX *ctx = init_client_ctx();
    SSL_library_init(); // init ssl lib
    int d = bind_addr("127.0.0.1", 6789);
    ssl = SSL_new(ctx); // hold data for the SSL cocnnection
    SSL_set_fd(ssl, d); // assigns a socket to a SSL structure
    if (SSL_connect(ssl) < 1) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

extern Soloscuro__Packet* sol_get_packet_response(uint8_t *buf, size_t len) {
    int bytes = get_response(buf, len); // wait for a response
    return soloscuro__packet__unpack(NULL, bytes, buf);
}

static int populate_buf(Soloscuro__Packet *mp, uint8_t *buf, size_t *len) {
    if (soloscuro__packet__get_packed_size(mp) > *len) { goto too_small; }

    *len = soloscuro__packet__pack(mp, buf);

    return EXIT_SUCCESS;
too_small:
    return EXIT_FAILURE;
}

extern int sol_send_request(Soloscuro__Packet *sp) {
    uint8_t buf[4096];
    size_t len = sizeof(buf);
    populate_buf(sp, buf, &len);
    send_request(buf, len);
    return EXIT_SUCCESS;
}

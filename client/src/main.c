#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connect.h"
#include "dso.pb-c.h"

#define TOKEN_FILE "token.dat"

uint8_t token[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static int write_token() {
    FILE *f = fopen(TOKEN_FILE, "wb+");

    if (!f) { goto no_file; }

    if (fwrite(token, 1, sizeof(token), f) != sizeof(token)) {
        goto write_failure;
    }

    fclose(f);

    return EXIT_SUCCESS;

write_failure:
    fclose(f);
no_file:
    return EXIT_FAILURE;
}

static int setup() {
    FILE *f = fopen(TOKEN_FILE, "rb");
    if (!f) {
        goto no_file;
    }

    if (fread(token, 1, sizeof(token), f) != sizeof(token)) {
        goto read_failure;
    }

    fclose(f);
    return EXIT_SUCCESS;

read_failure:
    fclose(f);
no_file:
    return EXIT_FAILURE;
}

static int create_create(char *username, char *password, Soloscuro__Packet *mp) {
    if (!mp) { return EXIT_FAILURE; }

    soloscuro__packet__init(mp);
    Soloscuro__Create create = SOLOSCURO__CREATE__INIT;

    create.username = username;
    create.password = password;

    mp->op_case = SOLOSCURO__PACKET__OP_CREATE_OP;
    mp->create_op = &create;

    return EXIT_SUCCESS;
}

static int create_auth(char *username, char *password, Soloscuro__Packet *mp) {
    if (!mp) { return EXIT_FAILURE; }

    soloscuro__packet__init(mp);
    Soloscuro__Auth auth = SOLOSCURO__AUTH__INIT;

    auth.username = username;
    auth.password = password;

    mp->op_case = SOLOSCURO__PACKET__OP_AUTH_OP;
    mp->auth_op = &auth;

    return EXIT_SUCCESS;
}

static int create_token(uint8_t tok[16], Soloscuro__Packet *mp) {
    if (!mp) { return EXIT_FAILURE; }

    soloscuro__packet__init(mp);
    Soloscuro__Token token = SOLOSCURO__TOKEN__INIT;

    token.token.len = 16;
    token.token.data = tok;

    mp->op_case = SOLOSCURO__PACKET__OP_TOKEN_OP;
    mp->token_op = &token;

    return EXIT_SUCCESS;
}

static int login() {
    Soloscuro__Packet *mp = NULL;
    Soloscuro__Packet sp;
    uint8_t buf[1024];

    printf("Logging in...\n");

    if (!setup()) {
        printf("Token found, authenticating...\n");
        create_token(token, &sp);
        sol_send_request(&sp);
        mp = sol_get_packet_response(buf, sizeof(buf));
        if (mp == NULL) {
            printf("ERROR: unable to unpack message!\n");
            goto failure;
        }

        switch(mp->op_case) {
            case SOLOSCURO__PACKET__OP_TOKEN_OP:
                printf("Token confirmed, authenticated.\n");
                printf("Username: '%s'\n", mp->token_op->username);
                goto success;
            case SOLOSCURO__PACKET__OP_ERROR_OP:
                fprintf(stderr, "ERROR: '%s'\n", mp->error_op->msg);
                goto try_auth;
            default:
                printf("Unknown packet received, trying auth\n");
                goto try_auth;
        }
    }

try_auth:
    if (mp) {
        soloscuro__packet__free_unpacked(mp, NULL);
        mp = NULL;
    }

    printf("Trying Authentication.\n");

    create_auth("test", "1234", &sp);
    sol_send_request(&sp);
    mp = sol_get_packet_response(buf, sizeof(buf));
    if (mp == NULL) {
        printf("ERROR: unable to unpack message!\n");
        goto failure;
    }
    switch(mp->op_case) {
        case SOLOSCURO__PACKET__OP_TOKEN_OP:
            printf("Auth confirmed, saving token.\n");
            memcpy(token, mp->token_op->token.data, sizeof(token));
            if (write_token()) {
                fprintf(stderr, "Warning: unable to write to token file.\n");
            }
            printf("Username: '%s'\n", mp->token_op->username);
            goto success;
        case SOLOSCURO__PACKET__OP_ERROR_OP:
            fprintf(stderr, "ERROR: '%s'\n", mp->error_op->msg);
            goto try_create;
        default:
            printf("Unknown packet received, trying create\n");
            goto try_create;
    }

try_create:
    printf("Trying Creation.\n");

    create_create("test", "1234", &sp);
    sol_send_request(&sp);
    mp = sol_get_packet_response(buf, sizeof(buf));
    if (mp == NULL) {
        printf("ERROR: unable to unpack message!\n");
        goto failure;
    }
    switch(mp->op_case) {
        case SOLOSCURO__PACKET__OP_TOKEN_OP:
            printf("Creation successful, saving token.\n");
            memcpy(token, mp->token_op->token.data, sizeof(token));
            if (write_token()) {
                fprintf(stderr, "Warning: unable to write to token file.\n");
            }
            printf("Username: '%s'\n", mp->token_op->username);
            goto success;
        case SOLOSCURO__PACKET__OP_ERROR_OP:
            fprintf(stderr, "ERROR: '%s'\n", mp->error_op->msg);
            goto failure;
        default:
            printf("Unknown packet received, trying create\n");
            goto failure;
    }

failure:
    if (mp) {
        soloscuro__packet__free_unpacked(mp, NULL);
        mp = NULL;
    }
    return EXIT_FAILURE;
success:
    if (mp) {
        soloscuro__packet__free_unpacked(mp, NULL);
        mp = NULL;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {

    if (sol_connect_to_server()) {
        fprintf(stderr, "Unable to connect to server, is it up?\n");
        return 1;
    }

    if (login()) {
        fprintf(stderr, "Unable to login. Exiting...\n");
        return 1;
    }

    /*
    switch(p->op_case) {
        case SOLOSCURO__PACKET__OP__NOT_SET:
            printf("NS\n");
            break;
        case SOLOSCURO__PACKET__OP_PING_OP:
            break;
        case SOLOSCURO__PACKET__OP_AUTH_OP:
            break;
        case SOLOSCURO__PACKET__OP_PLAYER_OP:
            break;
        case SOLOSCURO__PACKET__OP_STATS_OP:
            break;
        case SOLOSCURO__PACKET__OP_TOKEN_OP:
            memcpy(token, p->token_op->token.data, sizeof(token));
            if (write_token()) {
                fprintf(stderr, "Warning: unable to write to token file.\n");
            }
            break;
        case SOLOSCURO__PACKET__OP_ERROR_OP:
            fprintf(stderr, "ERROR: '%s'\n", p->error_op->msg);
            break;
        default:
            printf("Unknown!\n");
            break;
    }
    */
}

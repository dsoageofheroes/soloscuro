#include <stdio.h>
#include "dso.pb-c.h"

int main(int argc, char *argv[]) {
    uint8_t buf[1024];
    printf("HERE!\n");
    Main__Packet__Status status = MAIN__PACKET__STATUS__INIT;
    status.numplayers = 1234;
    printf("%lu\n", sizeof(status));
    Main__Packet mp = MAIN__PACKET__INIT;
    printf("%lu\n", sizeof(mp));
    printf("packed_size = %lu\n", main__packet__get_packed_size(&mp));

    // Set the packet type
    mp.op_case = MAIN__PACKET__OP_STATUS__OP;
    mp.status_op = &status;
    size_t packed_size = main__packet__pack(&mp, buf);
    printf("packed_size = %lu\n", packed_size);
    Main__Packet *mp2 = main__packet__unpack(NULL, packed_size, buf);
    printf("numplayers = %d\n", mp2->status_op->numplayers);
    printf("numplayers = %d\n", mp.status_op->numplayers);
    main__packet__free_unpacked(mp2, NULL);
}

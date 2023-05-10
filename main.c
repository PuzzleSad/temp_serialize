#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "temperature_serialize.h"

int main(){
        printf("Pagesize: %li\n", sysconf(_SC_PAGESIZE));
        printf("headersize: %lu\n", sizeof(fileheader_t) );
        temp_serialize_t ts;
        temp_serialize_fno_init( &ts, "temp_recordings", O_RDONLY );

        // test_temperature_serialize();
        

}

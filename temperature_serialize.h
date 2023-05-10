#ifndef TEMPERATURE_SERIALIZE_H
#define TEMPERATURE_SERIALIZE_H

#include <stdint.h>

typedef struct __attribute__((__packed__)) fileheader_t{


        uint32_t dataheader_offset;
}fileheader_t;

typedef struct __attribute__((__packed__)) data_header_t{

        uint32_t dataheader_offset;
}data_header_t;

typedef struct __attribute__((__packed__)) reading_t{

}reading_t;

#endif

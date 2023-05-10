#ifndef TEMPERATURE_SERIALIZE_H
#define TEMPERATURE_SERIALIZE_H

/* Might this cause an issue? We'll see */
#if _POSIX_C_SOURCE < 200112L
#define _POSIX_C_SOURCE 200112L
#endif

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define TEST_MODE
#define VERBOSE_WARNINGS

#ifndef TEST_MODE
#define NDEBUG
#endif

#define	O_READ	        4		
#define	O_WRITE	        2		
#define	O_CREATE	1 /* Create if file doesn't exist */

#define VERSION 1


/*
        Design rules
                All strings, even stored in the header, are null terminated
                Any data in strings not occupied by characters are null initialized
                dataheader_offset(s) == 0 are invalid
                Readings are packed, the first reading is found immediately at the end of a data_header

*/

static const char* format_name = "PuzzleSad's Temperature Record Format\n";

typedef struct __attribute__((__packed__)) fileheader_t{
        char     format_name[64];               //
        char     type_description[256];         //i.e. "DS18B20" | "Raspberry pi 400 builtin cpu temp sensor"
        uint32_t version;                       //Version of this format
        uint64_t UUID;                          //if the device has a UUID


        uint32_t dataheader_offset;
        /* Store any auxillary stuff between the header and the first data_header */
}fileheader_t;

typedef struct __attribute__((__packed__)) data_header_t{
        uint64_t time_base;             
        uint32_t dataheader_offset;     //next data_header
}data_header_t;

typedef struct __attribute__((__packed__)) reading_t{
        uint16_t time_offset;   //time_base + time_offset = time
        int32_t temp_mdegrees;
}reading_t;



typedef struct temp_serialize_t{
        char* path;
        fileheader_t* fileheader;


        int file_loaded;
        FILE *_fp;
        int _intfd;
}temp_serialize_t;


fileheader_t temp_fileheader_default_create();
int temp_serialize_fstream_init( temp_serialize_t *ts, const char* path, int open_mode );
int temp_serialize_fno_init( temp_serialize_t *ts, const char* path, int open_mode );

int temp_serialize_create( const char* new_name );

void ts_print_fileheader( fileheader_t* fh );

int test_temperature_serialize();


#endif

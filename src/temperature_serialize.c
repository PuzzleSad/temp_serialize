#include "../temperature_serialize.h"
#include "temperature_serialize_defs.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>


int test_temperature_serialize();

static inline void _struct_init(temp_serialize_t *ts);
static inline int _copy_path(temp_serialize_t *ts, const char* path);

static const char* _streamfd_parse_openmode( int open_mode );

static int _intfd_open(temp_serialize_t *ts, const char* path, int open_mode);
static int _streamfd_open(temp_serialize_t *ts, const char* path, int open_mode);
static int _check_header_valid( temp_serialize_t *ts );
static int _intfd_load( temp_serialize_t *ts ){
        if( ts->file_loaded == 1 ){
                fprintf(stderr, "Error, file reports having already been loaded\n");
                fprintf(stderr, "fileheader ptr: %p\n", (void*)ts->fileheader);
                return -1;
        }

        size_t size = 0;
        size += lseek( ts->_intfd, 0, SEEK_END );
        lseek( ts->_intfd, 0, SEEK_SET );

        ts->fileheader = aligned_alloc( sysconf(_SC_PAGESIZE), size );


        read( ts->_intfd, ts->fileheader, size );

        if( ts->fileheader != NULL ){
                ts->file_loaded = 1;
        }
        
        return 0;
}
static int _streamfd_load( temp_serialize_t *ts ){
        if( ts->file_loaded == 1 ){
                fprintf(stderr, "Error, file reports having already been loaded\n");
                fprintf(stderr, "fileheader ptr: %p\n", (void*)ts->fileheader);
                return -1;
        }

        return 0;
}       

int temp_serialize_fstream_init( temp_serialize_t *ts, const char* path, int open_mode ){
        int ret = 0;

        ret = _streamfd_open( ts, path, open_mode );
        if( ret != 0 ){ return ret; } 

        ret = _streamfd_load( ts );
        if( ret != 0 ){ return ret; }

        return ret;
}
int temp_serialize_fno_init( temp_serialize_t *ts, const char* path, int open_mode ){
        int ret = 0;

        ret = _intfd_open( ts, path, open_mode );
        if( ret != 0 ){ return ret; } 

        ret = _intfd_load( ts );
        if( ret != 0 ){ return ret; } 

        ret = _check_header_valid( ts );
        if( !ret ){ return ret; }


        return ret;
}
int temp_serialize_create( const char* new_name ){
        if( access( new_name, F_OK ) == 0 ){
                fprintf( stderr, "failure to create file, file exists\n%s\n", new_name );
                return EEXIST;
        }

        fileheader_t fh = temp_fileheader_default_create();

        FILE *fp = fopen( new_name, "w+");

        fwrite( &fh, sizeof(fileheader_t), 1, fp );

        fclose( fp );

        return 0;
}

fileheader_t temp_fileheader_default_create(){
        fileheader_t fh;
        memset( &fh, 0, sizeof(fileheader_t) );

        size_t format_strlen = strlen(format_name);
        memcpy( fh.format_name, format_name, format_strlen );

        fh.version = VERSION;

        return fh;
}       


// VERBOSE_WARNINGS
#ifdef VERBOSE_WARNINGS
        #define PATH_TEST(path, newpath)\
        do{\
                if( path != NULL ){\
                        fprintf(stderr, "Warning, temp_serialize_t re-init\n");\
                        fprintf(stderr, "First path: %s\nInput path: %s\n", path, newpath );\
                        return -1;\
                }\
        }while(0);
#else
        #define PATH_TEST(path, newpath)\
        do{\
                if( path != NULL ){\
                        return -1;\
                }\
        }while(0);
#endif /* VERBOSE_WARNINGS - PATH_TEST */

static inline int _copy_path(temp_serialize_t *ts, const char* path)
{
        PATH_TEST( ts->path, path );

        size_t len = strlen( path );
        ts->path = (char*)malloc( len + 1 );
        memcpy( ts->path, path, len );
        ts->path[len] = '\0';
        return 0;
}

static inline void _struct_init(temp_serialize_t *ts)
{
        memset( ts, 0, sizeof(temp_serialize_t) );
}

static const char* _streamfd_parse_openmode( int open_mode )
{
        switch(open_mode){
                case O_READ:
                return str_open_r;
                break;

                case O_WRITE:
                return str_open_rw;
                break;

                case O_CREATE:
                return str_open_create;
                break;

                default:
                printf("Invalid open_mode: %i\n", open_mode);
                return NULL;
        }
}

static int _intfd_open(temp_serialize_t *ts, const char* path, int open_mode)
{
        _struct_init( ts );
        _copy_path( ts, path );

        ts->_intfd = open( ts->path, open_mode );

        if( ts->_intfd < 0 ){
                fprintf( stderr, "Error opening file: %s\n", ts->path );
                perror("error: ");
                return errno;
        }

        return 0;
}
static int _streamfd_open(temp_serialize_t *ts, const char* path, int open_mode)
{
        _struct_init( ts );
        _copy_path( ts, path );

        ts->_fp = fopen( ts->path, _streamfd_parse_openmode( open_mode ) );

        //Test if file opened properly, print error
        if( ts->_fp == NULL ){
                fprintf( stderr, "Error opening file: %s\n", ts->path );
                perror("error: ");
                return errno;
        }

        return 0;
}
static int _check_header_valid( temp_serialize_t *ts )
{
        if( ts->file_loaded != 1 ){
                fprintf(stderr, "temp_serialize_t reports file not loaded. %i\n", ts->file_loaded );
                return 0;
        }

        if( strcmp( ts->fileheader->format_name, format_name ) != 0 ){
                fprintf(stderr, "file_header format name not valid\n");
                fprintf(stderr, "expected: %.64s\nrecieved: %.64s\n", format_name, ts->fileheader->format_name );
                return 0;
        }

        if( (ts->fileheader->version == 0) || (ts->fileheader->version < VERSION) ){
                fprintf(stderr, "file_header version not valid\n");
                fprintf(stderr, "expected: %"PRIu32"\nrecieved: %"PRIu32"\n", VERSION, ts->fileheader->version );
                return 0;
        }

        /* TODO other possible validity checks */


        return 1;
}
#ifdef TEST_MODE
const char* random_path = "path/path/path/path";

int _test_copy_path(){
        temp_serialize_t ts;
        _struct_init( &ts );

        assert( _copy_path( &ts, random_path ) == 0 );
        // re-copy path, this is supposed to fail
        assert( _copy_path( &ts, random_path ) != 0 );


        return 0;
}


int test_temperature_serialize(){
        printf("Testing temperature_serialize.c\n");
        _test_copy_path();
        return 0;
}
#else
int test_temperature_serialize(){ return 0; }

#endif

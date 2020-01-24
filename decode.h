#ifndef DECODE_H
#define DECODE_H



#include"common.h"
#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
#define MAX_SIZE(x)         x * 8



/* Perform the decoding */
Status do_decoding(EncodeInfo *encInfo);  					   /* DONE */

/* Get File pointers for i/p and o/p files */
Status dec_open_files(EncodeInfo *encInfo);                       			 /* DONE */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], EncodeInfo *encInfo);            /* DONE */

/* Extract Magic String */
Status decode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(EncodeInfo *encInfo);

/* Decode secret file size */
Status decode_secret_file_size(EncodeInfo *encInfo);

/* Decode secret file data*/
Status decode_secret_file_data(EncodeInfo *encInfo);

/* Decode secret file extension size */
Status decode_secret_file_extn_size(EncodeInfo *encInfo);

/* Close File pointers for i/p and o/p files */
Status dec_close_files(EncodeInfo *encInfo);                                             /* DONE */

#endif

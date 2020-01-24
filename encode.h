#ifndef ENCODE_H
#define ENCODE_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

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

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    char *new_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;
    int size_extn;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);               		    /* DONE */

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);            /* DONE */

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);  					   /* DONE */

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);                       			 /* DONE */

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);             /* DONE */

/* Get file size */
uint get_file_size(FILE *fptr);                              /* DONE */

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);   /* DONE */

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extension size */
Status encode_secret_file_extn_size(int extn_size, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest,EncodeInfo *encInfo);

/* Close File pointers for i/p and o/p files */
Status close_files(EncodeInfo *encInfo);                       			 /* DONE */


#endif

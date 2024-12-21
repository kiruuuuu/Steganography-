#ifndef DECODE_H
#define DECODE_H

#include "types.h"

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;  //Source file name
    FILE *fptr_src_image;      //source File pointer

    /* Secret File Info */
    char output_fname[20];     //output file name
    FILE *fptr_output;      //output file pointer
    int size_output_extn_file;  //extn_size
    long size_output_file;      //output file size 

} DecodeInfo;

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *deceInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *deceInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_d(DecodeInfo *deceInfo);

/* Store Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *deceInfo);

/* Encode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *deceInfo);

/* encode secret file extension size*/
Status decode_secret_file_extn_size(DecodeInfo *deceInfo);

/* Encode secret file size */
Status decode_secret_file_size(DecodeInfo *deceInfo);

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *deceInfo);

/* Encode a byte into LSB of image data array */
Status decode_byte_from_lsb(char *data, char *image_buffer);

/* encode size to lsb */
Status decode_size_from_lsb(int *size,char *image_buffer);

#endif
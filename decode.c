#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <unistd.h> 


Status open_files_d(DecodeInfo *deceInfo)
{
    deceInfo->fptr_src_image=fopen(deceInfo->src_image_fname,"r");
    // Do Error handling
	if (deceInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", deceInfo->src_image_fname);

		return e_failure;
	}
    return e_success;
}
Status read_and_validate_decode_args(char *argv[], DecodeInfo *deceInfo)
{
    if(argv[2] != NULL && strcmp((strstr(argv[2],".")),".bmp")==0)
    {
        deceInfo->src_image_fname=argv[2];
    }
    else
    {
        return e_failure;
    }
    if(argv[3] != NULL)
    {
       strcpy(deceInfo->output_fname,argv[3]);
    }
    else{
         strcpy(deceInfo->output_fname,"output");
    }
    return e_success;
}
Status decode_magic_string(char *magic_string, DecodeInfo *deceInfo)
{
    fseek(deceInfo->fptr_src_image,54,SEEK_SET);
    char buf[32];
    int size;
    fread(buf,32,1,deceInfo->fptr_src_image);
    decode_size_from_lsb(&size,buf);
    int n=strlen(MAGIC_STRING);
    if(size!=n)
    {
        return e_failure;
    }
    char str[size];
    char buffer[8];
    for (int i=0;i<size;i++)
    {
    fread(buffer,8,1,deceInfo->fptr_src_image);
    decode_byte_from_lsb(&str[i],buffer);
    }
    str[size]='\0';
   if(strcmp(str,"#*")==0)
   {
     printf("magic string is matched\n");
     return e_success;
   }
   else{
       printf("magic string is not matched");
       return e_failure;
   }
    //return e_success;
}

Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    char ch=0;
    for(int i=0;i<8;i++)
   {
    ch=(image_buffer[i] & 1)<<(7-i) | ch;
   }
   *data=ch;
   return e_success;
}
Status decode_size_from_lsb(int *data, char *image_buffer)
{
    int ch=0;
    for(int i=0;i<31;i++)
   {
    ch=(image_buffer[i] & 1)<<(31-i) | ch;
   }
   *data=ch;
   return e_success;
}
Status decode_secret_file_extn_size(DecodeInfo *deceInfo)
{
    int size1=0;
    char buffer[32];
    fread(buffer,32,1,deceInfo->fptr_src_image);
    decode_size_from_lsb(&size1,buffer);
    deceInfo->size_output_extn_file=size1;
    return e_success;
}
Status decode_secret_file_extn(DecodeInfo *deceInfo)
{
    char str[deceInfo->size_output_extn_file+1];
    char buffer[8];
    for (int i=0;i<(deceInfo->size_output_extn_file);i++)
    {
        fread(buffer,8,1,deceInfo->fptr_src_image);
        decode_byte_from_lsb(&str[i],buffer);
     
    }
    str[deceInfo->size_output_extn_file]='\0';
    char *str2=strcat(deceInfo->output_fname,str);
    deceInfo->fptr_output=fopen(str2,"w");
    return e_success;
}
Status decode_secret_file_size(DecodeInfo *deceInfo)
{
    int size2=0;
    char buf1[32];
    fread(buf1,32,1,deceInfo->fptr_src_image);
    decode_size_from_lsb(&size2,buf1);
    deceInfo->size_output_file=size2;
    return e_success;
}
/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *deceInfo)
{
    char str3[deceInfo->size_output_file];
    char buf2[8];
    for(int i=0;i<(deceInfo->size_output_file);i++)
    {
         fread(buf2,8,1,deceInfo->fptr_src_image);
         decode_byte_from_lsb(&str3[i],buf2);
    }
    str3[deceInfo->size_output_file]='\0';
    fwrite(str3,deceInfo->size_output_file,1,deceInfo->fptr_output);
    fclose(deceInfo->fptr_output);
    return e_success;
}
Status do_decoding(DecodeInfo *deceInfo)
{
     printf("INFO: Opening the requried files\n");
    if(open_files_d(deceInfo)==e_success)
    {
       printf("INFO: Opened\n");
       printf("\n=== DECODING PROCEDURE STARTED ===\n\n");
       printf("INFO: Decoding the magic string\n");
       if(decode_magic_string(MAGIC_STRING,deceInfo)==e_success)
       {
        printf("\nINFO: Done\n");
	    printf("INFO: Decoding the file extension size\n");
        if(decode_secret_file_extn_size(deceInfo)== e_success)
        {
           printf("INFO: Done\n");
           printf("INFO: Decoding the file extension\n");
            if(decode_secret_file_extn(deceInfo)==e_success)
            {
                printf("INFO: Done\n");
                 printf("INFO: Decoding the secret file size\n");
                if(decode_secret_file_size(deceInfo)==e_success)
                {
                    printf("INFO: Done\n");
                    printf("INFO: Decoding the file data\n");
                    if(decode_secret_file_data(deceInfo)== e_success)
                    {
                       printf("INFO: Done\n");
                        return e_success;
                    }
                    else
                    {
                        printf("ERROR: Decoding the secret file data is failure\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("ERROR: Decoding the secret file size failure\n");
                }
            }
            else
            {
                 printf("ERROR: Decoding the secret file extension failure\n");
                return e_failure;
            }

        }
        else
        {
            printf("ERROR: Decoding size of extension failure\n");
            return e_failure;
        }

       }
       else
       {
       printf("\nERROR: Magic string decoding failure, pass encoded .bmp file\n");
        return e_failure;
       }
    }
    else{
        printf("ERROR: Error in opening the required file\n");
        return e_failure;
    }
}
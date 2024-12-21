#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <unistd.h>

int main(int argc,char *argv[])
{
    if(argc==1)
    {
        printf(" Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
        printf("Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
        return 0;
    }
   if (argc <3) 
    {
        if (argc > 1 && strcmp(argv[1], "-e") == 0)
        {
            printf("Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
        }
        else
        {
            printf("Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
        }
        return 1; 
    }

        if(strstr(argv[2], ".") == NULL || strcmp(strrchr(argv[2], '.'), ".bmp") != 0)
        {
              printf(" Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
              printf(" Decoding: ./lsb_steg -d <.bmp_file>[output file]\n");
               return 0;
        }
        if(argc==5)
        {
        if(strstr(argv[4], ".") == NULL || strcmp(strrchr(argv[4], '.'), ".bmp") != 0)
        {
              printf(" Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
              printf(" Decoding: ./lsb_steg -d <.bmp_file>[output file]\n");
               return 0;
        }
        }
      if(check_operation_type(argv)== e_encode)
      {
       printf("INFO: Data encoding started\n");
	   sleep(1);
        EncodeInfo encInfo;
        if(read_and_validate_encode_args(argv,&encInfo) == e_success)
        {
            printf("INFO: read and validate is succussfull\n");
	        sleep(1);
            if(do_encoding(&encInfo)==e_success){
                printf("\n==== ENCODING DONE SUCCESSFULLY ====\n");
            }
            else{
                printf("ERROR: Encoding is failure\n");
            }
        }
        else{
            printf("Read and validates is failed\n");
            printf(" Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
            return 0;
        }

      }
      else if(check_operation_type(argv)== e_decode)
      {
        DecodeInfo deceInfo;
       printf("INFO: Data decoding started\n");
        if(read_and_validate_decode_args(argv,&deceInfo)== e_success)
        {
            printf("INFO: read and validate is succussfull\n");
            if(do_decoding(&deceInfo)==e_success)
            {
                printf("\n==== DECODING DONE SUCCESSFULLY ====\n");
            }
            else
            {
                printf("ERROR: Decoding is failure\n");
                return e_failure;
            }
        }
        else
        {
            printf("read and validate is failure\n");
            return e_failure;
        }
      }
      else
      {
          printf("ERROR: unsupported format\n");
      }

    return 0;
}

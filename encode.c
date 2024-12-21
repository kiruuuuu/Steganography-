#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <unistd.h> 

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Outpu Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);t: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
	// Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
	// Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}

	// Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}

	// Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	// Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}

	// No failure return e_success
	return e_success;
}
/* Check operation type */
OperationType check_operation_type(char *argv[])
{
	if(strcmp(argv[1],"-e")==0)
	{
		return e_encode;
	}
	else if(strcmp(argv[1],"-d")==0)
	{
		return e_decode;
	}
	else
	{
		printf( "ERROR: operation type failure\n" );
		return e_unsupported;
	}
}
/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
		if( argv[2] != NULL && strcmp ((strstr(argv[2],".")),".bmp" )==0)
		{
            encInfo->src_image_fname=argv[2];
		}
		else 
		{
		    return e_failure;
		}
	    if(argv[3] != NULL && (strchr(argv[3],'.'))!=NULL)
		{
			encInfo->secret_fname=argv[3];
			strcpy(encInfo->extn_secret_file, strchr(argv[3],'.'));
		}
		else
		{
				return e_failure;
		}
		if(argv[4] != NULL)
		{
			if(strcmp ((strstr(argv[4],".")),".bmp" )==0)
			{
				encInfo->stego_image_fname=argv[4];
			}
			else 
			{
				printf("ERROR: Please enter the .bmp file name for encoded output file name\n");
				return e_failure;
			}
		}
		else
		{
			encInfo->stego_image_fname="stgeno.bmp";
			printf("INFO: Encoded file Created as %s\n", encInfo->stego_image_fname);
		}
		return e_success;
}
//copying bmp header file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){
	
	char ptr[54]; 
	fseek(fptr_src_image,0,SEEK_SET);
	fread(ptr,54,1,fptr_src_image);   // read 54 bytes from source image
	fwrite(ptr,54,1,fptr_dest_image);  // write 54 bytes in the destination file
	return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
	//get the image size of bmp file
   encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);   //image_capacity valirable is storing for image size of bmp
    printf("INFO: Source Image capacity = %u bytes\n",encInfo->image_capacity);
   //get the size of secret.txt file
   encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);    //size_secret_file is storing for secret file size
   if(encInfo->image_capacity> (54 +(strlen(MAGIC_STRING)+4+strlen(encInfo->extn_secret_file)+4+encInfo->size_secret_file)*8))  //here i am checking bmp file capacity more than to size of secret file
   {
	return e_success;
   }
   else{
	return e_failure;
   }
}
// getting secret file size
uint get_file_size(FILE *fptr)
{
  fseek(fptr,0,SEEK_END);
  return ftell(fptr);
}

//encoding magic string
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
	    int size = strlen(magic_string);
	    char buffer1[32];
		fread(buffer1,32,1,encInfo->fptr_src_image);
		encode_size_to_lsb(size,buffer1);
		fwrite(buffer1,32,1,encInfo->fptr_stego_image);
        encode_data_to_image(magic_string, size, encInfo->fptr_src_image, encInfo->fptr_stego_image);
	    return e_success;
}

//encoding data to image
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char buffer[8];
	for(int i=0;i<size;i++)
	{
		fread(buffer,1,8,fptr_src_image);
		encode_byte_to_lsb(data[i],buffer);
		fwrite(buffer,1,8,fptr_stego_image);
	}
}
//encoding byte to lsb
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	 for(int i=0;i<8;i++)
    {
		image_buffer[i]=(image_buffer[i]&(~(1))|((unsigned)(data&(1<<(7-i)))>>(7-i)));
	}
}

//encoding secret file extension
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
      encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);
	  return e_success;
}
//encodeing secret file extension size
Status encode_secret_file_extn_size(int size,EncodeInfo *encInfo)
{
        char buffer1[32];
		fread(buffer1,32,1,encInfo->fptr_src_image);
		encode_size_to_lsb(size,buffer1);
		fwrite(buffer1,32,1,encInfo->fptr_stego_image);
		return e_success;
}
//Encoding size to lsb
Status encode_size_to_lsb(int size,char *image_buffer)
{
	for(int i=0;i<31;i++)
    {
		image_buffer[i]=(image_buffer[i]&(~(1))|((unsigned)(size&(1<<(31-i)))>>(31-i)));
	}
}

//encode secret file size
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
	   char buffer1[32];
	   //printf("--%d",file_size);
		fread(buffer1,32,1,encInfo->fptr_src_image);
		encode_size_to_lsb(file_size,buffer1);
		fwrite(buffer1,32,1,encInfo->fptr_stego_image);
   return e_success;
}
/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	rewind(encInfo->fptr_secret);
	char buffer[encInfo->size_secret_file+1];
	fread(buffer,encInfo->size_secret_file,1,encInfo->fptr_secret);
	encode_data_to_image(buffer, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);
	return e_success;
}
/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char ch;
	while(fread(&ch,1,1,fptr_src_image)!=0)
	{
		fwrite(&ch,1,1,fptr_stego_image);
	}
	return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
	printf("INFO: opening the requried files\n");
	if(open_files(encInfo) == e_success)
	{
		printf("INFO: Opened\n");
	    printf("INFO: Checking image capacity\n");
		if(check_capacity(encInfo)== e_success)
		{
			 printf("INFO: Check capacity Done\n");
	         printf("\n=== ENCODING PROCEDURE STARTED ===\n\n");
	         printf("INFO: copying the header file from bmp file\n");
			if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)== e_success)
			{
				printf("INFO: Done\n");
		        printf("INFO: Encoding the magic string\n");
				if(encode_magic_string(MAGIC_STRING,encInfo)== e_success)
				{
					printf("INFO: Done\n");
		            printf("INFO: Encoding the file extension size\n");
					if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)== e_success)
					{
						printf("INFO: Done\n");
			            
			            printf("INFO: Encoding the file extension\n");
			            
						if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)== e_success)
						{
							 printf("INFO: Done\n");
			                 
			                 printf("INFO: Encoding the secret file size\n");
			                 
							if(encode_secret_file_size(encInfo->size_secret_file,encInfo)== e_success)
							{
								printf("INFO: Done\n");
				                
				                printf("INFO: Encoding the file data\n");
				                
								if(encode_secret_file_data(encInfo)==e_success)
								{
									 printf("INFO: Done\n");
				                    
				                     printf("INFO: Copying the remaining image file data\n");
				                     
									if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)== e_success)
									{
										printf("INFO: Done\n");
										return e_success;
									}
									else
									{
										printf("ERROR: copying the remaining image data failure\n");
										return e_failure;
									}
								}
								else
								{
									 printf("ERROR: Encoding the secret file data is failure\n");
									return e_failure;
								}

							}
							else
							{
								printf("ERROR: Encoding the secret file size failure\n");
								return e_failure;
							}
						}
						else{
							printf("ERROR: Encoding the secret file extension failure\n");
							return e_failure;
						}
					}
					else
					{
						printf("ERROR: Encoding size of extensiom failure\n");
					}
					
				}
				else{
                   printf("ERROR: Magic string encoding failure\n");
					return e_failure;
				}
				
			}
			else
			{
				printf("ERROR: Copy of image header failure\n");
				return e_failure;
			}
		}
		else{
			printf("ERROR: Check capacity is failure\n");
			return e_failure;
		}
	}
	else
	{
		printf("ERROR: Error in opening the required file\n");
		return e_failure;
	}
	return e_success; 

}

	

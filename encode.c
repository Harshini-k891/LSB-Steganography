#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
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

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	if(strstr(argv[2],".bmp") != NULL)
	{
		encInfo->src_image_fname = argv[2];
		if(strstr(argv[3],".txt") != NULL)
		{
			encInfo->secret_fname=argv[3];
			if(argv[4] == NULL)
			{
				encInfo->stego_image_fname = "default.bmp";
			}
			else if(strstr(argv[4],".bmp") != NULL)
			{
				encInfo->stego_image_fname = argv[4];
			}
			else
			{
				return e_failure;
			}
			return e_success;
		}
	}
	return e_failure;
}

Status do_encoding(EncodeInfo *encInfo)
{
	if(open_files(encInfo) == e_success)
	{
		printf("File opened successfully\n");
		//cehck capacity
		if(check_capacity(encInfo) == e_success)
		{
			printf("Check capacity successfully\n");
		}
		else
		{
			return e_failure;
		}
	}
	else
	{
		return e_failure;
	
	}


	printf("Enter the magic string:");
	scanf("%[^\n]",encInfo->magic);

	if(check_capacity(encInfo) == e_success)
	{
		printf("capacity checked sucessfully\n");
	}
	else
	{
		printf("Source iamge capacity is not sufficient\n");
		return e_failure;
	}

	if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		printf("Header copied succesfully\n");
	}
	else
	{
		printf("failed to copy header\n");
		return e_failure;
	}

	if(encode_magic_string(encInfo->magic, encInfo) == e_success)
	{
		printf("Magic string encoded successfully\n");
	}
	else
	{
		printf("Failed to encode magic string\n");
	}

	if(encode_secret_file_extn_size(strlen(strchr(encInfo->secret_fname,'.')),encInfo) == e_success)
	{
		printf("Extension Size encoded successfully\n");
	}
	else
	{
		printf("Failed to encode secret extension\n");
		return e_failure;
	}
	if(encode_secret_file_extn(strchr(encInfo->secret_fname,'.'),encInfo) == e_success)
	{
		printf("Secret file extension encoded succesfully\n");
	}
	else
	{
		printf("Failed to encode secret file extension\n");
		return e_failure;
	}

	if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
	{
		printf("Secret file size encoded succesfully\n");
	}
	else
	{
		printf("Failed to encode secret file size\n");
	}

	if(encode_secret_file_data(encInfo) == e_success)
	{
		printf("scret data encoded succesfully\n");
	}
	else
	{
		printf("Failed to encode secret data\n");
	}

	if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		printf("Remaining image data copied successfully\n");
	}
	else
	{
		printf("Failed to copy remaining data\n");
		return e_failure;
	}
	return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

	
	printf("Image Size : %d\n", encInfo->image_capacity);
	printf("Sec file size: %ld\n", encInfo->size_secret_file);
	if(encInfo->image_capacity > (16+32+(strlen(encInfo->extn_secret_file) * 8) + 32+ (encInfo->size_secret_file * 8)))	
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

uint get_file_size(FILE *fptr)
{
	fseek(fptr,0,SEEK_END);
	long Size = ftell(fptr);
	rewind(fptr);

	return Size;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	char arr[54];
	fseek(fptr_src_image, 0 , SEEK_SET);
	fread(arr, 54, 1, fptr_src_image);
	fwrite(arr, 54, 1, fptr_dest_image);
	return e_success;
}
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
	if(encode_data_to_image(magic_string, strlen(magic_string), encInfo) == e_success)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
	char buffer[8];

	for(int i=0; i<size; i++)
	{
		if(fread(buffer, 1, 8, encInfo->fptr_src_image) != 8)
		{
			return e_failure;
		}

		 encode_byte_to_lsb(data[i], buffer);

		if( fwrite(buffer, 1, 8, encInfo->fptr_stego_image) != 8)
		{
			return e_failure;
		}
	}
	return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	unsigned char value = (unsigned char) data;
	for(int i=7; i>=0 ;i--)
	{
		image_buffer[7-i] = (image_buffer[7-i] & 0xFE) | ((value >> i) & 1);
	}
	return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
	if(encode_data_to_image(file_extn, strlen(file_extn), encInfo) == e_success)
	{
		return e_success;
	}
	return e_failure;
}

Status encode_secret_file_extn_size(int data, EncodeInfo *encInfo)
{
	if(encode_int_to_lsb(data, encInfo) == e_success)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

Status encode_int_to_lsb(int data, EncodeInfo *encInfo)
{
	char buffer[32];
	unsigned int value = (unsigned int) data;

	if(fread(buffer,1,32, encInfo->fptr_src_image) != 32)
	{
		return e_failure;
	}

	for(int i=31; i>=0; i--)
	{
		buffer[31-i] = (buffer[31-i] & 0xFE) | ((value >> i ) &1);
	}
		if(fwrite(buffer, 1, 32,encInfo-> fptr_stego_image)!= 32)
		{
			return e_failure;
		}

	return e_success;
}

Status encode_secret_file_size (long file_size, EncodeInfo *encInfo)
{
	if(encode_int_to_lsb(file_size, encInfo) == e_success)
	{
		return e_success;
	}
	return e_failure;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char buffer[encInfo->size_secret_file];
         if(fread(buffer, 1, (size_t)encInfo->size_secret_file, encInfo->fptr_secret) != (size_t)encInfo->size_secret_file)
       {
	       return e_failure;
       }

	return encode_data_to_image(buffer, encInfo->size_secret_file, encInfo);
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char ch;

	while(fread(&ch, 1, 1, fptr_src))
	{
		fwrite(&ch, 1, 1, fptr_dest);
	}
	return e_success;
}




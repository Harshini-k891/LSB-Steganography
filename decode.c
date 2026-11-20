#include<stdio.h>
#include<string.h>
#include "decode.h"
#include "types.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	if(argv[2] != NULL && strstr(argv[2], ".bmp") != NULL)
	{
		printf(".bmp file is present\n");
		decInfo->stego_image_fname = argv[2];
	}
	else
	{
		printf("Not enough aruguments\n Destination file name is not  present\n");
		return e_failure;
	}

	if(argv[3] != NULL)
	{
		printf("Destination fiel is present\n");
		char *token = strtok(argv[3], ".");
		decInfo -> output_file_fname = token;
	}

	else
	{
		decInfo->output_file_fname = "output.txt";
	}
	return e_success;
}

Status do_decoding(char *argv[], DecodeInfo *decInfo)
{

	if(open_files_dec(decInfo) == e_success)
	{
		printf("Files opened successfully\n");
	}
	else
	{
		printf("Failed to open files\n");
		return e_failure;
	}

	fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

	char magic[10];
	printf("Enter the magic string:");
	scanf(" %[^\n]",magic);

	int len = strlen(magic);

	if(decode_magic_string(len, decInfo) == e_success)
	{
		printf("Magic String decoded successfully\n");
		printf("User magic : %s\n",magic);
		printf("Decoded ,magic : %s\n",decInfo->magic_string);
	}
	else
	{
		printf("Failed to deocde magic string\n");
		return e_failure;
	}

	if(strcmp(magic, decInfo->magic_string) != 0)
	{
		printf("Magic string doesn not match\n");
		return e_failure;
	}

	if(decode_stego_file_extn_size(decInfo) == e_success)
	{
		printf("File extension size decoded successfully\n");
		printf("File extension size = %d bytes\n",decInfo->extn_size);
	}
	else
	{
		printf("Failed to decode file extension size\n");
		return e_failure;
	}

	if(decode_stego_file_extn(decInfo) == e_success)
	{
		printf("File extension decoded successfully\n");
		printf("File extension is %s\n", decInfo->extn);
	}

	else
	{
		printf("failed to decode fiel extenxion\n");
		return e_failure;
	}

	if(argv[3] != NULL)
	{
		strcat(decInfo->output_file_fname, decInfo->extn);
		printf("Output file name is %s\n",decInfo->output_file_fname);
	}
	else
	{
		printf("Output file is %s\n", decInfo->output_file_fname);
	}

	decInfo->fptr_output_file = fopen(decInfo->output_file_fname,"w");

	if(decode_stego_file_size(decInfo) == e_success)
	{
		printf("Secret File size decoded succesffuly\n");
		printf("Secret data size is %d\n",decInfo->secret_data_size);
	}
	else
	{
		printf("Failed to deocde secret file size\n");
		return e_failure;
	}

	if(decode_stego_file_secret_data(decInfo) == e_success)
	{
		printf("Secret data decoded successfully\n");
	}
	else
	{
		printf("failed to decode secret data\n");
		return e_failure;
	}
	return e_success;
}

Status open_files_dec(DecodeInfo *decInfo)
{
	decInfo -> fptr_stego_image = fopen(decInfo -> stego_image_fname,"r");
	if(decInfo -> fptr_stego_image == NULL)
	{
		printf("failed to open stego image file\n");
		return e_failure;
	}
	else
	{
		printf("Stego image File opened successfully\n");
	}

       return e_success;
}

Status decode_data_from_image(int size, DecodeInfo *decInfo)
{
	char buffer[8];

	for(int i=0; i<size; i++)
	{
		fread(buffer, 8, 1, decInfo->fptr_stego_image);
		decode_byte_from_lsb(&decInfo-> magic_string[i] , buffer);
	}
	decInfo-> magic_string[size] = '\0';
	return e_success;
}

Status decode_data_from_image2(int size, DecodeInfo *decInfo)
{
	char buffer[8];

	for(int i=0; i<size; i++)
	{
		fread(buffer, 8, 1, decInfo->fptr_stego_image);
		decode_byte_from_lsb(&decInfo->extn[i], buffer);
	}
	decInfo -> extn[size] = '\0';
	return e_success;
}

Status decode_data_from_image3(char *data, int size, DecodeInfo *decInfo)
{
	char buffer[8];

        for(int i=0; i<size; i++)
        {
                if (fread(buffer, 1, 8, decInfo->fptr_stego_image) != 8)
		{
			return e_failure;
		}
                decode_byte_from_lsb(&data[i], buffer);
		if(fwrite(&data[i],1,1,decInfo->fptr_output_file) != 1)
		{
			return e_failure;
		}
        }
        return e_success;
}

Status decode_int_from_lsb(DecodeInfo *decInfo)
{
	char buffer[32];
	unsigned int value = 0;

	if(fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)
	{
		return e_failure;
        }

        for(int i=0; i<32; i++)
        {
             value = (value << 1) |   ((unsigned char) buffer[i]& 1);
        }
	decInfo -> extn_size = (int)value;
        return e_success;
}
Status decode_int_from_lsb1(DecodeInfo *decInfo)
{
        char buffer[32];
	unsigned int value = 0;

        if(fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)
	{
		return e_failure;
	}

        for(int i=0; i<32; i++)
        {
                value = (value<<1) | ((unsigned char) buffer[i] & 1);
        }

	decInfo->secret_data_size = (int) value;
        return e_success;
}

Status decode_magic_string(int size, DecodeInfo *decInfo)
{
	if(decode_data_from_image(size, decInfo) == e_success)
	{
		return e_success;
	}
	return e_failure;
}

Status decode_stego_file_extn_size(DecodeInfo *decInfo)
{
	if(decode_int_from_lsb(decInfo) == e_success)
	{
		return e_success;
	}
	return e_failure;
}

Status decode_stego_file_extn(DecodeInfo *decInfo)
{
	if(decode_data_from_image2(decInfo->extn_size, decInfo) == e_success)
	{
		return e_success;
	}
	return e_failure;
}

Status decode_stego_file_size(DecodeInfo *decInfo)
{
	if(decode_int_from_lsb1(decInfo) == e_success)
	{
		return e_success;
	}
	return e_failure;
}

Status decode_stego_file_secret_data(DecodeInfo *decInfo)
{
	if(decInfo->secret_data_size <= 0)
	{
		return e_failure;
	}
	char buffer[decInfo->secret_data_size];

	if(decode_data_from_image3(buffer, decInfo->secret_data_size, decInfo) == e_success)
	{
		return e_success;
	}
	return e_failure;
}
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
	unsigned char value = 0;

    for (int i = 0; i < 8; i++)
    {
          value = (value << 1) | ((unsigned char)image_buffer[i] & 1);
    }

    *data = value;

    return e_success;
}



#ifndef DECODE_H
#define DECODE_H

#include<stdio.h>
#include "types.h"
typedef struct DecodeInfo
{
	char *stego_image_fname;
	FILE *fptr_stego_image;

	char magic_string[10];
	int extn_size;
	char extn[10];
	int secret_data_size;

	char *output_file_fname;
	FILE *fptr_output_file;
}DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(char *argv[], DecodeInfo *decInfo);

Status open_files_dec(DecodeInfo *decInfo);

Status decode_magic_string(int size, DecodeInfo *decInfo);

Status decode_data_from_image(int size, DecodeInfo *decInfo);

Status decode_data_from_image2(int size, DecodeInfo *decInfo);

Status decode_data_from_image3(char *data, int size, DecodeInfo *decInfo);

Status decode_byte_from_lsb(char *data, char *image_buffer);

Status decode_int_from_lsb(DecodeInfo *decInfo);

Status deocde_int_from_lsb1(DecodeInfo *decInfo);

Status decode_stego_file_extn_size(DecodeInfo *decInfo);

Status decode_stego_file_extn(DecodeInfo *decInfo);

Status decode_stego_file_size(DecodeInfo *decInfo);

Status decode_stego_file_secret_data(DecodeInfo *decInfo);

#endif

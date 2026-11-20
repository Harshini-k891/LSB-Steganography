#include<stdio.h>
#include<string.h>
#include"encode.h"
#include "decode.h"
#include"types.h"


int main(int argc, char *argv[])
{
	if(argc<2)
	{
		printf("Invalid arguments\n");
		return e_failure;
	}

	//validate the command args
	//we need to pass ./a.out -e secret.txt beautiful.bmp
	//-e->encoding
	//-d->decoding
	//argumenets count should be atleast 4 (if not -> print the error)

	if(check_operation_type(argv) == e_encode)
	{
		printf("You choose encoding\n");

		EncodeInfo encInfo;
		
		//encoding
		if(read_and_validate_encode_args(argv, &encInfo) == e_success)
		{
			printf("Read and Validate is succesfully\n");
			if(do_encoding(&encInfo) == e_success)
			{
				printf("Encoding successfully\n");
				return e_success;
			}
			else
			{
				printf("Encodoing unsuccesfull\n");
				return e_failure;
			}

		}
		else
		{
			printf("Read and Validate unsuccessful\n");
			return e_failure;
		}
		return e_success;
	}
	else if(check_operation_type(argv) == e_decode)
	{
		//decoding
		// if(check_operation_type(argv) == e_encode)
        
                printf("You choose decoding\n");

                DecodeInfo decInfo;

                //encoding
                if(read_and_validate_decode_args(argv, &decInfo) == e_success)
                {
                        printf("Read and Validate is succesfully\n");
                        if(do_decoding(argv,&decInfo) == e_success)
                        {
                                printf("Decoding successfully\n");
                                return e_success;
                        }
                        else
                        {
                                printf("Decodoing unsuccesfull\n");
                                return e_failure;
                        }

                }
                else
                {
                        printf("Read and Validate unsuccessful\n");
                        return e_failure;
                }
                return e_success;
        
	}

	else
	{
		printf("invalid option\n");
	}

}
 OperationType check_operation_type(char *argv[])	
{
	//check argv[1] is -e or -d
	if(strcmp(argv[1],"-e") == 0)
	{
		return e_encode;//-e

	}
	else if(strcmp(argv[1], "-d") == 0)
	{
		return e_decode;//-d
	}
	else
	{
	     return e_unsupported;
	}
}





#include"encode.h"
#include"decode.h"

Status read_and_validate_decode_args(char *argv[], EncodeInfo *encInfo)
{
	char cmp[100];
	strcpy(cmp,strstr(argv[2], ".bmp"));

	if(strcmp(cmp, ".bmp") == 0)
		return e_success;
	else
		return e_failure;
}

Status dec_open_files(EncodeInfo *encInfo)
{
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "r");

	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}

	encInfo->fptr_secret = fopen(encInfo->secret_fname, "w");

	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}
	
	return e_success;
}


Status do_decoding(EncodeInfo *encInfo)
{
	if(decode_magic_string(MAGIC_STRING,encInfo) == e_success)
	{
		sleep(1);
		fprintf(stdout,"INFO: Decoded Magic String.\n");
		if(decode_secret_file_extn_size(encInfo) == e_success)
		{
			sleep(1);
			fprintf(stdout,"INFO: Decoded Secret File Extension Size.\n");
			if(decode_secret_file_extn(encInfo) == e_success)
			{
				sleep(1);
				fprintf(stdout,"INFO: Decoded Secret File Extension.\n");
				if(decode_secret_file_size(encInfo) == e_success)
				{
					sleep(1);
					fprintf(stdout,"INFO: Decoded Secret File size.\n");
					if(decode_secret_file_data(encInfo) == e_success)
					{
						sleep(1);
						fprintf(stdout,"INFO: Decoded Image data.\n");
						return e_success;
					}
					else
					{
						sleep(1);
						fprintf(stderr,"ERROR: Failed in decoding Secret file data. \n");
						exit(2);
					}
				}
				else
				{
					sleep(1);
					fprintf(stderr,"ERROR: Failed in decoding Secret file size \n");
					exit(3);
				}
			}
			else
			{
				sleep(1);
				fprintf(stderr,"ERROR: Failed in decoding Extension. \n");
				exit(4);
			}
		}
		else
		{
			sleep(1);
			fprintf(stderr,"ERROR: Failed in decoding Extension size. \n");
			exit(5);
		}
	}
	else
	{
		sleep(1);
		fprintf(stderr,"ERROR: Failed in decoding magic string \n");
		exit(6);
	}
}

Status decode_magic_string(const char *magic_string,EncodeInfo *encInfo)
{
	fseek(encInfo->fptr_stego_image,54,SEEK_SET);
	
	int l = 7,read,write;
	uchar before_modified,after_modified,prev_modified = 0;
	uchar mask = 0x01;
	uchar magic_mask[sizeof(MAGIC_STRING)];
	for(int i = 0 ; i < strlen(magic_string) ; i++)
	{
		l = 7;
		prev_modified = 0;
		for(int j = 0 ; j < MAX_IMAGE_BUF_SIZE ; j++)
		{
			if((read = fread(&before_modified,1,1,encInfo->fptr_stego_image) != MAX_IMAGE_BUF_SIZE))
			{
				if(ferror(encInfo->fptr_stego_image))
				{
					fprintf(stderr,"ERROR : error in reading file\n");
					clearerr(encInfo->fptr_stego_image);
					return e_failure;
				}
			}
			uchar mod1 = (before_modified) & (mask);

			uchar mod2 = (mod1) << (l);

			after_modified = (mod2) | prev_modified;
			
			prev_modified = after_modified;

			--l;
		}
		magic_mask[i] = prev_modified;
	}
	if(strcmp(magic_mask,magic_string) == e_success)
		return e_success;
	else
		return e_failure;
}


Status decode_secret_file_extn_size(EncodeInfo *encInfo)
{
	fseek(encInfo->fptr_stego_image,0,SEEK_CUR);

	int l = 31,read,write;
	uchar before_modified,after_modified,prev_modified = 0;
	uchar mask = 0x01;
	for(int i = 0 ; i <  MAX_SIZE(sizeof(int)) ; i++)
	{
		if((read = fread(&before_modified ,1 ,1 , encInfo->fptr_stego_image)) != MAX_SIZE(sizeof(int)))
		{
			if(ferror(encInfo->fptr_stego_image))
			{
				fprintf(stderr,"ERROR : error in reading file\n");
				clearerr(encInfo->fptr_stego_image);
				return e_failure;
			}
		}
		uchar mod1 = (before_modified) & (mask);

		uchar mod2 = (mod1) << (l);
		
		after_modified = (mod2) | (prev_modified);

		prev_modified = after_modified;

		--l;
	}
	encInfo->size_extn = prev_modified;
	return e_success;
}

Status decode_secret_file_extn(EncodeInfo *encInfo)
{
	fseek(encInfo->fptr_stego_image,0,SEEK_CUR);

	int l = 7,read,write;
	uchar before_modified,after_modified,prev_modified = 0;
	uchar mask = 0x01;

	encInfo->extn_secret_file[MAX_FILE_SUFFIX];

	for(int i = 0 ; i < encInfo->size_extn ; i++)
	{
		l = 7;
		prev_modified = 0;
		for(int j = 0 ; j < MAX_IMAGE_BUF_SIZE ; j++)
		{
			if((read = fread(&before_modified, 1 , 1 , encInfo->fptr_stego_image)) != encInfo->image_capacity)
			{
				if(ferror(encInfo->fptr_stego_image))
				{
					fprintf(stderr,"ERROR : error in reading file\n");
					clearerr(encInfo->fptr_stego_image);
					return e_failure;
				}
			}
			
			uchar mod1 = (before_modified) & (mask);

			uchar mod2 = (mod1) << (l);

			after_modified = (mod2) | (prev_modified);

			prev_modified = after_modified;
			
			--l;
		}

		encInfo->extn_secret_file[i] = after_modified;

	}
	return e_success;
}


Status decode_secret_file_size(EncodeInfo *encInfo)
{
	fseek(encInfo->fptr_stego_image,0,SEEK_CUR);

	int l = 63,read,write;
	uchar before_modified,after_modified,prev_modified = 0;
	uchar mask = 0x01;
	
	for(int i = 0 ; i < MAX_SIZE(8)  ;i++)
	{
		if((read = fread(&before_modified , 1 , 1 ,encInfo->fptr_stego_image)) != MAX_SIZE(8))
		{
			if(ferror(encInfo->fptr_stego_image))
			{
				fprintf(stderr,"ERROR : error in reading file\n");
				clearerr(encInfo->fptr_stego_image);
				return e_failure;
			}
		}
		uchar mod1 = (before_modified) & (mask);

		uchar mod2 = (mod1) << (l);

		after_modified = (mod2) | (prev_modified);

		prev_modified = after_modified;

		encInfo->size_secret_file = after_modified;
		--l;
	}
	return e_success;
}

Status decode_secret_file_data(EncodeInfo *encInfo)
{
	fseek(encInfo->fptr_stego_image,0,SEEK_CUR);
	fseek(encInfo->fptr_secret,0,SEEK_SET);
	
	int l = 7,read,write;
	static int c = 0;
	uchar before_modified,after_modified,prev_modified = 0;
	uchar mask = 0x01;

	for(int i = 0 ; i < encInfo->size_secret_file ; i++)
	{
		l = 7;
		prev_modified = 0;
		for(int j = 0 ; j < MAX_IMAGE_BUF_SIZE ; j++)
		{
			if((read = fread(&before_modified ,1 ,1 ,encInfo->fptr_stego_image)) != MAX_IMAGE_BUF_SIZE)
			{
				if(ferror(encInfo->fptr_stego_image))
				{
					fprintf(stderr,"ERROR : error in reading file\n");
					clearerr(encInfo->fptr_stego_image);
					return e_failure;
				}
			}

			uchar mod1 = (before_modified) & (mask);

			uchar mod2 = (mod1) << l;

			after_modified = (mod2) | (prev_modified);

			prev_modified = after_modified;

			--l;
			c = 1;
		}
		if(feof(encInfo->fptr_secret))
		{
			fprintf(stderr,"ERROR : error in Writing file\n");
			clearerr(encInfo->fptr_secret);
			return e_failure;
		}
		if((write = fwrite(&after_modified, 1 , 1 , encInfo->fptr_secret)) != read)
		{
			fprintf(stderr,"ERROR : error in Writing file\n");
			return e_failure;
		}
	}
	if(c)
		return e_success;
	else
		return e_failure;

}

Status dec_close_files(EncodeInfo *encInfo)
{
	fclose(encInfo->fptr_stego_image);
	fclose(encInfo->fptr_secret);

	return e_success;
}

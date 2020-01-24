#include"encode.h"

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	char cmp[100]; 
	strcpy(cmp,strstr(argv[2], ".bmp"));
	if(argv[3] == NULL)
		return e_failure;
	encInfo->extn_secret_file[MAX_FILE_SUFFIX];
	
	int i = 0,b = 0;
	encInfo->secret_fname = argv[3];
	while(encInfo->secret_fname[i])
	{
		if(encInfo->secret_fname[i] == '.')
		{
			while(encInfo->secret_fname[i])
			{
				encInfo->extn_secret_file[b] = encInfo->secret_fname[i];
				i++;
				b++;
			}
			return e_success;
		}
		++i;
	}
	return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");

    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");

    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }
    return e_success;

}


Status do_encoding(EncodeInfo *encInfo)
{
	if(check_capacity(encInfo) != e_success)
		fprintf(stderr,"ERROR: file size is larger than image size \n");
	else
	{
		sleep(1);
		fprintf(stdout,"INFO: Capacity .OK \n");
		if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) != e_success)
		{
			fprintf(stderr,"ERROR: Unable to copy header file \n");
			exit(1);
		}
		else
		{
			sleep(1);
			fprintf(stdout,"INFO: Copied header file \n");
			if(encode_magic_string(MAGIC_STRING,encInfo) != e_success)
			{
				fprintf(stderr,"ERROR: Unable to copy magic string\n");
				exit(2);
			}
			else
			{
				sleep(1);
				fprintf(stdout,"INFO: Copied Magic string\n");
				if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) != e_success)
				{
					sleep(1);
					fprintf(stderr,"ERROR : Unable to copy file extension size\n");
					exit(3);
				}
				else
				{
					sleep(1);
					fprintf(stdout,"INFO: Copied File extension size\n");
					if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) != e_success )
					{
						sleep(1);
						fprintf(stderr,"ERROR : Unable to copy file extension\n");
						exit(4);
					}
					else
					{
						sleep(1);
						fprintf(stdout,"INFO: Copied File extension\n");
						if(encode_secret_file_size(encInfo->size_secret_file,encInfo) != e_success)
						{
							sleep(1);
							fprintf(stderr,"ERROR: Unable to copy file size. \n");
							exit(5);
						}
					
						else
						{
							sleep(1);
							fprintf(stdout,"INFO: Copied File size. \n");
						  if(encode_secret_file_data(encInfo) != 0)
						  {
							  sleep(1);
							  fprintf(stderr,"ERROR: Unable to copy remaining data. \n");
							  exit(6);
						  }
						 else
						 {
							 sleep(1);
							fprintf(stdout,"INFO: Copied data from secret file. \n");
					if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo) != e_success)
						                {
									sleep(1);
									fprintf(stderr,"ERROR : Unable to copy data. \n");
									exit(7);
								}
					else
						return e_success;
							}
						}
					}
				}

			}

			
		}
			
	}

	return e_success;
}

uint get_file_size(FILE *fptr)
{
	char ch;
	uint file_size;
	while ((ch = fgetc(fptr)) != EOF );
	file_size = ftell(fptr);
	rewind(fptr);
	return file_size;

}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, sizeof(int), 1, fptr_image);

    fread(&height, sizeof(int), 1, fptr_image);

    rewind(fptr_image);
    return width * height * 3;
}
Status check_capacity(EncodeInfo *encInfo)
{
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);	
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
	
	if(encInfo->size_secret_file == '\0')
	{
		fprintf(stderr,"ERROR: No data in secret file\n");
		return e_failure;
	}
	if(encInfo->size_secret_file+sizeof(MAGIC_STRING)+(2*sizeof(encInfo->extn_secret_file))+sizeof(encInfo->size_secret_file)> encInfo->image_capacity)
	{
		fprintf(stderr,"ERROR: file size is larger than image size \n");
		return e_failure;
	}
	return e_success;
}


Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	char header[55];
	int read,write;

	if((read = fread(header,1,54,fptr_src_image)) != 54)
	{
		if (ferror(fptr_src_image))
		{
			fprintf(stderr, "error in reading file. \n" );
			clearerr(fptr_src_image);
			return e_failure;
		}
	}
	if(feof(fptr_dest_image))
	{
		fprintf(stderr,"Error in writing file. \n");
		clearerr(fptr_dest_image);
		return e_failure;
	}
	if((write = fwrite(header,1,read,fptr_dest_image)) != read)
	{
		fprintf(stderr,"Error in writing file. \n");
		return e_failure;
	}

	return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	int read,write,i = 1,l = 7;
	uchar before_modified;
	uchar after_modified;
	int magic_size = strlen(magic_string);
	fseek(encInfo->fptr_src_image,54,SEEK_SET);
	uchar magic_mask[sizeof(MAGIC_STRING)];
	uint num = 7;
	strcpy(magic_mask,MAGIC_STRING);
	for(int  j = 0 ; magic_mask[j] != '\0' ; j++)
	{
		uchar mask = 0x80;
		i = 0;
		l = 7;
		fseek(encInfo->fptr_src_image,0,SEEK_CUR);
		
		for(int k = 0 ; k < MAX_IMAGE_BUF_SIZE; k++)
		{
			if((read = fread(&before_modified,1,1,encInfo->fptr_src_image)) != MAX_SIZE(strlen(magic_string)))
			{
				if(ferror(encInfo->fptr_src_image))
				{
					fprintf(stderr,"ERROR : error in reading file\n");
					clearerr(encInfo->fptr_src_image);
					return e_failure;
				}
				uchar mod1 = (before_modified) & (0xfe);
				mask=mask>>i;
				uchar mod2 = (magic_mask[j]) & (mask);
				if(i == 0)
					i = 1;

				mod2 = mod2 >> l;

				uchar after_modified = mod1 | mod2 ;

				if(feof(encInfo->fptr_stego_image))
				{
					fprintf(stderr,"Error in writing file. \n");
					clearerr(encInfo->fptr_stego_image);
					return e_failure;
				}

				if((write = fwrite(&after_modified , 1 , 1 , encInfo->fptr_stego_image)) != read)
				{
					fprintf(stderr,"Error in writing file. \n");
					return e_failure;
				}
				--l;
			}
		}
	}
	return e_success;
}


Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	int read,write,i = 0,l = 63;
	uchar before_modified;
	uchar after_modified;
	ulint mask = 0x8000000000000000;
	uint num = 63;
	fseek(encInfo->fptr_src_image,0,SEEK_CUR);
	fseek(encInfo->fptr_stego_image,0,SEEK_CUR);
	for(int k = 0 ; k < MAX_SIZE(8) ; k++)
	{
		if((read = fread(&before_modified,1,1,encInfo->fptr_src_image)) != (MAX_SIZE(8)))
		{
			if(ferror(encInfo->fptr_src_image))
			{
				fprintf(stderr,"ERROR : error in reading file\n");
				clearerr(encInfo->fptr_src_image);
				return e_failure;
			}
			uchar mod1 = (before_modified) & (0xfe);

			mask=mask>>i;
			ulint mod2 = (file_size) & (mask);
			if(i == 0)
				i = 1;

			mod2 = mod2 >> l;

			uchar after_modified = mod1 | mod2 ;

			if(feof(encInfo->fptr_stego_image))
			{
				fprintf(stderr,"ERROR: Error in writing file. \n");
				clearerr(encInfo->fptr_stego_image);
				return e_failure;
			}

#if 1
			if((write = fwrite(&after_modified , 1 , 1 , encInfo->fptr_stego_image)) != read)
			{
				fprintf(stderr,"ERROR: Error in writing file. \n");
				return e_failure;
			}
#endif
			--l;
		}
		else
			return e_failure;
	}
	return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	int read,write,i = 0,l = 7;
	uchar before_modified;
	uchar after_modified;
	uint num = 7;
	for(int  j = 0 ; file_extn[j] != '\0' ; j++)
	{
		uchar mask = 0x80;
		i = 0;
		l = 7;
		fseek(encInfo->fptr_src_image,0,SEEK_CUR);
		for(int k = 0 ; k < MAX_IMAGE_BUF_SIZE ; k++)
		{
			if((read = fread(&before_modified,1,1,encInfo->fptr_src_image)) != MAX_IMAGE_BUF_SIZE)
			{
				if(ferror(encInfo->fptr_src_image))
				{
					fprintf(stderr,"ERROR : error in reading file\n");
					clearerr(encInfo->fptr_src_image);
					return e_failure;
				}
			}
			uchar mod1 = (before_modified) & (0xfe);
			mask=mask>>i;
			uchar mod2 = (file_extn[j]) & (mask);
			
			if(i == 0)
				i = 1;
			mod2 = mod2 >> l;

			 after_modified = mod1 | mod2 ;

			if(feof(encInfo->fptr_stego_image))
			{
				fprintf(stderr,"ERROR:Error in writing file. \n");
				clearerr(encInfo->fptr_stego_image);
				return e_failure;
			}
			if((write = fwrite(&after_modified , 1 , 1 , encInfo->fptr_stego_image)) != read)
			{
				fprintf(stderr,"ERROR2:Error in writing file. \n");
				return e_failure;
			}
			--l;
		}
	}
	return e_success;
}

Status encode_secret_file_extn_size(int extn_size, EncodeInfo *encInfo)
{
	int read,write,i = 0 , l = 31;
	uchar before_modified;
	uchar after_modified;
	uint mask = 0x80000000;
	fseek(encInfo->fptr_src_image,0,SEEK_CUR);
	fseek(encInfo->fptr_stego_image,0,SEEK_CUR);
	for(int k = 0 ; k < MAX_SIZE(extn_size) ; k++)
	{
		if((read = fread(&before_modified,1,1,encInfo->fptr_src_image)) != MAX_SIZE(extn_size))
		{
			
			if(ferror(encInfo->fptr_src_image))
			{
				fprintf(stderr,"ERROR : error in reading file\n");
				clearerr(encInfo->fptr_src_image);
				return e_failure;
			}
			uchar mod1 = (before_modified) & (0xfe);

			mask = mask >> i;

			ulint mod2 = (extn_size) & (mask);
			if(i == 0)
				i = 1;

			mod2 = mod2 >> l;

			uchar after_modified = mod1 | mod2 ;

			if(feof(encInfo->fptr_stego_image))
			{
				fprintf(stderr,"ERROR: Error in writing file. \n");
				clearerr(encInfo->fptr_stego_image);
				return e_failure;
			}
			if((write = fwrite(&after_modified , 1 , 1 , encInfo->fptr_stego_image)) != read)
			{
				fprintf(stderr,"ERROR:Error in writing file. \n");
				return e_failure;
			}
			--l;
		}
	}
	return e_success;
}



Status encode_secret_file_data(EncodeInfo *encInfo)
{
	int firead,iread,write,i = 0, l = 7;
	uchar img_before_modified;
	uchar file_before_modified;
	uchar after_modified;
	uchar mask = 0x80;
	fseek(encInfo->fptr_src_image,0,SEEK_CUR);
	fseek(encInfo->fptr_secret,0,SEEK_SET);
	fseek(encInfo->fptr_stego_image,0,SEEK_CUR);
	for(long j = encInfo->size_secret_file ; j >= 0 ; j--)
	{
		mask = 0x80;
		i = 0;
		l = 7;
		if((firead = fread(&file_before_modified,1,1,encInfo->fptr_secret)) != encInfo->size_secret_file)
		{
			if(ferror(encInfo->fptr_secret))
			{
				fprintf(stderr,"Error in reding file. \n");
				clearerr(encInfo->fptr_secret);
				return e_failure;
			}

		}
		for(int k = 0 ; k < MAX_IMAGE_BUF_SIZE ; k++)
		{
			if((iread = fread(&img_before_modified,1,1,encInfo->fptr_src_image)) != encInfo->image_capacity)
			{
				if(ferror(encInfo->fptr_src_image))
				{
					fprintf(stderr,"Error in reding file. \n");
					clearerr(encInfo->fptr_src_image);
					return e_failure;
				}

			}
			uchar mod1 = (img_before_modified) & (0xfe);

			mask=mask>>i;
			ulint mod2 = (file_before_modified) & (mask);
			if(i == 0)
				i = 1;

			mod2 = mod2 >> l;

			uchar after_modified = mod1 | mod2 ;

			if(feof(encInfo->fptr_stego_image))
			{
				fprintf(stderr,"ERROR: Unable to write to file. \n");
				clearerr(encInfo->fptr_stego_image);
				return e_failure;
			}

			if((write = fwrite(&after_modified,1,1,encInfo->fptr_stego_image)) != iread)
			{
				fprintf(stderr,"ERROR: Unable to write to file. \n");
				clearerr(encInfo->fptr_stego_image);
				return e_failure;
			}
			--l;
		}
	}
	return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest,EncodeInfo *encInfo)
{

	uchar after[50];
	fseek(fptr_src,0,SEEK_CUR);
	fseek(fptr_dest,0,SEEK_CUR);
	int read;
	while((read = fread(after,1,50,fptr_src)) != 0)
		fwrite(after,1,read,fptr_dest);

	return e_success;
}


Status close_files(EncodeInfo *encInfo)
{
	fclose(encInfo->fptr_src_image);
	fclose(encInfo->fptr_secret);
	fclose(encInfo->fptr_stego_image);
	return e_success;
}

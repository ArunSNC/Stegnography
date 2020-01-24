#include"encode.h"
#include"decode.h"

int main(int argc, char *argv[])
{
	system("clear");
	EncodeInfo encInfo;
	switch(check_operation_type(argv))
	{
		case e_encode:
			encInfo.src_image_fname = argv[2];
			encInfo.secret_fname = argv[3];
			if(argc == 5)
				encInfo.stego_image_fname = argv[4];
			else
			{
				fprintf(stdout,"INFO: output file not mentioned\n");
				encInfo.stego_image_fname = "default.bmp";
				sleep(1);
				fprintf(stdout,"INFO: Generating a default file\n");
			}
			if(read_and_validate_encode_args(argv,&encInfo) == e_success)
			{
				sleep(1);
				fprintf(stdout,"INFO: Sucessfully validated the arguments. \n");
				if(open_files(&encInfo) != e_success)
				{
					sleep(1);
					fprintf(stderr,"ERROR: error in opening files. \n");
					exit(1);
				}
				else
				{
					sleep(1);
					fprintf(stdout,"INFO: opening files. \n");
					sleep(1);
					fprintf(stdout,"****************ENCODING STARTED*******************\n");
					if(do_encoding(&encInfo) != e_success)
					{
						fprintf(stderr,"ERROR: error in encoding process. \n");
						exit(2);
					}
					else
					{
						sleep(1);
						fprintf(stdout,"INFO: Sucessfully Encoded data. \n");
						sleep(1);
						fprintf(stdout,"****************ENCODING ENDED*******************\n");
						if(encInfo.image_capacity >= ftell(encInfo.fptr_stego_image))
						{
								fprintf(stderr,"ERROR: File corrupted. \n");
								exit(1);
						}
						if(close_files(&encInfo) != e_success)
						{
							sleep(1);
							fprintf(stderr,"ERROR: error in closing files. \n");
							exit(3);
						}
						else
						{
							sleep(1);
							fprintf(stdout,"INFO: Sucessfully closed the files. \n");
						}
					}
				}
			}
			else
			{
				sleep(1);
				fprintf(stderr,"ERROR: validation failure. \n");
				exit(4);
			}
			break;

		case e_decode:
			encInfo.stego_image_fname = argv[2];
			if(argc == 4)
				encInfo.secret_fname = argv[3];
			else
			{
				fprintf(stdout,"INFO: output file not mentioned\n");
				encInfo.secret_fname = "default.txt";
				sleep(1);
				fprintf(stdout,"INFO: Generating a default file\n");
			}
			if(read_and_validate_decode_args(argv,&encInfo) == e_success)
			{
				sleep(1);
				fprintf(stdout,"INFO: Sucessfully validated the arguments. \n");
				if(dec_open_files(&encInfo) == e_success)
				{
					sleep(1);
					fprintf(stdout,"INFO: opening files. \n");
					sleep(1);
					fprintf(stdout,"****************DECODING STARTED*******************\n");
					if(do_decoding(&encInfo) == e_success)
					{
						sleep(1);
						fprintf(stdout,"INFO: Sucessfully Decoded data. \n");
						sleep(1);
						fprintf(stdout,"****************DECODING ENDED*******************\n");

						if(dec_close_files(&encInfo) == e_success)
						{
							sleep(1);
							fprintf(stdout,"INFO: Sucessfully Closed Files. \n");
						}
					}
					else
					{
						sleep(1);
						fprintf(stderr,"ERROR: Error in Decoding data. \n");
					}
				}
				else
				{
					sleep(1);
					fprintf(stderr,"ERROR: error in opening files. \n");
					exit(1);
				}
			}
			else
			{
				sleep(1);
				fprintf(stderr,"ERROR: validation failure. \n");
				exit(4);
			}
			break;
		default:
			printf("unsupported\n");
			exit(1);
	}
	return 111;
}

OperationType check_operation_type(char *argv[])
{
        if((strcmp(argv[1],"-e")) == e_success)
                return e_encode;
        if(strcmp(argv[1],"-d") == e_success)
                return e_decode;
        else
                return e_unsupported;
}

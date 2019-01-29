#include "definitions.h"

extern int blockColumns,blockRows,type;

uint8_t * parallel_read(char *image_file, int blockOffset,int width){
	MPI_File inFile;
	if(MPI_File_open(MPI_COMM_WORLD, image_file, MPI_MODE_RDONLY, MPI_INFO_NULL, &inFile))
		puts("Error in open inFile");

	uint8_t *t0 = NULL;
    t0=(uint8_t *)(malloc(type*(blockRows+2)*(blockColumns+2)*sizeof(uint8_t))); //current image;
	if(t0==NULL){
		puts("Malloc failure");
		MPI_Abort(MPI_COMM_WORLD, -1);
		exit(-1);
	}

	MPI_Status status;
	int pointer = type*(blockColumns+3);    //ignore the first line and first byte, halo
	int tempOffset = type*blockOffset;
	for (int i = 1 ; i <= blockRows ; i++) {
		MPI_File_seek(inFile, tempOffset, MPI_SEEK_SET);
		//if(MPI_File_seek(inFile, tempOffset, MPI_SEEK_SET))
		//	puts("Error in seek");
		MPI_File_read(inFile, t0+pointer, type*blockColumns, MPI_BYTE, &status);
		//if(MPI_File_read(inFile, t0+pointer, type*blockColumns, MPI_BYTE, &status))
		//	puts("Error in read");
		//int count;
		//MPI_Get_count(&status,MPI_BYTE,&count);
		//if(count!=type*blockColumns)
		//	puts("Error in bytes read");
		pointer += (type*(blockColumns+2));   //ignore 2 halo bytes
		tempOffset += (type*width);
	}
	MPI_File_close(&inFile);
	return t0;
}

void parallel_write(uint8_t* t0,char *image_file,int blockOffset,int width){
	char *outImage = malloc((strlen(image_file) + 10) * sizeof(char));
	if(outImage==NULL){
		printf("Malloc failure\n");
		MPI_Abort(MPI_COMM_WORLD, -1);
		exit(-1);
	}
	sprintf(outImage,"filtered_%s",image_file);

	MPI_File outFile;
	MPI_File_open(MPI_COMM_WORLD, outImage, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &outFile);
	//if(MPI_File_open(MPI_COMM_WORLD, outImage, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &outFile))
	//	puts("Error in open outFile");

	MPI_Status status;
	int pointer = type*(blockColumns+3);
	int tempOffset = type*blockOffset;
	int mine=0;
	for (int i = 1 ; i <= blockRows ; i++) {
		MPI_File_seek(outFile, tempOffset, MPI_SEEK_SET);
		//if(MPI_File_seek(outFile, tempOffset, MPI_SEEK_SET))
		//	puts("Error in seek outFile");
		MPI_File_write(outFile, t0+pointer,type*blockColumns, MPI_BYTE, &status);
		//if(MPI_File_write(outFile, t0+pointer,type*blockColumns, MPI_BYTE, &status))
		//	puts("Error in write");
		//int count;
		//MPI_Get_count(&status,MPI_BYTE,&count);
		//if(count!=type*blockColumns)
		//	puts("Error in bytes written");
		pointer+=(type*(blockColumns+2));
		tempOffset+=type*width;
	}
	MPI_File_close(&outFile);
	free(outImage);
}

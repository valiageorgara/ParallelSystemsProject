#include"definitions.h"

extern int blockRows,blockColumns,type;

char * read_args(int argc,char **argv,int * numOfLoops,int *width, int *height, int idOfProcess,int numOfProcesses,int *allReduceLoops){
	if(argc <6 || argc>7){
		puts("You gave wrong number of arguments.");
		MPI_Abort(MPI_COMM_WORLD, -1);
		exit(-1);
	}
	char *image_file = malloc((strlen(argv[1])+1)*sizeof(char));
	if(image_file==NULL){
		puts("Malloc failure");
		MPI_Abort(MPI_COMM_WORLD, -1);
		exit(-1);
	}
	strcpy(image_file,argv[1]);

	if(idOfProcess==0){
		if(strcmp(argv[2],"rgb")==0 || strcmp(argv[2],"RGB")==0)
			type = RGB;
		else if(strcmp(argv[2],"grey")==0 || strcmp(argv[2],"GREY")==0)
			type = GREY;
		else{
			printf("You gave wrong image type. Input should be either rgb or grey\n");
			MPI_Abort(MPI_COMM_WORLD, -1);
			exit(-1);
		}

		*width = atoi(argv[3]);
		*height = atoi(argv[4]);
		*numOfLoops = atoi(argv[5]);
		blockRows = *height / sqrt(numOfProcesses);      //rows and columns in each block
		blockColumns = *width / sqrt(numOfProcesses);

		if(argc == 7)
			*allReduceLoops=atoi(argv[6]);
		else
			*allReduceLoops=0;
	}
	return image_file;
}

int allreduce(uint8_t *t0, uint8_t *t1,int numOfProcesses){
	int flag=1;
	for(int k=1;k<=blockRows;k++){
		for(int l=1;l<=blockColumns;l++)
			if(t0[k*(blockColumns+2)+l]!=t1[k*(blockColumns+2)+l]){
				flag=0;
				break;
			}
	}
	int sum;
	MPI_Allreduce(&flag,&sum,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
	if(sum==numOfProcesses){
		puts("Stopped with allreduce");
		return 1;
	}
	return 0;
}

void computeTime(int idOfProcess,int numOfProcesses,double totalTime){
	MPI_Status status;
	if(idOfProcess==0){
		double iTime;
		for(int i=1;i<numOfProcesses;i++){
			MPI_Recv(&iTime, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
			if(iTime>totalTime)
				totalTime=iTime;
		}
		printf("%f\n",totalTime);
	}
	else
        MPI_Send(&totalTime, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
}
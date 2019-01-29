#include"definitions.h"

int blockRows,blockColumns,type;

int main(int argc, char * argv[]){
  //FOR COMPILATION :  make mpi/ make openmp
  //FOR RUNNING : mpirun -np 4 ./exec waterfall_grey_1920_2520.raw grey 1920 2520 20 // mpirun -np 4 ./exec waterfall_1920_2520.raw rgb 1920 2520 20
  // mpiexec -f linux10.di.uoa.gr -n 4 ./exec waterfall_grey_1920_2520.raw grey 1920 2520 20
	int numOfLoops,width,height,idOfProcess,numOfProcesses,allReduceLoops;

	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		puts("Error initializing MPI");
        return -1;
	}
	MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &idOfProcess);

	char * image_file=read_args(argc,argv,&numOfLoops,&width,&height,idOfProcess,numOfProcesses,&allReduceLoops);
	filtermaking();

	MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&blockRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&blockColumns, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&numOfLoops, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&type, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&allReduceLoops, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int blockOffset = ((idOfProcess / (int)( sqrt(numOfProcesses))) * blockRows)*width + (idOfProcess % (int)(sqrt(numOfProcesses)) )* blockColumns;

	uint8_t *t0 = parallel_read(image_file,blockOffset,width);
	uint8_t *t1=(uint8_t *)(malloc(type*(blockRows+2)*(blockColumns+2)*sizeof(uint8_t))); //current image;
	if(t1==NULL){
		puts("Malloc failure");
		MPI_Abort(MPI_COMM_WORLD, -1);
		exit(-1);
	}

	initializeMPI();
	neighborInitialization(idOfProcess,numOfProcesses);
	MPI_Barrier(MPI_COMM_WORLD);
	double time = MPI_Wtime();

	for(int i=0;i<numOfLoops;i++){
		send_receive(t0);
		convolution(2,blockRows-2,2*type,(blockColumns-2)*type,t0,t1);
		outerPixelConvolution(t0,t1);

		if(allReduceLoops>0)
			if(i%allReduceLoops==0)
				if(allreduce(t0,t1,numOfProcesses))
					break;

		uint8_t *swap = t0;
		t0=t1;
		t1=swap;
	}

	time = MPI_Wtime()-time;
	parallel_write(t0,image_file,blockOffset,width);
	computeTime(idOfProcess,numOfProcesses,time);

	freeMPI();
	MPI_Finalize();
	free(image_file);
	free(t0);
	free(t1);
	return 0;
}

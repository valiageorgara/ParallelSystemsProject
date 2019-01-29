#include "definitions.h"

extern int blockRows,blockColumns,type;

MPI_Datatype rowDatatype,columnDatatype;

int north=-3;
int south=-3;
int west=-3;
int east=-3;
int north_east=-3;
int north_west=-3;
int south_west=-3;
int south_east=-3;

MPI_Request sendNorth=MPI_REQUEST_NULL,sendSouth=MPI_REQUEST_NULL,sendEast=MPI_REQUEST_NULL,sendWest=MPI_REQUEST_NULL,sendNorthEast=MPI_REQUEST_NULL,sendNorthWest=MPI_REQUEST_NULL,sendSouthEast=MPI_REQUEST_NULL,sendSouthWest=MPI_REQUEST_NULL;
MPI_Request receiveNorth=MPI_REQUEST_NULL,receiveSouth=MPI_REQUEST_NULL,receiveEast=MPI_REQUEST_NULL,receiveWest=MPI_REQUEST_NULL,receiveNorthEast=MPI_REQUEST_NULL,receiveNorthWest=MPI_REQUEST_NULL,receiveSouthEast=MPI_REQUEST_NULL,receiveSouthWest=MPI_REQUEST_NULL;

void initializeMPI(){
	MPI_Type_vector(blockRows, type, (blockColumns+2)*type, MPI_BYTE, &columnDatatype);
	MPI_Type_commit(&columnDatatype);

	MPI_Type_contiguous(blockColumns*type, MPI_BYTE, &rowDatatype);
	MPI_Type_commit(&rowDatatype);
}

void neighborInitialization(int idOfProcess,int numOfProcesses){
	if(idOfProcess>=sqrt(numOfProcesses)){           //north
		north= idOfProcess-sqrt(numOfProcesses);
		if(idOfProcess%(int)(sqrt(numOfProcesses))!=0)       //north_west
			north_west=north-1;
		if(idOfProcess%(int)(sqrt(numOfProcesses))!=sqrt(numOfProcesses)-1)  //north_east
			north_east=north+1;
	}
	if(idOfProcess< numOfProcesses-sqrt(numOfProcesses)){  //south
		south=idOfProcess+sqrt(numOfProcesses);
		if(idOfProcess%(int)(sqrt(numOfProcesses))!=0)              //south_west
			south_west=south-1;
		if(idOfProcess%(int)(sqrt(numOfProcesses))!=sqrt(numOfProcesses)-1)  //south_east
			south_east=south+1;
	}
	if(idOfProcess%(int)(sqrt(numOfProcesses))!=0)   //west
		west= idOfProcess-1;
	if(idOfProcess%(int)(sqrt(numOfProcesses))!=sqrt(numOfProcesses)-1)  //east
		east = idOfProcess+1;
}

void send_receive(uint8_t *t0){
			if(north>=0){
				MPI_Isend(&t0[(blockColumns+3)*type], 1, rowDatatype, north, 0, MPI_COMM_WORLD, &sendNorth);
				MPI_Irecv(&t0[type], 1, rowDatatype, north, 0, MPI_COMM_WORLD, &receiveNorth);
			}
			if(south>=0){
				MPI_Isend(&t0[((blockRows)*(blockColumns+2)+1)*type], 1, rowDatatype, south, 0, MPI_COMM_WORLD, &sendSouth);
				MPI_Irecv(&t0[((blockRows+1)*(blockColumns+2)+1)*type], 1, rowDatatype, south, 0, MPI_COMM_WORLD, &receiveSouth);
			}
			if(west>=0){
				MPI_Isend(&t0[(blockColumns+3)*type], 1, columnDatatype, west, 0, MPI_COMM_WORLD, &sendWest);
				MPI_Irecv(&t0[(blockColumns+2)*type], 1, columnDatatype, west, 0, MPI_COMM_WORLD, &receiveWest);
			}
			if(east>=0){
				MPI_Isend(&t0[(2*blockColumns+2)*type], 1, columnDatatype, east, 0, MPI_COMM_WORLD, &sendEast);
				MPI_Irecv(&t0[(2*blockColumns+3)*type], 1, columnDatatype, east, 0, MPI_COMM_WORLD, &receiveEast);
			}
			if(north_west>=0){
				MPI_Isend(&t0[(blockColumns+3)*type], type, MPI_BYTE, north_west, 0, MPI_COMM_WORLD, &sendNorthWest);
				MPI_Irecv(&t0[0], type, MPI_BYTE, north_west, 0, MPI_COMM_WORLD, &receiveNorthWest);
			}
			if(north_east>=0){
				MPI_Isend(&t0[(2*blockColumns+2)*type], type, MPI_BYTE, north_east, 0, MPI_COMM_WORLD, &sendNorthEast);
				MPI_Irecv(&t0[(blockColumns+1)*type], type, MPI_BYTE, north_east, 0, MPI_COMM_WORLD, &receiveNorthEast);
			}
			if(south_west>=0){
				MPI_Isend(&t0[((blockRows)*(blockColumns+2)+1)*type], type, MPI_BYTE, south_west, 0, MPI_COMM_WORLD, &sendSouthWest);
				MPI_Irecv(&t0[(blockRows+1)*(blockColumns+2)*type], type, MPI_BYTE, south_west, 0, MPI_COMM_WORLD, &receiveSouthWest);
			}
			if(south_east>=0){
				MPI_Isend(&t0[((blockRows)*(blockColumns+2)+blockColumns)*type], type, MPI_BYTE, south_east, 0, MPI_COMM_WORLD, &sendSouthEast);
				MPI_Irecv(&t0[((blockRows+2)*(blockColumns+2)-1)*type], type, MPI_BYTE, south_east, 0, MPI_COMM_WORLD, &receiveSouthEast);
			}
}

void outerPixelConvolution(uint8_t * t0, uint8_t *t1){
	MPI_Status status;
	//if (north >=0) {
		MPI_Wait(&receiveNorth, &status);
		convolution(1,1,2*type,blockColumns*type-1,t0,t1);
	//}else
		//for (int j = 2*type; j <= blockColumns*type-1; j++)
			//t1[(blockColumns+2)*type +j] = t0[(blockColumns+2)*type +j];
	//if (west >=0) {
		MPI_Wait(&receiveWest, &status);
		convolution(2,blockRows-1,type,2*type-1,t0,t1);
	//}else
		//for (int i = 2; i <= blockRows-1; i++)
			//for(int j=type;j<2*type;j++)
				//t1[(blockColumns+2)*i*type +j] = t0[(blockColumns+2)*i*type +j] ;
	//if (south >=0) {
		MPI_Wait(&receiveSouth, &status);
		convolution(blockRows,blockRows,2*type,blockColumns*type-1,t0,t1);
	//}
	//else
		//for (int j = 2*type; j<= blockColumns*type-1; j++) 
			//t1[(blockColumns+2)*blockRows*type +j] = t0[(blockColumns+2)*blockRows*type +j];
	//if (east >=0) {
		MPI_Wait(&receiveEast, &status);
		convolution(2,blockRows-1,blockColumns*type,blockColumns*type+type-1,t0,t1);
	//}else
		//for (int i = 2; i <= blockRows-1; i++)
			//for(int j=blockColumns*type;j<blockColumns*type+type;j++)
				//t1[(blockColumns+2)*i*type +j] = t0[(blockColumns+2)*i*type +j];
	//cornerPixelConvolution
	//if(north_west>=0){
		MPI_Wait(&receiveNorthWest, &status);
		convolution(1,1,type,2*type-1,t0,t1);
	//}else
		//for(int j=type;j<2*type;j++)
			//t1[(blockColumns+2)*type +j] = t0[(blockColumns+2)*type +j];
	//if(north_east>=0){
		MPI_Wait(&receiveNorthEast, &status);
		convolution(1,1,blockColumns,blockColumns+type-1,t0,t1);
	//}else
		//for(int j=blockColumns;j<blockColumns+type;j++)
			//t1[(blockColumns+2)*type +j] = t0[(blockColumns+2)*type +j];
	//if(south_west>=0){
		MPI_Wait(&receiveSouthWest, &status);
		convolution(blockRows,blockRows,type,2*type-1,t0,t1);
	//}else
		//for(int j=type;j<2*type;j++)
			//t1[(blockColumns+2)*blockRows*type +j] = t0[(blockColumns+2)*blockRows*type +j];
	//if(south_east>=0){
		MPI_Wait(&receiveSouthEast, &status);
		convolution(blockRows,blockRows,blockColumns,blockColumns+type-1,t0,t1);
	//}else
		//for(int j=blockColumns;j<blockColumns+type;j++)
			//t1[(blockColumns+2)*blockRows*type +j] = t0[(blockColumns+2)*blockRows*type +j];

	MPI_Wait(&sendNorth, &status);
	MPI_Wait(&sendWest, &status);
	MPI_Wait(&sendSouth, &status);
	MPI_Wait(&sendEast, &status);
	MPI_Wait(&sendNorthWest, &status);
	MPI_Wait(&sendNorthEast, &status);
	MPI_Wait(&sendSouthWest, &status);
	MPI_Wait(&sendSouthEast, &status);
}

void freeMPI(){
	MPI_Type_free(&columnDatatype);
	MPI_Type_free(&rowDatatype);
}
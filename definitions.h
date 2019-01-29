#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<mpi.h>
#ifdef _OPENMP
#include<omp.h>
#endif

#define RGB 3
#define GREY 1

//general.c
char * read_args(int ,char **,int *,int *, int *,int,int,int *);
int allreduce(uint8_t *, uint8_t *,int);
void computeTime(int ,int ,double);

//convolution.c
void filtermaking();
void convolution(int,int,int , int , uint8_t *,uint8_t *);

//parallelIO.c
uint8_t * parallel_read(char *, int ,int);
void parallel_write(uint8_t*,char *,int ,int);

//communications.c
void initializeMPI();
void neighborInitialization(int,int);
void send_receive(uint8_t *);
void outerPixelConvolution(uint8_t * , uint8_t *);
void freeMPI();
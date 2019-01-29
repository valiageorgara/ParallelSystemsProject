#include "definitions.h"

extern int type,blockColumns,blockRows;

/////////filter array, choose one
//int initialFilter[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}}; //sharpen
int initialFilter[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}}; //gaussian blur
//int initialFilter[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}}; //edge
float filter[3][3];

void filtermaking(){
	int sumOfFilterElements=0;
	for (int i = 0 ; i < 3 ; i++)
		for (int j = 0 ; j < 3 ; j++)
			sumOfFilterElements+=initialFilter[i][j];
	if(sumOfFilterElements==0){
		puts("Can't normalize filter");
		for(int i=0;i<3;i++)
			for(int j=0;j<3;j++)
				filter[i][j]=(float)(initialFilter[i][j]);
	}else
		for (int i = 0 ; i < 3 ; i++)
			for (int j = 0 ; j < 3 ; j++)
				filter[i][j] = (float)(initialFilter[i][j] / (float)(sumOfFilterElements));
}

void convolution(int starti, int endi, int startj, int endj, uint8_t *t0,uint8_t * t1){
#ifdef _OPENMP
#pragma omp parallel for shared(t0,t1) num_threads(2)
#endif
	for(int i=starti;i<=endi;i++)
		for(int j=startj;j<=endj;j++){
			float pixel = 0;
			int filter_i=0;
			for (int k = i-1; k <= i+1; k++){
				int filter_j=0;
				for (int l = j-type; l <= j+type; l+=type){
						pixel+=t0[(blockColumns+2)*k*type+l]*filter[filter_i][filter_j];
						filter_j++;
				}
				filter_i++;
			}
			if(pixel>255) pixel=255;
			if(pixel<0) pixel=0;
			t1[(blockColumns+2)*i*type+j]=pixel;
		}
}

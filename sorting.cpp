#include <stdio.h>
#include <chrono>
#include <sys/time.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>

int ITERS = 1;
int NUMTHREADS = 10;
int N = 10000000;
int K = 1000;
int CPAD = 8;

void get_walltime(double* wcTime) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    *wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);
}

void omp_countsort(int *tester, int *omp_ans, int *mid_array){

    int *tmp_mid;
    int thr_num; 
   
#pragma omp parallel private(tmp_mid, thr_num) shared(mid_array, tester)
{
    tmp_mid = new int[K];
    thr_num = omp_get_thread_num();
    
    //each thread works on a part of the unsorted array
    #pragma omp for
    for (int i = 0; i<N; i++)
        tmp_mid[tester[i]]++; 
    
    //prefix sums on each thread
    for (int i = 1; i<K; i++)
	tmp_mid[i] += tmp_mid[i-1];
   
    //add up all middle arrays to get one 
    #pragma omp for reduction(+:mid_array[:K*CPAD]) 
    for (int i = 0; i<NUMTHREADS; i++){
	for (int j = 0; j<K; j++){
	    mid_array[j*CPAD] += tmp_mid[j];
	}
    }

    for (int i = N-1; i>=0; i--){
	int t = tester[i];
	if (t % NUMTHREADS == thr_num){ //allocate sections of mid_array to certain threads
	    mid_array[t*CPAD]--;
	    omp_ans[mid_array[t*CPAD]] = t;
	}
    }

    delete[] tmp_mid;

}
}

void serial_countsort(int *tester, int *serial_ans, int *mid_array){

    //count by index
    for (int i = 0; i<N; i++)
        mid_array[tester[i]]++;

    //pre-fix sums
    for (int i = 1; i<K; i++)
        mid_array[i] += mid_array[i-1];
     
    //the final sort
    int spot = 0;
    int num = 0;
    for (int i = N-1; i>=0; i--){
        num = tester[i];
        spot = mid_array[num];
        serial_ans[spot-1] = num;
        mid_array[num]--;
    }
    
}

void dummy2(int *vals){
    if (vals[0] == 0)
        vals[0] = 0;
}

int main(){

	int i;
	double d_E;
	double d_S;

	//uncomment to use for case testing, or just change globals
	printf("CPAD Size: ");
	std::cin >> CPAD;
	//printf("Num Threads: ");
	//std::cin >> N;

	int *tester = new int[N];
        int *serial_ans = new int[N];
        int *omp_ans = new int[N];
        int *mid_array = new int[K];
	int *mid_array_omp = new int[K*CPAD]; //padding for openMP	

	omp_set_num_threads(NUMTHREADS);

	for (i = 0; i<N; i++){
		tester[i] = rand() % K;
		serial_ans[i] = 0;
		omp_ans[i] = 0;
	}

	for (i = 0; i < K; i++)
		mid_array[i] = 0;

	printf("\nCounting Sort\n");

	get_walltime(&d_S);
	serial_countsort(tester, serial_ans, mid_array);
	get_walltime(&d_E);
	printf("Serial Time: %f\n", (d_E-d_S));

	for (i = 0; i < K*CPAD; i++)
		mid_array_omp[i] = 0;
	
	get_walltime(&d_S);
	omp_countsort(tester, omp_ans, mid_array_omp);
	get_walltime(&d_E);
	printf("OpenMP Time: %f\n", (d_E-d_S));

	printf("Checking equilivance: ");
	bool checker = true;
	for (i = 0; i < N; i++){
		if (serial_ans[i] != omp_ans[i]) checker = false;
	}
	if (checker) printf("Returned lists are equal\n");
	else printf("Oops\n");

	//delete[] tester;
	//delete[] serial_ans;
	//delete[] omp_ans;
	//delete[] mid_array;
	return 0;
}

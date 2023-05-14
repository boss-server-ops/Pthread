#include<iostream>
#include<Windows.h>
#include<emmintrin.h>
#include<fstream>
#include<pthread.h>
using namespace std;
const int N = 2000;
const int Num_Threads=4;
float m[N][N];
typedef struct
{
    int k;
    int t_id;
}threadParam_t;

void *threadFunc(void *param)
{
    threadParam_t *p=(threadParam_t*)param;
    int k=p->k;
    int t_id=p->t_id;


    for(int i=k+t_id+1;i<N;i+=Num_Threads)
    {
        for(int j=k+1;j<N;j++)
        {

            m[i][j]=m[i][j]-m[i][k]*m[k][j];
        }
        m[i][k]=0;
    }
    pthread_exit(NULL);

}

void dynamicLU() {


    for (int k = 0; k < N; k++) {
        for (int j = k + 1; j < N; j++) {
            m[k][j] = m[k][j] / m[k][k];
        }
        m[k][k] = 1.0;

        int worker_count=Num_Threads;
        pthread_t handles[Num_Threads];
        threadParam_t param[Num_Threads];

        for(int t_id=0;t_id<worker_count;t_id++)
        {

            param[t_id].k=k;
            param[t_id].t_id=t_id;
        }
        for(int t_id=0;t_id<worker_count;t_id++)
        {

            pthread_create(handles+t_id, NULL, threadFunc, param+t_id);
        }

        for(int t_id=0;t_id<worker_count;t_id++)
        {
            pthread_join(handles[t_id], NULL);
        }
}
    }


int main()
{
	 ifstream infile("F:\\example.txt");
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{

			infile >> m[i][j];

		}
	}
    infile.close();
    cout<<m[0][1]<<endl;
	long long head, tail, freq; // timers

	 // similar to CLOCKS_PER_SEC
		 QueryPerformanceFrequency((LARGE_INTEGER*)  & freq);
	 // start time
		 QueryPerformanceCounter((LARGE_INTEGER*)& head);
	dynamicLU();
	QueryPerformanceCounter((LARGE_INTEGER*) & tail);
	 cout << "dynamicCol: " << (tail-head) * 1000.0 / freq << "ms" << endl;



}

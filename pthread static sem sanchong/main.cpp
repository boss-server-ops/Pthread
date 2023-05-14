#include<iostream>
#include<Windows.h>
#include<emmintrin.h>
#include<fstream>
#include<pthread.h>
#include<semaphore.h>
using namespace std;
const int N = 2000;
float m[N][N];
const int NUM_THREADS=4;
typedef struct
{
    int t_id;
}threadParam_t;

sem_t sem_leader;
sem_t sem_division[NUM_THREADS-1];
sem_t sem_elimination[NUM_THREADS-1];
void *threadFunc(void *param)
{
    threadParam_t *p=(threadParam_t*)param;
    int t_id=p->t_id;

    for(int k=0;k<N;k++)
    {

        if(t_id==0)
        {
            for (int j = k + 1; j < N; j++)
            {
                m[k][j] = m[k][j] / m[k][k];
            }
            m[k][k] = 1.0;
        }
        else
        {
            sem_wait(&sem_division[t_id-1]);
        }

        if(t_id==0)
        {
            for(int i=0;i<NUM_THREADS-1;i++)
            {
                sem_post(&sem_division[i]);
            }
        }

        for(int i=k+1+t_id;i<N;i+=NUM_THREADS)
        {

            for(int j=k+1;j<N;j++)
            {

                m[i][j]=m[i][j]-m[i][k]*m[k][j];
            }
            m[i][k]=0;
        }
        if(t_id==0)
        {
            for(int i=0;i<NUM_THREADS-1;i++)
            {
                sem_wait(&sem_leader);
            }
            for(int i=0;i<NUM_THREADS-1;i++)
            {
                sem_post(&sem_elimination[i]);
            }
        }
        else
        {
            sem_post(&sem_leader);
            sem_wait(&sem_elimination[t_id-1]);
        }

    }

    pthread_exit(NULL);

}

void staticLU() {


    sem_init(&sem_leader,0,0);
    for(int i=0;i<NUM_THREADS-1;i++)
    {
        sem_init(&sem_division[i],0,0);
        sem_init(&sem_elimination[i],0,0);
    }
    pthread_t handles[NUM_THREADS];
    threadParam_t param[NUM_THREADS];
    for(int t_id=0;t_id<NUM_THREADS;t_id++)
    {
        param[t_id].t_id=t_id;
        pthread_create(handles+t_id,NULL,threadFunc,param+t_id);

    }

    for(int t_id=0;t_id<NUM_THREADS;t_id++)
    {
        pthread_join(handles[t_id], NULL);
    }
    sem_destroy(&sem_leader);
    for(int i=0;i<NUM_THREADS;i++)
    {
        sem_destroy(&sem_division[i]);
        sem_destroy(&sem_elimination[i]);

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
	staticLU();
	QueryPerformanceCounter((LARGE_INTEGER*) & tail);
	 cout << "dynamicCol: " << (tail-head) * 1000.0 / freq << "ms" << endl;



}

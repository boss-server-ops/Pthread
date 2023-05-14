#include<iostream>
#include<Windows.h>
#include<emmintrin.h>
#include<fstream>
#include<pthread.h>
#include<semaphore.h>
using namespace std;
const int N = 2000;
float m[N][N];
const int NUM_THREADS=7;
typedef struct
{
    int t_id;
}threadParam_t;

sem_t sem_main;
sem_t sem_workerstart[NUM_THREADS];
sem_t sem_workerend[NUM_THREADS];
void *threadFunc(void *param)
{
    threadParam_t *p=(threadParam_t*)param;
    __m128 vt, va, vb, vc;
    int t_id=p->t_id;

    for(int k=0;k<N;k++)
    {

        sem_wait(&sem_workerstart[t_id]);
        int i=k+1+t_id;
        int j=0;

        for(;i<N;i+=NUM_THREADS)
        {
            vt = _mm_set_ps1(m[i][k]);
            for (j = k + 1; j + 4 <= N; j += 4)
			{
				va = _mm_loadu_ps(&m[k][j]);
				vb = _mm_loadu_ps(&m[i][j]);
				vc = _mm_mul_ps(vt, va);
				vb = _mm_sub_ps(vb, vc);
				_mm_storeu_ps(&m[i][j], vb);
			}
			while (j < N) {
				m[i][j] = m[i][j] - m[i][k] * m[k][j];
				j++;
			}
			m[i][k] = 0;
        }

        sem_post(&sem_main);
        sem_wait(&sem_workerend[t_id]);
    }

    pthread_exit(NULL);

}

void staticLU() {


    sem_init(&sem_main,0,0);
    for(int i=0;i<NUM_THREADS;i++)
    {
        sem_init(&sem_workerstart[i],0,0);
        sem_init(&sem_workerend[i],0,0);
    }
    pthread_t handles[NUM_THREADS];
    threadParam_t param[NUM_THREADS];
    for(int t_id=0;t_id<NUM_THREADS;t_id++)
    {
        param[t_id].t_id=t_id;
        pthread_create(handles+t_id,NULL,threadFunc,param+t_id);

    }
    __m128 vt, va, vb, vc;
    for (int k = 0; k < N; k++) {
        vt = _mm_set_ps1(m[k][k]);
        int j=0;
        for (j = k + 1; j+4 <= N; j+=4)
        {
            va = _mm_loadu_ps(&m[k][j]);
            va = _mm_div_ps(va, vt);
			_mm_storeu_ps(&m[k][j], va);
        }
        while (j < N) {
			m[k][j] = m[k][j] / m[k][k];
			j++;
		}
        m[k][k] = 1.0;


        for(int t_id=0;t_id<NUM_THREADS;t_id++)
        {
            sem_post(&sem_workerstart[t_id]);
        }
        for(int t_id=0;t_id<NUM_THREADS;t_id++)
        {
            sem_post(&sem_main);

        }
        for (int t_id = 0; t_id < NUM_THREADS; ++t_id)
        {
            sem_post(&sem_workerend[t_id]);
        }
    }
    for(int t_id=0;t_id<NUM_THREADS;t_id++)
    {
        pthread_join(handles[t_id], NULL);
    }
    sem_destroy(&sem_main);
    for(int i=0;i<NUM_THREADS;i++)
    {
        sem_destroy(&sem_workerstart[i]);
        sem_destroy(&sem_workerend[i]);

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

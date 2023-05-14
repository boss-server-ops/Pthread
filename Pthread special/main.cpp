#include<iostream>
#include<fstream>
#include<cmath>
#include<string>
#include<Windows.h>
#include<emmintrin.h>
#include<semaphore.h>
using namespace std;
const int NUM_THREADS=7;

int shouxiang(int integernum, int* beixiaoyuanmatrix, int beixiaoyuannum)
{
	int biaoshi = integernum * 32;//记录遍历到的编号

	for (int j = integernum - 1; j >= 0; j--)
	{
		for (int k = 31; k >= 0; k--)
		{
			biaoshi--;
			/*cout << "j" << j << "k" << k << endl;*/
			bool bit = (beixiaoyuanmatrix[j] >> k) & 1;
			if (bit == 1)
			{
				return biaoshi;
			};

		}

	}
	return 0;


}
typedef struct
{
    int t_id;
    int **beixiaoyuanmatrix;
    int integernum;
    int **xiaoyuanzimatrix;
    bool *biaoji;
    int beixiaoyuannum;
}threadParam_t;
sem_t sem_main;
sem_t sem_workerstart[NUM_THREADS];
sem_t sem_workerend[NUM_THREADS];

void* threadFunc2(void* param)
{

    threadParam_t *p=(threadParam_t*)param;
    int t_id=p->t_id;
    int integernum=p->integernum;
    int **beixiaoyuanmatrix=p->beixiaoyuanmatrix;
    int **xiaoyuanzimatrix=p->xiaoyuanzimatrix;
    int beixiaoyuannum=p->beixiaoyuannum;
    bool *biaoji=p->biaoji;
    for (int i = 0; i < beixiaoyuannum; i++)
    {

        for (int j = 0; j < 5; j++)
        {
            sem_wait(&sem_workerstart[t_id]);

            for (int k = t_id; k < beixiaoyuannum; k+=NUM_THREADS)
            {
                beixiaoyuanmatrix[i][k] ^= xiaoyuanzimatrix[j][k];
            }
            sem_post(&sem_main);
            sem_wait(&sem_workerend[t_id]);

        }
    }
    pthread_exit(NULL);

}
int daoru(ifstream& infile, int** b, int matrixcolnum, int integernum, int** shengge, int beixiaoyuannum)
{
	int i = 0;
	static int hang = matrixcolnum - 1;
	int colnum;
	char c;
	int integerindex;
	int bitindex;
	int cishu = 5;
	for (int j = 0; j < 5; j++)
	{
		for (int k = 0; k < integernum; k++)
		{

			b[j][k] = 0;
		}

	}
	while (cishu > 0)
	{
		cishu--;
		int pos = infile.tellg();
		infile >> colnum;
		/*cout << colnum << endl;*/
		if (colnum == hang)//首项符合要求，导入数据
		{
			infile.seekg(pos);
			while (1)
			{
				/*cout << "shouxiang" << shouxiang(integernum, b[1], 5) << endl;*/
				infile >> colnum;

				integerindex = colnum / 32;
				bitindex = colnum % 32;
				int bitnum = 1 << bitindex;
				b[i][integerindex] |= bitnum;
				infile.get(c);
				if (infile.peek() == '\n')
				{
					infile.ignore();
					i++;
					hang--;
					break;
				}
			}

		}
		else
		{

			infile.seekg(pos);
			for (int j = 0; j < integernum; j++)
			{
				b[i][j] = 0;

			}

			i++;
			hang--;

		}


		if (infile.peek() == EOF)
		{
			return 0;
		}


	}

	return 1;
}

int main()
{
    long long head2, tail2, freq2; // timers

	 // similar to CLOCKS_PER_SEC
	 QueryPerformanceFrequency((LARGE_INTEGER*)&freq2);
	 // start time
	 QueryPerformanceCounter((LARGE_INTEGER*)&head2);
	int matrixcolnum;
	int xiaoyuanzinum;
	int beixiaoyuannum;
	matrixcolnum = 254;
	xiaoyuanzinum = 106;
	beixiaoyuannum = 53;
	cout << "矩阵列数" << matrixcolnum << "消元子" << xiaoyuanzinum << "被消元行" << beixiaoyuannum << endl;

	int integernum = ceil(matrixcolnum / 32.0);

	int** shengge = new int* [beixiaoyuannum];//用来记录被消元行升格后的消元子

	for (int i = 0; i < beixiaoyuannum; i++)
	{
		shengge[i] = new int[integernum];
	}
	for (int i = 0; i < beixiaoyuannum; i++)
	{
		for (int j = 0; j < integernum; j++)
		{
			shengge[i][j] = 0;
		}
	}
	int** beixiaoyuanmatrix = new int* [beixiaoyuannum];
	bool* biaoji = new bool[beixiaoyuannum];
	for (int i = 0; i < beixiaoyuannum; i++)
	{
		biaoji[i] = 0;
	}
	for (int i = 0; i < beixiaoyuannum; i++)
	{
		beixiaoyuanmatrix[i] = new int[integernum];
	}
	for (int i = 0; i < beixiaoyuannum; i++)
	{
		for (int j = 0; j < integernum; j++)
		{
			beixiaoyuanmatrix[i][j] = 0;
		}
	}
	ifstream infile("F:\\并行\\资料\\其他资料\\data\\Groebner\\测试样例2 矩阵列数254，非零消元子106，被消元行53\\被消元行.txt");
	int index = 0;//记录读到了第几行
	int colnum;//读到的数据
	int integerindex;
	int bitindex;
	char c;
	while (1)
	{


		infile >> colnum;
		/*cout << colnum<<' ';*/
		integerindex = colnum / 32;
		bitindex = colnum % 32;
		int bitnum = 1 << bitindex;
		beixiaoyuanmatrix[index][integerindex] |= bitnum;
		infile.get(c);
		if (infile.peek() == '\n')
		{
			infile.ignore();
			index++;
		}
		if (infile.peek() == EOF)
		{
			break;
		}
	}
	infile.close();


	ifstream infile2("F:\\并行\\资料\\其他资料\\data\\Groebner\\测试样例2 矩阵列数254，非零消元子106，被消元行53\\消元子.txt");
	int** xiaoyuanzimatrix = new int* [xiaoyuanzinum];
	for (int i = 0; i < 5; i++)
	{
		xiaoyuanzimatrix[i] = new int[integernum];
	}
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < integernum; j++)
		{
			xiaoyuanzimatrix[i][j] = 0;
		}
	}



	int lastshouxiang = matrixcolnum - 5;//记录此位置之前的消元子的最后一个非0首项




	while (daoru(infile2, xiaoyuanzimatrix, matrixcolnum, integernum, shengge, beixiaoyuannum))
	{

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
            param[t_id].beixiaoyuanmatrix=beixiaoyuanmatrix;
            param[t_id].integernum=integernum;
            param[t_id].xiaoyuanzimatrix=xiaoyuanzimatrix;
            param[t_id].beixiaoyuannum=beixiaoyuannum;
            param[t_id].biaoji=biaoji;
            pthread_create(handles+t_id,NULL,threadFunc2,param+t_id);

        }
		for (int i = 0; i < beixiaoyuannum; i++)
		{
			if (biaoji[i] == 1)
			{
				continue;
			}

			for (int j = 0; j < 5; j++)
			{
				int xyzsx = shouxiang(integernum, xiaoyuanzimatrix[j], 5);//消元子首项
				for (int k = 0; k < beixiaoyuannum; k++)
				{
					if (biaoji[k] == 1 && k != i && shouxiang(integernum, beixiaoyuanmatrix[i], beixiaoyuannum) == shouxiang(integernum, beixiaoyuanmatrix[k], beixiaoyuannum))
					{
						for (int t = 0; t < integernum; t++)
						{
							beixiaoyuanmatrix[i][t] ^= beixiaoyuanmatrix[k][t];
						}
					}
				}
				if(shouxiang(integernum, beixiaoyuanmatrix[i], beixiaoyuannum) ==xyzsx)
				{
					for(int t_id=0;t_id<NUM_THREADS;t_id++)
                    {
                        sem_post(&sem_workerstart[t_id]);
                    }
                    for(int t_id=0;t_id<NUM_THREADS;t_id++)
                    {
                        sem_wait(&sem_main);

                    }
                    for (int t_id = 0; t_id < NUM_THREADS; ++t_id)
                    {
                        sem_post(&sem_workerend[t_id]);
                    }

				}
				else if(xyzsx==0)
				{
					for (int k = 0; k < beixiaoyuannum; k++)
					{
                        if (biaoji[k] == 1 && k != i && shouxiang(integernum, beixiaoyuanmatrix[i], beixiaoyuannum) == shouxiang(integernum, beixiaoyuanmatrix[k], beixiaoyuannum))
						{

							for (int t = 0; t < integernum; t++)
							{
								beixiaoyuanmatrix[i][t] ^= beixiaoyuanmatrix[k][t];
							}
						}

					}

				}
			}

			if (shouxiang(integernum, beixiaoyuanmatrix[i], beixiaoyuannum) > lastshouxiang)
			{
				biaoji[i] = 1;
				/*cout << shouxiang(integernum, beixiaoyuanmatrix[i], beixiaoyuannum) <<"    "<< lastshouxiang <<"   " << "biaoji" << shouxiang(integernum, beixiaoyuanmatrix[i], beixiaoyuannum) << endl;*/
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
        lastshouxiang -= 5;

	}


	cout << "消元结果" << endl;
	for (int i = 0; i < beixiaoyuannum; i++)
	{
		cout << shouxiang(integernum, beixiaoyuanmatrix[i], beixiaoyuannum) << endl;
	}
	infile2.close();
	delete[]xiaoyuanzimatrix;
	delete[]biaoji;
	delete[]beixiaoyuanmatrix;
	QueryPerformanceCounter((LARGE_INTEGER*)&tail2);
    cout << "Col: " << (tail2 - head2) * 1000.0 / freq2 << "ms" << endl;
}

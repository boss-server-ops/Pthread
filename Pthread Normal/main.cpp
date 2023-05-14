#include<iostream>
#include<Windows.h>
#include<emmintrin.h>
#include<fstream>
using namespace std;
const int N = 2000;
float m[N][N];

void serialLU() {
	for (int k = 0; k < N; k++) {
		for (int j = k + 1; j < N; j++) {
			m[k][j] = m[k][j] / m[k][k];
		}
		m[k][k] = 1.0;
		for (int i = k+1; i < N; i++) {
			for (int j = k+1; j < N; j++) {
				m[i][j] = m[i][j] - m[i][k] * m[k][j];
			}
			m[i][k] = 0;
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
			char c;
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
	serialLU();
	QueryPerformanceCounter((LARGE_INTEGER*) & tail);
	 cout << "serialCol: " << (tail-head) * 1000.0 / freq << "ms" << endl;



}

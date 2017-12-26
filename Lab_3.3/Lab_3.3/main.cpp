#include <iostream>///����/����� �� �������
#include <thread>///���������� � ��������
#include <fstream>///����/����� ������
#include <vector>///stl ���������� �������
using namespace std;///����������� ������������ ����
#define Max(a,b) ((a)>(b)?(a):(b))///����������� ������� ���������
#define Min(a,b) ((a)>(b)?(b):(a))// ��������
#define abs(a) ((a) > 0 ? (a): -(a))//������
typedef vector<vector<double>> Matrix;//����������� ���� ������
enum ThreadStates//������������ ��������� ������
{
	Waiting, Ready, Terminate, FindMin, Computing, Back
};

Matrix matrix;
vector<double> inp;//���������� � �������
int thread_cnt;///����� �������
int current_ind;//������ �������� ������

vector<thread> threads;//������ �������
vector<ThreadStates> states;//������ ��������� ������
vector<double> buffer;///������ ���������� ���������
bool b_forward = true;///������� ��� : ������ ��� ��������

void InputMatrix(string path)//������ ���������� �������
{
	ifstream fin(path);
	int n;
	fin >> n;
	for (int i = 0; i < n; i++)
	{
		vector<double> tmp;
		for (int i = 0; i < n; i++)
		{
			int k; fin >> k; tmp.push_back(k);
		}
		matrix.push_back(tmp);
	}
	for (int i = 0; i < n; i++)
	{
		int k; fin >> k; inp.push_back(k);
	}
	fin.close();
}
void OutputVector()//������� ������ ����������
{
	ofstream fout;
	fout.open("result.txt");
	for (int i = 0; i < inp.size(); i++)
		fout << "x" << i << " = " << inp[i] << " ";
	fout.close();
}

void ThreadFunc(int index, int i1 , int i2)///������� ������
{
	while (true)///������� ���� ������
	{
		switch (states[index])//�������� ��������� ������
		{
		case Terminate: return;//����� �� �������
		case Waiting: break;////�������� ������ ��� ������ ����
		case Ready: break;///�������� ������ ��� �������� ����
		case FindMin:///����� ������������ �����
		{
			double min = abs(matrix[i1][current_ind]);
			int res = i1;
			for (int i = Max(i1, current_ind); i < i2; i++)
			{
				if (abs(matrix[i][current_ind]) < min)
				{
					min = matrix[i][current_ind];
					res = i;
				}
			}
			buffer[index] = min;
			states[index] = Ready;
		} break;
		case Computing://������ ����������� ������� �� �������
		{
			for (int i = Max(i1, current_ind + 1); i < i2; i++)
			{
				double koef = matrix[i][current_ind];
				for (int j = current_ind; j < (int)matrix.size(); j++)
					matrix[i][j] -= koef * matrix[current_ind][j];
				inp[i] -= koef * inp[current_ind];
			}
			states[index] = Waiting;
		} break;
		case Back:///�������� ���
		{
			for (int i = Min(current_ind - 1, i2 - 1); i >= i1; i--)
			{
				double koef = matrix[i][current_ind];
				for (int j = (int)matrix.size() - 1; j >= current_ind; j--)
					matrix[i][j] -= koef * matrix[current_ind][j];
				inp[i] -= koef*inp[current_ind];
			}
			states[index] = Waiting;
		} break;
		}
	}
}

int main()
{
	InputMatrix("matrix.txt");///������ �������
	cout << "Enter quantity of threads: ";
	cin >> thread_cnt;//������ ��������� �������
	int str_cnt;//���������� ������� �� ���� �����
	if (thread_cnt > (int)matrix.size())
	{
		thread_cnt = (int)matrix.size();
		str_cnt = 1;
	}
	else str_cnt = (int)matrix.size() / thread_cnt;
	threads.resize(thread_cnt);///�������� ����� � ������ ��� ������
	buffer.resize(thread_cnt);//�������� ��� ������� ������ ����� ������������ �����
	{///������� ������ � ������������ ��� ��� ������� � ��������� ��
		int i1 = 0, i2 = str_cnt;
		current_ind = 0;
		for (int i = 0; i < thread_cnt - 1; i++)
		{
			states.push_back(FindMin);
			threads[i] = thread(ThreadFunc, i, i1, i2);
			i1 += str_cnt;
			i2 += str_cnt;
		}
		states.push_back(FindMin);
		if (thread_cnt != 1) threads[thread_cnt - 1] = thread(ThreadFunc, thread_cnt - 1, i2 - 1, matrix.size());
		else threads[thread_cnt - 1] = thread(ThreadFunc, thread_cnt - 1, 0, matrix.size());
	}
	while (states[0] != Terminate)//�� ��� ���, ���� ��� ������ �� �����������
	{
		///�������� ��������� �������
		bool ready = true;
		for (int i = 0; i < thread_cnt; i++)
			if (states[i] != Ready) { ready = false; break; }
		bool waiting = true;
		for (int i = 0; i < thread_cnt; i++)
			if (states[i] != Waiting) { waiting = false; break; }
		///���� ������ ���
		if (b_forward)
		{
			if (ready)//���� ��� ������ ������, �� ������ ����������� ������ � �������
			{
				int min_ind = 0;
				double min = abs(buffer[0]);
				for (int i = 0; i < thread_cnt; i++)
					if (abs(buffer[i]) < min)
					{
						min = buffer[i];
						min_ind = i;
					}
				if (matrix[min_ind][current_ind] != 0)
				{
					double r = matrix[min_ind][current_ind];
					for (int i = 0; i < (int)matrix.size(); i++)
					{
						matrix[min_ind][i] /= r;
					}
					inp[min_ind] /= r;
				}
				swap(matrix[current_ind], matrix[min_ind]);
				for (int i = 0; i < thread_cnt; i++) states[i] = Computing;
			}
			else if (waiting)//
			{
				current_ind++;
				if (current_ind >= (int)matrix.size() - 1)
				{
					b_forward = false;
					current_ind = matrix.size();
				}
				else for (int i = 0; i < thread_cnt; i++) states[i] = FindMin;
			}
		}
		else//�������� ��� : �������� ������� �� ���� �������
		{
			if (waiting)
			{
				current_ind--;
				if (current_ind >= 0)
				{
					double r = matrix[current_ind][current_ind];
					for (int i = 0; i < (int)matrix.size(); i++)
					{
						matrix[current_ind][i] /= r;
					}
					inp[current_ind] /= r;
					for (int i = 0; i < thread_cnt; i++) states[i] = Back;
				}///��������� ������ � ����������� ��������� � ������� ���������
				else
				{
					for (int i = 0; i < thread_cnt; i++) states[i] = Terminate;
					OutputVector();
				}
			}
		}
	}
	for (int i = 0; i < thread_cnt; i++) if (threads[i].joinable()) threads[i].join();///��������� ��� ������
	system("pause");
	
}




















































































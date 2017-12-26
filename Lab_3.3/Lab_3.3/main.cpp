#include <iostream>///ввод/вывод на консоль
#include <thread>///библиотека с потоками
#include <fstream>///ввод/вывод файлов
#include <vector>///stl библиотека вектора
using namespace std;///стандартное пространство имен
#define Max(a,b) ((a)>(b)?(a):(b))///определение функции максимума
#define Min(a,b) ((a)>(b)?(b):(a))// минимума
#define abs(a) ((a) > 0 ? (a): -(a))//модуля
typedef vector<vector<double>> Matrix;//определение типа Матриц
enum ThreadStates//перечисление состояний потока
{
	Waiting, Ready, Terminate, FindMin, Computing, Back
};

Matrix matrix;
vector<double> inp;//дополнение к матрице
int thread_cnt;///число потоков
int current_ind;//индекс текущего потока

vector<thread> threads;//массив потоков
vector<ThreadStates> states;//массив состояний потока
vector<double> buffer;///массив минимаьных элементов
bool b_forward = true;///текущих ход : прямой или обратный

void InputMatrix(string path)//фунция считывания матрицы
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
void OutputVector()//функция вывода результата
{
	ofstream fout;
	fout.open("result.txt");
	for (int i = 0; i < inp.size(); i++)
		fout << "x" << i << " = " << inp[i] << " ";
	fout.close();
}

void ThreadFunc(int index, int i1 , int i2)///функция потока
{
	while (true)///главный цикл потока
	{
		switch (states[index])//проверка состояния потока
		{
		case Terminate: return;//выйти из функции
		case Waiting: break;////ожидание потока при прямом ходе
		case Ready: break;///ожидание потока при обратном ходе
		case FindMin:///поиск минимального числа
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
		case Computing://отнять минимальную строчку от текущей
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
		case Back:///обратных ход
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
	InputMatrix("matrix.txt");///вводим матрицу
	cout << "Enter quantity of threads: ";
	cin >> thread_cnt;//вводим количетво потоков
	int str_cnt;//количество строчек на один поток
	if (thread_cnt > (int)matrix.size())
	{
		thread_cnt = (int)matrix.size();
		str_cnt = 1;
	}
	else str_cnt = (int)matrix.size() / thread_cnt;
	threads.resize(thread_cnt);///выделяем место в памяти для потока
	buffer.resize(thread_cnt);//выделяем для каждого потока буфер минимального числа
	{///создать потоки и распределить для них строчки и запустить их
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
	while (states[0] != Terminate)//до тех пор, пока все потоки не выключились
	{
		///просмотр состояний потоков
		bool ready = true;
		for (int i = 0; i < thread_cnt; i++)
			if (states[i] != Ready) { ready = false; break; }
		bool waiting = true;
		for (int i = 0; i < thread_cnt; i++)
			if (states[i] != Waiting) { waiting = false; break; }
		///если прямой ход
		if (b_forward)
		{
			if (ready)//если все потоки готовы, мы меняем минимальную строку с текущей
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
		else//обратный ход : отнимаем текущую от всех верхних
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
				}///перевести потоки в завершающее состояние и вывести результат
				else
				{
					for (int i = 0; i < thread_cnt; i++) states[i] = Terminate;
					OutputVector();
				}
			}
		}
	}
	for (int i = 0; i < thread_cnt; i++) if (threads[i].joinable()) threads[i].join();///завершаем все потоки
	system("pause");
	
}




















































































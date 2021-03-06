#include "stdafx.h"
#include <iostream>
#include <cmath>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

using namespace std;

int main() {
	setlocale(LC_ALL, "Russian");
	const int n = 4; //Размерность системы
	float a[n][n + 1], a0[n][n + 1];
	//A - расширенная матрица системы, A0 - ее копия для проверки решения
	int i, j, k;
	float buf, x[n], b[n];
	cout << "Матрица и вектор правой части";
	for (i = 0; i<n; i++) {
		cout << endl;
		for (j = 0; j<n + 1; j++) {
			a0[i][j] = a[i][j] = rand() % 10;
			//Нулевых элементов нет, деления на ноль не будет
			cout << a[i][j] << " ";
		}
	}

	//Прямой ход метода Гаусса
	for (i = 0; i<n - 1; i++)
		for (j = i + 1; j<n; j++) {
			buf = a[i][i] / a[j][i];
			for (k = 0; k <= n; k++) a[j][k] = a[j][k] * buf - a[i][k];
		}
	//Обратный ход метода Гаусса
	x[n - 1] = a[n - 1][n] / a[n - 1][n - 1];
	for (i = n - 2; i >= 0; i--) {
		buf = 0;
		for (j = i + 1; j<n; j++) buf += a[i][j] * x[j];
		x[i] = (a[i][n] - buf) / a[i][i];
	}

	cout << endl << "Решение" << endl;
	for (i = 0; i<n; i++) cout << round(x[i] * 100) / 100<< " ";

	cout << endl << "Проверка" << endl;
	for (i = 0; i<n; i++) {
		b[i] = 0;
		for (j = 0; j<n; j++) b[i] += a0[i][j] * x[j];
		cout << b[i] << " ";
	}
	system("pause");
}
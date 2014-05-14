#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct element {
	int row;
	int col;
}ELEMENT;

int matrix[4][4];
int result = 0;

void getMinor(int a[][3], int row, int col) {
	int i, j;
	int n = 0;
	int m = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (i != row && j != col) {
				a[n][m] = matrix[i][j];
				m++;
				if (m == 3) {
					n++;
					m = 0;
				}
			}
		}
	}
}

void printMatrix() {
	int i, j;
	for (i = 0; i < 4; ++i) {
		for (j = 0; j< 4; ++j) {
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
}


void * det(void * parametr) {
	
	ELEMENT * el = (ELEMENT *) parametr;
	int A[3][3] = {0};
	getMinor(A, el->row, el->col);
	result = result + matrix[el->row][el->col] * (int) pow(-1, el->row + el->col) * (A[0][0]*A[1][1]*A[2][2] + A[0][1]*A[1][2]*A[2][0] +
	A[0][2]*A[1][0]*A[2][1] - A[2][0]*A[1][1]*A[0][2] - A[2][1]*A[1][2]*A[0][0]
	- A[2][2]*A[1][0]*A[0][1]);
}


int main(int argc, char const *argv[])
{
	int i, j;
	for (i = 0; i < 4; ++i) {
		printf("Podaj %d wiersz: ", i+1);
		for (j = 0; j < 4; ++j) {
			scanf("%d", &matrix[i][j]);
		}
	}
	printf("\nMacierz: \n");
	printMatrix();
	
	pthread_t tid[4];

	ELEMENT * el = (ELEMENT *)malloc(sizeof(ELEMENT));
	el->row = 0;
	for (i = 0; i < 4; ++i) {
		el->col = i;
		pthread_create(&tid[i], NULL, det, (void *)el);
		pthread_join(tid[i], NULL);
	}

	printf("\n");
	printf("Wyznacznik: %d\n", result);
	free(el);
	return 0;
}
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

#define N 2048 // matrix order 2048
#define FactorIntToDouble 1.1

double firstMatrix[N][N] = {0.0};
double secondMatrix[N][N] = {0.0};
double matrixMultiResult[N][N] = {0.0};

void matrixInit() {
#pragma omp parallel for
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
           srand(row+col);

            firstMatrix[row][col] = (rand() % 10) * FactorIntToDouble;
            secondMatrix[row][col] = (rand() % 10) * FactorIntToDouble;
        }
    }
}





void smallMatrixMult(int upperOfRow, int bottomOfRow, int leftOfCol, int rightOfCol, int transLeft, int transRight) {
   #pragma omp parallel for collapse(2)

    for (int i = upperOfRow; i <= bottomOfRow; i++) {
        for (int j = leftOfCol; j <= rightOfCol; j++) {
        
        
        
        
            for (int k = transLeft; k <= transRight; k++) {
                matrixMultiResult[i][j] += firstMatrix[i][k] * secondMatrix[k][j];	//store result by threads
            }
        }
    }
    
    
}





//if the matrix is too big, just seperate it into smaller ones
void matrixMulti(int upperOfRow, int bottomOfRow, int leftOfCol, int rightOfCol, int transLeft, int transRight) {

    if ((bottomOfRow - upperOfRow) < 128) {  // Adjust this threshold as needed 2 ^ 7

        smallMatrixMult(upperOfRow, bottomOfRow, leftOfCol, rightOfCol, transLeft, transRight); //smallmult function call
    } 

else {
#pragma omp task
        matrixMulti(upperOfRow, (upperOfRow + bottomOfRow) / 2, leftOfCol, (leftOfCol + rightOfCol) / 2, transLeft, transRight);
#pragma omp task
        matrixMulti(upperOfRow, (upperOfRow + bottomOfRow) / 2, leftOfCol, (leftOfCol + rightOfCol) / 2, (transLeft + transRight) / 2 + 1, transRight);
#pragma omp task
        matrixMulti(upperOfRow, (upperOfRow + bottomOfRow) / 2, (leftOfCol + rightOfCol) / 2 + 1, rightOfCol, transLeft, (transLeft + transRight) / 2);
#pragma omp task
        matrixMulti(upperOfRow, (upperOfRow + bottomOfRow) / 2, (leftOfCol + rightOfCol) / 2 + 1, rightOfCol, (transLeft + transRight) / 2 + 1, transRight);
#pragma omp taskwait
    }
}



int main() {
    matrixInit();

    double t1 = omp_get_wtime();

        matrixMulti(0, N - 1, 0, N - 1, 0, N - 1);
    
    double t2 = omp_get_wtime();
    printf("Parallel execution time: %f seconds\n", t2 - t1);

    return 0;
}

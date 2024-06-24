#pragma once

typedef struct
{
    double **matrix;
    int row;
    int col;
    char alias[50];
} Matrix;

Matrix *multiplyMatrixByMatrix(Matrix *mA, Matrix *mB);
Matrix *multiplyMatrixByScalar(double scalar, Matrix *m);
double findDeterminant(Matrix *m);
void printMatrix(Matrix *m);
Matrix *getMatrixMinor(Matrix *m, int rowToRemove, int colToRemove);
Matrix *findInverse(Matrix *m);
Matrix *divideMatrixByScalar(double scalar, Matrix *m);
Matrix *transposeMatrix(Matrix *m);
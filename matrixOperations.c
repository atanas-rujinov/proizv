#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrixOperations.h"

Matrix *multiplyMatrixByMatrix(Matrix *mA, Matrix *mB)
{
    if (mA->col != mB->row)
    {
        printf("Cannot multiply matrices\n");
        return NULL;
    }

    Matrix *result = (Matrix *)malloc(sizeof(Matrix));
    result->row = mA->row;
    result->col = mB->col;
    result->matrix = (double **)malloc(result->row * sizeof(double *));
    for (int i = 0; i < result->row; i++)
    {
        result->matrix[i] = (double *)malloc(result->col * sizeof(double));
    }

    for (int i = 0; i < result->row; i++)
    {
        for (int j = 0; j < result->col; j++)
        {
            result->matrix[i][j] = 0;
            for (int k = 0; k < mA->col; k++)
            {
                result->matrix[i][j] += mA->matrix[i][k] * mB->matrix[k][j];
            }
        }
    }

    return result;
}

Matrix *multiplyMatrixByScalar(double scalar, Matrix *m)
{
    Matrix *result = (Matrix *)malloc(sizeof(Matrix));
    result->row = m->row;
    result->col = m->col;
    result->matrix = (double **)malloc(result->row * sizeof(double *));
    for (int i = 0; i < result->row; i++)
    {
        result->matrix[i] = (double *)malloc(result->col * sizeof(double));
    }

    for (int i = 0; i < result->row; i++)
    {
        for (int j = 0; j < result->col; j++)
        {
            result->matrix[i][j] = m->matrix[i][j] * scalar;
        }
    }

    return result;
}

double findDeterminant(Matrix *m)
{
    if (m->row != m->col)
    {
        printf("Cannot find determinant\n");
        return 0;
    }

    if (m->row > 4)
    {
        printf("Calculator is limited to 4x4 matrix\n");
        return 0;
    }

    if (m->row == 1)
    {
        return m->matrix[0][0];
    }

    if (m->row == 2)
    {
        return m->matrix[0][0] * m->matrix[1][1] - m->matrix[0][1] * m->matrix[1][0];
    }

    double determinant = 0;
    for (int i = 0; i < m->row; i++)
    {
        Matrix *subMatrix = (Matrix *)malloc(sizeof(Matrix));
        subMatrix->row = m->row - 1;
        subMatrix->col = m->col - 1;
        subMatrix->matrix = (double **)malloc(subMatrix->row * sizeof(double *));
        for (int j = 0; j < subMatrix->row; j++)
        {
            subMatrix->matrix[j] = (double *)malloc(subMatrix->col * sizeof(double));
        }

        for (int j = 1; j < m->row; j++)
        {
            int k = 0;
            for (int l = 0; l < m->col; l++)
            {
                if (l == i)
                    continue;
                subMatrix->matrix[j - 1][k] = m->matrix[j][l];
                k++;
            }
        }

        if (i % 2 == 0)
        {
            determinant += m->matrix[0][i] * findDeterminant(subMatrix);
        }
        else
        {
            determinant -= m->matrix[0][i] * findDeterminant(subMatrix);
        }

        for (int j = 0; j < subMatrix->row; j++)
        {
            free(subMatrix->matrix[j]);
        }
        free(subMatrix->matrix);
        free(subMatrix);
    }

    return determinant;
}

void printMatrix(Matrix *m)
{
    for (int i = 0; i < m->row; i++)
    {
        for (int j = 0; j < m->col; j++)
        {
            printf("%lf ", m->matrix[i][j]);
        }
        printf("\n");
    }
}

Matrix *getMatrixMinor(Matrix *m, int rowToRemove, int colToRemove)
{
    Matrix *minor = (Matrix *)malloc(sizeof(Matrix));
    minor->row = m->row - 1;
    minor->col = m->col - 1;
    minor->matrix = (double **)malloc(minor->row * sizeof(double *));
    for (int i = 0; i < minor->row; i++)
    {
        minor->matrix[i] = (double *)malloc(minor->col * sizeof(double));
    }

    int minorRow = 0, minorCol = 0;
    for (int i = 0; i < m->row; i++)
    {
        if (i == rowToRemove)
            continue;
        minorCol = 0;
        for (int j = 0; j < m->col; j++)
        {
            if (j == colToRemove)
                continue;
            minor->matrix[minorRow][minorCol] = m->matrix[i][j];
            minorCol++;
        }
        minorRow++;
    }

    return minor;
}

Matrix *findInverse(Matrix *m)
{
    double det = findDeterminant(m);
    if (det == 0)
    {
        printf("Matrix is not invertible\n");
        return NULL;
    }

    Matrix *inverse = (Matrix *)malloc(sizeof(Matrix));
    inverse->row = m->row;
    inverse->col = m->col;
    inverse->matrix = (double **)malloc(inverse->row * sizeof(double *));
    for (int i = 0; i < inverse->row; i++)
    {
        inverse->matrix[i] = (double *)malloc(inverse->col * sizeof(double));
    }

    if (m->row == 1)
    {
        inverse->matrix[0][0] = 1 / m->matrix[0][0];
        return inverse;
    }

    Matrix *cofactorMatrix = (Matrix *)malloc(sizeof(Matrix));
    cofactorMatrix->row = m->row;
    cofactorMatrix->col = m->col;
    cofactorMatrix->matrix = (double **)malloc(cofactorMatrix->row * sizeof(double *));
    for (int i = 0; i < cofactorMatrix->row; i++)
    {
        cofactorMatrix->matrix[i] = (double *)malloc(cofactorMatrix->col * sizeof(double));
    }

    for (int i = 0; i < m->row; i++)
    {
        for (int j = 0; j < m->col; j++)
        {
            Matrix *minor = getMatrixMinor(m, i, j);
            double minorDet = findDeterminant(minor);
            cofactorMatrix->matrix[i][j] = (i + j) % 2 == 0 ? minorDet : -minorDet;

            for (int k = 0; k < minor->row; k++)
            {
                free(minor->matrix[k]);
            }
            free(minor->matrix);
            free(minor);
        }
    }

    for (int i = 0; i < m->row; i++)
    {
        for (int j = 0; j < m->col; j++)
        {
            inverse->matrix[i][j] = cofactorMatrix->matrix[j][i] / det;
        }
    }

    for (int i = 0; i < cofactorMatrix->row; i++)
    {
        free(cofactorMatrix->matrix[i]);
    }
    free(cofactorMatrix->matrix);
    free(cofactorMatrix);

    return inverse;
}

Matrix *divideMatrixByScalar(double scalar, Matrix *m)
{
    Matrix *result = (Matrix *)malloc(sizeof(Matrix));
    result->row = m->row;
    result->col = m->col;
    result->matrix = (double **)malloc(result->row * sizeof(double *));
    for (int i = 0; i < result->row; i++)
    {
        result->matrix[i] = (double *)malloc(result->col * sizeof(double));
    }

    for (int i = 0; i < result->row; i++)
    {
        for (int j = 0; j < result->col; j++)
        {
            result->matrix[i][j] = m->matrix[i][j] / scalar;
        }
    }

    return result;
}

Matrix *transposeMatrix(Matrix *m)
{
    Matrix *result = (Matrix *)malloc(sizeof(Matrix));
    result->row = m->col;
    result->col = m->row;
    result->matrix = (double **)malloc(result->row * sizeof(double *));
    for (int i = 0; i < result->row; i++)
    {
        result->matrix[i] = (double *)malloc(result->col * sizeof(double));
    }

    for (int i = 0; i < result->row; i++)
    {
        for (int j = 0; j < result->col; j++)
        {
            result->matrix[i][j] = m->matrix[j][i];
        }
    }

    return result;
}

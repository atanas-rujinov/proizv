#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrixOperations.h"

#define MAX_MATRICES 100

char *stringifyMatrix(Matrix *m)
{
    int bufferSize = m->row * m->col * 100 + m->row + 1;
    char *buffer = (char *)malloc(bufferSize * sizeof(char));
    buffer[0] = '\0';

    for (int i = 0; i < m->row; i++)
    {
        for (int j = 0; j < m->col; j++)
        {
            char temp[100];
            sprintf(temp, "%lf ", m->matrix[i][j]);
            strcat(buffer, temp);
        }
        strcat(buffer, "\n");
    }

    return buffer;
}

void xorStringWithKey(char *str, const char *key)
{
    int strLen = strlen(str);
    int keyLen = strlen(key);

    for (int i = 0; i < strLen; i++)
    {
        str[i] ^= key[i % keyLen];
    }
}

Matrix *loadEncryptedMatrixFromFile(const char *filename, const char *key)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Could not open file %s\n", filename);
        return NULL;
    }

    char buffer[1024];
    fread(buffer, sizeof(char), 1024, file);
    fclose(file);

    xorStringWithKey(buffer, key);

    char *token = strtok(buffer, " \n");
    int row = atoi(token);
    token = strtok(NULL, " \n");
    int col = atoi(token);

    Matrix *m = (Matrix *)malloc(sizeof(Matrix));
    m->row = row;
    m->col = col;
    m->matrix = (double **)malloc(m->row * sizeof(double *));
    for (int i = 0; i < m->row; i++)
    {
        m->matrix[i] = (double *)malloc(m->col * sizeof(double));
    }

    for (int i = 0; i < m->row; i++)
    {
        for (int j = 0; j < m->col; j++)
        {
            token = strtok(NULL, " \n");
            m->matrix[i][j] = atof(token);
        }
    }

    return m;
}

Matrix *inputMatrixFromConsole()
{
    Matrix *m = (Matrix *)malloc(sizeof(Matrix));
    printf("Enter the number of rows: ");
    scanf("%d", &(m->row));
    printf("Enter the number of columns: ");
    scanf("%d", &(m->col));

    m->matrix = (double **)malloc(m->row * sizeof(double *));
    for (int i = 0; i < m->row; i++)
    {
        m->matrix[i] = (double *)malloc(m->col * sizeof(double));
    }

    printf("Enter the elements of the matrix:\n");
    for (int i = 0; i < m->row; i++)
    {
        for (int j = 0; j < m->col; j++)
        {
            scanf("%lf", &(m->matrix[i][j]));
        }
    }

    return m;
}

void saveEncryptedMatrixToFile(const char *filename, const char *encryptedMatrix)
{
    FILE *file = fopen(filename, "w");
    if (file)
    {
        fwrite(encryptedMatrix, sizeof(char), strlen(encryptedMatrix), file);
        fclose(file);
    }
    else
    {
        printf("Could not open file %s for writing\n", filename);
    }
}

void saveMatrixToFile(const char *filename, Matrix *m, const char *key)
{
    char *matrixString = stringifyMatrix(m);

    char dimensions[100];
    sprintf(dimensions, "%d %d\n", m->row, m->col);
    char *fullMatrixString = (char *)malloc((strlen(dimensions) + strlen(matrixString) + 1) * sizeof(char));
    strcpy(fullMatrixString, dimensions);
    strcat(fullMatrixString, matrixString);

    xorStringWithKey(fullMatrixString, key);
    saveEncryptedMatrixToFile(filename, fullMatrixString);

    free(matrixString);
    free(fullMatrixString);
}

int main()
{
    int choice;
    Matrix matrices[MAX_MATRICES];
    int matrixCount = 0;
    const char *key = "key";

    while (1)
    {
        printf("\n");
        printf("Choose an option:\n");
        printf("1. Load matrix from file\n");
        printf("2. Perform operation with matrices\n");
        printf("3. Enter a new matrix\n");
        printf("4. See all matrices\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1)
        {
            if (matrixCount >= MAX_MATRICES)
            {
                printf("Matrix storage limit reached\n");
                continue;
            }

            char filename[100];
            char alias[50];
            printf("Enter the path to the file: ");
            scanf("%s", filename);
            printf("Enter the alias for the matrix: ");
            scanf("%s", alias);

            Matrix *matrix = loadEncryptedMatrixFromFile(filename, key);
            if (matrix == NULL)
            {
                continue;
            }

            strcpy(matrix->alias, alias);
            matrices[matrixCount] = *matrix;
            matrixCount++;

            printf("Matrix loaded successfully\n");
        }
        else if (choice == 2)
        {
            printf("Choose an operation:\n");
            printf("1. Multiply two matrices\n");
            printf("2. Multiply matrix by scalar\n");
            printf("3. Find determinant\n");
            printf("4. Find inverse\n");
            printf("5. Divide matrix by scalar\n");
            printf("6. Transpose matrix\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            if (choice == 1)
            {
                char aliasA[50], aliasB[50];
                printf("Enter the alias of the first matrix: ");
                scanf("%s", aliasA);
                printf("Enter the alias of the second matrix: ");
                scanf("%s", aliasB);

                Matrix *matrixA = NULL, *matrixB = NULL;
                for (int i = 0; i < matrixCount; i++)
                {
                    if (strcmp(matrices[i].alias, aliasA) == 0)
                    {
                        matrixA = &matrices[i];
                    }
                    if (strcmp(matrices[i].alias, aliasB) == 0)
                    {
                        matrixB = &matrices[i];
                    }
                }

                if (matrixA == NULL || matrixB == NULL)
                {
                    printf("Matrix not found\n");
                    continue;
                }

                Matrix *result = multiplyMatrixByMatrix(matrixA, matrixB);
                if (result == NULL)
                {
                    printf("Matrix multiplication failed\n");
                    continue;
                }

                printf("Result:\n");
                printMatrix(result);

                printf("Do you want to save the result? (1 for yes, 0 for no): ");
                scanf("%d", &choice);
                if (choice == 1)
                {
                    char resultFilename[100];
                    printf("Enter the result file name: ");
                    scanf("%s", resultFilename);
                    saveMatrixToFile(resultFilename, result, key);
                }

                for (int i = 0; i < result->row; i++)
                {
                    free(result->matrix[i]);
                }
                free(result->matrix);
                free(result);
            }
            else if (choice == 2)
            {
                char aliasA[50];
                double scalar;
                printf("Enter the alias of the matrix: ");
                scanf("%s", aliasA);
                printf("Enter the scalar value: ");
                scanf("%lf", &scalar);

                Matrix *matrixA = NULL;
                for (int i = 0; i < matrixCount; i++)
                {
                    if (strcmp(matrices[i].alias, aliasA) == 0)
                    {
                        matrixA = &matrices[i];
                        break;
                    }
                }

                if (matrixA == NULL)
                {
                    printf("Matrix not found\n");
                    continue;
                }

                Matrix *result = multiplyMatrixByScalar(scalar, matrixA);
                if (result == NULL)
                {
                    printf("Matrix multiplication by scalar failed\n");
                    continue;
                }

                printf("Result:\n");
                printMatrix(result);

                printf("Do you want to save the result? (1 for yes, 0 for no): ");
                scanf("%d", &choice);
                if (choice == 1)
                {
                    char resultFilename[100];
                    printf("Enter the result file name: ");
                    scanf("%s", resultFilename);
                    saveMatrixToFile(resultFilename, result, key);
                }

                for (int i = 0; i < result->row; i++)
                {
                    free(result->matrix[i]);
                }
                free(result->matrix);
                free(result);
            }
            else if (choice == 3)
            {
                char aliasA[50];
                printf("Enter the alias of the matrix: ");
                scanf("%s", aliasA);

                Matrix *matrixA = NULL;
                for (int i = 0; i < matrixCount; i++)
                {
                    if (strcmp(matrices[i].alias, aliasA) == 0)
                    {
                        matrixA = &matrices[i];
                        break;
                    }
                }

                if (matrixA == NULL)
                {
                    printf("Matrix not found\n");
                    continue;
                }

                double determinant = findDeterminant(matrixA);
                printf("Determinant: %lf\n", determinant);
            }
            else if (choice == 4)
            {
                char aliasA[50];
                printf("Enter the alias of the matrix: ");
                scanf("%s", aliasA);

                Matrix *matrixA = NULL;
                for (int i = 0; i < matrixCount; i++)
                {
                    if (strcmp(matrices[i].alias, aliasA) == 0)
                    {
                        matrixA = &matrices[i];
                        break;
                    }
                }

                if (matrixA == NULL)
                {
                    printf("Matrix not found\n");
                    continue;
                }

                Matrix *result = findInverse(matrixA);
                if (result == NULL)
                {
                    printf("Matrix inversion failed\n");
                    continue;
                }

                printf("Result:\n");
                printMatrix(result);

                printf("Do you want to save the result? (1 for yes, 0 for no): ");
                scanf("%d", &choice);
                if (choice == 1)
                {
                    char resultFilename[100];
                    printf("Enter the result file name: ");
                    scanf("%s", resultFilename);
                    saveMatrixToFile(resultFilename, result, key);
                }

                for (int i = 0; i < result->row; i++)
                {
                    free(result->matrix[i]);
                }
                free(result->matrix);
                free(result);
            }
            else if (choice == 5)
            {
                char aliasA[50];
                double scalar;
                printf("Enter the alias of the matrix: ");
                scanf("%s", aliasA);
                printf("Enter the scalar value: ");
                scanf("%lf", &scalar);

                Matrix *matrixA = NULL;
                for (int i = 0; i < matrixCount; i++)
                {
                    if (strcmp(matrices[i].alias, aliasA) == 0)
                    {
                        matrixA = &matrices[i];
                        break;
                    }
                }

                if (matrixA == NULL)
                {
                    printf("Matrix not found\n");
                    continue;
                }

                Matrix *result = divideMatrixByScalar(scalar, matrixA);
                if (result == NULL)
                {
                    printf("Matrix division by scalar failed\n");
                    continue;
                }

                printf("Result:\n");
                printMatrix(result);

                printf("Do you want to save the result? (1 for yes, 0 for no): ");
                scanf("%d", &choice);
                if (choice == 1)
                {
                    char resultFilename[100];
                    printf("Enter the result file name: ");
                    scanf("%s", resultFilename);
                    saveMatrixToFile(resultFilename, result, key);
                }

                for (int i = 0; i < result->row; i++)
                {
                    free(result->matrix[i]);
                }
                free(result->matrix);
                free(result);
            }
            else if (choice == 6)
            {
                char aliasA[50];
                printf("Enter the alias of the matrix: ");
                scanf("%s", aliasA);

                Matrix *matrixA = NULL;
                for (int i = 0; i < matrixCount; i++)
                {
                    if (strcmp(matrices[i].alias, aliasA) == 0)
                    {
                        matrixA = &matrices[i];
                        break;
                    }
                }

                if (matrixA == NULL)
                {
                    printf("Matrix not found\n");
                    continue;
                }

                Matrix *result = transposeMatrix(matrixA);
                if (result == NULL)
                {
                    printf("Matrix transposition failed\n");
                    continue;
                }

                printf("Result:\n");
                printMatrix(result);

                printf("Do you want to save the result? (1 for yes, 0 for no): ");
                scanf("%d", &choice);
                if (choice == 1)
                {
                    char resultFilename[100];
                    printf("Enter the result file name: ");
                    scanf("%s", resultFilename);
                    saveMatrixToFile(resultFilename, result, key);
                }

                for (int i = 0; i < result->row; i++)
                {
                    free(result->matrix[i]);
                }
                free(result->matrix);
                free(result);
            }
            else
            {
                printf("Invalid choice\n");
            }
        }
        else if (choice == 3)
        {
            if (matrixCount >= MAX_MATRICES)
            {
                printf("Matrix storage limit reached\n");
                continue;
            }

            char alias[50];
            printf("Enter the alias for the matrix: ");
            scanf("%s", alias);

            Matrix *matrix = inputMatrixFromConsole();
            strcpy(matrix->alias, alias);
            matrices[matrixCount] = *matrix;
            matrixCount++;

            printf("Matrix entered successfully\n");

            printf("Do you want to save the matrix? (1 for yes, 0 for no): ");
            scanf("%d", &choice);
            if (choice == 1)
            {
                char filename[100];
                printf("Enter the file name: ");
                scanf("%s", filename);
                saveMatrixToFile(filename, matrix, key);
            }
        }
        else if (choice == 4)
        {
            for (int i = 0; i < matrixCount; i++)
            {
                printf("Matrix %d (%s):\n", i + 1, matrices[i].alias);
                printMatrix(&matrices[i]);
            }
        }
        else if (choice == 5)
        {
            break;
        }
        else
        {
            printf("Invalid choice\n");
        }
    }

    for (int i = 0; i < matrixCount; i++)
    {
        for (int j = 0; j < matrices[i].row; j++)
        {
            free(matrices[i].matrix[j]);
        }
        free(matrices[i].matrix);
    }

    return 0;
}
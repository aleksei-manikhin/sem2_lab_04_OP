#include "csv_reader.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Status csvResizeLine(char** line, size_t* capacity);
Status csvReadLine(FILE* file, char** line, size_t* capacity, int* hasLine);
int csvIsSpace(char symbol);
int csvIsEmptyLine(const char* line);
void csvSkipSpaces(const char** current);
Status csvParseNumber(const char** current, double* value);
Status csvParseRow(const char* line, double* values, size_t expected, size_t* columns);
Status csvInspectFile(FILE* file, size_t* rows, size_t* columns);
Status csvFillGrid(FILE* file, Grid* grid);
Status csvLoadGrid(FILE* file, Grid* grid);

Status csvReadGrid(const char* filePath, Grid* grid)
{
    Status status = OK;
    FILE* file = NULL;
    Grid loadedGrid;

    gridInit(&loadedGrid);
    if (filePath == NULL || grid == NULL)
        status = NULL_POINTER;
    else {
        file = fopen(filePath, "r");
        if (file == NULL)
            status = FILE_OPEN_ERROR;
        else {
            status = csvLoadGrid(file, &loadedGrid);
            fclose(file);
        }
    }
    if (status == OK) {
        gridFree(grid);
        *grid = loadedGrid;
    } else
        gridFree(&loadedGrid);

    return status;
}

Status csvResizeLine(char** line, size_t* capacity)
{
    Status status = OK;
    size_t newCapacity = *capacity == 0
                             ? CSV_INITIAL_LINE_CAPACITY
                             : *capacity * 2;
    char* resizedLine = NULL;

    if (newCapacity <= *capacity)
        status = MEMORY_ERROR;
    else {
        resizedLine = realloc(*line, newCapacity);
        if (resizedLine == NULL)
            status = MEMORY_ERROR;
        else {
            *line = resizedLine;
            *capacity = newCapacity;
        }
    }

    return status;
}

Status csvReadLine(FILE* file, char** line, size_t* capacity, int* hasLine)
{
    Status status = OK;
    size_t length = 0;
    int symbol = fgetc(file);

    *hasLine = symbol != EOF;
    while (status == OK && symbol != EOF && symbol != '\n') {
        if (length + 1 >= *capacity)
            status = csvResizeLine(line, capacity);
        if (status == OK) {
            (*line)[length++] = (char)symbol;
            symbol = fgetc(file);
        }
    }
    if (status == OK && *hasLine) {
        if (length + 1 >= *capacity)
            status = csvResizeLine(line, capacity);
        if (status == OK)
            (*line)[length] = '\0';
    }

    return status;
}

int csvIsSpace(char symbol)
{
    return symbol == ' ' || symbol == '\t'
           || symbol == '\r' || symbol == '\n'
           || symbol == '\v' || symbol == '\f';
}

int csvIsEmptyLine(const char* line)
{
    int isEmpty = 1;

    while (*line != '\0' && isEmpty) {
        if (!csvIsSpace(*line))
            isEmpty = 0;
        line++;
    }

    return isEmpty;
}

void csvSkipSpaces(const char** current)
{
    while (csvIsSpace(**current))
        (*current)++;
}

Status csvParseNumber(const char** current, double* value)
{
    Status status = OK;
    char* numberEnd = NULL;

    csvSkipSpaces(current);
    *value = strtod(*current, &numberEnd);
    if (numberEnd == *current || !isfinite(*value))
        status = INVALID_NUMBER;
    else {
        *current = numberEnd;
        csvSkipSpaces(current);
    }

    return status;
}

Status csvParseRow(const char* line, double* values, size_t expected, size_t* columns)
{
    Status status = OK;
    const char* current = line;
    double value = 0.0;

    *columns = 0;
    while (status == OK && *current != '\0') {
        status = csvParseNumber(&current, &value);
        if (status == OK && values != NULL && *columns < expected)
            values[*columns] = value;
        (*columns)++;
        if (status == OK && *current != '\0') {
            if (*current == ',' || *current == ';') {
                current++;
                if (*current == '\0')
                    status = INVALID_NUMBER;
            } else
                status = INVALID_NUMBER;
        }
    }
    if (status == OK && (expected != 0 && *columns != expected))
        status = INVALID_ROW_SIZE;

    return status;
}

Status csvInspectFile(FILE* file, size_t* rows, size_t* columns)
{
    Status status = OK;
    char* line = NULL;
    size_t capacity = 0;
    size_t currentColumns = 0;
    int hasLine = 1;

    while (status == OK && hasLine) {
        status = csvReadLine(file, &line, &capacity, &hasLine);
        if (status == OK && hasLine && !csvIsEmptyLine(line)) {
            status = csvParseRow(line, NULL, 0, &currentColumns);
            if (status == OK && *columns == 0)
                *columns = currentColumns;
            else if (status == OK && currentColumns != *columns)
                status = INVALID_ROW_SIZE;
            if (status == OK)
                (*rows)++;
        }
    }
    free(line);
    if (status == OK && *rows == 0)
        status = EMPTY_FILE;

    return status;
}

Status csvFillGrid(FILE* file, Grid* grid)
{
    Status status = OK;
    char* line = NULL;
    size_t capacity = 0;
    size_t columns = 0;
    size_t row = 0;
    int hasLine = 1;

    while (status == OK && hasLine) {
        status = csvReadLine(file, &line, &capacity, &hasLine);
        if (status == OK && hasLine && !csvIsEmptyLine(line)) {
            status = csvParseRow(line, grid->values + row * grid->columns,
                                 grid->columns, &columns);
            if (status == OK)
                row++;
        }
    }
    free(line);

    return status;
}

Status csvLoadGrid(FILE* file, Grid* grid)
{
    Status status = OK;
    size_t rows = 0;
    size_t columns = 0;

    status = csvInspectFile(file, &rows, &columns);
    if (status == OK)
        status = gridAllocate(grid, rows, columns);
    if (status == OK) {
        rewind(file);
        status = csvFillGrid(file, grid);
    }

    return status;
}

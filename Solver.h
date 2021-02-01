
#pragma once
#include <iostream>
#include "Matrix.h"
//#include "Matrix.cpp"

template <class T>
class Solver
{
public:
    Matrix<T> *A = nullptr;
    Matrix<T> *b = nullptr;
    int b_length = -1;
    // constructor where we want to preallocate ourselves
    Solver(Matrix<T> *A, Matrix<T> *b, int b_length);
    // constructor where we already have allocated memory outside
    //Solver(Matrix LHS, int cols, T *values_ptr);

    // destructor
    virtual ~Solver();

    void stationaryIterative(Matrix<T> &x, double &tol, int &it_max, bool isGaussSeidel);

    void lu_solve(Matrix<T> &x);
};
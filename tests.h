#include <iostream>
#include <math.h>
#include <chrono>
#include <vector>
#include <memory>
#include "Matrix.h"
#include "Matrix.cpp"
#include "CSRMatrix.h"
#include "CSRMatrix.cpp"
#include "Solver.h"
#include "Solver.cpp"
#include "SparseSolver.h"
#include "SparseSolver.cpp"
#include "TestRunner.h"
#include "utilities.h"
#include <memory>

bool test_residual_calculation()
{
    int size = 4;

    std::shared_ptr<double[]> values(new double[size * size]{1., 2., 3., 4., -4., -3., -2., -1., 0., 1., 2., 0., 1., 2., 3., 4.});

    Matrix<double> m = Matrix<double>(size, size, values);

    std::vector<double> b{4., -6., -4., 4.};
    std::vector<double> x{1., 1., -2., 2.};
    std::vector<double> b_output(size, 0);

    Solver<double> solver = Solver<double>(m, b);
    double res = solver.residualCalc(x, b_output);

    double expected = 2.;

    return res == expected;
}

bool test_mat_vec_mult()
{
    int size = 4;

    std::shared_ptr<int[]> values(new int[size * size]{1, 2, 3, 4, -4, -3, -2, -1, 0, 1, 2, 0, 1, 2, 3, 4});

    Matrix<int> m = Matrix<int>(size, size, values);

    std::vector<int> v{1, 2, -3, 2};
    std::vector<int> result(size, 0);

    m.matVecMult(v, result);

    std::vector<int> expected{4, -6, -4, 4};

    for (int i = 0; i < size; i++)
    {
        if (result[i] != expected[i])
        {
            TestRunner::testError("Result doesn't match expected values");
            return false;
        }
    }
    return true;
}

// test functions should start with 'test_' prefix
bool test_sparse_matmatmult_5x5()
{
    int nnzs = 14;
    int size = 5;

    std::shared_ptr<int[]> init_row_position1(new int[size + 1]{0, 2, 4, 7, 11, 14});
    std::shared_ptr<int[]> init_col_index1(new int[nnzs]{0, 4, 0, 1, 1, 2, 3, 0, 2, 3, 4, 1, 3, 4});
    std::shared_ptr<double[]> init_sparse_values1(new double[nnzs]{10, -2, 3, 9, 7, 8, 7, 3, 8, 7, 5, 8, 9, 13});
    CSRMatrix<double> sparse_matrix = CSRMatrix<double>(size, size, nnzs, init_sparse_values1, init_row_position1, init_col_index1);

    auto result = sparse_matrix.matMatMult(sparse_matrix);

    double expected_values[] = {100, -16, -18, -46, 57, 81, -6, 42, 119, 120, 105, 35, 51, 96, 120, 150, 94, 51, 176, 72, 180, 214};
    int expected_nnzs = 22;

    if (expected_nnzs != result->nnzs)
    {
        std::cerr << "nnzs do not match" << std::endl;
        return false;
    }
    bool outcome = TestRunner::assertArrays(&expected_values[0], &result->values[0], expected_nnzs);
    return outcome;
}

bool test_sparse_matmatmult_4x4()
{
    int nnzs = 4;
    int size = 4;

    std::shared_ptr<int[]> init_row_position1(new int[size + 1]{0, 2, 3, 4, 4});
    std::shared_ptr<int[]> init_col_index1(new int[nnzs]{1, 3, 0, 1});
    std::shared_ptr<double[]> init_sparse_values1(new double[nnzs]{1, 1, 1, 2});
    CSRMatrix<double> sparse_matrix1 = CSRMatrix<double>(size, size, nnzs, init_sparse_values1, init_row_position1, init_col_index1);

    std::shared_ptr<int[]> init_row_position2(new int[4 + 1]{0, 0, 2, 3, 4});
    std::shared_ptr<int[]> init_col_index2(new int[nnzs]{0, 2, 3, 2});
    std::shared_ptr<double[]> init_sparse_values2(new double[nnzs]{1, 1, 2, 1});
    CSRMatrix<double> sparse_matrix2 = CSRMatrix<double>(4, 4, nnzs, init_sparse_values2, init_row_position2, init_col_index2);

    auto result = sparse_matrix1.matMatMult(sparse_matrix2);

    int expected_nnzs = 4;
    double expected_values[] = {1, 2, 2, 2};
    int expected_row_pos[] = {0, 2, 2, 4, 4};
    int expected_col_ind[] = {0, 2, 0, 2};

    bool vals = TestRunner::assertArrays(&expected_values[0], &result->values[0], expected_nnzs);
    bool rows = TestRunner::assertArrays(&expected_row_pos[0], &result->row_position[0], expected_nnzs);
    bool cols = TestRunner::assertArrays(&expected_col_ind[0], &result->col_index[0], expected_nnzs);
    bool outcome = vals && rows && cols;

    return outcome;
}

bool test_check_dimensions_matching()
{
    Matrix<int> m = Matrix<int>(3, 3, true);
    std::vector<int> v(3, 0);
    try
    {
        checkDimensions(m, v);
        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

bool test_check_dimensions_not_matching()
{
    Matrix<int> m = Matrix<int>(2, 3, true);
    std::vector<int> v(3, 0);

    try
    {
        checkDimensions(m, v);
        return false;
    }
    catch (const std::exception &e)
    {
        // we expect error to be thrown here
        return true;
    }
}

bool test_dense_jacobi_and_gauss_seidl()
{
    int size = 4;
    double tol = 1e-6;
    int it_max = 1000;

    std::shared_ptr<double[]> init_dense_values(new double[size * size]{10., 2., 3., 5., 1., 14., 6., 2., -1., 4., 16., -4, 5., 4., 3., 11.});

    Matrix<double> dense_mat = Matrix<double>(size, size, init_dense_values);
    std::vector<double> b = {1., 2., 3., 4.};

    Solver<double> dense_solver = Solver<double>(dense_mat, b);

    std::vector<double> x_j(size, 0);
    std::vector<double> x_gs(size, 0);

    auto t1 = std::chrono::high_resolution_clock::now();
    dense_solver.stationaryIterative(x_j, tol, it_max, false);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration<double>(t2 - t1).count();
    std::cout << "Time taken for Jacobi: " << duration << " s " << std::endl
              << std::endl;

    t1 = std::chrono::high_resolution_clock::now();
    dense_solver.stationaryIterative(x_gs, tol, it_max, true);
    t2 = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration<double>(t2 - t1).count();
    std::cout << "Time taken for Gauss Seidel: " << duration << " s " << std::endl
              << std::endl;

    std::vector<double> output_b(size, 0);

    double j_res = dense_solver.residualCalc(x_j, output_b);
    double gs_res = dense_solver.residualCalc(x_gs, output_b);

    // passes if residual for both is small enough
    return TestRunner::assertBelowTolerance(j_res, tol) && TestRunner::assertBelowTolerance(gs_res, tol);
}

// Sparse Jacobi and Gauss-Seidel
bool test_sparse_stationary_iterative()
{
    int size = 6;
    double tol = 1e-6;
    int it_max = 1000;
    int nnzs = 12;

    std::shared_ptr<double[]> init_sparse_values(new double[size * size]{5., 1., 1., 1., 5., 1., 5., 1., 5., 1., 5., 5.});
    std::shared_ptr<int[]> init_row_position(new int[size * size]{0, 4, 5, 7, 9, 11, 12});
    std::shared_ptr<int[]> init_col_index(new int[size * size]{0, 2, 3, 4, 1, 0, 2, 0, 3, 0, 4, 5});

    std::vector<double> b = {17., 10., 16., 21., 26., 30.};

    CSRMatrix<double> sparse_matrix = CSRMatrix<double>(size, size, nnzs, init_sparse_values, init_row_position, init_col_index);
    SparseSolver<double> sparse_solver = SparseSolver<double>(sparse_matrix, b);
    std::vector<double> x(size, 0);

    // JACOBI
    auto t1 = std::chrono::high_resolution_clock::now();
    sparse_solver.stationaryIterative(x, tol, it_max, false);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration<double>(t2 - t1).count();
    std::cout << "Time taken for sparse Jacobi: " << duration << " s " << std::endl
              << std::endl;

    std::vector<double> output_b(size, 0);

    printVector(x);

    if (sparse_solver.residualCalc(x, output_b) > 1e-6)
    {
        TestRunner::testError("Sparse Jacobi residual is above 1e-6");
        return false;
    }

    // GAUSS SEIDEL
    t1 = std::chrono::high_resolution_clock::now();
    sparse_solver.stationaryIterative(x, tol, it_max, true);
    t2 = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration<double>(t2 - t1).count();
    std::cout << "Time taken for sparse Gauss-Seidel: " << duration << " s " << std::endl
              << std::endl;

    printVector(x);

    double residual = sparse_solver.residualCalc(x, output_b);

    return TestRunner::assertBelowTolerance(residual, 1e-6);
}

// Sparse Jacobi
bool test_sparse_jacobi_random()
{
    // leave it at 10 since jacobi is not very stable for large matrices
    int size = 10;
    double tol = 1e-6;
    int it_max = 1000;

    CSRMatrix<double> m = CSRMatrix<double>(size, 0.7);
    std::vector<double> b(size, 0);
    for (int i = 0; i < size; i++)
    {
        b[i] = i;
    }

    SparseSolver<double> sparse_solver = SparseSolver<double>(m, b);
    std::vector<double> x(size, 0);

    auto t1 = std::chrono::high_resolution_clock::now();
    sparse_solver.stationaryIterative(x, tol, it_max, false);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration<double>(t2 - t1).count();
    std::cout << "Time taken for sparse Jacobi with random 10x10 matrix: " << duration << " s " << std::endl
              << std::endl;

    std::vector<double> output_b(size, 0);

    printVector(x);

    if (sparse_solver.residualCalc(x, output_b) > 1e-6)
    {
        TestRunner::testError("Sparse Jacobi residual is above 1e-6");
        return false;
    }
    return true;
}

// Sparse Gauss Seidel
bool test_sparse_gauss_seidel_random()
{
    int size = 100;
    double tol = 1e-6;
    int it_max = 1000;

    CSRMatrix<double> m = CSRMatrix<double>(size, 0.7);
    std::vector<double> b(size, 0);
    for (int i = 0; i < size; i++)
    {
        b[i] = i;
    }

    SparseSolver<double> sparse_solver = SparseSolver<double>(m, b);
    std::vector<double> x(size, 0);

    auto t1 = std::chrono::high_resolution_clock::now();
    sparse_solver.stationaryIterative(x, tol, it_max, true);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration<double>(t2 - t1).count();
    std::cout << "Time taken for sparse Gauss-Seidel with random 100x100 matrix: " << duration << " s " << std::endl
              << std::endl;

    std::vector<double> output_b(size, 0);

    printVector(x);

    if (sparse_solver.residualCalc(x, output_b) > 1e-6)
    {
        TestRunner::testError("Sparse Gauss Seidel residual is above 1e-6");
        return false;
    }
    return true;
}

bool test_sparse_CG()
{
    int size = 4;
    double tol = 1e-6;
    int it_max = 1000;
    int nnzs = 4;

    std::shared_ptr<int[]> init_row_position(new int[size + 1]{0, 1, 2, 3, 4});
    std::shared_ptr<int[]> init_col_index(new int[nnzs]{0, 1, 2, 3});
    std::shared_ptr<double[]> init_sparse_values(new double[nnzs]{2, 1, 3, 7});

    std::vector<double> b = {6.4, 7.8, 56.7, 51.1};
    std::vector<double> x(size, 0);

    CSRMatrix<double> sparse_matrix = CSRMatrix<double>(size, size, nnzs, init_sparse_values, init_row_position, init_col_index);
    SparseSolver<double> sparse_solver = SparseSolver<double>(sparse_matrix, b);

    auto t1 = std::chrono::high_resolution_clock::now();
    sparse_solver.conjugateGradient(x, tol, it_max);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration<double>(t2 - t1).count();
    std::cout << "Time taken for sparse conjugate gradient solver: " << duration << " s " << std::endl
              << std::endl;

    std::vector<double> output_b(size, 0);

    double residual = sparse_solver.residualCalc(x, output_b);

    return TestRunner::assertBelowTolerance(residual, 1e-6);
}

bool test_lu_dense()
{
    int size = 4;
    std::vector<double> x(size, 0);
    std::vector<double> output_b(size, 0);

    std::shared_ptr<double[]> init_dense_values(new double[size * size]{10., 2., 3., 5., 1., 14., 6., 2., -1., 4., 16., -4, 5., 4., 3., 11.});

    Matrix<double> dense_mat = Matrix<double>(size, size, init_dense_values);
    std::vector<double> b = {1., 2., 3., 4.};

    Solver<double> dense_solver = Solver<double>(dense_mat, b);

    Matrix<double> LU(size, size, true);

    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<int> piv = dense_solver.lu_decomp(LU);
    dense_solver.lu_solve(LU, piv, x);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration<double>(t2 - t1).count();
    std::cout << "Time taken for LU: " << duration << " s " << std::endl
              << std::endl;

    double residual = dense_solver.residualCalc(x, output_b);

    return TestRunner::assertBelowTolerance(residual, 1e-6);
}

bool test_jacobi_dense_random()
{
    int size = 100;
    double tol = 1e-6;
    int it_max = 1000;

    std::vector<double> x(size, 0);
    std::vector<double> output_b(size, 0);

    auto solver = Solver<double>(size);

    auto t1 = std::chrono::high_resolution_clock::now();

    solver.stationaryIterative(x, tol, it_max, false);

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration<double>(t2 - t1).count();

    std::cout << "Time taken for Jacobi: " << duration << " s " << std::endl
              << std::endl;

    double residual = solver.residualCalc(x, output_b);

    return TestRunner::assertBelowTolerance(residual, 1e-6);
}

bool test_gauss_seidel_dense_random()
{
    int size = 100;
    double tol = 1e-6;
    int it_max = 1000;

    std::vector<double> x(size, 0);
    std::vector<double> output_b(size, 0);

    auto solver = Solver<double>(size);

    auto t1 = std::chrono::high_resolution_clock::now();

    solver.stationaryIterative(x, tol, it_max, true);

    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration<double>(t2 - t1).count();

    std::cout << "Time taken for Gauss-Seidel: " << duration << " s " << std::endl
              << std::endl;

    double residual = solver.residualCalc(x, output_b);

    return TestRunner::assertBelowTolerance(residual, 1e-6);
}

bool test_lu_dense_random()
{
    int size = 100;
    std::vector<double> x(size, 0);
    std::vector<double> output_b(size, 0);

    auto solver = Solver<double>(size);

    Matrix<double> LU(size, size, true);

    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<int> piv = solver.lu_decomp(LU);
    solver.lu_solve(LU, piv, x);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration<double>(t2 - t1).count();
    std::cout << "Time taken for LU: " << duration << " s " << std::endl
              << std::endl;

    // use a second b array
    std::vector<double> b_new(size, 0);
    std::vector<double> x_new(size, 0);
    for (int i = 0; i < size; i++)
    {
        b_new[i] = i + 1;
    }

    t1 = std::chrono::high_resolution_clock::now();
    solver.lu_solve(LU, piv, x_new, b_new);
    t2 = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration<double>(t2 - t1).count();
    std::cout << "Time taken for LU with second b: " << duration << " s " << std::endl
              << std::endl;

    double residual = solver.residualCalc(x, output_b);
    return TestRunner::assertBelowTolerance(residual, 1e-6);
}

bool test_cholesky()
{
    int size = 4;
    int nnzs = 14;

    std::shared_ptr<int[]> init_row_position1(new int[size + 1]{0, 3, 6, 10, 14});
    std::shared_ptr<int[]> init_col_index1(new int[nnzs]{0, 2, 3, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3});
    std::shared_ptr<double[]> init_sparse_values1(new double[nnzs]{9, -27, 18, 9, -9, -27, -27, -9, 99, -27, 18, -27, -27, 121});

    CSRMatrix<double> sparse_matrix1 = CSRMatrix<double>(size, size, nnzs, init_sparse_values1, init_row_position1, init_col_index1);

    std::vector<double> b = {-0.5, 1.5, -2.5, 4.5};

    SparseSolver<double> sparse_solver = SparseSolver<double>(sparse_matrix1, b);

    std::shared_ptr<CSRMatrix<double>> R = sparse_solver.cholesky_decomp();

    std::vector<double> x(size, 0);

    sparse_solver.cholesky_solve(*R, x);

    std::cout << "Result: ";
    for (int i = 0; i < x.size(); i++)
    {
        std::cout << " " << x[i] << " ";
    }
    std::cout << "\n";

    std::vector<double> b_estimate(size, 0);

    if (sparse_solver.residualCalc(x, b_estimate) > 1e-6)
    {
        TestRunner::testError("Sparse Cholesky residual is above 1e-6");
        return false;
    }

    return true;
}

bool test_random_cholesky()
{
    int size = 100;
    double sparsity = 0.7;

    CSRMatrix<double> sparse_matrix1 = CSRMatrix<double>(size, sparsity);

    std::vector<double> b(size, 0);
    for (int i = 0; i < size; i++)
    {
        b[i] = i;
    }

    SparseSolver<double> sparse_solver = SparseSolver<double>(sparse_matrix1, b);

    std::shared_ptr<CSRMatrix<double>> R = sparse_solver.cholesky_decomp();

    std::vector<double> x(size, 0);

    sparse_solver.cholesky_solve(*R, x);

    std::vector<double> b_estimate(size, 0);

    double residual = sparse_solver.residualCalc(x, b_estimate);
    return TestRunner::assertBelowTolerance(residual, 1e-6);
}

bool test_sparse_lu()
{
    int size = 6;
    int nnzs = 12;

    std::shared_ptr<double[]> init_sparse_values(new double[size * size]{5., 1., 1., 1., 5., 1., 5., 1., 5., 1., 5., 5.});
    std::shared_ptr<int[]> init_row_position(new int[size * size]{0, 4, 5, 7, 9, 11, 12});
    std::shared_ptr<int[]> init_col_index(new int[size * size]{0, 2, 3, 4, 1, 0, 2, 0, 3, 0, 4, 5});

    std::vector<double> b = {17., 10., 16., 21., 26., 30.};
    std::vector<int> perm = {0, 1, 2, 3, 4, 5};

    CSRMatrix<double> sparse_matrix = CSRMatrix<double>(size, size, nnzs, init_sparse_values, init_row_position, init_col_index);
    SparseSolver<double> sparse_solver = SparseSolver<double>(sparse_matrix, b);
    std::vector<double> x(size, 0);

    auto LU = sparse_solver.lu_decomp();
    sparse_solver.lu_solve(*LU, perm, x);

    double expected[] = {1, 2, 3, 4, 5, 6};

    for (int i = 0; i < size; i++)
    {
        if (expected[i] != x[i])
        {
            std::cout << expected[i] << " does not match " << x[i] << std::endl;
            return false;
        }
    }

    return true;
}

bool test_random_sparse_lu()
{
    int size = 100;
    double sparsity = 0.7;

    CSRMatrix<double> sparse_matrix1 = CSRMatrix<double>(size, sparsity);

    std::vector<double> b(size, 0);
    std::vector<int> perm(size, 0);

    for (int i = 0; i < size; i++)
    {
        b[i] = i + 1;
        perm[i] = i;
    }
    // NOTE: we haven't implemented partial pivoting, but the permutation vector could be used in the lu_solve method
    SparseSolver<double> sparse_solver = SparseSolver<double>(sparse_matrix1, b);
    std::vector<double> x(size, 0);

    auto LU = sparse_solver.lu_decomp();
    sparse_solver.lu_solve(*LU, perm, x);

    std::vector<double> b_estimate(size, 0);

    double residual = sparse_solver.residualCalc(x, b_estimate);
    return TestRunner::assertBelowTolerance(residual, 1e-6);
}

bool test_random_sparse_matrix()
{
    int size = 10;
    CSRMatrix<double> rand_sparse = CSRMatrix<double>(size, 0.4);

    if (rand_sparse.rows != size || rand_sparse.cols != size)
    {
        TestRunner::testError("Dimensions of random matrix are incorrect");
        return false;
    }

    // compare to transpose to see whether it is symmetric
    std::shared_ptr<CSRMatrix<double>> transpose = rand_sparse.transpose();

    for (int i = 0; i < size + 1; i++)
    {
        if (rand_sparse.row_position[i] != transpose->row_position[i])
        {
            TestRunner::testError("row_position of random matrix and its transpose do not match");
            return false;
        }
    }
    for (int i = 0; i < rand_sparse.nnzs; i++)
    {
        if (rand_sparse.col_index[i] != transpose->col_index[i])
        {
            TestRunner::testError("col_index of random matrix and its transpose do not match");
            return false;
        }
        if (rand_sparse.values[i] != transpose->values[i])
        {
            TestRunner::testError("values of random matrix and its transpose do not match");
            return false;
        }
    }

    return true;
}

void run_tests()
{
    // MATRIX
    TestRunner test_runner_matrix = TestRunner("Matrix");
    test_runner_matrix.test(&test_mat_vec_mult, "matrix vector multiplication.");

    // CSRMATRIX
    TestRunner test_runner_csrmatrix = TestRunner("CSRMatrix");
    test_runner_csrmatrix.test(&test_sparse_matmatmult_4x4, "sparse matMatMult for two sparse 4x4 matrices.");
    test_runner_csrmatrix.test(&test_sparse_matmatmult_5x5, "sparse matMatMult for multiplying a 5x5 sparse matrix by itself.");
    test_runner_csrmatrix.test(&test_random_sparse_matrix, "constructor to create a random sparse matrix.");

    // SOLVER
    TestRunner test_runner_solver = TestRunner("Solver");
    test_runner_solver.test(&test_residual_calculation, "calcResidual method.");
    test_runner_solver.test(&test_dense_jacobi_and_gauss_seidl, "stationaryIterative: dense Jacobi and Gauss-Seidel solver for 4x4 matrix.");
    test_runner_solver.test(&test_jacobi_dense_random, "dense Jacobi with a random 100x100 matrix");
    test_runner_solver.test(&test_gauss_seidel_dense_random, "dense Gauss Seidel with a random 100x100 matrix");
    test_runner_solver.test(&test_lu_dense, "dense LU solver for 4x4 matrix.");
    test_runner_solver.test(&test_lu_dense_random, "dense LU with random matrices.");

    // SPARSE SOLVER
    TestRunner test_runner_ss = TestRunner("SparseSolver");
    test_runner_ss.test(&test_sparse_stationary_iterative, "sparse Jacobi solver for 4x4 matrix.");
    test_runner_ss.test(&test_sparse_jacobi_random, "sparse Jacobi solver for random 10x10 matrix.");
    test_runner_ss.test(&test_sparse_gauss_seidel_random, "sparse Gauss-Seidel solver for random 100x100 matrix.");
    test_runner_ss.test(&test_sparse_CG, "sparse conjugate gradient solver for 4x4 matrix.");
    test_runner_ss.test(&test_sparse_lu, "sparse LU decomposition.");
    test_runner_ss.test(&test_random_sparse_lu, "LU method with random 100x100 matrix.");
    test_runner_ss.test(&test_cholesky, "Cholesky method.");
    test_runner_ss.test(&test_random_cholesky, "Cholesky method with random 100x100 matrix.");

    // UTILITIES
    TestRunner test_runner_utils = TestRunner("Utilities");
    test_runner_utils.test(&test_check_dimensions_matching, "checkDimensions for matching matrices.");
}
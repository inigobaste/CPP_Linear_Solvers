#include "TestRunner.h"
#include <iostream>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"
#define BLUE "\033[34m"

TestRunner::TestRunner()
{
}

TestRunner::~TestRunner()
{
    this->completeRun();
}

void TestRunner::test(bool (*fun_ptr)(), std::string title)
{
    int total = this->testsFailed + this->testsSucceeded;
    std::cout << std::endl
              << BLUE << "Test " << total + 1 << ": " << title << RESET << std::endl;

    bool outcome = fun_ptr();
    if (outcome)
    {
        this->testsSucceeded += 1;
        std::cout << GREEN << "\xE2\x9C\x93"
                  << " Passed" << RESET << std::endl;
    }
    else
    {
        this->testsFailed += 1;
        std::cout
            << RED << "\u2717"
            << " Failed" << RESET << std::endl;
    }
}

void TestRunner::completeRun()
{
    int total = this->testsFailed + this->testsSucceeded;
    std::cout << std::endl;
    if (this->testsFailed == 0)
    {
        std::cout
            << GREEN << "\xE2\x9C\x93"
            << " "
            << total << "/" << total << " tests passed." << RESET << std::endl;
    }
    else
    {
        std::cout
            << RED
            << " " << this->testsFailed << "/" << total << " tests failed." << RESET << std::endl
            << std::endl;
    }
}

bool TestRunner::assertArrays(double *arr1, double *arr2, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (arr1[i] != arr2[i])
        {
            std::cerr << RED << "Values do not match: " << arr1[i] << " != " << arr2[i] << RESET << std::endl;
            return false;
        }
    }
    return true;
}

bool TestRunner::assertArrays(int *arr1, int *arr2, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (arr1[i] != arr2[i])
        {
            std::cerr << RED << "Values do not match: " << arr1[i] << " != " << arr2[i] << RESET << std::endl;
            return false;
        }
    }
    return true;
}
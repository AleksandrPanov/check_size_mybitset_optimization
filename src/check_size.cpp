#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <numeric>
#include <chrono>
#include <bitset>
typedef double Particle;

inline void calculate(std::vector<Particle> &cell)
{
    const int N = cell.size();
    #pragma ivdep
    for (int i = 0; i < N; i++)
    {
        cell[i] = cell[i] * 0.9 + 1.1;
    }
}

void f_check_size(std::vector<std::vector<Particle> > &v, const int numIter)
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    int elapsed_mseconds = 0;
    const int size = v.size();
    start = std::chrono::system_clock::now();
    for (int i = 0; i < numIter; i++)
    {
#pragma omp parallel for
        for (int i = 0; i < size; i++)
        {
            if (v[i].empty() == false)
            {
                calculate(v[i]);
            }
        }
    }
    end = std::chrono::system_clock::now();
    elapsed_mseconds += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Time: " << elapsed_mseconds << " ms\n";
}

void f_check_size_bitset(std::vector<std::vector<Particle> > &v, const int numIter)
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::bitset<1000 * 1000> bs;
    int elapsed_mseconds = 0;
    const int size = v.size();
    start = std::chrono::system_clock::now();
    #pragma omp parallel for
    for (int i = 0; i < size; i++)
    {
        bs[i] = !v[i].empty();
    }

    for (int i = 0; i < numIter; i++)
    {
#pragma omp parallel for
        for (int i = 0; i < size; i++)
        {
            if (bs[i])
                calculate(v[i]);
        }
    }
    end = std::chrono::system_clock::now();
    elapsed_mseconds += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Time: " << elapsed_mseconds << " ms\n";
}

void f_check_size_vector(std::vector<std::vector<Particle> > &v, const int numIter)
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::vector<int> bs(1000 * 1000);
    int elapsed_mseconds = 0;
    const int size = v.size();
    start = std::chrono::system_clock::now();
#pragma omp parallel for
    for (int i = 0; i < size; i++)
    {
        bs[i] = !v[i].empty();
    }

    for (int i = 0; i < numIter; i++)
    {
#pragma omp parallel for
        for (int i = 0; i < size; i++)
        {
            if (bs[i])
                calculate(v[i]);
        }
    }
    end = std::chrono::system_clock::now();
    elapsed_mseconds += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Time: " << elapsed_mseconds << " ms\n";
}

std::vector<std::vector<Particle> > initCells(int size, double p)
{
    std::vector<std::vector<Particle> > cells(size);
    std::vector<int> v(size);
    std::iota(&v[0], &v[size * p], 1);
    std::random_device rd;
    std::mt19937 g(rd());
    g.seed(0);
    std::shuffle(v.begin(), v.end(), g);

    for (int i = 0; i < size; i++)
        if (v[i] != 0) // непустые €чейки распределены случайно
            cells[i] = std::vector<Particle>(100, v[i]);
    return cells;
}
int main()
{
    const int size = 1000 * 1000; // число €чеек
    const int numIter = 20000; // количество итераций
    const double p = 0.005; // дол€ непустых €чеек равна половине процента (5000 непустых €чеек)

    std::vector<std::vector<Particle> > cells = initCells(size, p);
    f_check_size_vector(cells, numIter);
    std::cout << cells[350].front();
    system("pause");
}
#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <numeric>
#include <chrono>
typedef double Particle;

inline void calculate(std::vector<Particle> &cell)
{
    const int N = cell.size();
    #pragma ivdep
    for (int i = 0; i < N; i++)
    {
        cell[i]= cell[i] * 0.9 + 1.1;
    }
}

typedef unsigned int uint;
struct Bitset
{
    Bitset(int n) :numBits(n)
    {
        numBitsInElement = sizeof(uint) * 8;
        bts = std::vector<uint>(n / numBitsInElement + 1);
    }
    void setBit(uint bit)
    {
        const int index = bit / numBitsInElement;
        bts[index] |= 1 << bit % numBitsInElement;
    }
    uint getElement(int index) const
    {
        return bts[index];
    }
    int getSize() const { return bts.size(); }
private:
    std::vector<uint> bts;
    int numBits;
    int numBitsInElement;
};

void f_check_size_mybitset(std::vector<std::vector<Particle> > &v, const int numIter)
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    Bitset bs(1000 * 1000);
    int elapsed_mseconds = 0;
    const int size = v.size();
    start = std::chrono::system_clock::now();
#pragma omp parallel for
    for (int i = 0; i < size; i++)
    {
        if (v[i].empty() == false)
            bs.setBit(i);
    }

    const int numElem = bs.getSize();
    const int numBitsInElement = sizeof(uint) * 8;
    for (int i = 0; i < numIter; i++)
    {
        #pragma omp parallel for
        for (uint i = 0; i < numElem; i++)
        {
            //получаем элемент битсета содержащий numBitsInElement €чеек
            uint tmp = bs.getElement(i);
            //пока есть непустые €чейки
            while (tmp != 0u)
            {
                //находим "младшую" непустую €чейку
                const uint lowbit = tmp & (~tmp + 1u);
                //исключаем "младшую" непустую €чейку
                tmp ^= lowbit;
                //находим индекс €чейки за const число операций
                //на текущих данных это вариант быстрее, чем в цикле
                unsigned int x = (unsigned int)lowbit - 1u;
                x = x - ((x >> 1u) & 0x55555555u);
                x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
                x = (x + (x >> 4u)) & 0x0F0F0F0Fu;
                x = x + (x >> 8u);
                x = x + (x >> 16u);
                x &= 0x0000003Fu;
                //обрабатываем "частицы" в непустой €чейке
                calculate(v[i * numBitsInElement + x]);
            }
        }
    }
    end = std::chrono::system_clock::now();
    elapsed_mseconds += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Time: " << elapsed_mseconds << " ms\n";
}

std::vector<std::vector<Particle> > initCells(int size, double p)
{
    srand(0);
    std::vector<std::vector<Particle> > cells(size);
    std::vector<int> v(size);
    std::iota(&v[0], &v[size * p], 1);
    std::random_device rd;
    std::mt19937 g(rd());
    g.seed(0);
    std::shuffle(v.begin(), v.end(), g);

    for (int i = 0; i < size; i++)
        if (v[i] != 0)
            cells[i] = std::vector<Particle>(100, v[i]);
    return cells;
}
int main()
{
    const int size = 1000 * 1000;
    const int numIter = 20000;
    const double p = 0.005;
    std::vector<std::vector<Particle> > cells = initCells(size, p);
    f_check_size_mybitset(cells, numIter);
    std::cout << cells[350].front();
}
#include <chrono>
#include <vector>

using namespace std;

#ifndef PYTHON_TESTER
#define EpicStruct_SIZE 4
#define List_SIZE 100000
#endif

class Timer
{
public:
    Timer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const {
        return std::chrono::duration_cast<ms_>
            (clock_::now() - beg_).count();
    }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::milli> ms_;
    std::chrono::time_point<clock_> beg_;
};


struct EpicStruct
{
    char m_memory[EpicStruct_SIZE];

    EpicStruct()
    {
    }

    explicit EpicStruct(size_t val)
    {
        memset(m_memory, val % 127, sizeof(m_memory));
    }

    void print()
    {
        for( int i = 0; i < EpicStruct_SIZE; ++i)
            printf("%d", m_memory[i]);
    }
};


double insert_stl_version_rvalueref()
{
    Timer tmr;
    vector<EpicStruct> vec;
    for(size_t i = 0; i < List_SIZE; ++i)
    {
        vec.insert(vec.begin(), EpicStruct(i));
    }

    return tmr.elapsed();
}

double insert_normal()
{
    Timer tmr;
    vector<EpicStruct> vec;
    for(size_t i = 0; i < List_SIZE; ++i)
    {
        EpicStruct tmp = EpicStruct(i);
        vec.insert(vec.begin(), tmp);
    }
    return tmr.elapsed();
}


int main()
{
    double t = insert_stl_version_rvalueref();
    printf("RValueRef insert:%.2f ms\n", t);

    t = insert_normal();
    printf("Normal insert:%.2f ms\n", t);
}

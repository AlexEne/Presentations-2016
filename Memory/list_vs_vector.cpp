#include <chrono>
#include <list>
#include <vector>
#include <random>
#include <iostream>
#include<conio.h>

//Just in case i can't run this
// Results are:
//Elapsed time vector: 3005.84 ms
//Elapsed time list : 11817.53 ms
//Elapsed time MyVector : 2230.59 ms

using namespace std;


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
#define EpicStruct_SIZE 15
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


// !!!!!!!!!!!!!!! This is a bad implementation of a vector. !!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!! No, really it is a bad one.               !!!!!!!!!!!!!!!!!!!!!!!
//Provided just as an example so people get a general idea of what's under the hood.
// Use this anywhere else besides 'demo' purposes and you deserve whatever bugs you get.
template<class T>
class MyVector
{
public:
    typedef T* iterator;

    MyVector()
    {
        m_AllocatedSize = 2;
        m_Size = 0;
        m_Data = new T[m_AllocatedSize];
    }

    ~MyVector()
    {
        delete[] m_Data;
    }

    void push_back(T element)
    {
        //One element is guaranteed to be available for end()
        // + 1 for the new one
        // + 1 for the end() reserved space.
        if ((m_Size + 2) >= m_AllocatedSize)
        {
            //Grow by half ( this is what microsoft SDL vector does. I know because I checked the code ).
            // Don't trust me for it, go check.
            size_t newSize = m_AllocatedSize + m_AllocatedSize / 2;
            T* m_NewData = new T[newSize];
            
            memcpy(m_NewData, m_Data, m_Size*sizeof(T));
            delete[] m_Data;
            
            m_AllocatedSize = newSize;
            m_Data = m_NewData;
        }
        m_Data[m_Size++] = element;
    }

    iterator begin()
    {
        return m_Data;
    }

    iterator end()
    {
        return m_Data + m_Size;
    }

    //Taken from here:
    //
    //
    void rotate(iterator first, iterator n_first, iterator last)
    {
        iterator next = n_first;
        while(first != n_first)
        {
            swap(*first, *next);

            first++;
            next++;

            if(next == last)
                next = n_first;
            else if (first == n_first)
                n_first = next;
        }
    }

    
//#define STL_WAY 
    iterator insert(iterator& it, T value)
    {
#ifdef STL_WAY
        // This is how STL does it, using rotate, but not using temporary variables that hold ELEMENTS.
        // Uncomment the define above in order to enable this behaviour.
        size_t off = it - m_Data;
        push_back(value);
        it = m_Data+off;
        rotate(it, end() - 1, end());
#elif MY_WAY
        // Faster insert
        // Consumes more memory in theory due to extra temporary variables that hold ELEMENTS(tmp and tmp2)
        size_t off = it - m_Data;
        push_back(value);
        iterator new_location = m_Data+off;
        it = new_location;
        T tmp = *(end() - 1);
        while(new_location != end())
        {
            std::swap(tmp, *new_location);
            new_location++;
        }
#else  //memmove way
        size_t off = it - m_Data;
        push_back(value);
        iterator new_location = m_Data+off;
        it = new_location;

        memmove(it+1, it, (m_Size-off)*sizeof(T));
        (*it)=value;
#endif
        return it;
    }

    size_t size()
    {
        return m_Size;
    }

private:
    size_t m_Size;
    size_t m_AllocatedSize;
    T*     m_Data;
};


template<class T>
double test_container(size_t count)
{
    T container;
    T::iterator it;

    srand(42);
    Timer tmr;

    container.push_back(EpicStruct(0));

    for (size_t i = 0; i < count; ++i)
    {
        size_t pos = rand() % container.size();

        it = container.begin();
        for (size_t p = 0; p < pos; ++p)
        {
            volatile char temp = (*it).m_memory[0]; //Touch each element from 0 to pos by reading it in a temp. This won't get optimized away due to volatile
            it++;
        }
        
        container.insert(it, EpicStruct(i));
    }
    
    double t = tmr.elapsed();

#if _DEBUG 
    //If you want you can also print or save to file the struct. Just to check that they are the same in the end.
    for(it = container.begin(); it != container.end(); ++it)
        (*it).print();
    printf("\n");
#endif

    return t;
}


int main()
{
    size_t count = 20;

    double t = test_container<vector<EpicStruct>>(count);
    printf("Elapsed time vector: %.2f ms\n", t);

    t = test_container<list<EpicStruct>>(count);
    printf("Elapsed time list: %.2f ms\n", t);

    t = test_container<MyVector<EpicStruct>>(count);
    printf("Elapsed time MyVector: %.2f ms\n", t);

    _getch();
    return 0;
}

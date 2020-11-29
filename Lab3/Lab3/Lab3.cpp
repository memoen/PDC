// Lab3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
/*
#include <iostream>
#include <omp.h>
#include <math.h>
using namespace std;
int main()
{
    
    uint64_t g, e, p;
    cin >> g;
    cin >> e;
    cin >> p;


    /*uint64_t _64bitVariable;
    cin >> _64bitVariable;*/
/*    uint64_t v = e;
    uint8_t i = 0, parts[8] = { 0 };
    do parts[i++] = v & 0xFF; while (v >>= 8);


    uint8_t results[8] = { 0 };
#pragma omp parallel num_threads(8)
#pragma omp for
    for (int j = 0; j < 8; j++) 
    {
        results[j] = (int8_t)pow(g, pow(2,j))%p;
        results[j] = (int8_t)pow(results[j],parts[j]) % p;
    }



    cout << endl;
}
*/
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
#include <omp.h>
#include <iostream>
#include <math.h>


using namespace std;

uint16_t inline mpow2(uint16_t shftAmt) {
    return (1 << shftAmt);
}

int main()
{
    cout << omp_get_num_threads() << endl << omp_get_max_threads();
    omp_set_num_threads(4);
    cout << omp_get_num_threads() << endl << omp_get_max_threads();
    uint64_t g;
    uint64_t e;
    uint64_t p;

    uint16_t resultArray[4] = { 0, 0, 0, 0 };
    uint64_t result = 0;

    cout << "Enter g: ";
    cin >> g;
    cout << "Enter e: ";
    cin >> e;
    cout << "Enter p: ";
    cin >> p;

    double startTime, endTime, ticks;

    uint16_t shiftArray[4] = { 0, 0, 0, 0 };
    shiftArray[0] = e;
    for (int i = 1; i < sizeof(shiftArray) / sizeof(shiftArray[0]); i++)
    {
        shiftArray[i] = e >> 16 * i;
    }
    int index = -1;
    for (int i = 0; i < sizeof(shiftArray) / sizeof(shiftArray[0]); i++)
    {
        if (shiftArray[i] == 0)
        {
            int j = i + 1;
            for (; j < sizeof(shiftArray) / sizeof(shiftArray[0]); j++)
            {
                if (shiftArray[j] == 0)
                    continue;
                break;
            }
            if (j == sizeof(shiftArray) / sizeof(shiftArray[0]))
            {
                index = i;
                break;
            }
        }
    }

    auto r0 = resultArray[0];
    auto r1 = resultArray[1];
    auto r2 = resultArray[2];
    auto r3 = resultArray[3];
    startTime = omp_get_wtime();


/*#pragma omp parallel for num_threads(4)// private(p,g,index)
    for (int i = 0; i < sizeof(shiftArray) / sizeof(shiftArray[0]); i++)
    {
        if (i >= index)
        {
            resultArray[i] = 0;
        }
        else
        {
            resultArray[i] = (int64_t)pow(g, pow(2, i)) % p;
            resultArray[i] = (int64_t)pow(resultArray[i], shiftArray[i]) % p;
        }
    }*/



#pragma omp parallel sections private(p,g)
    {
        #pragma omp section 
        {
            //printf("id = %d, \n", omp_get_thread_num());
            if (0 >= index)
            {
                r0 = 0;
            }
            else
            {
                r0 = (int64_t)pow(g, mpow2(0)) % p;
                r0 = (int64_t)pow(r0, r0) % p;
            }
        }
        #pragma omp section
        {
            //printf("id = %d, \n", omp_get_thread_num());
            if (1 >= index)
            {
                r1 = 0;
            }
            else
            {
                r1 = (int64_t)pow(g, mpow2(1)) % p;
                r1 = (int64_t)pow(r1, r1) % p;
            }
        }
        #pragma omp section 
        {
            //printf("id = %d, \n", omp_get_thread_num());
            if (2 >= index)
            {
                r2 = 0;
            }
            else
            {
                r2 = (int64_t)pow(g, mpow2(2)) % p;
                r2 = (int64_t)pow(r2, r2) % p;
            }
        }
        #pragma omp section 
        {
            //printf("id = %d, \n", omp_get_thread_num());
            if (3 >= index)
            {
                r3 = 0;
            }
            else
            {
                r3 = (int64_t)pow(g, mpow2(3)) % p;
                r3 = (int64_t)pow(r3, r3) % p;
            }
        }
    }

    endTime = omp_get_wtime();
    ticks = omp_get_wtick();

    for (int i = sizeof(resultArray) / sizeof(resultArray[0]) - 1; i >= 0; i--)
    {
        result += (uint64_t)resultArray[i] << (16 * i);
    }

    result = result % p;
    cout << "Result: " << result << "\n";
    cout << "Duration: " << endTime - startTime << "\n";
    cout << "Timer accuracy: " << ticks << "\n";
}
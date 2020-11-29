// Lab1_MMX.cpp : by triod315, 2020
//

#include <iostream>
#include <chrono>

template<typename T>
void printArray(T array[], int size)
{
    for (int i = 0; i < size; i++)
        std::cout << int(array[i]) << " ";
    std::cout << std::endl;
}

int* partition(int* Lb, int* Ub)
{
    int* pivot_ptr;

    __asm
    {
        mov     edx, Ub
        mov     eax, Lb
        sub     edx, eax
        shr     edx, 3                  //(Ub-Lb)/2
 
        mov     edi, [eax + edx * 4]    //pivot pointer
                                        //pivot = *(Lb+(Ub-Lb)/2)
        cmp     eax, Ub                 //eax=Lb
        ja      b0                      //return Lb;
 
    b1: mov     eax, Lb
        cmp     [eax], edi              //edi=pivot
        jnl     b3                      //while (*Lb < pivot)
        add     Lb, 4                   //Lb++;   
        jmp     short b1

    b4: sub     Ub, 4
    b3: mov     eax, Ub
        cmp[eax], edi                   //while (*Ub > pivot)
        jg      b4                      //Ub-- 
        mov     ecx, Lb
        cmp     ecx, eax                //eax=Ub
        ja      b5                      //if(Lb <= Ub)
        sub     Ub, 4                   //swap( Lb++, Ub-- )
        add     Lb, 4
        mov     edx, [eax]              //eax=Ub
        xchg    edx, [ecx]              //ecx=Lb
        mov     [eax], edx              //*Ub<-->*Lb;
    b5: mov     eax, Lb
        cmp     eax, Ub
        jbe     b1                      //while (*Lb < pivot)
    b0:
        mov     pivot_ptr, eax
    }
    return pivot_ptr;
}


void quick_sort(int* Lb, int* Ub)
{
    __asm
    {
        mov     eax, Lb
        cmp     eax, Ub                 //if (Lb >= Ub)
        jnb     exit1      

        push    Ub
        push    eax                     //eax=Lb
        call    partition
        add     esp, 8

        mov     edi, eax                //eax=pivot_ptr
        sub     eax, 4                  //pivot_ptr-1
        push    eax
        push    Lb
        call    quick_sort  
        add     esp, 8

        push    Ub
        push    edi                     //edi=pivot_ptr
        call    quick_sort  
        add     esp, 8
        exit1:

    }
}

int main()
{
    typedef std::chrono::high_resolution_clock Clock;

    int a = 0;
    int8_t array1[] = { 10, 8, 6, 4, 18, 16, 14, 12, 2, 0};
    int8_t array2[] = { 11, 9, 7, 5, 19, 17, 15, 13, 3, 1};

    int8_t res[16];

    std::cout << "Initial arrays\n";
    printArray(array1, 10);
    printArray(array2, 10);
    
    int array[] = { 10, 8, 6, 4, 18, 16, 14, 12, 2, 0, 11, 9, 7, 5, 19, 17, 15, 13, 3, 1 };

    std::cout << "------------------DESCENDING-------------------\n";

    auto t1 = Clock::now();
    quick_sort(&array[0], &array[19]);

    int* array_end = &array[19];

    /*reverse array*/
    __asm 
    {
        lea     esi, array
        mov     edi, array_end
        mov     ecx, 10

    L1:
        mov     eax, [esi]
        mov     ebx, [edi]
        mov     [edi], eax
        mov     [esi], ebx
        add     esi, TYPE array
        sub     edi, TYPE array
        loop    L1
    }

    auto t2 = Clock::now();
    std::cout <<"Quick sort time\t"<< std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns\n";
    printArray(array, 20);
    
    t1 = Clock::now();
    __asm
    {
        lea     ecx, array1
        lea     edi, array2
        lea     ebx, res

        /*order first 8 numbers*/
        movq    mm0, qword ptr[edi]
        punpckhbw mm0, qword ptr[ecx]
        ; psllw mm0, 1
        movq    qword ptr[ebx], mm0
        add     ebx, 8

        /*order second 8 numbers*/
        movq    mm0, [edi]          
        punpcklbw mm0, [ecx]
        movq    qword ptr[ebx], mm0
        add     ebx, 8

        /*order last 4 numbers*/
        pxor    mm0, mm0
        pxor    mm1, mm1
        xor eax, eax
        xor esi, esi

        mov ebx, 1


        mov     ax, 8[edi]
        mov     si, [edi+8*ebx+0]

        movd    mm1, esi
        movd    mm0, eax
        punpcklbw mm0, mm1
        movq    qword ptr[ebx], mm0
        lea     eax, res
        
    }
    t2 = Clock::now();
    std::cout << "MMX time \t" << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns\n";
    printArray(res, 20);

    

    std::cout << "------------------ASCENDING-------------------\n";

    t1 = Clock::now();
    quick_sort(&array[0], &array[19]);

    t2 = Clock::now();
    std::cout << "Quick sort time\t" << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns\n";
    printArray(array, 20);

    int8_t array1d[] = { 12, 14, 16, 18, 4, 6, 8, 10, 0, 2 };
    int8_t array2d[] = { 13, 15, 17, 19, 5, 7, 9, 11, 1, 3 };

    t1 = Clock::now();
    __asm
    {
        lea     ecx, array2d
        lea     edi, array1d
        lea     ebx, res


        /*order last 4 numbers*/
        pxor    mm0, mm0
        pxor    mm1, mm1
        xor eax, eax
        xor esi, esi
        mov     ax, 8[edi]
        mov     si, 8[ecx]

        movd    mm1, esi
        movd    mm0, eax
        punpcklbw mm0, mm1
        movq    qword ptr[ebx], mm0
        add     ebx, 4

        /*order first 8 numbers*/
        movq    mm0, qword ptr [edi]
        punpckhbw mm0, qword ptr [ecx]
        movq    qword ptr [ebx], mm0
        add     ebx, 8
        
        /*order second 8 numbers*/
        movq    mm0, [edi]
        punpcklbw mm0, [ecx]
        movq    qword ptr [ebx], mm0
        add     ebx, 8
        
        lea     eax, res
    }
    t2 = Clock::now();
    std::cout << "MMX time \t" << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns\n";
    printArray(res, 20);
}

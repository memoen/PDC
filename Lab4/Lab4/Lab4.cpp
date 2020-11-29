/*
 * This file is part of the PDC_Labs distribution (https://github.com/xxxx or http://xxx.github.io).
 * Copyright (c) 2020 Oleksandr Hryshchuk.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

using namespace std;

/// <summary>
/// Sin function using Taylor series for n = 3
/// </summary>
/// <param name="x"></param>
/// <returns>Sin(x)</returns>
double custom_sin(double x) 
{
    double x_3 = x * x * x;
    double x_5 = x_3 * x * x;

    return x - x_3 / 6 + x_5 / 120;
}

/// <summary>
/// Cos function using Taylor series for n = 3
/// </summary>
/// <param name="x"></param>
/// <returns>Cos(x)</returns>
double custom_cos(double x) 
{
    double x_2 = x * x;

    return 1 - x_2 / 2 + x_2*x_2 / 24;
}

/// <summary>
/// Generate a coeficient
/// </summary>
/// <param name="size">size of array</param>
/// <returns>Generated array</returns>
double * genrate_random_array(int size) 
{
    double * array = new double[size];
    for (size_t i = 0; i < size; i++)
    {
        array[i] = ((double(rand()) / double(RAND_MAX)) * 2) - 1;
    }
    return array;
}

int main(int *argc, char * argv[])
{
    int N = atoi(argv[1]);

    int numtasks, pid, elements_per_process, n_elements_recieved;
    MPI_Status status;

    MPI_Init(argc, &argv);
    double t1, t2;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    elements_per_process = N / numtasks;    
    // master process 
    if (pid == 0) {
        t1 = MPI_Wtime();
        int index, i;
        //cout << "Total number of process: \t" << numtasks << endl;
        // check if more than 1 processes are run 
        if (numtasks > 1) {
            // distributes the portion of array 
            // to child processes to calculate 
            // their partial sums 
            for (i = 1; i < numtasks - 1; i++) {
                index = i * elements_per_process;

                MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }

            // last process adds remaining elements 
            index = i * elements_per_process;
            int elements_left = N - index;

            MPI_Send(&elements_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

        }

        // master process add its own sub array 
        double sum = 0;
        for (i = 0; i < elements_per_process; i++)
            sum += (((double(rand()) / double(RAND_MAX)) * 2) - 1) * cos(i * 0.0001);

        // collects partial sums from other processes 
        int tmp;
        for (i = 1; i < numtasks; i++) {
            MPI_Recv(&tmp, 1, MPI_INT,
                MPI_ANY_SOURCE, 0,
                MPI_COMM_WORLD,
                &status);
            int sender = status.MPI_SOURCE;

            sum += tmp;
        }
        //printf("Sum of array is : %d\n", sum);
        t2 = MPI_Wtime();
        cout << "{" << N << ", " << t2-t1<<"}, ";
    }
    else {
        MPI_Recv(&n_elements_recieved,
            1, MPI_INT, 0, 0,
            MPI_COMM_WORLD,
            &status);

        // calculates its partial sum 
        int partial_sum = 0;
        for (int i = 0; i < n_elements_recieved; i++)
            partial_sum += (((double(rand()) / double(RAND_MAX)) * 2) - 1) *cos((i+pid*elements_per_process)*0.001);

        // sends the partial sum to the root process 
        MPI_Send(&partial_sum, 1, MPI_INT,
            0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();   
}

 


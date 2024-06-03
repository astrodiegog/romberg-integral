#include <mpi.h>
#include <math.h>
#include <stdio.h>

int procID; /*process rank*/
int nprocs; /*number of processes executing simulation*/
int root;   /*rank of root process*/

MPI_Comm world_comm; /*global communicator*/

void InitializeMPI(int *pargc, char **pargv[])
{
    /* Initialize MPI*/
    MPI_Init(pargc, pargv);

    /* set process ids in comm world */
    MPI_Comm_rank(MPI_COMM_WORLD, &procID);

    /* find number of processes in comm world */
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    /* set root process rank */
    root = 0;

    /* set global communicator */
    world_comm = MPI_COMM_WORLD;
}

float TrapIntegral(float (*f)(float), float a, float b, int n, int verb)
{
    /* Declare variables to hold timing info */
    double start, end;

    /* Declare for loop iterator */
    int i;

    /* Declare process integral, global integral, and integration variables */
    double integral_proc, integral, h, sum, x;

    /* Start timing */
    start = MPI_Wtime();

    /* Calculate the step width size */
    h = (b - a) / (double)n;

    /* Initialize for loop sum */
    sum = 0.0;

    if (verb)
    {
        printf("\t %.0i: starting for loop with stepsize h=%3f \n", procID, h);
    }

    // proc 0 works on i=0,nprocs,2*nprocs,..., n
    // proc 1 works on i=1,1+nprocs,1+2*nprocs,...,n
    for (i = procID + 1; i <= n; i += nprocs)
    {
        x = a + (h * ((double)i - 0.5));
        if (verb)
        {
            printf("\t\t %.0i: inside for loop, i= %.0i, x= %.4f\n", procID, i, x);
        }
        sum += f(x);
    }

    /* Calculate Summa[f(x)]*dx */
    integral_proc = h * sum;

    if (verb)
    {
        printf("\t %.0i: finished for loop with mypi=%3f \n", procID, integral_proc);
    }

    /* Bring the integral values from each process all together */
    MPI_Reduce(&integral_proc, &integral, 1, MPI_DOUBLE, MPI_SUM, root, world_comm);

    /* Finish timing */
    end = MPI_Wtime();

    if (procID == root)
    {
        printf("Answer is approximately %.16f \n", integral);
        printf("Integral took %.5f with %.0i nprocs and %.0i subintervals\n", end - start, nprocs, n);
    }

    return integral;
}
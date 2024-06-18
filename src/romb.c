#include <mpi.h>
#include <stdio.h>

int procID; /*process rank*/
int nprocs; /*number of processes executing simulation*/
int root;   /*rank of root process*/

MPI_Comm world_comm, workers_comm;    /*global, worker communicators*/
MPI_Group world_group, workers_group; /*global, worker groups*/

float my_fabs(float num)
{
    if (num > 0)
    {
        return num;
    }
    else
    {
        return -num;
    }
}

float my_pow(float base, int exponent)
{
    int i;
    float answer = 1;

    for (i = 0; i < exponent; i++)
    {
        answer *= base;
    }

    return answer;
}

void InitializeMPI(int *pargc, char **pargv[])
{
    int ranks[1];
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

    // // grab group that world_comm is in, call it world_group
    // MPI_Comm_group(world_comm, &world_group);
    // ranks[0] = root;

    // // exclude root from world_group + create workers_group
    // MPI_Group_excl(world_group, 1, ranks, &workers_group);

    // // create workers comm from workers group using old world comm
    // MPI_Comm_create(world_comm, workers_group, &workers_comm);

    // // only needed workers group to create workers comm, can release it
    // MPI_Group_free(&workers_group);
}

float R_m0_sum(float (*f)(float), float a, float b, int n)
{
    /* Declare for loop iterator */
    int i;

    /* Declare process sum, global sum, and integration variables */
    double summa_proc, h_n, x;

    /* Calculate the step width size */
    h_n = (b - a) / my_pow(2, n);

    /* Initialize for loop sum for this process */
    summa_proc = 0.0;

#ifdef DEBUG
    printf("\t %.0i: starting for loop with imax = %.0f, stepsize h=%3f \n", procID, my_pow(2, n - 1), h_n);
#endif

    // proc 0 works on i=0,nprocs,2*nprocs,..., n
    // proc 1 works on i=1,1+nprocs,1+2*nprocs,...,n
    for (i = procID + 1; i <= my_pow(2, n - 1); i += nprocs)
    {
        x = a + (2 * (double)i - 1) * h_n;

#ifdef DEBUG
        printf("\t\t %.0i: inside for loop, i = %.0i, x = %.5f \n", procID, i, x);
#endif
        summa_proc += f(x);
    }

#ifdef DEBUG
    printf("\t %.0i: finished for loop with myvalue = %.3f \n", procID, summa_proc);
#endif

    return summa_proc * h_n;
}

#include "mpi.h"
#include <stdio.h>

#include "integrands.h"
#include "romb.h"

int main(int argc, char **argv)
{
    /* Declare variables to hold timing info */
    double start, end;

    /* Declare the iterator, current, and max_number n-step to use */
    int n_curr, n, n_max;

    /* Declare the max accuracy to use */
    double acc;

    /* Declare integration bounds */
    float a, b;

    /* Declare process sum and global sum*/
    double summa_proc, summa;

    /* Declare iterator to fill out R(-,j) array */
    int m;

    /* Declare number to calculate 4^m */
    float fourM;

    /* Declare convergence flag */
    int converged;

    /* Initialize MPI processes */
    InitializeMPI(&argc, &argv);

    if (procID == root)
    {
        if (argc < 3)
        {
            fprintf(stderr, "Usage: %s n number\n", argv[1]);
            MPI_Abort(world_comm, 1);
        }
        sscanf(argv[1], "%i", &n_max);
        sscanf(argv[2], "%lf", &acc);
    }

#ifdef DEBUG
    printf("--- We debuggin! --- \n");
#endif

    /* Broadcast n_max (one mpi int) to all worker processes */
    MPI_Bcast(&n_max, 1, MPI_INT, root, world_comm);
    
#ifdef DEBUG
    printf("\t %.0i: Just broadcasted n_max=%d!\n", procID, n_max);
#endif

    /* Declare Romberg R(i-1,j) and R(i,j) arrays and pointers*/
    float R_old[n_max], R_curr[n_max];
    float *pR_old = &R_old[0];
    float *pR_curr = &R_curr[0];
    float *pR_temp;

    /* Set the lower and upper integration bounds */
    a = -1.0;
    b = 1.0;

    /* Calculate R(0,0) */
    pR_old[0] = (0.5) * (b - a) * (integrandONE(a) + integrandONE(b));

    for (n = 1; n < n_max; n++)
    {
    #ifdef DEBUG
        printf("\t %.0i: Broadcastin %d !\n", procID, n);
    #endif
        /* Broadcast n (one mpi int) to all processes */
        MPI_Bcast(&n, 1, MPI_INT, root, world_comm);

#ifdef DEBUG
        printf("\t %.0i: Calling my R(%d,0)!\n", procID, n);
#endif
        /* Calculate R(n,0) for this process */
        summa_proc = R_m0_sum(integrandONE, a, b, n);

#ifdef DEBUG
        printf("\t %.0i: We're back! My R(%d,0)= %3f!\n", procID, n, summa_proc);
#endif

        /* Bring the sum values from each process all together */
        MPI_Reduce(&summa_proc, &summa, 1, MPI_DOUBLE, MPI_SUM, root, world_comm);

        if (procID == root)
        {
            /* Place R(n,0) */
            pR_curr[0] = summa + (0.5 * pR_old[0]);

#ifdef DEBUG
            printf("R(%d,0) = ", n);
            printf("(1/2) * R(%d,0) ", n - 1);
            printf("+ Summa \n");
            printf("\t R(%d,0) = %.3f \n", n - 1, pR_old[0]);
            printf("\t Summa = %.3f \n", summa);

            printf("\t R(%d,0) = %f\n", n, pR_curr[0]);
#endif

            /* Fill out current row R(n,m) */
            for (m = 1; m <= n; m++)
            {
                fourM = my_pow(4, m);
                pR_curr[m] = (1 / (fourM - 1)) * (fourM * pR_curr[m - 1] - pR_old[m - 1]);

#ifdef DEBUG
                printf("R(%d,%d)= ", n, m);
                printf("(1/(4^%d - 1)) * (", m);
                printf("(4^%d * R(%d,%d) ", m, n, m - 1);
                printf("- R(%d,%d))\n", n - 1, m - 1);

                printf("\tR(%d,%d) = %3f \n", n, m - 1, pR_curr[m - 1]);
                printf("\tR(%d,%d) = %3f \n", n - 1, m - 1, pR_old[m - 1]);
                printf("\tR(%d,%d) = %3f \n", n, m, pR_curr[m]);
#endif
            }

/* Print out current row if DEBUG */
#ifdef DEBUG
            printf("Row %d: \t", n);
            for (m = 0; m <= n; m++)
            {
                printf("%f \t", pR_curr[m]);
            }
            printf("\n");
#endif
        }

        /* Check accuracy condition after first row */
        if ((n > 1) && ((my_fabs(pR_old[n - 1] - pR_curr[n])) < acc))
        {
            printf("WE ACCURATE. time 2 break\n");
            /* Break out of for loop */
#ifdef DEBUG
            printf("We are done calculating! Answer we find is %f\n", pR_curr[n]);
#endif
            MPI_Abort(world_comm, 1);
        }
        else
        {
            pR_temp = pR_old;
            pR_old = pR_curr;
            pR_curr = pR_temp;
        }
        
    }


    MPI_Finalize();



    return 0;
}

#include "mpi.h"
#include <stdio.h>

#include "integrands.h"
#include "romb.h"

typedef enum
{
    false,
    true
} bool;

int main(int argc, char **argv)
{
    /* Initialize the number of subintervals to use */
    int n;

    /* Initialize integration bounds */
    float a, b;

    /* Declare whether we're being verbose in the application */
    bool verb = true;

    /* Initialize MPI processes */
    InitializeMPI(&argc, &argv);

    /* Set the lower and upper integration bounds */
    a = -1.0;
    b = 1.0;

    while (1)
    {
        /* Have root handle finding the number of intervals to use */
        if (procID == root)
        {
            printf("Enter the number of intervals: (0 quits) \n");
            scanf("%d", &n);

            if (verb)
            {
                printf("Doing calculation on %.0i intervals \n", n);
            }
        }

        /* Broadcast n to all processes */
        MPI_Bcast(&n, 1, MPI_INT, 0, world_comm);

        if (verb)
        {
            printf("\t %.0i: broadcast of %.0i has been sent to all children\n", procID, n);
        }

        if (n == 0)
        {
            if (verb)
            {
                printf("\t %.0i: all done now, going back to master\n", procID);
            }
            break;
        }
        else
        {
            TrapIntegral(integrandONE, a, b, n, verb);
        }
    }

    if (verb)
    {
        printf("\t %0i: We are done calculating, back at master\n", procID);
    }

    MPI_Finalize();

    return 0;
}

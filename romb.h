int procID; /*process rank*/
int nprocs; /*number of processes executing simulation*/
int root;   /*rank of root process*/

MPI_Comm world_comm; /*global communicator*/

/*\fn void InitializeMPI(void) */
/* Routine to initialize MPI */
void InitializeMPI(int *pargc, char **pargv[]);

/*\fn int TrapIntegral(int *, int)*/
/* Routine to compute trapezoidal integration scheme*/
void TrapIntegral(float (*f)(float), float a, float b, int n, int verb);

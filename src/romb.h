int procID; /*process rank*/
int nprocs; /*number of processes executing simulation*/
int root;   /*rank of root process*/

MPI_Comm world_comm, workers;         /*global, worker communicators*/
MPI_Group world_group, workers_group; /*global, worker groups*/

/*\fn void InitializeMPI(void) */
/* Routine to initialize MPI */
void InitializeMPI(int *pargc, char **pargv[]);

/*\fn float TrapIntegral(int, int)*/
/* Routine to compute trapezoidal integration scheme*/
float MidPointIntegral(float (*f)(float), float a, float b, int n);

/*\fn float TrapIntegral(int, int)*/
/* Routine to compute m=0 Romberg integration scheme*/
float R_m0(float (*f)(float), float a, float b, int n);

/*\fn float pow(float, int)*/
/* Routine to calculate base number to integer exponent */
float my_pow(float base, int exponent);

/*\fn float fabs(float)*/
/* Routine to return absolute value of a float */
float my_fabs(float num);

/**/
float R_m0_sum(float (*f)(float), float a, float b, int n);

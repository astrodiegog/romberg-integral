# romberg-integral

Romberg integration scheme that implements some parallelization. My goal was to

1. Create a standalone program that utilizes MPI to complete a portion of the integration scheme to get me more familiar with MPI, and
2. Learn how Makefiles and compilation generally works

## Method

Given an integration domain $(a,b)$, an _n_-th order of precision, and an _m_-th order of extrapolation, we can define a step size $h(n) = (b-a)/2^n$. With this step size, we can define the three equations for the Romberg integration scheme.

We first calculate the $R(0,0)$ term.

$$
R(0,0) = h(1) \left(f(a) + f(b) \right)
$$

We can now define an _n_-th order of precision that is similar to a general trapezoid scheme.

$$
R(n,0) = (0.5)R(n-1,0) + h(n) \sum_{k=1}^{2^{n-1}} f(a + h(n) (2k-1))
$$

For some _m_-th order of extrapolation, we can combine the _m-1_ order of extrapolation with the same order of extrapolation but lower precision of _n-1_. 

$$
R(n,m) = \frac{1}{4^m - 1} \left(4^m R(n,m-1) - R(n-1,m-1)\right)
$$

where the _m_-th order of extrapolation can take values $n \geq m$.

## Compilation

First need to run `prereq.sh` to ensure that `gcc` is installed.

With the GNU compiler installed, we can use `CC` to compile the source files onto object files, and also to link the object files together onto a binary file.

The included Makefile is inspired by the Makefile Tutorial I found online (link below), as well as the Makefile used in [Cholla](https://raw.githubusercontent.com/cholla-hydro/cholla/main/Makefile).

Since I wanted to test using MPI, I use `mpicc` to compile on my computer.

Compiling with `TYPE=debug` will show print statements of what the program is doing along the way.

## Implementation

The program takes in two arguments to run:

1. Maximum _n_-th order of precision desired
2. Minimum accuracy desired

For example, `mpirun -n 4 bin/romberg.default.dag 8 0.0000001` will run with 4 processes to either a maximum $n=8$ order of precision or when the differences between the highest extrapolated values are smaller than 0.0000001.

After broadcasting $n_{\textrm{max}}$ to all processes, the program calculates $R(0,0)$ and initiates a for loop up to $n_{\textrm{max}}$. Then, the program broadcasts _n_ to all processes and each process calls `R_m0_sum` in `src/romb.c`. In this function, each process calculates its summation contribution of the sum

$$
\sum_{k=1}^{2^{n-1}} f(a + h(n) (2k-1))
$$

After returning its integral contribution, each process will reduce onto one value for $R(n,0)$. From here, we loop through and calculate each $R(n,m)$ term until $m = n$. If the highest order of extrapolation reaches the desired accuracy, we end the program. In practice, I currently do this by calling `MPI_Abort`, but a better program will more elegantly end this program.


## Da End

This was mostly a project to get more comfortable with how to use some MPI functions, how to compile a program with my own Makefile, and building an integrator. While the end product isn't anything pretty, I learned quite a bit along the way.

I've tested this integrator to find that it can accurately calculate

$$
\int_{a=-1}^{b=1} \frac{4}{1+x^2} \textrm{d}x = 2\pi
$$

To test with other functions, feel free to add other values into `src/integrands.c` and replace the three current function calls to `integrandONE`.

Cheers!

Resources:
[Wikipedia](https://en.wikipedia.org/wiki/Romberg%27s_method)
[Makefile Tutorial](https://makefiletutorial.com)
[Compiling Programs Guide](http://www.delorie.com/djgpp/doc/ug/basics/compiling.html)
[What is CC?](https://stackoverflow.com/questions/8862450/in-makefiles-what-do-cc-and-ld-stand-for)

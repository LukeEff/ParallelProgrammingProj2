#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


int	NowYear;		// 2023 - 2028
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// rye grass height in inches
int	NowNumRabbits;		// number of rabbits in the current population

int main( int argc, char *argv[ ] )
{
#ifdef _OPENMP
    //fprintf( stderr, "OpenMP is supported -- version = %d\n", _OPENMP );
#else
    fprintf( stderr, "No OpenMP support!\n" );
    return 1;
#endif
}
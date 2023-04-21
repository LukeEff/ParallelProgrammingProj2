#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "Barrier.h"

// #define CSV

unsigned int seed = 0;


// Util functions
float
Ranf( unsigned int *seedp,  float low, float high )
{
    float r = (float) rand_r( seedp );              // 0 - RAND_MAX

    return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

float
Sqr( float x )
{
    return x*x;
}


// Global Variables
int	NowYear;		// 2023 - 2028
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// rye grass height in inches
int	NowNumRabbits;		// number of rabbits in the current population
Barrier barrier = Barrier();


// Constants
const float RYEGRASS_GROWS_PER_MONTH =		20.0;
const float ONE_RABBITS_EATS_PER_MONTH =	 1.0;

const float AVG_PRECIP_PER_MONTH =	       12.0;	// average
const float AMP_PRECIP_PER_MONTH =		4.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				60.0;
const float MIDPRECIP =				14.0;
const int START_YEAR = 2023;
const int TRIAL_DURATION_YEARS = 6;

void Rabbits()
{
    while (NowYear < START_YEAR + TRIAL_DURATION_YEARS)
    {
        int nextNumRabbits = NowNumRabbits;
        int carryingCapacity = (int) (NowHeight);
        if (nextNumRabbits < carryingCapacity)
            nextNumRabbits++;
        else if (nextNumRabbits > carryingCapacity)
            nextNumRabbits--;

        if (nextNumRabbits < 0)
            nextNumRabbits = 0;

        // DoneComputing barrier:
        barrier.WaitBarrier( );

        NowNumRabbits = nextNumRabbits;

        // DoneAssigning barrier:
        barrier.WaitBarrier();

        // DonePrinting barrier:
        barrier.WaitBarrier();
    }
}

void RyeGrass()
{
    while (NowYear < START_YEAR + TRIAL_DURATION_YEARS)
    {
        float nextHeight = NowHeight;

        float tempFactor = exp(-Sqr((NowTemp - MIDTEMP) / 10.));
        float precipFactor = exp(-Sqr((NowPrecip - MIDPRECIP) / 10.));

        nextHeight += tempFactor * precipFactor * RYEGRASS_GROWS_PER_MONTH;
        nextHeight -= (float) NowNumRabbits * ONE_RABBITS_EATS_PER_MONTH;

        // DoneComputing barrier:
        barrier.WaitBarrier( );

        NowHeight = nextHeight;

        // DoneAssigning barrier:
        barrier.WaitBarrier();

        // DonePrinting barrier:
        barrier.WaitBarrier();
    }
}

void Watcher()
{
    while (NowYear < START_YEAR + TRIAL_DURATION_YEARS)
    {
        // DoneComputing barrier:
        barrier.WaitBarrier( );

        // DoneAssigning barrier:
        barrier.WaitBarrier();

#ifdef CSV
        fprintf(stderr, "%4d , %2d , %6.2lf , %5.2lf ,  %6.2lf , %3d\n",
                NowYear, NowMonth, NowTemp, NowPrecip, NowHeight, NowNumRabbits);
#else
        fprintf(stderr, "Year: %4d ; Month: %2d ; Temp F: %6.2lf ; Precipitation in: %5.2lf ; Height: %6.2lf ; Rabbits: %3d\n",
                NowYear, NowMonth, NowTemp, NowPrecip, NowHeight, NowNumRabbits);
#endif

        NowMonth++;

        if (NowMonth >= 12)
        {
            NowMonth = 0;
            NowYear++;
        }

        float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

        float temp = AVG_TEMP - AMP_TEMP * cos( ang );
        NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

        float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
        NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
        if( NowPrecip < 0. )
            NowPrecip = 0.;


        // DonePrinting barrier:
        barrier.WaitBarrier();
    }
}

void MyAgent()
{
    while (NowYear < START_YEAR + TRIAL_DURATION_YEARS)
    {
        // DoneComputing barrier:
        barrier.WaitBarrier();

        // DoneAssigning barrier:
        barrier.WaitBarrier();

        // DonePrinting barrier:
        barrier.WaitBarrier();
    }
}

int main( int argc, char *argv[ ] )
{
#ifdef _OPENMP
    //fprintf( stderr, "OpenMP is supported -- version = %d\n", _OPENMP );
#else
    fprintf( stderr, "No OpenMP support!\n" );
    return 1;
#endif
    // Setup the now global variables
    NowMonth =    0;
    NowYear  = START_YEAR;
    NowNumRabbits = 1;
    NowHeight =  5.;

    // Setup the barrier

    omp_set_num_threads( 3 );	// or 4
    barrier.InitBarrier( 3 );		// or 4

    // Calculate the curent environmental parameters
    float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

    float temp = AVG_TEMP - AMP_TEMP * cos( ang );
    NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
    NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
    if( NowPrecip < 0. )
        NowPrecip = 0.;

    omp_set_num_threads( 4 );	// same as # of sections
#pragma omp parallel sections
    {
#pragma omp section
        {
            Rabbits( );
        }

#pragma omp section
        {
            RyeGrass( );
        }

#pragma omp section
        {
            Watcher( );
        }

#pragma omp section
        {
            MyAgent( );	// your own
        }
    }
}
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

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
unsigned int seed = 0;

int	NowYear;		// 2023 - 2028
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// rye grass height in inches
int	NowNumRabbits;		// number of rabbits in the current population


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

void Rabbits()
{
    int nextNumRabbits = NowNumRabbits;
    int carryingCapacity = (int)( NowHeight );
    if( nextNumRabbits < carryingCapacity )
        nextNumRabbits++;
    else
    if( nextNumRabbits > carryingCapacity )
        nextNumRabbits--;

    if( nextNumRabbits < 0 )
        nextNumRabbits = 0;
}

void RyeGrass()
{
    float nextHeight = NowHeight;

    float tempFactor = exp(   -Sqr(  ( NowTemp - MIDTEMP ) / 10.  )   );
    float precipFactor = exp(   -Sqr(  ( NowPrecip - MIDPRECIP ) / 10.  )   );

    nextHeight += tempFactor * precipFactor * RYEGRASS_GROWS_PER_MONTH;
    nextHeight -= (float)NowNumRabbits * ONE_RABBITS_EATS_PER_MONTH;
}

void Watcher()
{

}

void MyAgent()
{

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
    NowYear  = 2023;
    NowNumRabbits = 1;
    NowHeight =  5.;

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
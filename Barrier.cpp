//
// Created by Luke Fernandez on 4/21/23.
//

#include "Barrier.h"

// specify how many threads will be in the barrier:
//	(also init's the Lock)

void
Barrier::InitBarrier( int n )
{
    NumInThreadTeam = n;
    NumAtBarrier = 0;
    omp_init_lock( &Lock );
}

void
Barrier::WaitBarrier( )
{
    omp_set_lock( &Lock );
    {
        NumAtBarrier++;
        if( NumAtBarrier == NumInThreadTeam )
        {
            NumGone = 0;
            NumAtBarrier = 0;
            // let all other threads get back to what they were doing
            // before this one unlocks, knowing that they might immediately
            // call WaitBarrier( ) again:
            while( NumGone != NumInThreadTeam-1 );
            omp_unset_lock( &Lock );
            return;
        }
    }
    omp_unset_lock( &Lock );

    while( NumAtBarrier != 0 );	// this waits for the nth thread to arrive

#pragma omp atomic
    NumGone++;			// this flags how many threads have returned
}
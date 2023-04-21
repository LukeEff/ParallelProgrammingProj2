//
// Created by Luke Fernandez on 4/21/23.
//

#ifndef PROJECT2_BARRIER_H
#define PROJECT2_BARRIER_H
#include <omp.h>

class Barrier {
    omp_lock_t	Lock;
    volatile int	NumInThreadTeam;
    volatile int	NumAtBarrier;
    volatile int	NumGone;
public:
    void	InitBarrier( int );
    void	WaitBarrier( );
};


#endif //PROJECT2_BARRIER_H

#ifndef _ASD_UTIL_HPP_ 
#define _ASD_UTIL_HPP_

#include <cmath>
using namespace std;

double get_seed();
void   set_seed(double);
double random8();
unsigned random_ab(unsigned, unsigned);

double SEED = 0.0;

double get_seed()
{
    return SEED;
}

void set_seed(double s)
{
    SEED = s;
}

// Algoritmo 8 degli appunti per un random in [0, 1)
double random8()
{
    unsigned a = 16087,
             m = 2147483647,
             q = 127773,
             r = 2836;
    double hi, lo, test;
    
    hi = ceil(SEED / q);
    lo = SEED - q*hi;
    test = a*lo - r*hi;

    if(test < 0.0)
        set_seed(test + m);
    else
        set_seed(test);

    return SEED / m;
}

// random integer in [a, b)
unsigned random_ab(unsigned a, unsigned b)
{
    return floor( a + (b-a)*random8() );
}

#endif

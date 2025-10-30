#pragma one
#include <cstdlib>

//8.1
inline double rand_doub(){
    return std::rand() / (RAND_MAX + 1.0);
}

inline double rand_doub(double min, double max){
    return min+(max-min)*rand_doub();
}

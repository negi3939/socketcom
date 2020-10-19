#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Geometry>
#ifndef GCC3p3 
#include <Eigen/StdVector>
#endif

#include "sequence.h"

#if defined(SEQUENCE_IS_MAIN)
int main(){
    Sequence<double,VectorXd> hoge;
    VectorXd a = VectorXd::Zero(6);
    double t = 1;
    a << 1,2,3,4,5,6;
    for(int ii=0;ii<10;ii++){
        hoge.push_back(t,a);
        t += 1.0;
        for(int jj=0;jj<a.size();jj++){
            a(jj) *= 2; 
        }
    }
    double ttt;
    std::cout << "size is "<<  hoge.size() << std::endl;
    VectorXd geta;
    hoge.show(2);
     
}
#endif
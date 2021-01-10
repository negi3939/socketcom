#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <vector>

#include "mymath.h"
#include "sequence.h"
#include "socketcom.h"


void samle_func(Sockcom *so){
  std::string st;
  st = "start";
  so->sendd(st,0);
}

int main(){
  Sockcom_c *comv;
  comv= new Sockcom_c(2000,"127.0.0.1");
  std::cout << "input s" << std::endl;
  while(getchar()!='s'){}
  samle_func(comv);
  comv->waittoallend();
  delete comv;
}


#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <vector>

#include "sequence.h"
#include "socketcom.h"

int main(){
  Sockcom_s *com;
  com = new Sockcom_s();
  com->waittoend();
  delete com;
}
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "sequence.h"
#include "socketcom.h"

Sockcom_s::Sockcom_s(){
  std::cout << "this is server" << std::endl;
  port = 2000;
  char *ipp = "127.0.0.1";
  ip = ipp;
  sock = connectFromClientSingle( port ,ip);
  std::string buffer;
  recvv(buffer);
  std::cout << buffer << std::endl;
  buffer = "connectted";
  sendd(buffer);
}

Sockcom_s::Sockcom_s(int p,char *ipp){
  std::cout << "this is server" << std::endl;
  port = p;
  ip = ipp;
  sock = connectFromClientSingle( port ,ip);
  std::string buffer;
  recvv(buffer);
  std::cout << buffer << std::endl;
  buffer = "connectted";
  sendd(buffer);
}

Sockcom_s::~Sockcom_s(){
  close(sock);
}

Sockcom_c::Sockcom_c(){
  int port=2000;
  char *host="127.0.0.1";

  sock = connectToServer( port , host );
  if( sock < 0 ){
    perror( "Couldn't connect to server\n" );
    exit( 1 );
  }
  std::string buffer;
  buffer = "00/99+";
  sendd(buffer); 
  recvv(buffer);
  std::cout << buffer << std::endl;
}

Sockcom_c::Sockcom_c(int p,char *h){
  port = p;
  host = h;
  sock = connectToServer( port , host );
  if( sock < 0 ){
    perror( "Couldn't connect to server\n" );
    exit(1);
  }
  std::string buffer;
  buffer = "00/99+";
  sendd(buffer); 
  recvv(buffer);
  std::cout << buffer << std::endl;
}

Sockcom_c::~Sockcom_c(){
  close(sock);
}

int Sockcom::connectToServer( int portnum , const char *ip_addr ){
  struct sockaddr_in   addr;
  unsigned short       port;
  int                  sock , flag;
  
  if( portnum < 1024 || portnum > 65535 ){
    fprintf( stderr , "error: invarid port number %d\n" , portnum );
    sock = -2;
  }else{
    sock             = socket( PF_INET , SOCK_STREAM , 0 );
    port             = (unsigned short)portnum;
    addr.sin_port    = htons( port );
    inet_aton( ip_addr , &(addr.sin_addr) );
    addr.sin_family  = PF_INET;
    flag             = connect( sock , (struct sockaddr *)&addr , sizeof( struct sockaddr_in ) );
    if( flag < 0 ){
      sock = flag;
    }
  }

  return sock;
}


int Sockcom::connectFromClientSingle( int portnum , const char *ip_addr ){
  struct sockaddr_in saddr;
  unsigned short     port;
  int                sock1 , L;
  if( portnum < 1024 || portnum > 65535 ){
    fprintf( stderr , "error: invarid port number %d\n" , portnum );
    sock1 = -2;
  }else{
    sock1            = socket( PF_INET , SOCK_STREAM , 0 );
    saddr.sin_family = PF_INET;
    port             = (unsigned short)portnum;
    saddr.sin_port   = htons( port );
    inet_aton( ip_addr , &(saddr.sin_addr) );
    bind( sock1 , (struct sockaddr *)&saddr , sizeof( struct sockaddr_in ) );
    L                = listen( sock1 , 1 );
  }
  struct sockaddr_in caddr;
  socklen_t          length;
  int                sock2;
  length = sizeof( struct sockaddr_in );
  sock2  = accept( sock1 , (struct sockaddr *)&caddr , &length );
  close( sock1 );
  return sock2;
}

int Sockcom::sendd(void *buf,int len){
  int ret;
  ret = send( sock , buf , len , 0 );
  return ret;
}
int Sockcom::sendd(std::string s){
  int ret;
  char *sendch = new char[s.length()+1];
  std::strcpy(sendch,s.c_str());
  ret = send( sock , sendch , s.length(), 0 );
  delete[] sendch;
  return ret;
}

int Sockcom::sendd(double num){
  return send( sock , &num , sizeof(double) , 0 );
}

int Sockcom::sendd(float num){
  return send( sock , &num , sizeof(double) , 0 );
}


int Sockcom::sendd(int num){
  int ret;
  ret = send( sock , &num , sizeof(int) , 0 );
  return ret;
}

int Sockcom::sendd(VectorXd &eigenV){
  double *senddouble = new double[eigenV.size()];
  for(int ii = 0;ii<eigenV.size();ii++){
    senddouble[ii] = eigenV(ii);
  }
  int ret = send( sock , senddouble , eigenV.size()*sizeof(double) , 0 );
  delete[] senddouble;
  return ret;
}

int Sockcom::sendd(VectorXf &eigenV){
  float *senddouble = new float[eigenV.size()];
  for(int ii = 0;ii<eigenV.size();ii++){
    senddouble[ii] = eigenV(ii);
  }
  int ret = send( sock , senddouble , eigenV.size()*sizeof(float) , 0 );
  delete[] senddouble;
  return ret;
}

int Sockcom::sendd(std::vector<VectorXd> &vectorV){
  double *senddouble = new double[vectorV.size()*vectorV[0].size()];
  for(int ii = 0;ii<vectorV.size();ii++){
    for(int jj = 0;jj<vectorV[0].size();jj++){
      senddouble[ii*vectorV[0].size()+jj] = vectorV[ii](jj);
    }
  }
  int ret = send( sock , senddouble , vectorV.size()*vectorV[0].size()*sizeof(double) , 0 );
  delete[] senddouble;
  return ret;
}
int Sockcom::sendd(std::vector<VectorXf> &vectorV){
  float *senddouble = new float[vectorV.size()*vectorV[0].size()];
  for(int ii = 0;ii<vectorV.size();ii++){
    for(int jj = 0;jj<vectorV[0].size();jj++){
      senddouble[ii*vectorV[0].size()+jj] = vectorV[ii](jj);
    }
  }
  int ret = send( sock , senddouble , vectorV.size()*vectorV[0].size()*sizeof(float) , 0 );
  delete[] senddouble;
  return ret;
}

int Sockcom::sendd(Sequence<double,VectorXd> &seq){
  double *senddouble = new double[seq.size()*(seq.vecsize()+1)];
  double timm;
  VectorXd vvv(seq.vecsize());
  for(int ii = 0;ii<seq.size();ii++){
    seq.get(ii,timm,vvv);
    senddouble[ii*(seq.vecsize()+1)] = timm; 
    for(int jj = 0;jj<seq.vecsize();jj++){
      senddouble[ii*(seq.vecsize()+1) + jj+1] = vvv(jj);
    }
  }
  int ret = send( sock , senddouble , seq.size()*(seq.vecsize()+1)*sizeof(double) , 0 );
  delete[] senddouble;
  return ret;
}

int Sockcom::sendd(Sequence<float,VectorXf> &seq){
  float *senddouble = new float[seq.size()*(seq.vecsize()+1)];
  float timm;
  VectorXf vvv(seq.vecsize());
  for(int ii = 0;ii<seq.size();ii++){
    seq.get(ii,timm,vvv);
    senddouble[ii*(seq.vecsize()+1)] = timm; 
    for(int jj = 0;jj<seq.vecsize();jj++){
      senddouble[ii*(seq.vecsize()+1) + jj+1] = vvv(jj);
    }
  }
  int ret = send( sock , senddouble , seq.size()*(seq.vecsize()+1)*sizeof(float) , 0 );
  delete[] senddouble;
  return ret;
}

int Sockcom::recvv(void *buf){
  int ret = recv( sock , buf , MAX_SIZE , 0 );
  return ret;
}

int Sockcom::recvv(std::string &s){
  char *buf = new char[MAX_SIZE];
  int ret = recv( sock , buf , MAX_SIZE , 0 );
  char *buff = new char[ret];
  for(int ii=0;ii<ret;ii++){
    buff[ii] = buf[ii];
  }
  std::string bs(buff);
  delete[] buf;
  delete[] buff;
  s = bs;
  return ret;
}

int Sockcom::recvv(double *num){
  int ret = recv( sock , num , sizeof(double) , 0 );
  return ret;
}

int Sockcom::recvv(float *num){
  int ret = recv( sock , num , sizeof(double) , 0 );
  return ret;
}

int Sockcom::recvv(VectorXd &eigenV){
 double *revd = new double[eigenV.size()];
 int ret = recv( sock , revd , eigenV.size()*sizeof(double) , 0 );
  for(int ii = 0;ii<eigenV.size();ii++){
    eigenV(ii) = revd[ii];
  }
  delete[] revd;
  return ret;
}

int Sockcom::recvv(VectorXf &eigenV){
  float *revd = new float[eigenV.size()];
 int ret = recv( sock , revd , eigenV.size()*sizeof(float) , 0 );
  for(int ii = 0;ii<eigenV.size();ii++){
    eigenV(ii) = revd[ii];
  }
  delete[] revd;
  return ret;
}

int Sockcom::recvv(std::vector<VectorXd> &vectorV,int n,int an){
  double *revd = new double[n*an];
  int ret=0;
  while(1){
    ret += recv( sock , revd+ret , MAX_SIZE , 0 );
    if(ret>=n*an*sizeof(double)){
      break;
    }
  }
  int ii = 0;
  int retd = ret/sizeof(double);
  VectorXd bufv(n);
  while(1){
    if(ii>retd){break;}
    for(int jj=0;jj<n;jj++){
      bufv(jj) = revd[ii];
      ii++;
    }
    vectorV.push_back(bufv);
  }
  delete[] revd;
  return ret;
}

int Sockcom::recvv(std::vector<VectorXf> &vectorV,int n,int an){
  float *revd = new float[n*an];
  int ret=0;
  while(1){
    ret += recv( sock , revd+ret , MAX_SIZE , 0 );
    if(ret>=n*an*sizeof(float)){
      break;
    }
  }
  int ii = 0;
  int retd = ret/sizeof(float);
  VectorXf bufv(n);
  while(1){
    if(ii>retd){break;}
    for(int jj=0;jj<n;jj++){
      bufv(jj) = revd[ii];
      ii++;
    }
    vectorV.push_back(bufv);
  }
  delete[] revd;
  return ret;
}

int Sockcom::recvv(Sequence<double,VectorXd> &seq,int n,int an){
  double *revd = new double[n+1];
  int ret=0;
  double timm;
  VectorXd bufv(n);
  while(1){
    ret += recv( sock , revd , (n+1)*sizeof(double) , 0 );
    timm = revd[0];
    for(int jj=1;jj<n+1;jj++){
      bufv(jj-1) = revd[jj];
    }
    seq.push_back(timm,bufv);
    if(ret>=(n+1)*an*sizeof(double)){
      break;
    }
  }
  delete[] revd;
  return ret;
}

int Sockcom::recvv(Sequence<float,VectorXf> &seq,int n,int an){
  float *revd = new float[n+1];
  int ret=0;
  float timm;
  VectorXf bufv(n);
  while(1){
    ret += recv( sock , revd , (n+1)*sizeof(float) , 0 );
    timm = revd[0];
    for(int jj=1;jj<n+1;jj++){
      bufv(jj-1) = revd[jj];
    }
    seq.push_back(timm,bufv);
    if(ret>=(n+1)*an*sizeof(float)){
      break;
    }
  }
  delete[] revd;
  return ret;
}

#if defined(COMMCLIENT_IS_MAIN)
int main(){
  Sockcom_c *comv;
  comv= new Sockcom_c(2000,"192.168.4.200");
  Sequence<float,VectorXf> hogeseq;
  std::vector<VectorXf> hogevector;
  std::string st;
  st = "start";
  comv->sendd(st);
  //comv->recvv(hogevector,6,1000);
  //PRINT_MAT(hogevector[999]);
  comv->recvv(hogeseq,6,100);
  hogeseq.show(99);
  delete comv;
}
#endif

#if defined(COMMSERVER_IS_MAIN)
int main(){
  Sockcom_s *com;
  com = new Sockcom_s(2000,"192.168.4.200");
  std::string st;
  com->recvv(st);
  std::cout << st << std::endl;
  VectorXf hoge(6);
  std::vector<VectorXf> hogevector;
  hoge(0) = 1;
  hoge(1) = 2;
  hoge(2) = 3;
  hoge(3) = 4;
  hoge(4) = 5;
  hoge(5) = 6;
  float timmm;
  VectorXf hov(6);
  Sequence<float,VectorXf> hogeseq;
  for(int ii=0;ii<1000;ii++){
    timmm = 0.1*(float)ii;
    for(int jj=0;jj<6;jj++){
      hoge(jj) = ii*jj;
      hov(jj) = hoge(jj);
    }
    hogeseq.push_back(timmm,hov);
    hogevector.push_back(hov);
  }
  //com->sendd(hogevector);
  com->sendd(hogeseq);
  delete com;
}
#endif
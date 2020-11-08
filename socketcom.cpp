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


Sockcom::Sockcom(){
  max_clinum = 10;
  endfl = 0;
}

Sockcom::~Sockcom(){}

void Sockcom::connectToServer( int portnum , const char *ip_addr ){
  struct sockaddr_in   addr;
  unsigned short       port;
  int                  sock2=0 , flag=0;
  pthread_mutex_init(&mutex,NULL);
  long l_count =0;
  while(sock2<=0){
    if( portnum < 1024 || portnum > 65535 ){
      fprintf( stderr , "error: invarid port number %d\n" , portnum );
      sock2 = -2;
    }else{
      sock2             = socket( PF_INET , SOCK_STREAM , 0 );
      port             = (unsigned short)portnum;
      addr.sin_port    = htons( port );
      inet_aton( ip_addr , &(addr.sin_addr) );
      addr.sin_family  = PF_INET;
      flag             = connect( sock2 , (struct sockaddr *)&addr , sizeof( struct sockaddr_in ) );
      if( flag < 0 ){
        sock2 = flag;
      }
    }
    l_count++;
    if(l_count%10000==0){std::cout << "wait for connecting.   sock is " << sock2 << " count " << l_count <<  std::endl;}
  }
  int thnum = 0;
  sock[thnum] = sock2;
  Structthis *sockksend = new Structthis;
	sockksend->instthis = (void*)this;
  sockksend->send = (void*)thnum;
  pthread_create(&sockthread[0],NULL,this->launchthread,sockksend);
  sleep(1);
  delete sockksend;
}

void Sockcom::waittoend(){
  int l_endfl=0;
  while(1){
    pthread_mutex_lock(&mutex);
    l_endfl = endfl;
    pthread_mutex_unlock(&mutex);
    if(l_endfl!=0){break;}
  }
}

void Sockcom::exitsock(){
  pthread_mutex_lock(&mutex);
  endfl = 1;
  pthread_mutex_unlock(&mutex);
}

void Sockcom::connectFromClient( int portnum , const char *ip_addr ){
  struct sockaddr_in saddr;
  unsigned short     port;
  int                sock1 , L;
  pthread_mutex_init(&mutex,NULL);
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
  for(int ii=0;ii<max_clinum;ii++){
    sock2  = accept( sock1 , (struct sockaddr *)&caddr , &length );
    sock[ii] = sock2;
    Structthis *sockksend = new Structthis;
	  sockksend->instthis = (void*)this;
    sockksend->send = (void*)ii;
    pthread_create(&sockthread[ii],NULL,this->launchthread,sockksend);
    sleep(1);
    delete sockksend;
  }
  close( sock1 );
}

int Sockcom::sendd(void *buf,int len,int thrnum){
  int ret;
  ret = send( sock[thrnum] , buf , len , 0 );
  return ret;
}
int Sockcom::sendd(std::string s,int thrnum){
  int ret;
  char *sendch = new char[s.length()+1];
  std::strcpy(sendch,s.c_str());
  ret = send( sock[thrnum] , sendch , s.length(), 0 );
  delete[] sendch;
  return ret;
}

int Sockcom::sendd(double num,int thrnum){
  return send( sock[thrnum] , &num , sizeof(double) , 0 );
}

int Sockcom::sendd(float num,int thrnum){
  return send( sock[thrnum] , &num , sizeof(double) , 0 );
}


int Sockcom::sendd(int num,int thrnum){
  int ret;
  ret = send( sock[thrnum] , &num , sizeof(int) , 0 );
  return ret;
}

int Sockcom::sendd(VectorXd &eigenV,int thrnum){
  double *senddouble = new double[eigenV.size()];
  for(int ii = 0;ii<eigenV.size();ii++){
    senddouble[ii] = eigenV(ii);
  }
  int ret = send( sock[thrnum] , senddouble , eigenV.size()*sizeof(double) , 0 );
  delete[] senddouble;
  return ret;
}

int Sockcom::sendd(VectorXf &eigenV,int thrnum){
  float *senddouble = new float[eigenV.size()];
  for(int ii = 0;ii<eigenV.size();ii++){
    senddouble[ii] = eigenV(ii);
  }
  int ret = send( sock[thrnum] , senddouble , eigenV.size()*sizeof(float) , 0 );
  delete[] senddouble;
  return ret;
}

int Sockcom::sendd(std::vector<VectorXd> &vectorV,int thrnum){
  double *senddouble = new double[vectorV.size()*vectorV[0].size()];
  for(int ii = 0;ii<vectorV.size();ii++){
    for(int jj = 0;jj<vectorV[0].size();jj++){
      senddouble[ii*vectorV[0].size()+jj] = vectorV[ii](jj);
    }
  }
  int ret = send( sock[thrnum] , senddouble , vectorV.size()*vectorV[0].size()*sizeof(double) , 0 );
  delete[] senddouble;
  return ret;
}
int Sockcom::sendd(std::vector<VectorXf> &vectorV,int thrnum){
  float *senddouble = new float[vectorV.size()*vectorV[0].size()];
  for(int ii = 0;ii<vectorV.size();ii++){
    for(int jj = 0;jj<vectorV[0].size();jj++){
      senddouble[ii*vectorV[0].size()+jj] = vectorV[ii](jj);
    }
  }
  int ret = send( sock[thrnum] , senddouble , vectorV.size()*vectorV[0].size()*sizeof(float) , 0 );
  delete[] senddouble;
  return ret;
}

int Sockcom::sendd(Sequence<double,VectorXd> &seq,int thrnum){
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
  int ret = send( sock[thrnum] , senddouble , seq.size()*(seq.vecsize()+1)*sizeof(double) , 0 );
  delete[] senddouble;
  return ret;
}

int Sockcom::sendd(Sequence<float,VectorXf> &seq,int thrnum){
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
  int ret = send( sock[thrnum] , senddouble , seq.size()*(seq.vecsize()+1)*sizeof(float) , 0 );
  delete[] senddouble;
  return ret;
}

int Sockcom::recvv(void *buf,int thrnum){
  int ret = recv( sock[thrnum] , buf , MAX_SIZE , 0 );
  return ret;
}

int Sockcom::recvv(std::string &s,int thrnum){
  char *buf = new char[MAX_SIZE];
  int ret = recv( sock[thrnum] , buf , MAX_SIZE , 0 );
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

int Sockcom::recvv(double *num,int thrnum){
  int ret = recv( sock[thrnum] , num , sizeof(double) , 0 );
  return ret;
}

int Sockcom::recvv(float *num,int thrnum){
  int ret = recv( sock[thrnum] , num , sizeof(double) , 0 );
  return ret;
}

int Sockcom::recvv(VectorXd &eigenV,int thrnum){
 double *revd = new double[eigenV.size()];
 int ret = recv( sock[thrnum] , revd , eigenV.size()*sizeof(double) , 0 );
  for(int ii = 0;ii<eigenV.size();ii++){
    eigenV(ii) = revd[ii];
  }
  delete[] revd;
  return ret;
}

int Sockcom::recvv(VectorXf &eigenV,int thrnum){
  float *revd = new float[eigenV.size()];
 int ret = recv( sock[thrnum] , revd , eigenV.size()*sizeof(float) , 0 );
  for(int ii = 0;ii<eigenV.size();ii++){
    eigenV(ii) = revd[ii];
  }
  delete[] revd;
  return ret;
}

int Sockcom::recvv(std::vector<VectorXd> &vectorV,int n,int an,int thrnum){
  double *revd = new double[n*an];
  int ret=0;
  while(1){
    ret += recv( sock[thrnum] , revd+ret , MAX_SIZE , 0 );
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

int Sockcom::recvv(std::vector<VectorXf> &vectorV,int n,int an,int thrnum){
  float *revd = new float[n*an];
  int ret=0;
  while(1){
    ret += recv( sock[thrnum] , revd+ret , MAX_SIZE , 0 );
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

int Sockcom::recvv(Sequence<double,VectorXd> &seq,int n,int an,int thrnum){
  double *revd = new double[n+1];
  int ret=0;
  double timm;
  VectorXd bufv(n);
  while(1){
    ret += recv( sock[thrnum] , revd , (n+1)*sizeof(double) , 0 );
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

int Sockcom::recvv(Sequence<float,VectorXf> &seq,int n,int an,int thrnum){
  float *revd = new float[n+1];
  int ret=0;
  float timm;
  VectorXf bufv(n);
  while(1){
    ret += recv( sock[thrnum] , revd , (n+1)*sizeof(float) , 0 );
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

void Sockcom::sock_func(void *send){
  std::cout << "Override this function! " << std::endl;
}

Sockcom_s::Sockcom_s(){
  std::cout << "this is server" << std::endl;
  port = 2000;
  char *ipp = "127.0.0.1";
  ip = ipp;
  init();
}

Sockcom_s::Sockcom_s(int p,char *ipp){
  std::cout << "this is server" << std::endl;
  port = p;
  ip = ipp;
  init();
}

Sockcom_s::~Sockcom_s(){
  close(sock);
  delete[] sock;
  delete[] sockthread;
}

void Sockcom_s::init(){
  sock = new int[max_clinum];
  sockthread = new pthread_t[max_clinum];
  connectFromClient( port ,ip);
}

void Sockcom_s::sock_func(void *send){
  int thnum = (int) send;
  std::string buffer;
  recvv(buffer,thnum);
  std::cout << buffer << std::endl;
  buffer = "connectted";
  sendd(buffer,thnum);
  sock_functest(thnum);
}

void Sockcom_s::sock_functest(int thnum){
  std::string st;
  recvv(st,thnum);
  std::cout << st << std::endl;
  VectorXf hoge(6);
  std::vector<VectorXf> hogevector;
  hoge(0) = 1;hoge(1) = 2;hoge(2) = 3;
  hoge(3) = 4;hoge(4) = 5;hoge(5) = 6;
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
  sendd(hogeseq,thnum);
  exitsock();
}


Sockcom_c::Sockcom_c() : Sockcom(){
  port=2000;
  char *h ="127.0.0.1";
  host = h;
  init();
}

Sockcom_c::Sockcom_c(int p,char *h) : Sockcom(){
  port = p;
  host = h;
  init();
}

Sockcom_c::~Sockcom_c(){
  close(sock);
  delete[] sock;
  delete[] sockthread;
}

void Sockcom_c::init(){
  sock = new int[1];
  sockthread = new pthread_t[1];
  connectToServer( port , host );
}

void Sockcom_c::sock_func(void *send){
  std::cout << "thread launched" << std::endl;
  int thnum = (int) send;
  if( sock[0] < 0 ){
    perror( "Couldn't connect to server\n" );
    exit( 1 );
  }
  std::string buffer;
  buffer = "00/99+";
  sendd(buffer,thnum); 
  recvv(buffer,thnum);
  std::cout << buffer << std::endl;
  sock_functest(thnum);
}

void Sockcom_c::sock_functest(int thnum){
  Sequence<float,VectorXf> hogeseq;
  std::vector<VectorXf> hogevector;
  std::string st;
  st = "start";
  sendd(st,thnum);
  recvv(hogeseq,6,100,thnum);
  hogeseq.show(99);
  exitsock();
}

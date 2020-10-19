#ifndef COMMINICATION_SCOK_H
#define COMMINICATION_SCOK_H

using namespace Eigen;
#define PRINT_MAT(X) std::cout << #X << ":\n" << X << std::endl << std::endl
#define MAX_SIZE    (1024)

class Sockcom {
  protected:
    int sock;
    int port;
    int connectToServer( int portnum , const char *ip_addr );
    int connectFromClientSingle( int portnum , const char *ip_addr );
  public:
    int sendd(void *buf,int len);
    int sendd(std::string s);
    int sendd(double num);
    int sendd(float num);
    int sendd(int num);
    int sendd(VectorXd &eigenV);
    int sendd(VectorXf &eigenV);
    int sendd(std::vector<VectorXd> &vectorV);
    int sendd(std::vector<VectorXf> &vectorV);
    int sendd(Sequence<double,VectorXd> &seq);
    int sendd(Sequence<float,VectorXf> &seq);
    int recvv(void *buf);
    int recvv(std::string &s);
    int recvv(double *num);
    int recvv(float *num);
    int recvv(VectorXd &eigenV);
    int recvv(VectorXf &eigenV);
    int recvv(std::vector<VectorXd> &vectorV,int n,int an);
    int recvv(std::vector<VectorXf> &vectorV,int n,int an);
    int recvv(Sequence<double,VectorXd> &seq,int n,int an);
    int recvv(Sequence<float,VectorXf> &seq,int n,int an);
};

class Sockcom_s : public Sockcom{
  protected:
    char *ip;
  public:
    Sockcom_s();
    Sockcom_s(int p,char *ipp);
    ~Sockcom_s();
};

class Sockcom_c : public Sockcom{
  protected:
    char *host;
  public:
    Sockcom_c();
    Sockcom_c(int p,char *h);
    ~Sockcom_c();
};

#endif
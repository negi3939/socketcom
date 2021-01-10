#ifndef SEQUENCE_H
#define SEQUENCE_H

using namespace Eigen;
#define PRINT_MAT(X) std::cout << #X << ":\n" << X << std::endl << std::endl

template<class Type1,class Type2>
class Sequence{
    private:
        int maxdatasize;
        int vecrsize;
        int datasize;
        double *time;
        double **vec;
    public:
        Sequence();
        Sequence(int n);
        ~Sequence();
        int size();
        int vecsize();
        void clear();
        void push_back(Type1 l_t,Type2 l_v);
        void get(int n,Type1 &l_t,Type2 &l_v);
        void show(int n);
};

template<class Type1,class Type2>
Sequence<Type1,Type2>::Sequence(){
    maxdatasize = 1000;
    vecrsize = 7;
    datasize = 0;
    time = new double[maxdatasize];
    vec = new double*[maxdatasize];
    for(int ii=0;ii<maxdatasize;ii++){
        vec[ii] = new double[vecrsize];
    }
}
template<class Type1,class Type2>
Sequence<Type1,Type2>::Sequence(int n){
    maxdatasize = 1000;
    vecrsize = n;
    datasize = 0;
    time = new double[maxdatasize];
    vec = new double*[maxdatasize];
    for(int ii=0;ii<maxdatasize;ii++){
        vec[ii] = new double[vecrsize];
    }
}
template<class Type1,class Type2>
Sequence<Type1,Type2>::~Sequence(){
}
template<class Type1,class Type2>
int Sequence<Type1,Type2>::size(){
    return datasize;
}
template<class Type1,class Type2>
int Sequence<Type1,Type2>::vecsize(){
    return vecrsize;
}
template<class Type1,class Type2>
void Sequence<Type1,Type2>::clear(){
    datasize = 0;
    delete[] time;
    for(int ii=0;ii<maxdatasize;ii++){
        delete[] vec[ii];
    }
    delete[] vec;
    time = new double[maxdatasize];
    vec = new double*[maxdatasize];
    for(int ii=0;ii<maxdatasize;ii++){
        vec[ii] = new double[maxdatasize];
    }
}
template<class Type1,class Type2>
void Sequence<Type1,Type2>::push_back(Type1 l_t,Type2 l_v){
    if(datasize<maxdatasize){
        time[datasize] = l_t;
        //std::cout << "l_v size " << l_v.size() << ", vecrsize " << vecrsize << std::endl;
        for(int ii=0;ii<vecrsize;ii++){
            vec[datasize][ii] = l_v(ii);
        }
        datasize++;
        //std::cout << datasize << " , "<< vec[datasize-1][0] << std::endl;
    }else{
        std::cout << "seq has full memorry" << std::endl;
        exit(0);
    }
}
template<class Type1,class Type2>
void Sequence<Type1,Type2>::get(int n,Type1 &l_t,Type2 &l_v){
    if(n>datasize-1){
        std::cout << "ERROR out of range" << std::endl;
        exit(0);
    }
    l_t = time[n];
    l_v.resize(vecrsize);
    for(int ii=0;ii<vecrsize;ii++){
        l_v(ii) = vec[n][ii];
    }
}
template<class Type1,class Type2>
void Sequence<Type1,Type2>::show(int n){
    std::cout << "time is " << time[n] << std::endl;
    std::cout << "vec is "<< std::endl;
    for(int ii=0;ii<vecrsize-1;ii++){
    std::cout << vec[n][ii] << " , "<< std::flush;
    }
    std::cout << vec[n][vecrsize-1] <<  std::endl;
}

#endif
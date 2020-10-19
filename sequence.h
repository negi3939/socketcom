#ifndef SEQUENCE_H
#define SEQUENCE_H

using namespace Eigen;
#define PRINT_MAT(X) std::cout << #X << ":\n" << X << std::endl << std::endl

template<class Type1,class Type2>
class Sequence{
    private:
        int vecrsize;
        int datasize;
        std::vector<Type1> time;
        std::vector<Type2> vec;
    public:
        Sequence();
        Sequence(int n);
        ~Sequence();
        int size();
        int vecsize();
        void push_back(Type1 l_t,Type2 l_v);
        void get(int n,Type1 &l_t,Type2 &l_v);
        void show(int n);
};

template<class Type1,class Type2>
Sequence<Type1,Type2>::Sequence(){
    vecrsize = 6;
    datasize = 0;
}
template<class Type1,class Type2>
Sequence<Type1,Type2>::Sequence(int n){
    vecrsize = n;
    datasize = 0;
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
void Sequence<Type1,Type2>::push_back(Type1 l_t,Type2 l_v){
    time.push_back(l_t);
    vec.push_back(l_v);
    datasize++;
}
template<class Type1,class Type2>
void Sequence<Type1,Type2>::get(int n,Type1 &l_t,Type2 &l_v){
    if(n>datasize-1){
        std::cout << "ERROR out of range" << std::endl;
        exit(0);
    }
    l_t = time[n];
    l_v = vec[n];
}
template<class Type1,class Type2>
void Sequence<Type1,Type2>::show(int n){
    std::cout << "time is " << time[n] << std::endl;
    #ifndef GCC3p3
    std::cout << "vec is "<< std::endl;
    std::cout << vec[n] << std::endl;
    #endif
}

#endif
#include "graph.hh"
#include<immintrin.h>
#include<pthread.h>
#define min(a,b)    ((a)<(b))?(a):(b)

const int thread_num=64;
struct ARG{
        int *Distance;
        int vertex_num_;
        int rank;};

pthread_barrier_t barrier;
void* shorten_path(void *argv);

Graph Graph::apsp() {
    Graph result(*this);
    int *Distance = result.get_raw_ptr();

    pthread_t threads[thread_num];          // 创建线程对象
    pthread_barrier_init(&barrier,NULL,vertex_num_);
    ARG argv;
    argv.Distance=result.get_raw_ptr();
    argv.vertex_num_=result.vertex_num_;

    for(int i=0;i!=thread_num;++i)
    {
        argv.rank=i;
        pthread_create(&threads[i],NULL,shorten_path,(void*)&argv);
    }

    return result;
}

void* shorten_path(void *argv)
{
    auto argvp=reinterpret_cast<ARG*>(argv);
    int id=argvp->rank;
    int num=argvp->vertex_num_;
    int *dis_mat=argvp->Distance;
    int start=id*num/thread_num;
    int end=(id+1)*num/thread_num;

    for(int k=0;k!=num;++k)
        {
            pthread_barrier_wait(&barrier);

            for(int i=start;i!=end;++i)
                for(int j=0;j!=num;++j){
                    int local_=0;
                    if(*(dis_mat+i*num+j)>(local_=*(dis_mat+i*num+k)+*(dis_mat+k*num+j)))
                        *(dis_mat+i*num+j)=local_;
        }
    return NULL;
}

//   int ij = 0, ik = 0, kj = 0;
//     for (int k = 0; k < vertex_num_; ++k) {
//         ij = 0;
//         ik = k;
//         kj = k * vertex_num_;

//         for (int i = 0; i < vertex_num_; ++i) {
//             int Distance_ik = Distance[ik];
//             for (int j = 0; j < vertex_num_; ++j) {
//                 result(i, j) = min(Distance[ij], Distance_ik + Distance[kj]);
//                 ++ij;
//                 ++kj;
//             }
//             ik += vertex_num_;
//         }
//     }
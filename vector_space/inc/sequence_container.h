#ifndef SEQUENCE_CONTAINER_H
#define SEQUENCE_CONTAINER_H
#include "declarations.h"
namespace zinhart
{
    template<lluint ...M>
    struct sequence{ };
    template<lluint N, lluint ... M>
    struct gen_sequence : gen_sequence<N-1,N-1,M...> {};
    template<lluint ...M>
    struct gen_sequence<0,M...> : sequence<M...>{ };
    template <class T,lluint N>
    class Sequence_Container
    {
        public:
            //std::array<T,N> data;
            vector_space<T,1> data;
            template<class ... Args>
            Sequence_Container(Args&&...args);
            template <class F>
            void operate_runtime(lluint start,lluint finish,F && func);
            template <class F>
            void operate_compile_time(F && func);
            template<class F, lluint... Is>
            void operate_compile_time(F && func, sequence<Is...>);
            Sequence_Container<T,N> & operator = (const Sequence_Container<T,N> & another);
    };

}
#endif // SEQUENCE_CONTAINER_H

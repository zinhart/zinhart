#include "../inc/sequence_container.h"
#include <algorithm>
namespace zinhart
{
    template <class T,lluint N>
    template<class ... Args>
    Sequence_Container<T,N>::Sequence_Container(Args&&...args) : data{{std::forward<Args>(args)...}}
    {
        static_assert(sizeof...(Args) == N,"Error in Sequece Constructor");
    }
    template <class T,lluint N>
    template <class F>
    void Sequence_Container<T,N>::operate_runtime (lluint start,lluint finish,F && func)
    {
     std::for_each(data.begin()+start,data.begin()+finish,func);
    }
    template <class T,lluint N>
    template <class F>
    void Sequence_Container<T,N>::operate_compile_time(F && func)
    {
        expand_sequence(std::forward<F>(func),gen_sequence<N>());
    }
    template <class T,lluint N>
    template<class F, lluint... Is>
    void Sequence_Container<T,N>::operate_compile_time(F && func, sequence<Is...>)
    {
        (std::forward<F>(func))(data[Is]...);
    }
    template <class T,lluint N>
    Sequence_Container<T,N> & Sequence_Container<T,N>::operator = (const Sequence_Container<T,N> & another)
    {
        this->data = another.data;
    }
}

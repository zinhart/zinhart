#ifndef DECLARATIONS_H
#define DECLARATIONS_H
#include <string>
#include <type_traits>
#include<iterator>
#include <memory>
//file has all class/struct/function declarations
namespace zinhart
{
    //typedefs
    typedef long long unsigned int lluint;
    //ptrdiff max is 2 bill min is - 2 billion, maybe provide a typedef to reflect big int    
    //alias templates
    //Structs
	//template<class Arg1,class Arg2,class Result,typename = RequireSimilarTypes<Result,Arg1,Arg2>  >
	  //struct binary_predicate;
	template<class Container>
	  struct scalar_addition;
	template<class Container>
	  struct scalar_multiplication;
	template<class Container>
	  struct matrix_subtraction;
	template<class Container>
	  struct matrix_sum;
	template<class Container>
	  struct hadamard_product;
	template<class Container>
	  struct matrix_product;
	template<class Container>
	  class null_functor;
    //Classes
    template <class T,class Allocator>
    class vector_space_base;
    template <class T,lluint N,template<typename> class Add = zinhart::null_functor,template <typename> class Multiply = zinhart::null_functor,class Allocator = std::allocator<T>>
    class vector_space;
    template<class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    class vector_space<T,1,Add,Multiply,Allocator>;
    template<class T, template<typename> class Add,template <typename> class Multiply>
    class vector_space<T,0,Add,Multiply>;
    template <class T,lluint N,template<typename> class Add = zinhart::null_functor,template <typename> class Multiply = zinhart::null_functor,class Allocator = std::allocator<T>>
    class sub_space;
    template<class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    class sub_space<T,1,Add,Multiply,Allocator>;
    template <class T>
    class sub_space<T,0>{};
    template<typename>
    class allocator;
    template<>
    class allocator<void>;
    template<class Iterator, class Container>
    class generic_iterator;
    template <class T,lluint N>
    class Sequence_Container;
    template<class T, lluint D>
    class Ndimensions_initializerlist_traits;
    template <class T>
    class Ndimensions_initializerlist_traits<T,0>;

    template<class T, lluint ... D>
    class Ndimensions_initializerlist;
    template<class T,lluint First ,lluint ... Rest>
    class Ndimensions_initializerlist<T,First,Rest...>;
}//end tool
#endif // DECLARATIONS_H

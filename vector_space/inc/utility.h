#ifndef UTILITY_H
#define UTILITY_H
#include <type_traits>
#include <iterator>
#include <functional>
#include <algorithm>
#include <numeric> // accumulate
#include <iostream>
#include <cassert>
using namespace std;
namespace zinhart
{
  #if __cplusplus < 201703L
  template <class T>
	T  gcd(T &a,T &b){return (b==T()) ? a : gcd(b, a % b);}
  template <class T>
	T lcm(T &a,T &b){return std::abs(a*b)/gcd(a,b);}
  #endif

  //General functions
  template<class Base,class Exp, typename = RequireUnsignedWhole<void,Base,Exp>>
	Base modular_exponentiation(Base base,Exp exp)
	{
	  Base result = 1;
	  while (exp)
	  {
		if (exp & 1)
		  result *= base;
		exp >>= 1;
		base *= base;
	  }
	  return result;
	}
  template<class Integer,typename = RequireUnsignedWhole<void,Integer,Integer> >
	bool is_prime(const Integer & number)
	{
	  if(number < 2)
		return false;
	  if(number==2)
		return true;
	  if(number % 2 ==0)
		return false;
	  for(Integer i = 3; (i*i) <= number; i+=2)
	{
	  if(number % i == 0)
		return false;
	}
	return true;
  }
  //difference between null and std::reference_wrapper is checking the state of the underlying pointer, i.e checking if there is a null reference
  template <class T>
   class null_reference
   {
	 public:
	   typedef T type;
	   null_reference(){ptr = nullptr;}
	   null_reference(T& ref) noexcept : ptr(std::addressof(ref)) {}
       null_reference(T&&) = delete;
       null_reference(const null_reference&) noexcept = default;
	   null_reference& operator=(const null_reference& x) noexcept = default; 
	   bool is_nullptr()const{return ptr == nullptr;}
	   operator T& () const noexcept {assert(ptr != nullptr); return  *ptr; }
       T & get() const noexcept {assert(ptr != nullptr); return *ptr; }
#if __cplusplus >= 201703L
	   explicit operator bool()const noexcept{return ptr == nullptr;}
	   template< class... ArgTypes >
		 typename std::result_of<T&(ArgTypes&&...)>::type operator() ( ArgTypes&&... args ) const {zassert(ptr != nullptr);return std::invoke(get(), std::forward<ArgTypes>(args)...);}  
#endif
	 private:
	   T* ptr;

   };
template <class ForwardIteratorIt1,class ForwardIteratorIt2,class RetVal,class BinaryFunction>
  RetVal accumulate(ForwardIteratorIt1 first_1,ForwardIteratorIt1 last_1,ForwardIteratorIt2 first_2,RetVal && ret,BinaryFunction f)
  {
	for(; first_1 != last_1; ++first_1, ++first_2)
	  ret += f(*first_1, *first_2);
	return ret;
  }
template <class ForwardIteratorIt1,class ForwardIteratorIt2,class RetVal,class BinaryFunction1,class BinaryFunction2>
  RetVal accumulate(ForwardIteratorIt1 first_1,ForwardIteratorIt1 last_1,ForwardIteratorIt2 first_2,RetVal && ret,BinaryFunction1 f_1,BinaryFunction2 f_2)
  {
	for(; first_1 != last_1; ++first_1, ++first_2)
	  ret += f_2(f_1(*first_1, *first_2),f_1(*first_1, *first_2));
	return ret;
  }
template<class InputIt1,class InputIt2,class BinaryFunction>
  void for_each(InputIt1 first_1,InputIt1 last_1,InputIt2 first_2,BinaryFunction f )
  {
	for(; first_1 != last_1; ++first_1, ++first_2)
	    *first_1 = f(*first_1, *first_2);
  }
template<class Container>
  struct null_functor
  {
	ZINHART_CONSTEXPR Container  operator()(const Container & lhs,const Container & rhs)const
	{
	  return lhs;
	}
  };
template<class Container>
  struct scalar_addition 
  {
	
	ZINHART_CONSTEXPR Container  operator()(const Container & lhs,const Container & rhs)const
	{
	  Container ret(lhs);
	  zinhart::for_each(ret.begin(),ret.end(),rhs.cbegin(),[](typename Container::reference init_1,typename Container::const_reference init_2){return init_1 + init_2;} );	
	  return ret;
	}

  };
template<class Container>
  struct scalar_multiplication
  {
	ZINHART_CONSTEXPR Container  operator()(const Container & lhs,const Container & rhs)const
	{
	  Container ret(lhs);
	  zinhart::for_each(ret.begin(),ret.end(),rhs.cbegin(),[](typename Container::reference init_1,typename Container::const_reference init_2){return init_1 * init_2;} );	
	  return ret;
	}
  };
template<class Container,class Row , class Col> // since type deduction occurs && can bind to r/l values
  void transpose(Container & vs,/*typename Container::size_type &&*/  Row && row, /*typename Container::size_type &&*/ Col && col)
  {
	typedef typename Container::size_type stype;
	static_assert( (std::is_convertible<Row,lluint>::value == true) && (std::is_convertible<Col,lluint>::value == true), "YOU CAN ONLY PASS INTEGERS FOR ROWS AND COLS TO TRANSPOSE" );
	//typedef typename Container::value_type vtype;
	Container vs_temp;
	vs_temp.reshape(col,row);
	vs_temp.resize(0);
  	if(row == col)
	{
	  for(stype i = 0; i<row;++i)
		for(stype j =0; j < col;++j)
		{
		  vs_temp.emplace_back(vs[j][i]);
		}
	}
	else
	{ 
	  for(stype i = 0; i<col;++i)
		for(stype j =0; j< row;++j)
		{
		  vs_temp.emplace_back(vs[j][i]);
		}
	}	
	vs = std::move(vs_temp);
  }
template<class Container>
  struct matrix_subtraction
  {
	ZINHART_CONSTEXPR Container  operator()(const Container & lhs, const Container & rhs)
	{
	  typedef typename Container::size_type stype;
	  Container ret;
	  ret.reshape(lhs.dimensions());
	  const stype lhs_size = lhs.dimensions().size();
	  const stype rhs_size = rhs.dimensions().size();
	  const stype lhs_1 = lhs.dimensions()[lhs_size - 2];//rows
	  const stype lhs_2 = lhs.dimensions()[lhs_size - 1];//cols
	  const stype rhs_1 = rhs.dimensions()[rhs_size - 2];//rows
	  const stype rhs_2 = rhs.dimensions()[rhs_size - 1];//cols
	  assert( (lhs_1 == rhs_1) && (lhs_2 == rhs_2) );
  	  ret.resize(0);
	  //ret.clear();
	  for(stype i = 0; i < lhs.size();++i)
  	  {
		ret.emplace_back(*(lhs.cbegin() + i) - *(rhs.cbegin() + i));
	  }
	  return ret;	
	}

  };
template<class Container>
  struct matrix_sum
  {
	//static_assert(std::is_base_of<zinhart::vector_space_base,Container>::value,"matrix_addition is only to be used with vector space containing vectors rank >= 2 (^_^)");
	ZINHART_CONSTEXPR Container  operator()(const Container & lhs, const Container & rhs)
	{
	  typedef typename Container::size_type stype;
	  Container ret;
	  ret.reshape(lhs.dimensions());
	  const stype lhs_size = lhs.dimensions().size();
	  const stype rhs_size = rhs.dimensions().size();
	  const stype lhs_1 = lhs.dimensions()[lhs_size - 2];//rows
	  const stype lhs_2 = lhs.dimensions()[lhs_size - 1];//cols
	  const stype rhs_1 = rhs.dimensions()[rhs_size - 2];//rows
	  const stype rhs_2 = rhs.dimensions()[rhs_size - 1];//cols
	  assert( (lhs_1 == rhs_1) && (lhs_2 == rhs_2) );
  	  ret.resize(0);
	  //ret.clear();
	  for(stype i = 0; i < lhs.size();++i)
  	  {
		ret.emplace_back(*(lhs.cbegin() + i) + *(rhs.cbegin() + i));
	  }
	  return ret;	
	}

  };
template <class Container>
  struct hadamard_product
  {
	ZINHART_CONSTEXPR Container operator()(const Container & lhs, const Container & rhs)const
	{
	  typedef typename Container::size_type stype;
	  typedef typename Container::const_iterator citer;
	  typedef typename Container::iterator iter;
	  stype lhs_row = lhs.dimensions()[0];
	  stype lhs_col = lhs.dimensions()[1];
	  stype rhs_row = lhs.dimensions()[0];
	  stype rhs_col = lhs.dimensions()[1];
	  assert(lhs_row == rhs_row);
	  assert(lhs_col == rhs_col);
	  assert(lhs.size() == rhs.size());
	  assert(lhs.capacity() == rhs.capacity());
	  Container ret;
	  ret.reshape(lhs.dimensions());
	  iter ret_begin;
	  citer lhs_begin, rhs_begin;
	  for(ret_begin = ret.begin(), lhs_begin = lhs.cbegin(), rhs_begin = rhs.cbegin(); lhs_begin != lhs.cend(); ++ret_begin, ++lhs_begin, ++rhs_begin)
	  {
		(*ret_begin) = (*lhs_begin) * (*rhs_begin);
	  }
	  return ret;
	}
  };
template<class Container>
  struct matrix_product
  {
	typedef typename Container::value_type vtype;
    typedef typename Container::scalar_addition sa;
	typedef typename Container::scalar_multiplication sm;
	typedef typename Container::size_type stype;
	private:
	  template<class SubContainer>
	  void do_multiplication(SubContainer && lhs,SubContainer  && rhs,Container & result)const
	  {
		stype lhs_row = *(lhs.dimensions().end()-2);
		stype rhs_row = *(rhs.dimensions().end()-2);
		stype rhs_col = *(rhs.dimensions().end()-1);
		for(stype i =0; i < lhs_row;++i)
		{
		  for(stype j = 0; j < rhs_col;++j)
		  {
			vtype sum = 0;
			for(stype k = 0; k < rhs_row;++k)
			{
			  sum += lhs[i][k] * rhs[k][j];
			}
			result.push_back(sum);
		  }
		}
	  }
	public:

	ZINHART_CONSTEXPR Container operator()(const Container & lhs, const Container & rhs)const
	{
	  //typedef typename Container::iterator citer;//container iterator
	  typename std::remove_const<Container>::type ret;
	  stype lhs_size = lhs.dimensions().size();
	  stype rhs_size = rhs.dimensions().size();
	  stype lhs_1 = lhs.dimensions()[lhs_size - 2];// M
	  stype lhs_2 = lhs.dimensions()[lhs_size - 1];// N
	  stype rhs_1 = rhs.dimensions()[rhs_size - 2];// N
	  stype rhs_2 = rhs.dimensions()[rhs_size - 1];// P
	  assert(lhs_2 == rhs_1); 
  	  stype limit = 0;	
	  //preprocess new dimenstions for ret;
	  {
  		typedef typename decltype(lhs.dimensions())::iterator iter;
		iter begin_1,end_1;
		typename std::remove_const<decltype(lhs.dimensions())>::type dim = lhs.dimensions();
		begin_1 = dim.begin(), end_1 = dim.end() - 2;
		dim.assign(begin_1,end_1);
		dim.push_back(lhs_1);
		dim.push_back(rhs_2);
		ret.reshape(dim);
		ret.resize(0);
		limit = std::accumulate(dim.begin(),dim.end()-2,stype(1),std::multiplies<stype>());
	  }
	  typename std::remove_const<Container>::type lhs_copy = lhs;
	  typename std::remove_const<Container>::type rhs_copy = rhs;
	  //stype row = lhs_1, col = lhs_1;
	  stype lhs_start = 0,rhs_start = 0,stride = 1;
	  lhs_size = lhs_1 * lhs_2 ,rhs_size = rhs_1 * rhs_2 ;
	  for(stype i = 0; i < limit;++i,lhs_start +=lhs_1 * lhs_2,rhs_start += rhs_1 * rhs_2)
	  {	
		decltype(lhs_copy.slice(lhs_start,lhs_size,stride)) lhs_slice = lhs_copy.slice(lhs_start,lhs_size,stride );
		decltype(rhs_copy.slice(rhs_start,rhs_size,stride)) rhs_slice = rhs_copy.slice(rhs_start,rhs_size,stride);
		do_multiplication(vector_space<vtype,2>(lhs_slice.begin(),lhs_slice.end(),{lhs_1,lhs_2}),vector_space<vtype,2>(rhs_slice.begin(),rhs_slice.end(),{rhs_1,rhs_2}),ret); 
	  }
	  return ret;	
	}

  };
  template <class Type>
  void destroy(Type * pointer)
  {
	  pointer->~Type();
    }

    template <bool>
    struct destroy_helper
    {
        public:
            template<class ForwardIterator>
            static void destroy(ForwardIterator first, ForwardIterator last)
            {
                for(;first!=last;++first)
                    destroy(std::addressof(*first));
            }
    };
    template <>
    struct destroy_helper<true>
    {
        public:
            template<class ForwardIterator>
            static void destroy(ForwardIterator first, ForwardIterator Last)
            {}
    };
    template<class ForwardIterator>
    void destroy(ForwardIterator first,ForwardIterator last)
    {
        typedef typename std::iterator_traits<ForwardIterator>::value_type vtyp;
        destroy_helper<std::is_trivially_destructible<vtyp>::value>::destroy(first,last);
    }

    template<class ForwardIterator,class Allocator>
    void destroy(ForwardIterator first,ForwardIterator last,Allocator & alloc)
    {
        #if __cplusplus >= 201103L
            typedef std::allocator_traits<Allocator> alloc_traits;
            for(;first!=last;++first)
                alloc_traits::destroy(alloc,std::addressof(*first));
        #else
            //c++ 2003
        #endif
    }
    /*This is for non-default allocators,
    according to stl_construct.h still need to test if this applies me 4/27/2017 @ 3:31,
    very important!
    */
    template<class ForwardIterator,class Type>
    void destroy(ForwardIterator first,ForwardIterator last,allocator<Type> & alloc)
    {
        destroy(first,last);
    }
    template<class ForwardIterator, class Size,class Type,class Allocator>
    ForwardIterator uninitialized_fill_nvals_allocator(ForwardIterator first, Size size, const Type &value, Allocator &allocator)
    {
        ForwardIterator temp=first;
        ZINHART_TRY
        {
            #if __cplusplus >= 201103L
            typedef std::allocator_traits<Allocator> alloc_traits;
            for(; size > 0; --size,++temp)
                 alloc_traits::construct(allocator,std::addressof(*temp),value);
            return temp;
            #else

            #endif

        }
        ZINHART_CATCH(...)
        {
            #if __cplusplus >= 201103L
            ///typedef std::allocator_traits<Allocator> alloc_traits;
            destroy(first,temp,allocator);
            ZINHART_THROW_EXCEPTION_AGAIN;
            #else
                    //c++ 2003
            #endif
        }

    }

    template<class ForwardIterator,class InputIterator,class Allocator>
    ForwardIterator uninitialized_copy_allocator(InputIterator begin, InputIterator end, ForwardIterator result,Allocator & allocator)
    {
        ForwardIterator temp = result;
        typedef std::allocator_traits<Allocator> traits;
        ZINHART_TRY
        {
            for(;begin != end; ++begin, ++temp)
            {
                traits::construct(allocator,std::addressof(*temp),*begin);
            }
            return temp;
        }
        ZINHART_CATCH(...)
        {
            destroy(result,temp,allocator);
            ZINHART_THROW_EXCEPTION_AGAIN;
        }
    }

    template<class ForwardIterator,class InputIterator,class Allocator>
    ForwardIterator uninitialized_move_ifnoexcept_allocator(InputIterator begin, InputIterator end, ForwardIterator result,Allocator & allocator)
    {
        return uninitialized_copy_allocator(ZINHART_MAKE_MOVE_ITERATOR_IF_NO_EXCEPT(begin),ZINHART_MAKE_MOVE_ITERATOR_IF_NO_EXCEPT(end),result,allocator);
    }

    template<class ForwardIterator,class InputIterator,class Allocator>
    ForwardIterator uninitialized_move_allocator(InputIterator begin, InputIterator end, ForwardIterator result,Allocator & allocator)
    {
        return uninitialized_copy_allocator(begin,end,result,allocator);
    }

    template<class ForwardIterator,typename Size,typename Allocator>
    ForwardIterator uninitialized_default_nvals_allocator(ForwardIterator first,Size size,Allocator & alloc)
    {
        ForwardIterator temp = first;
        typedef std::allocator_traits<Allocator> traits;
        ZINHART_TRY
        {
            for(;size>0;--size,++temp)
            {
                traits::construct(alloc,std::addressof(*temp));
            }
            return temp;
        }
        ZINHART_CATCH(...)
        {
            destroy(first,temp,alloc);
            ZINHART_THROW_EXCEPTION_AGAIN;
        }

    }
    #if __cplusplus >= 201103L
    template<typename T, bool = or_<std::is_copy_constructible<typename T::value_type>,std::is_nothrow_move_constructible<typename T::value_type>>::value>
    struct generic_shrink_to_fit_helper {static bool shrink(T&)noexcept { return false;}};

    template<typename T>
    struct generic_shrink_to_fit_helper<T,true>
    {
      static bool shrink(T & container)
      {
          ZINHART_TRY
          {

              T (ZINHART_MAKE_MOVE_ITERATOR_IF_NO_EXCEPT(container.begin()),
                ZINHART_MAKE_MOVE_ITERATOR_IF_NO_EXCEPT(container.end()),
                container.get_allocator()).swap(container);
              return true;
          }
          ZINHART_CATCH(...)
          { return false; }
      }
    };
	template<typename T, bool = or_<std::is_copy_constructible<typename T::value_type>,std::is_nothrow_move_constructible<typename T::value_type>>::value>
	  struct vector_space_shrink_to_fit_helper {static bool shrink(T&)noexcept { return false;}};

    template<typename T>
	  struct vector_space_shrink_to_fit_helper<T,true>
	  {
		static bool shrink(T & vs)
		{
          ZINHART_TRY
          {
		  	T (ZINHART_MAKE_MOVE_ITERATOR_IF_NO_EXCEPT(vs.begin()),
                ZINHART_MAKE_MOVE_ITERATOR_IF_NO_EXCEPT(vs.end()),
				vs.dimensions(),
                vs.get_allocator()).swap(vs);
              return true;
          }
          ZINHART_CATCH(...)
          { return false; }
		}
	  };
    template <typename Alloc>
    struct allocate_on_move
    {
        static void do_allocate_on_move(Alloc & one, Alloc & two, std::true_type)
        {
            one = std::move(two);
        }
        static void do_allocate_on_move(Alloc & one, Alloc & two, std::false_type)
        { }
    };
    template <typename Alloc>
    struct allocate_on_copy
    {
        static void do_allocate_on_copy(Alloc & one, Alloc & two, std::true_type)
        {
            one = two;
        }
        static void do_allocate_on_copy(Alloc & one, Alloc & two, std::false_type)
        { }

    };
    template <typename Alloc>
    struct allocate_on_swap
    {
        static void do_allocate_on_swap(Alloc & one, Alloc & two, std::true_type)
        {
           std::swap(one,two);
        }
        static void do_allocate_on_swap(Alloc & one, Alloc & two, std::false_type)
        { }
    };

    #endif

}
#endif

#ifndef ZINHART_TRAITS_H
#define ZINHART_TRAITS_H
#include <type_traits>
namespace zinhart
{
    //implementation of or
    template <typename ...>
    struct or_;

    template<>
    struct or_<> : public std::false_type{};

    template<typename T1>
    struct or_<T1> : public T1 {};

    template<typename T1,typename T2>
    struct or_<T1,T2> : public std::conditional<T1::value,T1,T2>::type {};

    template <typename T1,typename T2, typename T3,typename ... Tn>
    struct or_<T1,T2,T3,Tn...> : public std::conditional<T1::value,T1 ,or_<T2,T3,Tn...>>::type {};

    //implementation of and
    template <typename ...>
    struct and_;

    template<>
    struct and_<> :public std::true_type{};

    template<typename T1>
    struct and_<T1> : public T1 {};

    template<typename T1,typename T2>
    struct and_<T1,T2> : public std::conditional<T1::value,T2,T1>::type {};

    template <typename T1,typename T2, typename T3,typename ... Tn>
    struct and_<T1,T2,T3,Tn...> : public std::conditional<T1::value, and_<T2,T3,Tn...>,T1>::type {};


    //implementation of not
    template<typename T>
    struct not_
    : public std::integral_constant<bool, !T::value>
    {};/**/

    //implementation of move_if_noexcept_cond
    template<class T>
    struct my_move_if_noexcept_cond : public and_<not_<std::is_nothrow_move_constructible<T>>,std::is_copy_constructible<T>>::type{};

    template<typename T>
    constexpr typename
    std::conditional<my_move_if_noexcept_cond<T>::value,const T&,T&&>::type my_move_if_noexcept_cond_func(T & ref) ZINHART_NOEXCEPT
    {
        return std::move(ref);
    }/**/

    //implementation of make move iterator
    template<class Iterator,class Return_Type = typename std::conditional<my_move_if_noexcept_cond
                                                           <typename std::iterator_traits<Iterator>::value_type>::value,
                                                           Iterator,std::move_iterator<Iterator>>::type>
    Return_Type my_make_move_if_noexcept_iterator(Iterator iter)
    {
            return Return_Type(iter);
    }/**/

    #if __cplusplus >= 201103L
    #define ZINHART_MAKE_MOVE_ITERATOR_IF_NO_EXCEPT(Iter) my_make_move_if_noexcept_iterator(Iter)
    #else
    #define ZINHART_MAKE_MOVE_ITERATOR_IF_NO_EXCEPT(Iter)
    #endif


    template <class InputIterator>
	  using RequireAnInputIter = typename std::enable_if<std::is_convertible< typename std::iterator_traits<InputIterator>::iterator_category,std::input_iterator_tag  >::value >::type;
    template <class ReturnType,class UnsignedReal,class ...UnsignedIntegers>
	  using RequireUnsignedReal = typename std::enable_if<and_<std::is_convertible<UnsignedReal,UnsignedIntegers>...>::value,ReturnType>::type;
    template <class ReturnType,class UnsignedWhole,class ...UnsignedWholeNumbers>
	  using RequireUnsignedWhole = typename std::enable_if<and_<std::is_convertible<UnsignedWhole,UnsignedWholeNumbers>...>::value,ReturnType>::type;
	template<class ReturnType,class T1,class...TN>
	  using RequireSimilarTypes = typename std::enable_if<and_<std::is_convertible<T1,TN>...>::value,ReturnType>::type;
	template <class ReturnType, class T1, class ...TN>
	  using RequireSameTypes = typename std::enable_if<and_<std::is_same<T1,TN>...>::value, ReturnType>::type;
    template <class True,class Unknown>
	  using RequireTrue = typename std::enable_if<and_<True,Unknown>::value>::type;
    template <class False,class Unknown>
	  using RequireFalse = typename std::enable_if<!and_<False,Unknown>::value>::type;
		//template <typename >
	///ADD RVALUE
	template<class T, class Add>
	 struct has_operator_addition_impl_r
	 {
	   template<class U, class V>
		 static auto test(U*) -> decltype(std::declval<U>() + std::declval<V&&>());
	   template<typename, typename>
		 static auto test(...) -> std::false_type;
  	   using type = typename std::is_same<T, decltype(test<T, Add>(0))>::type;
	 };
   	///ADD CONST REFERENCE
    template<class T, class Add>
	  struct has_operator_addition_impl_const_r
	  {
		template<class U, class V>
		  static auto test(U) -> decltype(std::declval<U>() + std::declval<const V &>());
		template<typename, typename>
		  static auto test(...) -> std::false_type;
		using type = typename std::is_same<T&, decltype(test<T, Add>(0))>::type;
	  };

	///MULTIPLY RVALUE
	template<class T, class Multiply>
	 struct has_operator_multiply_impl_r
	 {
	   template<class U, class V>
		 static auto test(U*) -> decltype(std::declval<U>() + std::declval<V&&>());
	   template<typename, typename>
		 static auto test(...) -> std::false_type;
  	   using type = typename std::is_same<T, decltype(test<T, Multiply>(0))>::type;
	 };
   	///MULTIPLY CONST REFERENCE
    template<class T, class Multiply>
	  struct has_operator_multiply_impl_const_r
	  {
		template<class U, class V>
		  static auto test(U) -> decltype(std::declval<U>() + std::declval<const V &>());
		template<typename, typename>
		  static auto test(...) -> std::false_type;
		using type = typename std::is_same<T&, decltype(test<T, Multiply>(0))>::type;
	  };

	/*//metaprogramming for nested initializer lists
	template<class T,lluint levels>
	  struct nested_ilist_trait
	  {
		using type = std::initializer_list<typename nested_ilist_trait<T,levels -1>::type>;
	  };
	template<class T>
	  struct nested_ilist_trait<T,0>
	  {
		using type = T;
	  };
	template<class T,lluint levels>
	  using nested_ilist = typename nested_ilist_trait<T,levels>::type;
	template<class T,lluint ...N>
	  struct nested_ilist_contents;
	template<class T, lluint First,lluint ...Rest>
	  struct nested_ilist_contents<T,First,Rest...> ;
	template<typename T,lluint Last>
	  struct nested_ilist_contents<T,Last>;*/
    //original
    //using RequireUnsignedIntegers = typename std::enable_if<and_<std::is_same<int,UnsignedIntegers>...>::value,ReturnType>::type;
}
#endif

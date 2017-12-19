#ifndef ITERATOR_H
#define ITERATOR_H
#include "declarations.h"
namespace zinhart
{
    template<class Iterator, class Container>
    class generic_iterator
    {
        protected:
            Iterator current;
            typedef std::iterator_traits<Iterator> traits_type;
        public:
            typedef Iterator iterator_type;
            typedef typename traits_type::iterator_category iterator_category;
            typedef typename traits_type::value_type value_type;
            typedef typename traits_type::difference_type difference_type;
            typedef typename traits_type::reference reference;
            typedef typename traits_type::pointer pointer;
            ZINHART_CONSTEXPR generic_iterator()ZINHART_NOEXCEPT;
            explicit generic_iterator(const Iterator &iter)ZINHART_NOEXCEPT;
            template <class Iter>
            generic_iterator(const generic_iterator<Iter,typename std::enable_if<(std::is_same<Iter,typename Container::pointer >::value),Container>::type> & iter)ZINHART_NOEXCEPT;
            //template<typename _Iter>
            //__normal_iterator(const __normal_iterator<_Iter,typename __enable_if<(std::__are_same<_Iter, typename _Container::pointer>::__value),Container>::__type>& __i) _GLIBCXX_NOEXCEPT
            reference operator *()const ZINHART_NOEXCEPT;
            pointer operator->() const ZINHART_NOEXCEPT;
            generic_iterator & operator ++()ZINHART_NOEXCEPT;
            generic_iterator & operator ++(int)ZINHART_NOEXCEPT;
            generic_iterator & operator --()ZINHART_NOEXCEPT;
            generic_iterator & operator --(int)ZINHART_NOEXCEPT;
            reference operator [](difference_type diff)const ZINHART_NOEXCEPT;
            generic_iterator & operator += (difference_type diff) ZINHART_NOEXCEPT;
            generic_iterator & operator -= (difference_type diff) ZINHART_NOEXCEPT;
            generic_iterator operator + (difference_type diff) const ZINHART_NOEXCEPT;
            generic_iterator  operator - (difference_type diff) const ZINHART_NOEXCEPT;
            const Iterator & base() const ZINHART_NOEXCEPT;
    };
    template <class Iterator_Left,class Iterator_Right, class Container>
    inline bool operator == (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)ZINHART_NOEXCEPT;
    template <class Iterator, class Container>
    inline bool operator == (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT;

    template <class Iterator_Left,class Iterator_Right, class Container>
    inline bool operator != (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)ZINHART_NOEXCEPT;
    template <class Iterator, class Container>
    inline bool operator != (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT;

    template <class Iterator_Left,class Iterator_Right, class Container>
    inline bool operator < (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)ZINHART_NOEXCEPT;
    template <class Iterator, class Container>
    inline bool operator < (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT;

    template <class Iterator_Left,class Iterator_Right, class Container>
    inline bool operator > (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)ZINHART_NOEXCEPT;
    template <class Iterator, class Container>
    inline bool operator > (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT;

    template <class Iterator_Left,class Iterator_Right, class Container>
    inline bool operator <= (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)ZINHART_NOEXCEPT;
    template <class Iterator, class Container>
    inline bool operator <= (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT;

    template <class Iterator_Left,class Iterator_Right, class Container>
    inline bool operator >= (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)ZINHART_NOEXCEPT;
    template <class Iterator, class Container>
    inline bool operator >= (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT;

    template <class Iterator_Left,class Iterator_Right, class Container>
    #if __cplusplus >= 20141215L
    inline auto operator - (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)noexcept  -> decltype(return lhs.base() - rhs.base(); );
    #elif __cplusplus >= 201103L
    inline typename generic_iterator<Iterator_Left,Container>::difference_type operator - (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs);
    #else
    #endif
    template <class Iterator, class Container>
    inline typename generic_iterator<Iterator,Container>::difference_type operator - (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT;

    template <class Iterator, class Container>
    inline generic_iterator<Iterator,Container> operator + (typename generic_iterator<Iterator,Container>::difference_type diff,const generic_iterator<Iterator,Container> & iter)ZINHART_NOEXCEPT;
}
#endif // ITERATOR_H

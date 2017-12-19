#include "../inc/iterator.h"
namespace zinhart
{
    template <class Iterator, class Container>
    ZINHART_CONSTEXPR generic_iterator<Iterator,Container>::generic_iterator()ZINHART_NOEXCEPT
        : current(Iterator()) { }

    template <class Iterator, class Container>
    generic_iterator<Iterator,Container>::generic_iterator(const Iterator &iter)ZINHART_NOEXCEPT
        : current(iter) { }

    template <class Iterator, class Container>
    template <class Iter>
    generic_iterator<Iterator,Container>::generic_iterator(const generic_iterator<Iter,typename std::enable_if<(std::is_same<Iter,typename Container::pointer >::value),Container>::type> & iter)ZINHART_NOEXCEPT
        :current( iter.base() ){}

    template <class Iterator, class Container>
    typename generic_iterator<Iterator,Container>::reference generic_iterator<Iterator,Container>::operator *()const ZINHART_NOEXCEPT
    {return * current;}

    template <class Iterator, class Container>
    typename generic_iterator<Iterator,Container>::pointer generic_iterator<Iterator,Container>::operator->() const ZINHART_NOEXCEPT
    {return current;}

    template <class Iterator, class Container>
    generic_iterator<Iterator,Container> & generic_iterator<Iterator,Container>::operator ++()ZINHART_NOEXCEPT
    {++current; return *this;}

    template <class Iterator, class Container>
    generic_iterator<Iterator,Container> & generic_iterator<Iterator,Container>::operator ++(int)ZINHART_NOEXCEPT
    {return generic_iterator(current++);}

    template <class Iterator, class Container>
    generic_iterator<Iterator,Container> & generic_iterator<Iterator,Container>::operator --()ZINHART_NOEXCEPT
    {--current; return *this;}

    template <class Iterator, class Container>
    generic_iterator<Iterator,Container> & generic_iterator<Iterator,Container>::operator --(int)ZINHART_NOEXCEPT
    {return generic_iterator(current--);}

    template <class Iterator, class Container>
    typename generic_iterator<Iterator,Container>::reference generic_iterator<Iterator,Container>::operator [](difference_type diff)const ZINHART_NOEXCEPT
    {return current[diff];}

    template <class Iterator, class Container>
    generic_iterator<Iterator,Container> & generic_iterator<Iterator,Container>::operator += (generic_iterator<Iterator,Container>::difference_type diff) ZINHART_NOEXCEPT
    {current += diff; return *this;}

    template <class Iterator, class Container>
    generic_iterator<Iterator,Container> & generic_iterator<Iterator,Container>::operator -= (difference_type diff) ZINHART_NOEXCEPT
    {current -= diff; return *this;}

    template <class Iterator, class Container>
    generic_iterator<Iterator,Container>  generic_iterator<Iterator,Container>::operator + (difference_type diff) const ZINHART_NOEXCEPT
    {return generic_iterator(current + diff);}

    template <class Iterator, class Container>
    generic_iterator<Iterator,Container>  generic_iterator<Iterator,Container>::operator - (difference_type diff) const ZINHART_NOEXCEPT
    {return generic_iterator(current - diff);}

    template <class Iterator, class Container>
    const Iterator & generic_iterator<Iterator,Container>::base() const ZINHART_NOEXCEPT
    {return current;}

    template <class Iterator_Left,class Iterator_Right, class Container>
    inline bool operator == (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() == rhs.base();}

    template <class Iterator, class Container>
    inline bool operator == (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() == rhs.base();}

    template <class Iterator_Left,class Iterator_Right, class Container>
    inline bool operator != (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() != rhs.base();}

    template <class Iterator, class Container>
    inline bool operator != (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() != rhs.base();}

    template <class Iterator_Left,class Iterator_Right, class Container>
    inline bool operator < (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() < rhs.base();}

    template <class Iterator, class Container>
    inline bool operator < (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() < rhs.base();}

    template <class Iterator_Left,class Iterator_Right, class Container>
    inline bool operator > (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() > rhs.base();}
    template <class Iterator, class Container>
    inline bool operator > (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() > rhs.base();}

    template <class Iterator_Left,class Iterator_Right, class Container>
    inline bool operator <= (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() <= rhs.base();}
    template <class Iterator, class Container>
    inline bool operator <= (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() <= rhs.base();}

    template <class Iterator_Left,class Iterator_Right, class Container>
    inline bool operator >= (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() >= rhs.base(); }
    template <class Iterator, class Container>
    inline bool operator >= (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() >= rhs.base(); }

    template <class Iterator_Left,class Iterator_Right, class Container>
    #if __cplusplus >= 20141215L
    inline auto operator - (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)noexcept -> decltype(return lhs.base() - rhs.base(); )
    #elif __cplusplus >= 201103L
    inline typename generic_iterator<Iterator_Left,Container>::difference_type operator - (const generic_iterator<Iterator_Left,Container> & lhs,const generic_iterator<Iterator_Right,Container> & rhs)
    #else
    #endif
    {return lhs.base() - rhs.base();}

    template <class Iterator, class Container>
    inline typename generic_iterator<Iterator,Container>::difference_type operator - (const generic_iterator<Iterator,Container> & lhs,const generic_iterator<Iterator,Container> & rhs)ZINHART_NOEXCEPT
    {return lhs.base() - rhs.base();}

    template <class Iterator, class Container>
    inline generic_iterator<Iterator,Container> operator + (typename generic_iterator<Iterator,Container>::difference_type diff,const generic_iterator<Iterator,Container> & iter)ZINHART_NOEXCEPT
    {return generic_iterator<Iterator,Container>(iter.base() + diff ); }

}

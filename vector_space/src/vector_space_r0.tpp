#include "../inc/vector_space.h"
#include <algorithm>
namespace zinhart
{
/*
-----------------------------------------------------------------------------
vector_space Rank 0(scalar)                                                                   |
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Constructors                                                                 |
-----------------------------------------------------------------------------
*/
    template<class T, template<typename> class Add,template <typename> class Multiply>
    vector_space<T,0,Add,Multiply>::vector_space()ZINHART_NOEXCEPT:_data(0){}
    template <class T,template<typename> class Add,template <typename> class Multiply>
    vector_space<T,0,Add,Multiply>::vector_space(vector_space &  vs)ZINHART_NOEXCEPT:_data( vs.get()){}
    template <class T,template<typename> class Add,template <typename> class Multiply>
    vector_space<T,0,Add,Multiply>::vector_space(vector_space &&  vs)ZINHART_NOEXCEPT:_data( vs._data){ vs._data=T();}
/*
-----------------------------------------------------------------------------
Destructor                                                                   |
-----------------------------------------------------------------------------
*/
    template <class T,template<typename> class Add,template <typename> class Multiply>
    vector_space<T,0,Add,Multiply>::~vector_space()ZINHART_NOEXCEPT{}
/*
-----------------------------------------------------------------------------
Modifiers                                                                    |
-----------------------------------------------------------------------------
 */
    template <class T,template<typename> class Add,template <typename> class Multiply>
    void vector_space<T,0,Add,Multiply>::set(const_reference value)ZINHART_NOEXCEPT
    {
        _data=value;
    }
    template <class T,template<typename> class Add,template <typename> class Multiply>
    void vector_space<T,0,Add,Multiply>::set(value_type && value)ZINHART_NOEXCEPT
    {
        _data=value;
    }
    template <class T,template<typename> class Add,template <typename> class Multiply>
    vector_space<T,0,Add,Multiply> & vector_space<T,0,Add,Multiply>::operator =(const vector_space & vs)ZINHART_NOEXCEPT
    {
        vector_space<T,0,Add,Multiply> tmp(vs);
        *this=std::move(tmp);
        return *this;
    }
    template <class T,template<typename> class Add,template <typename> class Multiply>
    vector_space<T,0,Add,Multiply> & vector_space<T,0,Add,Multiply>::operator =(vector_space &&  vs) ZINHART_NOEXCEPT
    {
        _data= vs._data;
         vs._data=T();
        return *this;
    }
    template <class T,template<typename> class Add,template <typename> class Multiply>
    void vector_space<T,0,Add,Multiply>::swap(vector_space &  vs)ZINHART_NOEXCEPT
    {
        std::swap(_data, vs._data);
    }

/*
-----------------------------------------------------------------------------
Accessors                                                                    |
-----------------------------------------------------------------------------
*/
    template <class T,template<typename> class Add,template <typename> class Multiply>
    typename vector_space<T,0,Add,Multiply>::const_reference vector_space<T,0,Add,Multiply>::get()const ZINHART_NOEXCEPT
    {
        return _data;
    }
/*
-----------------------------------------------------------------------------
Non-Member Functions                                                         |
-----------------------------------------------------------------------------
*/
    template <class T,template<typename> class Add,template <typename> class Multiply>
    std::ostream & operator <<(std::ostream & os, const vector_space<T,0,Add,Multiply> & vs)
    {
        os<< vs.get()<<"\n";
        return os;
    }
    template <class T,template<typename> class Add,template <typename> class Multiply>
    inline bool operator ==(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs)
    {
        return lhs.get()==rhs.get();

        //return (lhs.size() == rhs.size() && std::equal(lhs.begin(),lhs.end(),rhs.begin()));
    }
    template <class T,template<typename> class Add,template <typename> class Multiply>
    inline bool operator !=(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs)
    {
        return !(lhs == rhs);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply>
    inline bool operator <(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs)
    {
        return lhs < rhs;
        //return std::lexicographical_compare(lhs.begin(),lhs.end(),rhs.begin(),rhs.end());
    }
    template <class T,template<typename> class Add,template <typename> class Multiply>
    inline bool operator <=(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs)
    {
        return !(rhs < lhs);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply>
    inline bool operator >(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs)
    {
        return rhs < lhs;
    }
    template <class T,template<typename> class Add,template <typename> class Multiply>
    inline bool operator >=(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs)
    {
        return !(lhs < rhs);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply>
    inline void swap(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs)
    {
        lhs.swap(rhs);
    }
}

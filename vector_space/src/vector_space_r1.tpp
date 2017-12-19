#include "../inc/vector_space.h"
#include <algorithm>
#include <numeric>
#include <memory>
#include <limits>
namespace zinhart
{
/*
-----------------------------------------------------------------------------
vector_space Rank 1                                                          |
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
Internal Functions                                                           |
-----------------------------------------------------------------------------
*/
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    void vector_space<T,1,Add,Multiply,Allocator>::default_initialize(const size_type & size)
    {
        this->memory_cache.end = zinhart::uninitialized_default_nvals_allocator(this->memory_cache.begin,size,this->get_memory_cache_alloc_type());
    }

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    void vector_space<T,1,Add,Multiply,Allocator>::fill_initialize(const size_type & size, const_reference value)
    {
        this->memory_cache.end = zinhart::uninitialized_fill_nvals_allocator(this->memory_cache.begin,size,value,this->get_memory_cache_alloc_type());
    }

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class Integer>
    void vector_space<T,1,Add,Multiply,Allocator>::initialize_dispatcher(Integer size, const_reference value , std::true_type)
    {
        this->memory_cache.end_of_cache = this->memory_cache.begin + static_cast<size_type>(size);
        fill_initialize(static_cast<size_type>(size),value);
    }

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class InputIterator>
    void vector_space<T,1,Add,Multiply,Allocator>::initialize_dispatcher(InputIterator first, InputIterator last , std::false_type)
    {
        typedef typename std::iterator_traits<InputIterator>::iterator_category iter_category;
        range_initialize(first,last,iter_category());
    }

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class InputIterator>
    void vector_space<T,1,Add,Multiply,Allocator>::range_initialize(InputIterator first,InputIterator last,std::input_iterator_tag)
    {
        for(;first!=last;++first)
        #if __cplusplus >= 201103L
             emplace_back(*first);
        #else
            push_back(*first);
        #endif
    }

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class ForwardIterator>
    void vector_space<T,1,Add,Multiply,Allocator>::range_initialize(ForwardIterator first,ForwardIterator last,std::forward_iterator_tag)
    {
        const size_type size= std::distance(first,last);
        this->memory_cache.begin = this->allocate_cache_of_size(size);
        this->memory_cache.end_of_cache = this->memory_cache.begin + size;
        //my implementation of std::__initialized_copy_a
        this->memory_cache.end = zinhart::uninitialized_copy_allocator(first,last,this->memory_cache.begin,this->get_memory_cache_alloc_type());

    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class Integer>
    void vector_space<T,1,Add,Multiply,Allocator>::assign_dispatcher(Integer size,const_reference values,std::true_type)
    {
        //fill assign
        fill_assign(size,values);
    }

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class InputIterator>
    void vector_space<T,1,Add,Multiply,Allocator>::assign_dispatcher(InputIterator first, InputIterator last,std::false_type)
    {
        typedef typename std::iterator_traits<InputIterator>::iterator_category iter_category;
        assign_helper(first,last,iter_category());
    }

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    void vector_space<T,1,Add,Multiply,Allocator>::fill_assign(size_type & size, const_reference value)
    {
        if(size > this->capacity())
        {
            vector_space<T,1,Add,Multiply,Allocator> temp(size,value,get_memory_cache_alloc_type());
            temp.memory_cache.memory_cache_swap_data(this->memory_cache);
        }
        else if (size > this->size())
        {
            std::fill(begin(),end(),value);
            this->memory_cache.end = zinhart::uninitialized_fill_nvals_allocator(this->memory_cache.begin,size - this->size(),value,this->get_memory_cache_alloc_type());
        }
        else
            erase_at_end(std::fill_n(this->memory_cache.begin,size,value));
    }

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class InputIterator>
    void vector_space<T,1,Add,Multiply,Allocator>::assign_helper(InputIterator first, InputIterator last , std::input_iterator_tag)
    {
        pointer current(this->memory_cache.begin);
		typedef typename std::iterator_traits<InputIterator>::iterator_category iter_category;
        for(; first != last && current != this->memory_cache.end; ++current,++first)
          *current = *first;
        if(first == last)
            erase_at_end(current);
        else
            range_insert(end(),first,last,iter_category());
    }

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class ForwardIterator>
    void vector_space<T,1,Add,Multiply,Allocator>::assign_helper(ForwardIterator first, ForwardIterator last,std::forward_iterator_tag)
    {
        const size_type length = std::distance(first,last);
        if(length > capacity())
        {
            pointer temp(allocate_and_copy(length,first,last));
            zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
            deallocate_cache_of_size(this->memory_cache.begin,this->memory_cache.end_of_cache - this->memory_cache.begin);
            this->memory_cache.begin = temp;
            this->memory_cache.end = this->memory_cache.begin + length;
            this->memory_cache.end_of_cache = this->memory_cache.end;
        }
        else if(this->size() >= length)
            erase_at_end(std::copy(first, last, this->memory_cache.begin));
        else
        {
            ForwardIterator mid = first;
            std::advance(mid,this->size());
            std::copy(first,mid,this->memory_cache.begin);
            this->memory_cache.end = zinhart::uninitialized_copy_allocator(mid,last,this->memory_cache.end,get_memory_cache_alloc_type());
        }
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class Integer>
    void vector_space<T,1,Add,Multiply,Allocator>::insert_dispatcher(iterator pos, const_reference value, Integer size,std::true_type)
    {
        //fill insert
        fill_insert(pos,static_cast<size_type>(size),value);

    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class InputIterator>
    void vector_space<T,1,Add,Multiply,Allocator>::insert_dispatcher(iterator pos,InputIterator first, InputIterator last , std::false_type)
    {
        typedef typename std::iterator_traits<InputIterator>::iterator_category iter_category;
        range_insert(pos,first,last,iter_category());
    }

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    void vector_space<T,1,Add,Multiply,Allocator>::fill_insert(iterator pos, const size_type & size,const_reference value)
    {
         if(size != 0)
         {
             if(size_type(this->memory_cache.end_of_cache - this->memory_cache.end) >= size)
             {
                 value_type value_copy = value;
                 const size_type elems_after = end() - pos;
                 pointer old_end(this->memory_cache.end);
                 if(elems_after > size)
                 {
                     zinhart::uninitialized_move_allocator(this->memory_cache.end - size,this->memory_cache.end,this->memory_cache.end,this->get_memory_cache_alloc_type());
                     this->memory_cache.end += size;
                     ZINHART_MOVE_BACKWARD3(pos.base(), old_end - size, old_end);
                     std::fill(pos.base(),pos.base() + size,value_copy);
                 }
                 else
                 {
                     this->memory_cache.end = zinhart::uninitialized_fill_nvals_allocator(this->memory_cache.end,size - elems_after,value_copy,this->get_memory_cache_alloc_type());
                     zinhart::uninitialized_move_allocator(pos.base(),old_end,this->memory_cache.end,this->get_memory_cache_alloc_type());
                 }
             }
             else
             {
                 const size_type length = check_length(size);
                 const size_type elems_before = pos - this->begin();
                 pointer new_begin(this->allocate_cache_of_size(length));
                 pointer new_end(new_begin);
                 ZINHART_TRY
                 {
                     zinhart::uninitialized_fill_nvals_allocator(new_begin + elems_before,size,value,this->get_memory_cache_alloc_type());
                     new_end = pointer();
                     new_end = zinhart::uninitialized_move_ifnoexcept_allocator(this->memory_cache.begin,pos.base(),new_begin,this->get_memory_cache_alloc_type());
                     new_end += size;
                     new_end = zinhart::uninitialized_move_ifnoexcept_allocator(pos.base(),this->memory_cache.end,new_end,this->get_memory_cache_alloc_type());
                 }
                 ZINHART_CATCH(...)
                 {
                     if(!new_end)
                         zinhart::destroy(new_begin + elems_before, new_begin + elems_before + size,this->get_memory_cache_alloc_type());
                     else
                         zinhart::destroy(new_begin, new_end,this->get_memory_cache_alloc_type());
                     this->deallocate_cache_of_size(new_begin,length);
                     ZINHART_THROW_EXCEPTION_AGAIN;
                 }
                 zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
                 deallocate_cache_of_size(this->memory_cache.begin, this->memory_cache.end_of_cache - this->memory_cache.begin);
                 this->memory_cache.begin = new_begin;
                 this->memory_cache.end = new_end;
                 this->memory_cache.end_of_cache = new_begin + length;
             }
         }
    }

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class InputIterator>
    void vector_space<T,1,Add,Multiply,Allocator>::range_insert(iterator pos, InputIterator first, InputIterator last,std::input_iterator_tag)
    {
	  for(; first!=last; ++first)
        {
            pos=insert(pos,*first);
            ++pos;
        }
    }

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class ForwardIterator>
    void vector_space<T,1,Add,Multiply,Allocator>::range_insert(iterator pos, ForwardIterator first, ForwardIterator last,std::forward_iterator_tag)
    {
        if(first != last)
        {
            const size_type length = std::distance(first,last);
            if(size_type(this->memory_cache.end_of_cache - this->memory_cache.end) >= length)
            {
                const size_type elems_after = end() - pos;
                pointer old_end(this->memory_cache.end);
                if(elems_after > length)
                {
                    zinhart::uninitialized_move_allocator(this->memory_cache.end - length,this->memory_cache.end,this->memory_cache.end,this->get_memory_cache_alloc_type());
                    ZINHART_MOVE_BACKWARD3(pos.base(),old_end - length, old_end);
                    std::copy(first,last,pos);
                }
                else
                {
                    ForwardIterator mid = first;
                    std::advance(mid,elems_after);
                    zinhart::uninitialized_copy_allocator(mid,last,this->memory_cache.end,this->get_memory_cache_alloc_type());//std::uninitialized_copy(mid,last,this->memory_cache.end);
                    this->memory_cache.end += length - elems_after;
                    zinhart::uninitialized_move_allocator(pos.base(),old_end,this->memory_cache.end,this->get_memory_cache_alloc_type());
                    this->memory_cache.end += elems_after;
                    std::copy(first, mid, pos);
                }
            }
            else
            {
                const size_type len = check_length(length);
                pointer new_begin(this->allocate_cache_of_size(len));
                pointer new_end(new_begin);
                ZINHART_TRY
                {
                    new_end = zinhart::uninitialized_move_ifnoexcept_allocator(this->memory_cache.begin,pos.base(),new_begin,this->get_memory_cache_alloc_type());
                    new_end = zinhart::uninitialized_copy_allocator(first,last,new_end,this->get_memory_cache_alloc_type());
                    new_end = zinhart::uninitialized_move_ifnoexcept_allocator(pos.base(),this->memory_cache.end,new_end,this->get_memory_cache_alloc_type());

                }
                ZINHART_CATCH(...)
                {
                    zinhart::destroy(new_begin,new_end,this->get_memory_cache_alloc_type());
                    deallocate_cache_of_size(new_begin,length);
                    ZINHART_THROW_EXCEPTION_AGAIN;
                }
                zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
                deallocate_cache_of_size(this->memory_cache.begin, this->memory_cache.end_of_cache - this->memory_cache.begin);
                this->memory_cache.begin = new_begin;
                this->memory_cache.end = new_end;
                this->memory_cache.end_of_cache = new_begin + len;
            }
        }
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::size_type vector_space<T,1,Add,Multiply,Allocator>::check_length(const size_type & size)
    {
        if(max_size() - this->size()< size)
            ZINHART_THROW_EXCEPTION_AGAIN std::length_error("vector_space<T,1>::check_length max_size() - this->size()< size");
        const size_type length = this->size() + std::max(this->size(),size);
        return (length < this->size() || length > max_size()) ? max_size() : length;

    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    void vector_space<T,1,Add,Multiply,Allocator>::check_range(size_type size)
    {
        if(size>this->size())
             ZINHART_THROW_EXCEPTION_AGAIN std::out_of_range("vector_space<T,1>::check_range Parameter size > this->size()");
    }

    #if __cplusplus >= 201103L
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template<class...Args>
    void vector_space<T,1,Add,Multiply,Allocator>::emplace_back_helper(Args&&...args)
    {
        const size_type length = check_length(size_type(1));
        pointer new_begin(this->allocate_cache_of_size(length));
        pointer new_end(new_begin);
        ZINHART_TRY
        {
            alloc_traits::construct(this->memory_cache,new_begin+this->size(),std::forward<Args>(args)...);
            new_end = pointer();
            new_end = zinhart::uninitialized_move_ifnoexcept_allocator(this->memory_cache.begin,this->memory_cache.end,new_begin,this->get_memory_cache_alloc_type());
            ++new_end;
        }
        ZINHART_CATCH(...)
        {
            if(!new_end)
                alloc_traits::destroy(this->memory_cache,new_begin+this->size());
            else
                zinhart::destroy(new_begin,new_end,this->get_memory_cache_alloc_type());
            deallocate_cache_of_size(new_begin,length);
            ZINHART_THROW_EXCEPTION_AGAIN;
        }
        zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
        deallocate_cache_of_size(this->memory_cache.begin, this->memory_cache.end_of_cache - this->memory_cache.begin);
        this->memory_cache.begin = new_begin;
        this->memory_cache.end = new_end;
        this->memory_cache.end_of_cache = new_begin + length;
    }
    #endif
    #if __cplusplus >= 201103L
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template<class...Args>
    void vector_space<T,1,Add,Multiply,Allocator>::insert_helper(iterator pos,Args&&...args)
    #else
    template<class T, class Allocator>
    void insert_helper(iterator pos,const_reference value);
    #endif
    {
        if(this->memory_cache.end != this->memory_cache.end_of_cache)
        {
            alloc_traits::construct(this->memory_cache,this->memory_cache.end,ZINHART_MOVE( *(this->memory_cache.end - 1) ));
            ++this->memory_cache.end;
            #if __cplusplus < 201103L
            value_type value_temp=value;
            #endif
            ZINHART_MOVE_BACKWARD3(pos.base(),this->memory_cache.end - 2,this->memory_cache.end - 1);
            #if __cplusplus < 201103L
                *pos= value_temp;
            #else
                *pos = value_type(std::forward<Args>(args)...);
            #endif
        }
        else
        {
            const size_type length= check_length(size_type(1));
            const size_type elements_before = pos - begin();
            pointer new_begin(this->allocate_cache_of_size(length));
            pointer new_end(new_begin);
            ZINHART_TRY
            {
                alloc_traits::construct(this->memory_cache,new_begin + elements_before,
                #if __cplusplus >= 201103L
                std::forward<Args>(args)...);
                #else
                value);
                #endif
                new_end = pointer();
                new_end = zinhart::uninitialized_move_ifnoexcept_allocator(this->memory_cache.begin,pos.base(),new_begin,this->get_memory_cache_alloc_type());
                ++new_end;
                new_end = zinhart::uninitialized_move_ifnoexcept_allocator(pos.base(),this->memory_cache.end,new_end,this->get_memory_cache_alloc_type());
            }
            ZINHART_CATCH(...)
            {
                if(!new_end)
                    alloc_traits::destroy(this->memory_cache,new_begin+elements_before);
                else
                    zinhart::destroy(new_begin,new_end,this->get_memory_cache_alloc_type());
                deallocate_cache_of_size(new_begin,length);
                ZINHART_THROW_EXCEPTION_AGAIN;
            }
            zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
            deallocate_cache_of_size(this->memory_cache.begin, this->memory_cache.end_of_cache - this->memory_cache.begin);
            this->memory_cache.begin = new_begin;
            this->memory_cache.end = new_end;
            this->memory_cache.end_of_cache = new_begin + length;
        }

    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::erase_helper(iterator pos)
    {
       if(pos + 1 != end())
           ZINHART_MOVE3(pos+1,end(),pos);
       --this->memory_cache.end;
       alloc_traits::destroy(this->memory_cache,this->memory_cache.end);
       return pos;
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::erase_helper(iterator first,iterator last)
    {
       if(first != last )
       {
           if(last != end())
               ZINHART_MOVE3(last,end(),first);
           erase_at_end( first.base() + (end() - last) );
       }
       return first;
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    void vector_space<T,1,Add,Multiply,Allocator>::erase_at_end(pointer pos) ZINHART_NOEXCEPT
    {
        zinhart::destroy(pos, this->memory_cache.end,this->get_memory_cache_alloc_type());
        this->memory_cache.end = pos;
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    void vector_space<T,1,Add,Multiply,Allocator>::default_append(size_type size)
    {
        if(size != 0)
        {
            if(size_type(this->memory_cache.end_of_cache - this->memory_cache.end) >= size)
            {
                 this->memory_cache.end = zinhart::uninitialized_default_nvals_allocator(this->memory_cache.end,size,this->get_memory_cache_alloc_type());
            }
            else
            {
                const size_type length = check_length(size);
                //const size_type old_size = this->size();
                pointer new_begin(this->allocate_cache_of_size(length));
                pointer new_end(new_begin);
                ZINHART_TRY
                {
                    new_end = zinhart::uninitialized_move_ifnoexcept_allocator(this->memory_cache.begin,this->memory_cache.end,new_begin,this->get_memory_cache_alloc_type());
                    //my implementation of td::__uninitialized_default_n_a
                    new_end = zinhart::uninitialized_default_nvals_allocator(new_end,size,this->get_memory_cache_alloc_type());
                }
                ZINHART_CATCH(...)
                {
                    zinhart::destroy(new_begin,new_end,this->get_memory_cache_alloc_type());
                    deallocate_cache_of_size(new_begin,length);
                    ZINHART_THROW_EXCEPTION_AGAIN;
                }
                zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
                deallocate_cache_of_size(this->memory_cache.begin, this->memory_cache.end_of_cache - this->memory_cache.begin);
                this->memory_cache.begin = new_begin;
                this->memory_cache.end = new_end;
                this->memory_cache.end_of_cache = new_begin + length;
            }
        }
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <class ForwardIterator>
    typename vector_space<T,1,Add,Multiply,Allocator>::pointer vector_space<T,1,Add,Multiply,Allocator>::allocate_and_copy(size_type size, ForwardIterator first, ForwardIterator last)
    {
        pointer result = this->allocate_cache_of_size(size);
        ZINHART_TRY
        {
            zinhart::uninitialized_copy_allocator(first,last,result,this->get_memory_cache_alloc_type());
            return result;
        }
        ZINHART_CATCH(...)
        {
            deallocate_cache_of_size(result,size);
            ZINHART_THROW_EXCEPTION_AGAIN;
        }
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    bool vector_space<T,1,Add,Multiply,Allocator>::shrink_to_fit_helper()
    {
         if(capacity() == size())
             return false;
         return zinhart::generic_shrink_to_fit_helper<vector_space>::shrink(*this);
    }
    #if __cplusplus >= 201103L
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template<class UPtr>
    UPtr * vector_space<T,1,Add,Multiply,Allocator>::data_ptr(UPtr * ptr) const
    {
        return ptr;
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template<class Ptr>
    typename std::pointer_traits<Ptr>::element_type* vector_space<T,1,Add,Multiply,Allocator>::data_ptr(Ptr  ptr) const
    {
        return empty() ? nullptr : std::addressof(*ptr);
    }
    #else
    template<class Ptr>
    Ptr data_ptr (Ptr ptr) const
    {
        return ptr;
    }
    #endif

    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    void vector_space<T,1,Add,Multiply,Allocator>::move_assign(vector_space<T,1,Add,Multiply,Allocator> && vs,std::true_type)noexcept
    {
        vector_space tmp(get_allocator());
        this->memory_cache.memory_cache_swap_data(tmp.memory_cache);
        this->memory_cache.memory_cache_swap_data(vs.memory_cache);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    void vector_space<T,1,Add,Multiply,Allocator>::move_assign(vector_space<T,1,Add,Multiply,Allocator> && vs,std::false_type)
    {

    }

/*
-----------------------------------------------------------------------------
Constructors                                                                 |
-----------------------------------------------------------------------------
*/
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    vector_space<T,1,Add,Multiply,Allocator>::vector_space()
    #if __cplusplus >= 201103L
    noexcept(std::is_nothrow_default_constructible<Allocator>::value)
    #endif
        : Base()
    {
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    vector_space<T,1,Add,Multiply,Allocator>::vector_space(const allocator_type & alloc)ZINHART_NOEXCEPT
        :Base(alloc)
    {
 	}
    #if __cplusplus >= 201103L
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    vector_space<T,1,Add,Multiply,Allocator>::vector_space(const size_type & size,const_reference value,const allocator_type & alloc)
        :Base(size,alloc)
    {
        fill_initialize(size,value);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    vector_space<T,1,Add,Multiply,Allocator>::vector_space(size_type && size,value_type && value,const allocator_type && alloc)
        :Base(size,alloc)
    {
       fill_initialize(size,value);
    }
    #else
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    vector_space<T,1,Add,Multiply,Allocator>::vector_space(const size_type  size,value_type value= value_type(),const allocator_type & alloc = allocator_type())
        :Base(size,alloc)
    {
       fill_initialize(size,value);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    vector_space<T,1,Add,Multiply,Allocator>::vector_space(const size_type & size,const reference value= value_type(),const allocator_type & alloc = allocator_type())
    :Base(size,alloc)
    {
        fill_initialize(size,value);
    }
    #endif
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    vector_space<T,1,Add,Multiply,Allocator>::vector_space( const vector_space & vs)
        : Base( vs.size(),
          alloc_traits::select_on_container_copy_construction( vs.get_memory_cache_alloc_type()))
    {
        this->memory_cache.end = zinhart::uninitialized_copy_allocator(vs.cbegin(), vs.cend(),this->memory_cache.begin,this->get_memory_cache_alloc_type());
    }
    #if __cplusplus >= 201103L
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    vector_space<T,1,Add,Multiply,Allocator>::vector_space(vector_space && vs)noexcept
        : Base(std::move(vs))
    {
    }
    //Move constructor with alternative allocator
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    vector_space<T,1,Add,Multiply,Allocator>::vector_space(const vector_space & vs,const allocator_type & alloc)
        :Base( vs.size(),alloc)
    {
        this->memory_cache.end = zinhart::uninitialized_copy_allocator(vs.cbegin(), vs.cend(),this->memory_cache.begin,this->get_memory_cache_alloc_type());
    }
    //Move constructor with alternative allocator
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    vector_space<T,1,Add,Multiply,Allocator>::vector_space(vector_space &&  vs, const allocator_type & alloc)
    noexcept
        :Base(std::move( vs),alloc)
    {
        if( vs.getAllocator()!=alloc)
        {
            zinhart::uninitialized_move_allocator(vs.begin(),vs.end(),this->memory_cache.begin,this->get_memory_cache_alloc_type());
            vs.clear();
        }
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    vector_space<T,1,Add,Multiply,Allocator>::vector_space(std::initializer_list<value_type> ilist,const allocator_type & alloc)
        :Base(alloc)
    {
        range_initialize(ilist.begin(),ilist.end(),std::random_access_iterator_tag());
    }
    #endif
    #if __cplusplus >= 201103L
    //If an input iterator is used there will be log(2n) calls to copy constructor, otherwise it is linear
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <typename InputIterator,typename>
              //typename  std::enable_if<std::is_convertible< typename std::iterator_traits<InputIterator>::iterator_category,std::input_iterator_tag  >::value >::type>
    vector_space<T,1,Add,Multiply,Allocator>::vector_space(InputIterator first, InputIterator last, const allocator_type & alloc)
        :Base(alloc)
    {
        initialize_dispatcher(first,last,std::false_type());
    }
    #else
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    template <InputIterator>
    vector_space(InputIterator first, InputIterator last,const allocator_type & alloc=allocator_type())
    {
        initialize_dispatcher(first,last,Integral());
    }
    #endif
/*
-----------------------------------------------------------------------------
Destructor                                                                   |
-----------------------------------------------------------------------------
*/
    //Only erases elements, does not touch pointed to memory if T is/are pointers
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    vector_space<T,1,Add,Multiply,Allocator>::~vector_space()ZINHART_NOEXCEPT
    {
        zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
    }
/*
-----------------------------------------------------------------------------
Accessors                                                                    |
-----------------------------------------------------------------------------
*/
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::reference vector_space<T,1,Add,Multiply,Allocator>::operator [](size_type index) ZINHART_NOEXCEPT
    {
        return *(this->memory_cache.begin +index);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::const_reference vector_space<T,1,Add,Multiply,Allocator>::operator [](size_type index) const ZINHART_NOEXCEPT
    {
        return *(this->memory_cache.begin +index);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::reference vector_space<T,1,Add,Multiply,Allocator>::front() ZINHART_NOEXCEPT
    {
        return *begin();
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::reference vector_space<T,1,Add,Multiply,Allocator>::back() ZINHART_NOEXCEPT
    {
        return *(end()-1);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::const_reference vector_space<T,1,Add,Multiply,Allocator>::front()const ZINHART_NOEXCEPT
    {
        return *begin();
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::const_reference vector_space<T,1,Add,Multiply,Allocator>::back()const ZINHART_NOEXCEPT
    {
        return *(end()-1);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::reference vector_space<T,1,Add,Multiply,Allocator>::at(const size_type index)ZINHART_NOEXCEPT
    {
        check_range(index);
        return (*this)[index];
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::const_reference vector_space<T,1,Add,Multiply,Allocator>::at(const size_type index) const ZINHART_NOEXCEPT
    {
        check_range(index);
        return (*this)[index];
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  typename vector_space<T,1,Add,Multiply,Allocator>::pointer vector_space<T,1,Add,Multiply,Allocator>::data()ZINHART_NOEXCEPT
	  {
	  	return data_ptr(this->memory_cache.begin);
	  }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  typename vector_space<T,1,Add,Multiply,Allocator>::const_pointer vector_space<T,1,Add,Multiply,Allocator>::data()const ZINHART_NOEXCEPT
	  {
	  	return data_ptr(this->memory_cache.begin);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  template<class IntegralType>
	  vector_space<T,1,Add,Multiply> vector_space<T,1,Add,Multiply,Allocator>::slice(IntegralType && start, IntegralType && size, IntegralType && step) ZINHART_NOEXCEPT
	  {
		//ideally would like a type trait for forwarding/universal references instead of failing in ret constructor or smthing like that, is_integral does not account for this
		//static_assert(std::is_integral<IntegralType>::value,"Only integral types may be accepted in this slice");
		vector_space<T,1,Add,Multiply> ret(size);
		ret.resize(0);
		auto select = begin() + start;
		for(size_type i = 0; i < size; ++i,select+=step)
		{
		  ret.emplace_back(*select);
		}
		return ret;
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  template<class IntegralType>
	  vector_space<T,1,Add,Multiply> vector_space<T,1,Add,Multiply,Allocator>::gslice(const vector_space<IntegralType,1> & start, const vector_space<IntegralType,1> & sizes, const vector_space<IntegralType,1> & steps)
	  {
		static_assert(std::is_integral<IntegralType>::value,"Only integral types may be accepted in this gslice");
		assert(sizes.size() == steps.size() && (start.size() <= sizes.size()) );
		using stype = size_type;
		auto volume = std::accumulate(sizes.cbegin(),sizes.cend(),stype(1),std::multiplies<IntegralType>());
		vector_space<T,1,Add,Multiply> ret(volume, T());
		ret.resize(0);
		for(stype i = 0; i < start.size(); ++i)
		{
		  auto temp = slice(start[i], sizes[i], steps[i]);
		  for(stype j = 0; j < temp.size(); ++j)
		  {
		   ret.emplace_back(temp[j]);
		  }
		}
  		return ret;
	  }
/*
-----------------------------------------------------------------------------
Iterators                                                                    |
-----------------------------------------------------------------------------
*/
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::begin()ZINHART_NOEXCEPT
    {
        return iterator(this->memory_cache.begin);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::end()ZINHART_NOEXCEPT
    {
        return iterator(this->memory_cache.end);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::const_iterator vector_space<T,1,Add,Multiply,Allocator>::cbegin()const ZINHART_NOEXCEPT
    {
        return const_iterator(this->memory_cache.begin);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::const_iterator vector_space<T,1,Add,Multiply,Allocator>::cend()const ZINHART_NOEXCEPT
    {
        return const_iterator(this->memory_cache.end);
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::reverse_iterator vector_space<T,1,Add,Multiply,Allocator>::rbegin()ZINHART_NOEXCEPT
    {
        return reverse_iterator(end());
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::reverse_iterator vector_space<T,1,Add,Multiply,Allocator>::rend()ZINHART_NOEXCEPT
    {
        return reverse_iterator(begin());
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::const_reverse_iterator vector_space<T,1,Add,Multiply,Allocator>::crbegin()const ZINHART_NOEXCEPT
    {
        return  const_reverse_iterator(cend());
    }
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
    typename vector_space<T,1,Add,Multiply,Allocator>::const_reverse_iterator vector_space<T,1,Add,Multiply,Allocator>::crend()const ZINHART_NOEXCEPT
    {
	  return  const_reverse_iterator(cbegin());
	}
/*
-----------------------------------------------------------------------------
Capacity                                                                     |
-----------------------------------------------------------------------------
*/
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  bool vector_space<T,1,Add,Multiply,Allocator>::empty()const ZINHART_NOEXCEPT
	  {
		return cbegin() == cend();
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  typename vector_space<T,1,Add,Multiply,Allocator>::size_type vector_space<T,1,Add,Multiply,Allocator>::size()const ZINHART_NOEXCEPT
	  {
		return size_type(this->memory_cache.end - this->memory_cache.begin);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  typename vector_space<T,1,Add,Multiply,Allocator>::size_type vector_space<T,1,Add,Multiply,Allocator>::capacity()const ZINHART_NOEXCEPT
	  {
		return size_type(this->memory_cache.end_of_cache - this->memory_cache.begin);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  constexpr typename vector_space<T,1,Add,Multiply,Allocator>::size_type vector_space<T,1,Add,Multiply,Allocator>::order()const ZINHART_NOEXCEPT
	  {
		return 1;
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  typename vector_space<T,1,Add,Multiply,Allocator>::size_type vector_space<T,1,Add,Multiply,Allocator>::max_size()const ZINHART_NOEXCEPT
	  {
		return alloc_traits::max_size(get_memory_cache_alloc_type());
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::reserve(size_type size)
	  {
		if(size>max_size())
		  ZINHART_THROW_EXCEPTION_AGAIN std::length_error("vector_space<T,1>::reserve requested size > max_size()");
		if(this->capacity() < size)
		{
		  const size_type old_size = this->size();
		  pointer temp = allocate_and_copy(size,ZINHART_MAKE_MOVE_ITERATOR_IF_NO_EXCEPT(this->memory_cache.begin),ZINHART_MAKE_MOVE_ITERATOR_IF_NO_EXCEPT(this->memory_cache.end));
		  zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
		  deallocate_cache_of_size(this->memory_cache.begin, this->memory_cache.end_of_cache - this->memory_cache.begin);
		  this->memory_cache.begin = temp;
		  this->memory_cache.end = temp + old_size;
		  this->memory_cache.end_of_cache = this->memory_cache.begin + size;
		}
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::shrink_to_fit()
	  {shrink_to_fit_helper();}
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  template<class Type,class Alloc, typename>
	 vector_space<Type,1,Add,Multiply,Alloc> vector_space<T,1,Add,Multiply,Allocator>::dimensions()const ZINHART_NOEXCEPT
	 {
	   return vector_space<Type,1,Add,Multiply,Alloc>(1,1);
	 }
/*
-----------------------------------------------------------------------------
Modifiers                                                                    |
-----------------------------------------------------------------------------
*/
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::assign(const size_type & size,const_reference value)
	  {
		this->fill_assign(size,value);
	  }
#if __cplusplus >= 201103L
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  template <class InputIterator, typename>
	  void vector_space<T,1,Add,Multiply,Allocator>::assign(InputIterator start,InputIterator finish)
	  {
		assign_dispatcher(start,finish,std::false_type());
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::assign(std::initializer_list<value_type> ilist)
	  {
		this->assign_helper(ilist.begin(),ilist.end(),std::random_access_iterator_tag());
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::assign(size_type && size,const_reference value)
	  {
		this->fill_assign(size,value);
	  }

#else
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  template <class InputIterator>
	  void assign(InputIterator start,InputIterator finish)
	  {
		//have to fix this, true type is c++11 code
		assign_dispatcher(start,finish,std::false_type());
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::assign(const size_type size,const_reference value)
	  {
		this->fill_assign(size,value);
	  }
#endif
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  //destroy all elements with alloc traits, don't touch capacity
	  void vector_space<T,1,Add,Multiply,Allocator>::clear() ZINHART_NOEXCEPT
	  {
		erase_at_end(this->memory_cache.begin);
	  }
#if __cplusplus >= 201103L
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  template<class...Args>
	  typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::emplace(const_iterator pos,Args&&...args)
	  {
		const size_type length = pos - begin();
		if(this->memory_cache.end != this->memory_cache.end_of_cache && pos == end())
		{
		  alloc_traits::construct(this->memory_cache,this->memory_cache.end,std::forward<Args>(args)...);
		  ++this->memory_cache.end;
		}
		else
		  insert_helper(begin() + (pos - cbegin()),std::forward<Args>(args)...);
		return iterator(this->memory_cache.begin + length);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  template<class...Args>
	  typename vector_space<T,1,Add,Multiply,Allocator>::reference vector_space<T,1,Add,Multiply,Allocator>::emplace_back(Args&&...args)
	  {
		if(this->memory_cache.end != this->memory_cache.end_of_cache)
		{
		  alloc_traits::construct(this->memory_cache,this->memory_cache.end,std::forward<T>(args)...);
		  ++this->memory_cache.end;
		}
		else
		  emplace_back_helper(std::forward<Args>(args)...);
		return *iterator(end());
	  }
#endif
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::insert(const_iterator pos,const_reference value)
	  {
		const size_type length = pos - begin();
		if(this->memory_cache.end != this->memory_cache.end_of_cache && pos == begin())
		{
		  alloc_traits::construct(this->memory_cache,this->memory_cache.end,value);
		  ++this->memory_cache.end;
		}
		else
		{
#if __cplusplus >= 201103L
		  const_iterator position = begin() + (position - cbegin());
		  if(this->memory_cache.end != this->memory_cache.end_of_storage)
		  {
			value_type value_copy = value;
			insert_helper(pos,std::move(value_copy));
		  }
		  else
			insert_helper(position,value);
#else
		  insert_helper(pos,value);
#endif

		}
		return iterator(this->memory_cache.begin + length);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::insert(const iterator pos,size_type size,const_reference value)
	  {
		difference_type offset = pos - cbegin();
		fill_insert(begin() + offset,size,value);
		return begin() + offset;
	  }

#if __cplusplus >= 201103L
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  template<class InputIterator,typename>
	  typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::insert(const_iterator pos,InputIterator begin,InputIterator end)
	  {
		difference_type offset = pos - cbegin();
		insert_dispatcher(this->begin() + offset, begin, end,std::false_type());
		return this->begin() + offset;
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::insert(const_iterator pos,std::initializer_list<value_type> ilist)
	  {
		return this->insert(pos,ilist.begin(),ilist.end());
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::insert(const_iterator pos, value_type && value)
	  {
		return emplace(pos,std::move(value));
	  }

#else
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  template<class InputIt>
	  void vector_space<T,1,Add,Multiply,Allocator>::insert(iterator pos,InputIt begin,InputIt end)
	  {

	  }
#endif
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::erase(const_iterator pos)
	  {
		return erase_helper(begin() + (pos-cbegin()));
	  }
#if __cplusplus >= 201103L
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::erase(const_iterator first, const_iterator last)
	  {
		iterator begin = this->begin();
		const_iterator cbegin=this->cbegin();
		return erase_helper(begin + (first - cbegin),begin + (last - cbegin));
	  }
#else
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  typename vector_space<T,1,Add,Multiply,Allocator>::iterator vector_space<T,1,Add,Multiply,Allocator>::erase(iterator first,iterator last)
	  {
		return erase_helper(first,last);
	  }
#endif

#if __cplusplus >= 201103L
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::resize(size_type && size)
	  {
		if(size > this->size())
		  default_append(size - this->size());
		else if(size < this->size())
		  erase_at_end(this->memory_cache.begin + size);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::resize(const size_type & size,const_reference value)
	  {
		if(size > this->size())
		  insert(end(),size - this->size(),value);
		else if(size < this->size())
		  erase_at_end(this->memory_cache.begin + size);
	  }

#else
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::resize(const size_type & size)
	  {
		if(size > this->size())
		  default_append(size-this->size());
		else if(size < this->size())
		  erase_at_end(this->memory_cache.begin + size);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::resize(const size_type &size,const_reference value)
	  {
		if(size > this->size())
		  insert(end(),size-this->size(),value);
		else
		  erase_at_end(this->memory_cache.begin + size);
	  }
#endif
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::push_back(const_reference value)
	  {
		if(this->memory_cache.end != this->memory_cache.end_of_cache)
		{
		  alloc_traits::construct(this->memory_cache,this->memory_cache.end,value);
		  ++this->memory_cache.end;
		}
		else //if(true)
#if __cplusplus >= 201103L
		  emplace_back_helper(value);
#else
		//insert_aux
		insert_helper(end(),value);
#endif
	  }
#if __cplusplus >= 201103L
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::push_back(value_type && value)
	  {
		emplace_back(std::move(value));
	  }
#else
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::push_back(value_type value)
	  {
	  }
#endif
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::pop_front()ZINHART_NOEXCEPT
	  {
		//rotate beginning to the back then pop back;
		//simple right rotation
		//std::rotate(rbegin(),rbegin()+1,rend());
		//simple left rotation
		std::rotate(begin(),begin()+1,end());
		pop_back();
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::pop_back()ZINHART_NOEXCEPT
	  {
		--this->memory_cache.end;
		alloc_traits::destroy(this->memory_cache,this->memory_cache.end);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  void vector_space<T,1,Add,Multiply,Allocator>::swap(vector_space & vs) ZINHART_NOEXCEPT
	  {
		this->memory_cache.memory_cache_swap_data( vs.memory_cache);
		if(alloc_traits::propagate_on_container_swap::value)
		  std::swap(get_memory_cache_alloc_type(), vs.get_memory_cache_alloc_type());

	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  vector_space<T,1,Add,Multiply,Allocator> & vector_space<T,1,Add,Multiply,Allocator>::operator = (const vector_space &  vs)
	  {
		if(&vs != this)
		{
#if __cplusplus >= 201103L
		  if(alloc_traits::propagate_on_container_copy_assignment::value)
		  {
#if __cplusplus >= 201600
			if(!alloc_traits::is_always_equal::value && get_memory_cache_alloc_type() !=  vs.get_memory_cache_alloc_type())
#else
			  if(get_memory_cache_alloc_type() !=  vs.get_memory_cache_alloc_type())
#endif
			  {
				this->clear();
				deallocate_cache_of_size(this->memory_cache.begin,this->memory_cache.end_of_cache - this->memory_cache.begin);
				this->memory_cache.begin = nullptr;
				this->memory_cache.end = nullptr;
				this->memory_cache.end_of_cache = nullptr;
			  }
			get_memory_cache_alloc_type() = alloc_traits::select_on_container_copy_construction( vs.get_memory_cache_alloc_type());
		  }
#endif
		  const size_type length =  vs.size();
		  if(length > capacity())
		  {

			pointer temp = allocate_and_copy(length, vs.cbegin(), vs.cend());
			zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
			deallocate_cache_of_size(this->memory_cache.begin,this->memory_cache.end_of_cache - this->memory_cache.begin);
			this->memory_cache.begin = temp;
			this->memory_cache.end_of_cache = this->memory_cache.begin + length;
		  }
		  else if (this->size() >= length)
		  {
			zinhart::destroy(std::copy(vs.cbegin(),vs.cend(),this->begin()),this->end(),this->get_memory_cache_alloc_type());
		  }
		  else
		  {
			std::copy( vs.memory_cache.begin, vs.memory_cache.begin + size(),this->memory_cache.begin);
			zinhart::uninitialized_copy_allocator(vs.memory_cache.begin + size(), vs.memory_cache.end,this->memory_cache.end,this->get_memory_cache_alloc_type() );

		  }
		  this->memory_cache.end = this->memory_cache.begin + length;
		}
		return *this;
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  vector_space<T,1,Add,Multiply,Allocator>  vector_space<T,1,Add,Multiply,Allocator>::operator + (const vector_space &  vs)const
	  {
		Add<vector_space> F;
		return F(*this,vs);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  vector_space<T,1,Add,Multiply,Allocator>  vector_space<T,1,Add,Multiply,Allocator>::operator * (const vector_space &  vs)const
	  {
		Multiply<vector_space> G;
		return G(*this,vs);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  vector_space<T,1,Add,Multiply,Allocator> & vector_space<T,1,Add,Multiply,Allocator>::operator += (const vector_space & vs)
	  {
		Add<vector_space> F;
		if(this->empty())
		{
		  this->resize(vs.size());
		}
		*this = F(*this,vs);
  		return *this;
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  vector_space<T,1,Add,Multiply,Allocator> & vector_space<T,1,Add,Multiply,Allocator>::operator *= (const vector_space & vs)
	  {
		Multiply<vector_space> G;
		if(this->empty())
		{
		  this->resize(vs.size());
		}
		*this = G(*this,vs);
  		return *this;
	  }

#if __cplusplus >= 201103L
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  vector_space<T,1,Add,Multiply,Allocator> & vector_space<T,1,Add,Multiply,Allocator>::operator =(vector_space && vs) noexcept
	  {
		if(alloc_traits::propagate_on_container_move_assignment::value || get_memory_cache_alloc_type() == vs.get_memory_cache_alloc_type())
		{
		  vector_space<T,1,Add,Multiply,Allocator> temp(get_allocator());
		  this->memory_cache.memory_cache_swap_data(temp.memory_cache);
		  this->memory_cache.memory_cache_swap_data(vs.memory_cache);
		  std::swap(get_memory_cache_alloc_type(),vs.get_memory_cache_alloc_type());
		}
		else
		{
		  pointer current(this->memory_cache.begin);
		  std::move_iterator<decltype(vs.begin())> first = std::make_move_iterator(vs.begin());
		  std::move_iterator<decltype(vs.end())> last = std::make_move_iterator(vs.end());
		  for(;first != last && current != this->memory_cache.end;++first,++current)
			*current = * first;
		  if(first == last)
			erase_at_end(current);
		  else
			insert(end(),first,last);
		  vs.clear();
		}
		return *this;
	  }

	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  vector_space<T,1,Add,Multiply,Allocator> & vector_space<T,1,Add,Multiply,Allocator>::operator =(std::initializer_list<value_type> ilist)
	  {
		this->assign(ilist.begin(),ilist.end());
		return *this;
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  vector_space<T,1,Add,Multiply,Allocator> & vector_space<T,1,Add,Multiply,Allocator>::operator + (vector_space &&  vs)
	  {
		Add<vector_space> F;
		return F(*this,vs);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  vector_space<T,1,Add,Multiply,Allocator> & vector_space<T,1,Add,Multiply,Allocator>::operator * (vector_space &&  vs)
	  {
		Multiply<vector_space> G;
		return G(*this,vs);
	  }
	
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  vector_space<T,1,Add,Multiply,Allocator> & vector_space<T,1,Add,Multiply,Allocator>::operator += (vector_space && vs)
	  {
		Add<vector_space> F;
		if(this->empty())
		{
		  this->resize(vs.size());
		}
		*this = F(*this,vs);
  		return *this;
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  vector_space<T,1,Add,Multiply,Allocator> & vector_space<T,1,Add,Multiply,Allocator>::operator *= (vector_space && vs)
	  {
		Multiply<vector_space> G;
		if(this->empty())
		{
		  this->resize(vs.size());
		}
		*this = G(*this,vs);
  		return *this;

	  }
#endif
/*
-----------------------------------------------------------------------------
Non-Member Funcs                                                             |
-----------------------------------------------------------------------------
*/
	template <class T,template<typename> class Add, /*template<typename> class Add1,*/template <typename> class Multiply, /*template<typename> class Multiply1,*/class Allocator>
	  inline bool operator ==(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs)
	  {
		return (lhs.size() == rhs.size() && std::equal(lhs.cbegin(),lhs.cend(),rhs.cbegin()));
	  }
	template <class T,template<typename> class Add,/*template <typename> class Add1,*/template <typename> class Multiply,/* template<typename> class Multiply1,*/class Allocator>
	  inline bool operator !=(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs)
	  {
		return !(lhs == rhs);
	  }
	template <class T,template<typename> class Add, template<typename> class Add1,template <typename> class Multiply, template<typename> class Multiply1,class Allocator>
	  inline bool operator <(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add1,Multiply1,Allocator> &rhs)
	  {
		return std::lexicographical_compare(lhs.cbegin(),lhs.cend(),rhs.cbegin(),rhs.cend());
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  inline bool operator <=(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs)
	  {
		return !(rhs < lhs);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  inline bool operator >(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs)
	  {
		return rhs < lhs;
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  inline bool operator >=(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs)
	  {
		return !(lhs < rhs);
	  }
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  inline void swap(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs)
	  {
		lhs.swap(rhs);
	  }

}

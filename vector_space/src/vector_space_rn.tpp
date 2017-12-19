#include "../inc/vector_space.h"
#include "../inc/sequence_container.h"
#include <algorithm>
#include<iostream>
#include <numeric>
#include <cassert>
#include <iterator>
#include <memory>
namespace zinhart
{
/*
-----------------------------------------------------------------------------
vector_space Rank >= 2                                                         |
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
Internal Functions                                                           |
-----------------------------------------------------------------------------
*/
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::allocate_and_initialize_private_members()
	  {
		try
		{
		  {
			std::shared_ptr<size_type[]> space_swap(new size_type[N],[](size_type * init){delete [] init;});
			std::shared_ptr<size_type[]> basis_swap(new size_type[N],[](size_type * init){delete [] init;});
			space = space_swap;
			basis = basis_swap;
		  }
			std::fill(space.get(),space.get() + order(), size_type(0));
			std::fill(basis.get(),basis.get() + order(), size_type(0));
 
		}
		catch(...)
		{
		  ZINHART_THROW_EXCEPTION_AGAIN;
		}
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::size_type vector_space<T,N,Add,Multiply,Allocator>::stack_factor(const size_type & length)const
	  {
		
		size_type constant = std::accumulate(space.get() + 1,space.get()+N,size_type(1),std::multiplies<size_type>());
		size_type ef = 0;//expansion_factor
		while( (space[0]<<ef) * constant < length)
		  ++ef;
		return (ef > 0) ? ef : 1;
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::size_type vector_space<T,N,Add,Multiply,Allocator>::unstack_factor(const size_type & length)const
	  {
		size_type sf = 0;//shrinkage factor, i.e numbers of layers to remove while still keep structure
		size_type constant = std::accumulate(space.get() + 1,space.get()+N,size_type(1),std::multiplies<size_type>());
		//if the size is prime or capacity is not a multiple of size then it can't be represent by the integers in the dimension vector,  (`_`)
		// (capacity() % size() == 0) is a necessary condition otherwise would loop forever
		if(!is_prime(space[0]) && (capacity() % size() == 0) )
		{
		  while ( (space[0] >> sf) * constant != length )
		  {
			//std::cout<<"Expression: "<<(space[0]>>sf*constant) <<" shrinkage factor: "<<sf <<" target: "<<length<<"\n";
		  	++sf;
			if(sf>=60 )
			  break;//order 10 ^18
		  }
		}
		return sf;
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::stack(const size_type & length)
	  {
		space[0] <<= stack_factor(length);
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::unstack(const size_type & length)
	  {
		space[0] >>= unstack_factor(length);
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class ForwardIterator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::size_type vector_space<T,N,Add,Multiply,Allocator>::hyper_volume(ForwardIterator begin,ForwardIterator end)const
	  {
		return std::accumulate(begin,end,size_type(1),std::multiplies<size_type>());
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class ForwardIterator>
	  void vector_space<T,N,Add,Multiply,Allocator>::dimensions_check(std::true_type,ForwardIterator start,ForwardIterator finish,size_type && capacity)const
	  {
		ZINHART_TRY
		{
		  if(start == nullptr || finish == nullptr)
		  {
			ZINHART_THROW_EXCEPTION_AGAIN std::domain_error("The dimensions of this vector_space space have not been specified,so call reshape"); 
		  }
	  	  if(capacity != std::accumulate(start,finish,size_type(1),std::multiplies<size_type>()))
			ZINHART_THROW_EXCEPTION_AGAIN std::domain_error("capacity() != PI(dimensions()[i]),so call reshape");
		}
		ZINHART_CATCH(...)
		{
		  ZINHART_THROW_EXCEPTION_AGAIN;
		}
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class ForwardIterator>
	  void vector_space<T,N,Add,Multiply,Allocator>::dimensions_check(std::false_type,ForwardIterator start,ForwardIterator finish)const
	  {
		ZINHART_TRY
		{

		  if(start == nullptr || finish == nullptr)
		  {
			ZINHART_THROW_EXCEPTION_AGAIN std::domain_error("The dimensions of this vector_space space have not been specified,so call reshape"); 
		  }
		  if(std::any_of(start,finish,[](size_type & init){return init == 0;}))
			ZINHART_THROW_EXCEPTION_AGAIN  std::domain_error("Found 0 as a dimension size,so call reshape");
		}
		ZINHART_CATCH(...)
		{
		  ZINHART_THROW_EXCEPTION_AGAIN;
		}
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class Type,template<typename> class add,template<typename> class multiply,class Alloc,typename>
	  void vector_space<T,N,Add,Multiply,Allocator>::generate_basis(const vector_space<Type,1,add,multiply,Alloc> &dimensions)
	  {
		/*
		 * want to establish the number of elements in the 1 dimensional space are the same as N @ compile time,
		 * this is essentially a space vs ease of implementation trade off, if I use a static array(ease of implementation) instead of a dynamic one
		 * to hold space and _subspacelengths then the line directly below this one
		 * static_assert(sizeof(dimensions.size())==N,"# of elements in the parameter pack does not match the second parameter of the class template");
		 * becomes possible and at the same time it would enable me to use static assertions to make sure that the capacity() and sigma of space are the same i.e compile time evaluation
		 * On the other hand using dynamic arrays for _dimension and _subspacelengths means that there will be no extra overhead. As a for instance of this
		 * with a static array of 3 elements of say uints thats a total of 12 bytes for space + 12 bytes for _subspacelengths = 24 bytes overhead total, and this scales
		 * so with 4 elements now its 32 bytes total. A 5d is 40 bytes extra and a 6d space is 48 bytes extra, see the pattern?
		 * dynamic arrays cap this @ a total of 8 bytes since size_type comes from the allocator and is uint.
		 */
		std::copy(dimensions.cbegin(),dimensions.cend(),space.get());
		size_type count=0;
		//the expression ->basis[i] *= space[count] where count = 1 to space[N]
		std::generate(basis.get(),basis.get()+N,[this,&count](){++count;return std::accumulate(space.get()+count,space.get()+N,1U,std::multiplies<size_type>());});
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class Type,template<typename> class add,template<typename> class multiply,class Alloc,typename> 
	  void vector_space<T,N,Add,Multiply,Allocator>::generate_basis(vector_space<Type,1,add,multiply,Alloc> &&dimensions)
	  {
		std::move(dimensions.begin(),dimensions.end(),space.get());
		//the expression ->basis[i] *= space[count] where count = 1 to space[N]
		size_type count=0;
		std::generate(basis.get(),basis.get()+N,[this,&count](){++count;return std::accumulate(space.get()+count,space.get()+N,1U,std::multiplies<size_type>());});
	  }
#if __cplusplus >= 201103L
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class...Args,typename>
	  void vector_space<T,N,Add,Multiply,Allocator>::generate_basis(Args&&...args)
	  {
		size_type dimensions[N]{args...};
		std::move(dimensions,dimensions+N,space.get());
		//the expression ->basis[i] *= space[count] where count = 1 to space[N]
		size_type count=0;
		std::generate(basis.get(),basis.get()+N,[this,&count](){++count;return std::accumulate(space.get()+count,space.get()+N,1U,std::multiplies<size_type>());});
	  }
#endif
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::default_initialize(const size_type & size)
	  {
		this->memory_cache.end = zinhart::uninitialized_default_nvals_allocator(this->memory_cache.begin,size,this->get_memory_cache_alloc_type());
	  }

	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::fill_initialize(const size_type & size, const_reference value)
	  {
		this->memory_cache.end = zinhart::uninitialized_fill_nvals_allocator(this->memory_cache.begin,size,value,this->get_memory_cache_alloc_type());
	  } 	
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::allocate_and_initialize(const size_type & requested_capacity, const value_type & value)
	  {
		check_length(requested_capacity,std::true_type());
		zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
	  	deallocate_cache_of_size(this->memory_cache.begin, this->memory_cache.end_of_cache - this->memory_cache.begin);
		this->memory_cache.begin = allocate_cache_of_size(requested_capacity);
		fill_initialize(requested_capacity,value);
	    this->memory_cache.end_of_cache = this->memory_cache.end;
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class Integer>
	  void vector_space<T,N,Add,Multiply,Allocator>::initialize_dispatcher(Integer size, const_reference value , std::true_type)
	  {
		this->memory_cache.end_of_cache = this->memory_cache.begin + static_cast<size_type>(size);
		fill_initialize(static_cast<size_type>(size),value);
	  }

	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class InputIterator>
	  void vector_space<T,N,Add,Multiply,Allocator>::initialize_dispatcher(InputIterator first, InputIterator last , std::false_type)
	  {
		typedef typename std::iterator_traits<InputIterator>::iterator_category iter_category;
		range_initialize(first,last,iter_category());
	  }

	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class InputIterator>
	  void vector_space<T,N,Add,Multiply,Allocator>::range_initialize(InputIterator first,InputIterator last,std::input_iterator_tag)
	  {
		for(;first!=last;++first)
#if __cplusplus >= 201103L
		  emplace_back(*first);
#else
		  push_back(*first);
#endif
	  }

	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class ForwardIterator>
	  void vector_space<T,N,Add,Multiply,Allocator>::range_initialize(ForwardIterator first,ForwardIterator last,std::forward_iterator_tag)
	  {
		const size_type size = std::distance(first,last);
		this->memory_cache.begin = this->allocate_cache_of_size(size);
		this->memory_cache.end_of_cache = this->memory_cache.begin + size;
		this->memory_cache.end = zinhart::uninitialized_copy_allocator(first,last,this->memory_cache.begin,this->get_memory_cache_alloc_type());

	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class ForwardIterator>
	  void vector_space<T,N,Add,Multiply,Allocator>::range_initialize(ForwardIterator first,ForwardIterator last)
	  {
		const size_type size = hyper_volume(space.get(),space.get()+N);
		this->memory_cache.begin = this->allocate_cache_of_size(size);
		this->memory_cache.end_of_cache = this->memory_cache.begin + size;
		this->memory_cache.end = zinhart::uninitialized_copy_allocator(first,last,this->memory_cache.begin,this->get_memory_cache_alloc_type());
	  }
	 
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class Integer>
	  void vector_space<T,N,Add,Multiply,Allocator>::assign_dispatcher(Integer size,const_reference values,std::true_type)
	  {
		fill_assign(size,values);
	  }

	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class InputIterator>
	  void vector_space<T,N,Add,Multiply,Allocator>::assign_dispatcher(InputIterator first, InputIterator last,std::false_type)
	  {
		typedef typename std::iterator_traits<InputIterator>::iterator_category iter_category;
		assign_helper(first,last,iter_category());
	  }

	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::fill_assign(size_type & size, const_reference value)
	  {
		if(size > this->capacity())
		{
		  stack(size);
		  zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
		  deallocate_cache_of_size(this->memory_cache.begin, this->memory_cache.end_of_cache - this->memory_cache.begin);
		  size_type hyper_volume = std::accumulate(space.get(),space.get()+N,size_type(1),std::multiplies<size_type>() );
		  pointer new_begin(this->allocate_cache_of_size(hyper_volume));
		  pointer new_end(new_begin);
		  new_end+=size;
		  this->memory_cache.begin = new_begin;
		  this->memory_cache.end = new_end;
		  this->memory_cache.end_of_cache = new_begin + hyper_volume;
		  std::fill(begin(),end(),value);
		}
		else if (size > this->size())
		{
		  std::fill(begin(),end(),value);
		  this->memory_cache.end = zinhart::uninitialized_fill_nvals_allocator(this->memory_cache.begin,size - this->size(),value,this->get_memory_cache_alloc_type());
		}
		else
		  erase_at_end(std::fill_n(this->memory_cache.begin,size,value));
	  }

	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class InputIterator>
	  void vector_space<T,N,Add,Multiply,Allocator>::assign_helper(InputIterator first, InputIterator last, std::input_iterator_tag)
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
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class ForwardIterator>
	  void vector_space<T,N,Add,Multiply,Allocator>::assign_helper(ForwardIterator first, ForwardIterator last,std::forward_iterator_tag)
	  {
		const size_type length = std::distance(first,last);
		if(length > capacity())
		{
		  stack(length);
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
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class Integer>
	  void vector_space<T,N,Add,Multiply,Allocator>::insert_dispatcher(iterator pos, const_reference value, Integer size,std::true_type)
	  {
		fill_insert(pos,static_cast<size_type>(size),value);

	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class InputIterator>
	  void vector_space<T,N,Add,Multiply,Allocator>::insert_dispatcher(iterator pos,InputIterator first, InputIterator last , std::false_type)
	  {
		typedef typename std::iterator_traits<InputIterator>::iterator_category iter_category;
		range_insert(pos,first,last,iter_category());
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::fill_insert(iterator pos, const size_type & size,const_reference value)
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
			  this->memory_cache.end = std::uninitialized_fill_n(this->memory_cache.end, size - elems_after,value_copy);
			  zinhart::uninitialized_move_allocator(pos.base(),old_end,this->memory_cache.end,this->get_memory_cache_alloc_type());
			  this->memory_cache.end += elems_after;
			  std::fill(pos.base(),old_end,value_copy);
			}
		  }
		  else
		  {
			//const size_type length = check_length(size);
			stack(capacity()+size);
  			const size_type length = hyper_volume(space.get(),space.get()+N);
  			check_length(length,std::true_type());
			const size_type elems_before = pos - this->begin();			
			pointer new_begin(this->allocate_cache_of_size(capacity()+length));
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
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class InputIterator>
	  void vector_space<T,N,Add,Multiply,Allocator>::range_insert(iterator pos, InputIterator first, InputIterator last,std::input_iterator_tag)
	  {
		for(; first!=last; ++first)
		{
		  pos=insert(pos,*first);
		  ++pos;
		}
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class ForwardIterator>
	  void vector_space<T,N,Add,Multiply,Allocator>::range_insert(iterator pos, ForwardIterator first, ForwardIterator last,std::forward_iterator_tag)
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
			  this->memory_cache.end += length;
			  ZINHART_MOVE_BACKWARD3(pos.base(),old_end - length, old_end);
			  std::copy(first,last,pos);
			}
			else
			{
			  ForwardIterator mid = first;
			  std::advance(mid,elems_after);
			  zinhart::uninitialized_copy_allocator(mid,last,this->memory_cache.end,this->get_memory_cache_alloc_type());
			  this->memory_cache.end += length - elems_after;
			  zinhart::uninitialized_move_allocator(pos.base(),old_end,this->memory_cache.end,this->get_memory_cache_alloc_type());
			  this->memory_cache.end += elems_after;
			  std::copy(first, mid, pos);
			}
		  }
		  else
		  {
			//const size_type len = check_length(length);
			stack(length);
  			const size_type len = hyper_volume(space.get(),space.get()+N);
  			check_length(len,std::true_type());
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
	//Note in both check_length functions this->size() will be equal to capacity()
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::check_length(const size_type & size,std::true_type)
	{
	  if(max_size() - this->size() < size)
		ZINHART_THROW_EXCEPTION_AGAIN std::length_error("cannot allocate anymore space");
	}
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::size_type vector_space<T,N,Add,Multiply,Allocator>::check_length(const size_type & size)
	  {
		if(max_size() - this->size()< size)
		  ZINHART_THROW_EXCEPTION_AGAIN std::length_error("cannot allocate anymore space");
		const size_type length = this->size() + std::max(this->size(),size);
		return (length < this->size() || length > max_size()) ? max_size() : length;
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::check_range(size_type size)
	  {
		if(size>this->size())
		  ZINHART_THROW_EXCEPTION_AGAIN std::out_of_range("Parameter size > this->size()");
	  }

#if __cplusplus >= 201103L
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class...Args>
	  void vector_space<T,N,Add,Multiply,Allocator>::emplace_back_helper(Args&&...args)
	  {
		const size_type length = check_length(size_type(1));
		stack(length);
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
		  ZINHART_THROW_EXCEPTION_AGAIN ;
		}
		zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
		deallocate_cache_of_size(this->memory_cache.begin, this->memory_cache.end_of_cache - this->memory_cache.begin);
		this->memory_cache.begin = new_begin;
		this->memory_cache.end = new_end;
		this->memory_cache.end_of_cache = new_begin + length;
	  }
#endif
#if __cplusplus >= 201103L
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class Arg>
	  void vector_space<T,N,Add,Multiply,Allocator>::insert_helper(iterator pos,Arg && arg)	  
#else
	  template<class T, class Allocator>
	  void insert_helper(iterator pos,const_reference value);
#endif
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
	  *pos = std::forward<Arg>(arg);
#endif
	}
#if __cplusplus >= 201103L
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class ...Args>
	  void vector_space<T,N,Add,Multiply,Allocator>::realloc_insert(iterator pos,Args&&...args)
#else
	  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::realloc_insert(iterator pos,const T & value)
#endif
	  {
		const size_type length = check_length(size_type(1));
		const size_type elems_before = pos - begin();
		pointer new_begin(this->allocate_cache_of_size(length));
		pointer new_end(new_begin);
		ZINHART_TRY
		{
		  alloc_traits::construct(this->memory_cache,elems_before,
#if __cplusplus >= 201103L
			  std::forward<Args>(args)...);
#else
		  value);
#endif
		  new_end = pointer();
		  new_end = zinhart::uninitialized_move_ifnoexcept_allocator(this->memory_cache.begin,pos.base(),new_begin,get_memory_cache_alloc_type());
		  ++new_end;
		  new_end = zinhart::uninitialized_move_ifnoexcept_allocator(pos.base(),this->memory_cache.end,new_end,get_memory_cache_alloc_type());
		}
		ZINHART_CATCH(...)
		{
		  if(!new_end)
		  alloc_traits::destroy(this->memory_cache,new_begin+elems_before);
		  else
			zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,get_memory_cache_alloc_type());
		  this->memory_cache.deallocate_cache_of_size(new_begin,length);		
		  ZINHART_THROW_EXCEPTION_AGAIN;
		}
		zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
		deallocate_cache_of_size(this->memory_cache.begin, this->memory_cache.end_of_cache - this->memory_cache.begin);
		this->memory_cache.begin = new_begin;
		this->memory_cache.end = new_end;
		this->memory_cache.end_of_cache = new_begin + length;
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::erase_helper(iterator pos)
	  {
		if(pos+1 != end())
		  ZINHART_MOVE3(pos+1,end(),pos);
		--this->memory_cache.end;
		alloc_traits::destroy(this->memory_cache,this->memory_cache.end);
		return pos;
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::erase_helper(iterator first,iterator last)
	  {
		if(first != last )
		{
		  if(last != end())
			ZINHART_MOVE3(last,end(),first);
		  erase_at_end( first.base() + (end() - last) );
		}
		return first;
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::erase_at_end(pointer pos) ZINHART_NOEXCEPT
	  {
		zinhart::destroy(pos, this->memory_cache.end,this->get_memory_cache_alloc_type());
		this->memory_cache.end = pos;
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::default_append(size_type size)
	  {
		if(size != 0)
		{
		  if(size_type(this->memory_cache.end_of_cache - this->memory_cache.end) >= size)
		  {
			this->memory_cache.end = zinhart::uninitialized_default_nvals_allocator(this->memory_cache.end,size-this->size(),get_memory_cache_alloc_type());
		  }
		  else
		  {
  			stack(size);
  			const size_type length = hyper_volume(space.get(),space.get()+N);
  			check_length(length,std::true_type());
//			const size_type length = check_length(size);
			pointer new_begin(this->allocate_cache_of_size(length));
			pointer new_end(new_begin);
			ZINHART_TRY
			{
			  new_end = zinhart::uninitialized_move_ifnoexcept_allocator(this->memory_cache.begin,this->memory_cache.end,new_begin,this->get_memory_cache_alloc_type());
			  //my implementation of td::__uninitialized_default_n_a
			  new_end = zinhart::uninitialized_default_nvals_allocator(new_end,length-size,this->get_memory_cache_alloc_type());
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
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class ForwardIterator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::pointer vector_space<T,N,Add,Multiply,Allocator>::allocate_and_copy(size_type size, ForwardIterator first, ForwardIterator last)
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
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class ForwardIterator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::pointer vector_space<T,N,Add,Multiply,Allocator>::allocate_and_move( size_type size, ForwardIterator first, ForwardIterator last)
	  {
		pointer result = this->allocate_cache_of_size(size);
		ZINHART_TRY
		{
		  zinhart::uninitialized_move_ifnoexcept_allocator(first,last,result,this->get_memory_cache_alloc_type());
		  return result;
		}
		ZINHART_CATCH(...)
		{
		  deallocate_cache_of_size(result,size);
		  ZINHART_THROW_EXCEPTION_AGAIN;
		}
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  bool vector_space<T,N,Add,Multiply,Allocator>::shrink_to_fit_helper()
	  {
		if(capacity() == size())
		  return false;
		bool unstack = (unstack_factor(size()) == 0) ? false : true;
		if(unstack)
		  this->unstack(size());
		return (unstack == false) ? false : zinhart::vector_space_shrink_to_fit_helper<vector_space>::shrink(*this);
	  }
#if __cplusplus >= 201103L
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class UPtr>
	  UPtr * vector_space<T,N,Add,Multiply,Allocator>::data_ptr(UPtr * ptr) const
	  {
		return ptr;
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class Ptr>
	  typename std::pointer_traits<Ptr>::element_type* vector_space<T,N,Add,Multiply,Allocator>::data_ptr(Ptr  ptr) const
	  {
		return empty() ? nullptr : std::addressof(*ptr);
	  }
#else
	template<class Ptr>
	  Ptr vector_space<T,N,Add,Multiply,Allocator>::data_ptr(Ptr ptr) const
	  {
		return ptr;
	  }
#endif
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::move_assign(vector_space<T,1,Add,Multiply,Allocator> && vs,std::true_type)noexcept
	  {
		vector_space tmp(get_allocator());
		this->memory_cache.memory_cache_swap_data(tmp.memory_cache);
		this->memory_cache.memory_cache_swap_data(vs.memory_cache);
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  void vector_space<T,N,Add,Multiply,Allocator>::move_assign(vector_space<T,1,Add,Multiply,Allocator> && vs,std::false_type)
	  {

	  }


/*
-----------------------------------------------------------------------------
Constructors                                                                |
-----------------------------------------------------------------------------
*/
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  vector_space<T,N,Add,Multiply,Allocator>::vector_space()
#if __cplusplus >= 201103L
	  noexcept(std::is_nothrow_default_constructible<Allocator>::value)
#endif
	  : Base()
	  {
		allocate_and_initialize_private_members();
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  vector_space<T,N,Add,Multiply,Allocator>::vector_space(const allocator_type & alloc)ZINHART_NOEXCEPT
	  :Base(alloc)
	  {
		//basically a null object
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class Type,template<typename> class F1,template <typename> class F2,class Alloc /*=std::allocator<T>*/>
	  vector_space<T,N,Add,Multiply,Allocator>::vector_space(vector_space<Type, 1, F1, F2, Alloc> && dim)
	  {
		assert(dim.size() == order());
		allocate_and_initialize_private_members();
		generate_basis(std::move(dim));
		dimensions_check(std::false_type(),space.get(),space.get() + order());
		size_type requested_capacity = hyper_volume(space.get(),space.get() + order());
		allocate_and_initialize(requested_capacity, value_type());
		//default_initialize(size());
		dimensions_check(std::true_type(),space.get(),space.get() + order(),capacity());
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  vector_space<T,N,Add,Multiply,Allocator>::vector_space(const vector_space & vs)
	  : Base(vs.capacity(), alloc_traits::select_on_container_copy_construction( vs.get_memory_cache_alloc_type()))
	  {
		dimensions_check(std::false_type(),vs.space.get(),vs.space.get()+N);
		dimensions_check(std::true_type(),vs.space.get(),vs.space.get()+N,vs.capacity());
		allocate_and_initialize_private_members();
	  	generate_basis(vs.dimensions());
		this->memory_cache.end = zinhart::uninitialized_copy_allocator(vs.cbegin(), vs.cend(),this->memory_cache.begin,this->get_memory_cache_alloc_type());
		dimensions_check(std::false_type(),vs.space.get(),vs.space.get()+N);
		dimensions_check(std::true_type(),vs.space.get(),vs.space.get()+N,vs.capacity());
	  }

#if __cplusplus >= 201103L
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class ...Args,typename>
	  vector_space<T,N,Add,Multiply,Allocator>::vector_space(Args&&...args) ZINHART_NOEXCEPT
	  : Base(std::forward<Args>(args)...)
	  {
	  	static_assert(N==sizeof...(Args),"vector_space<T,N,Add,Multiply,Allocator> variadic ctor: you passed more args then order of space");
		allocate_and_initialize_private_members();
		generate_basis(std::forward<Args>(args)...);
		dimensions_check(std::false_type(),space.get(),space.get() + order());
		//size_type mem = hyper_volume(space.get(),space.get() + order());
	//	allocate_and_initialize(mem, mem);
	//	default_initialize(size());
	    fill_initialize(capacity(),T());
		dimensions_check(std::true_type(),space.get(),space.get() + order(),capacity());
	  }

	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class ...Args,typename>
	  vector_space<T,N,Add,Multiply,Allocator>::vector_space(Args&...args) ZINHART_NOEXCEPT
	  : Base(std::forward<Args>(args)...)
	  {
	  	static_assert(N==sizeof...(Args),"vector_space<T,N,Add,Multiply,Allocator> variadic ctor: you passed more args then order of space");
		allocate_and_initialize_private_members();
		generate_basis(std::forward<Args>(args)...);
		dimensions_check(std::false_type(),space.get(),space.get() + order());
		//size_type mem = hyper_volume(space.get(),space.get() + order());
		//allocate_and_initialize(mem, mem);
		//default_initialize(size());
		fill_initialize(capacity(),T());
		dimensions_check(std::true_type(),space.get(),space.get() + order(),capacity());
	  }

	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  vector_space<T,N,Add,Multiply,Allocator>::vector_space(vector_space && vs)noexcept
	  :Base(std::move(vs))
	  {	
		space = vs.space,basis = vs.basis;
		vs.space.reset(),vs.basis.reset();
		vs.allocate_and_initialize_private_members();
		dimensions_check(std::false_type(),space.get(),space.get()+N);
		dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  vector_space<T,N,Add,Multiply,Allocator>::vector_space(const vector_space& vs,const allocator_type & alloc)
	  :Base( vs.size(),alloc)
	  {
		allocate_and_initialize_private_members();
		//uninitialized copy a
		initialize_dispatcher( vs.cbegin(), vs.cend(),std::false_type());
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  vector_space<T,N,Add,Multiply,Allocator>::vector_space(vector_space &&  vs, const allocator_type & alloc)noexcept
	  :Base(std::move(vs),alloc)
	  {
		if(vs.get_allocator() != alloc)
		{
		  allocate_and_initialize_private_members();
		  space = vs.space;
		  basis = vs.basis;
  		  this->memory_cache.end = zinhart::uninitialized_move_allocator(vs.begin(),vs.end(),this->memory_cache.begin,get_memory_cache_alloc_type());
		  vs.clear();
		}
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  vector_space<T,N,Add,Multiply,Allocator>::vector_space(const std::initializer_list<value_type> ilist,const allocator_type & alloc)
	  {
		allocate_and_initialize_private_members();
	  	//range_initialize(ilist.begin(),ilist.end(),std::random_access_iterator_tag());
	  }

#endif
#if __cplusplus >= 201103L
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <typename InputIterator,typename>
	  //typename  std::enable_if<std::is_convertible< typename std::iterator_traits<InputIterator>::iterator_category,std::input_iterator_tag  >::value >::type>
	  vector_space<T,N,Add,Multiply,Allocator>::vector_space(InputIterator first, InputIterator last,vector_space<T,1,Add,Multiply,Allocator> && dimensions ,const allocator_type & alloc)
	  :Base(alloc)
	  {
		allocate_and_initialize_private_members();
		generate_basis(std::move(dimensions));
		range_initialize(first,last);
		dimensions_check(std::false_type(),space.get(),space.get()+N);
		dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <typename InputIterator,typename>
            //typename std::enable_if<std::is_convertible< typename std::iterator_traits<InputIterator>::iterator_category,std::input_iterator_tag  >::value >::type>
      vector_space<T,N,Add,Multiply,Allocator>::vector_space(InputIterator first, InputIterator last,std::initializer_list<size_type> dimensions ,const allocator_type & alloc )
	 :Base(alloc)
	  {
		allocate_and_initialize_private_members();
		generate_basis( vector_space<T,1,Add,Multiply,Allocator>(dimensions.begin(),dimensions.end()));
		range_initialize(first,last);
		dimensions_check(std::false_type(),space.get(),space.get()+N);
		dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());

	  }
#else
	template<class T,class Allocator>
	  template <InputIterator>
	  vector_space(InputIterator first, InputIterator last,const allocator_type & alloc=allocator_type())
	  {
	  }
#endif

/*
-----------------------------------------------------------------------------
Destructor                                                                  |
-----------------------------------------------------------------------------
*/
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  vector_space<T,N,Add,Multiply,Allocator>::~vector_space()ZINHART_NOEXCEPT
	  {
		for(iterator start=begin();start!=end();++start)
		  alloc_traits::destroy(get_memory_cache_alloc_type(),&start);
	  }
/*
-----------------------------------------------------------------------------
Accessors                                                                   |
-----------------------------------------------------------------------------
*/
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  sub_space<T,N-1,Add,Multiply,Allocator> vector_space<T,N,Add,Multiply,Allocator>::operator [](size_type index)
	  {
		assert(index < space[0]);
		//static_assert(Subvector_space<T,N>::indexCheck(index,basis[0]),"invalid index");
		//return Subvector_space<T,N-1>(&_data[ index * basis[0] ],space+1,basis+1);
		return sub_space<T,N-1,Add,Multiply,Allocator>(&this->memory_cache.begin[ index * basis[0] ],space.get()+1,basis.get()+1);
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  const sub_space<T,N-1,Add,Multiply,Allocator> vector_space<T,N,Add,Multiply,Allocator>::operator [](size_type index)const
	  {
		assert(index < space[0]);
		//static_assert(index<basis[0],"invalid index");
		//return sub_space<T,N-1>(&_data[ index * basis[0] ],space+1,basis+1);
		return sub_space<T,N-1,Add,Multiply,Allocator>(&this->memory_cache.begin[ index * basis[0] ],space.get()+1,basis.get()+1);
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::reference vector_space<T,N,Add,Multiply,Allocator>::front() ZINHART_NOEXCEPT
	  {
		return *begin();
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::reference vector_space<T,N,Add,Multiply,Allocator>::back() ZINHART_NOEXCEPT
	  {
		return *(end()-1);
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class ...Args>
	  typename vector_space<T,N,Add,Multiply,Allocator>::reference vector_space<T,N,Add,Multiply,Allocator>::at(Args&&...args) ZINHART_NOEXCEPT
	  {
		static_assert(sizeof...(Args) == N , "Illegal number of functions args");
		dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
		size_type indices[N]={args...};
		long count=-1;
		if(std::any_of(indices,indices+N,[&count,this](size_type & init){++count;return init>=space[count];})==true)
		ZINHART_THROW_EXCEPTION_AGAIN std::out_of_range("An index in the parameter pack is out of bounds");
		return *(begin() + std::inner_product(indices,indices+N,basis.get(),size_type(0)) );
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class ...Args>
	  typename vector_space<T,N,Add,Multiply,Allocator>::const_reference vector_space<T,N,Add,Multiply,Allocator>::at(const Args&...args)const ZINHART_NOEXCEPT
	  {
		static_assert(sizeof...(Args) == N , "Illegal number of functions args");
		dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
		size_type indices[N]={args...};
		long count=-1;
		if(std::any_of(indices,indices+N,[&count,this](size_type & init){++count;return init>=space[count];})==true)
		  ZINHART_THROW_EXCEPTION_AGAIN std::out_of_range("An index in the parameter pack is out of bounds");
		count=0;
		return *(cbegin() + std::inner_product(indices,indices+N,basis.get(),size_type(0)) );
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::pointer vector_space<T,N,Add,Multiply,Allocator>::data() ZINHART_NOEXCEPT
	  {
		return (empty() == false) ? pointer(this->memory_cache.begin) : nullptr;
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::const_pointer vector_space<T,N,Add,Multiply,Allocator>::data()const ZINHART_NOEXCEPT
	  {
		return (empty() == false) ? const_pointer(this->memory_cache.begin) : nullptr;
	  }
	template <class T,lluint N, template<typename> class Add,template <typename> class Multiply,class Allocator>
	  template<class IntegralType>
	  vector_space<T,1,Add,Multiply> vector_space<T,N,Add,Multiply,Allocator>::slice(IntegralType && start, IntegralType && size, IntegralType && step) ZINHART_NOEXCEPT
	  {
		vector_space<T,1,Add,Multiply> ret(size);
		ret.resize(0);
		auto select = begin() + start;
		for(size_type i = 0; i < size; ++i, select+=step)
		{
		  ret.emplace_back(*select);
		}
		return ret;
	  }
	template <class T,lluint N,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  template<class IntegralType>
	  vector_space<T,1,Add,Multiply> vector_space<T,N,Add,Multiply,Allocator>::gslice(const vector_space<IntegralType,1> & start, const vector_space<IntegralType,1> & sizes, const vector_space<IntegralType,1> & steps)
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
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class Type,class Alloc,typename>
	  vector_space<Type,1,Add,Multiply,Alloc> vector_space<T,N,Add,Multiply,Allocator>::dimensions() ZINHART_NOEXCEPT
	  {
		//dimensions_check(std::false_type(),space.get(),space.get() + order());
		return vector_space<Type,1,Add,Multiply,Alloc>(space.get(),space.get() + order());
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class Type,class Alloc,typename>
	  vector_space<Type,1,Add,Multiply,Alloc> vector_space<T,N,Add,Multiply,Allocator>::dimensions() const ZINHART_NOEXCEPT
	  {
		//dimensions_check(std::false_type(),space.get(),space.get() + order());
		return vector_space<Type,1,Add,Multiply,Alloc>(space.get(),space.get() + order());
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class Type,class Alloc,typename>
	  vector_space<Type,1,Add,Multiply,Alloc> vector_space<T,N,Add,Multiply,Allocator>::request_basis()ZINHART_NOEXCEPT
	  {
		dimensions_check(std::false_type(),basis.get(),basis.get()+N);
		return vector_space<Type,1,Add,Multiply,Alloc>(basis.get(),basis.get()+N);
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class Type,class Alloc,typename>
	  const vector_space<Type,1,Add,Multiply,Alloc> vector_space<T,N,Add,Multiply,Allocator>::request_basis() const ZINHART_NOEXCEPT
	  {
		dimensions_check(std::false_type(),basis.get(),basis.get()+N);
		return vector_space<Type,1,Add,Multiply,Alloc>(basis.get(),basis.get()+N);
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template <class ...Args,typename>
	  typename vector_space<T,N,Add,Multiply,Allocator>::size_type vector_space<T,N,Add,Multiply,Allocator>::map(Args...args)
	  {
		static_assert(sizeof...(Args)==N,"Args in parameter pack must be == the order of the space (-_-)");
		dimensions_check(std::false_type(),space.get(),space.get()+N);
		dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
		size_type coordinates[N] = {args...};
		return std::inner_product(coordinates,coordinates+N,basis.get(),size_type(0)); 
	  }
/*
-----------------------------------------------------------------------------
Iterators                                                                    |
-----------------------------------------------------------------------------
*/
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::begin() ZINHART_NOEXCEPT
	  {
		return iterator(this->memory_cache.begin);
	  }

	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::end() ZINHART_NOEXCEPT
	  {
		return iterator(this->memory_cache.end);
	  }

	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::const_iterator vector_space<T,N,Add,Multiply,Allocator>::cbegin()const ZINHART_NOEXCEPT
	  {
		return const_iterator(this->memory_cache.begin);
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::const_iterator vector_space<T,N,Add,Multiply,Allocator>::cend()const ZINHART_NOEXCEPT
	  {
		return const_iterator(this->memory_cache.end);
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::reverse_iterator vector_space<T,N,Add,Multiply,Allocator>::rbegin() ZINHART_NOEXCEPT
	  {
		return reverse_iterator(end());
	  }
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::reverse_iterator vector_space<T,N,Add,Multiply,Allocator>::rend() ZINHART_NOEXCEPT
	  {
		return reverse_iterator(begin());
	  }
	  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
		typename vector_space<T,N,Add,Multiply,Allocator>::const_reverse_iterator vector_space<T,N,Add,Multiply,Allocator>::crbegin()const ZINHART_NOEXCEPT
	  {
		return const_reverse_iterator(cend());
	  }
	  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
		typename vector_space<T,N,Add,Multiply,Allocator>::const_reverse_iterator vector_space<T,N,Add,Multiply,Allocator>::crend()const ZINHART_NOEXCEPT
	  {
		return const_reverse_iterator(cbegin());
	  }
/*
-----------------------------------------------------------------------------
Capacity                                                                    |
-----------------------------------------------------------------------------
*/
	  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
		bool vector_space<T,N,Add,Multiply,Allocator>::empty()const ZINHART_NOEXCEPT
		{
		  return cbegin() == cend();
		}
	  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
		typename vector_space<T,N,Add,Multiply,Allocator>::size_type vector_space<T,N,Add,Multiply,Allocator>::size()const ZINHART_NOEXCEPT
		{
		  return size_type(this->memory_cache.end - this->memory_cache.begin);
		}
	  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
		typename vector_space<T,N,Add,Multiply,Allocator>::size_type vector_space<T,N,Add,Multiply,Allocator>::capacity()const ZINHART_NOEXCEPT
		{
		  return size_type(this->memory_cache.end_of_cache - this->memory_cache.begin);
		}
	  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
		constexpr typename vector_space<T,N,Add,Multiply,Allocator>::size_type vector_space<T,N,Add,Multiply,Allocator>::order()const ZINHART_NOEXCEPT
		{
		  return N;
		}
	  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
		typename vector_space<T,N,Add,Multiply,Allocator>::size_type vector_space<T,N,Add,Multiply,Allocator>::max_size()const ZINHART_NOEXCEPT
	  {
		return alloc_traits::max_size(get_memory_cache_alloc_type());
	  }
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::reserve(size_type size)
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  if(size>max_size())
		ZINHART_THROW_EXCEPTION_AGAIN std::length_error("vector_space<T,N>::reserve requested size > max_size()");
	  if(this->capacity() < size)
	  {
		stack(size);
		const size_type old_size = this->size();
		size = hyper_volume(space.get(),space.get()+N);
		pointer temp = allocate_and_copy(size,
										 ZINHART_MAKE_MOVE_ITERATOR_IF_NO_EXCEPT(this->memory_cache.begin),
										 ZINHART_MAKE_MOVE_ITERATOR_IF_NO_EXCEPT(this->memory_cache.end)
										 );
		zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
		deallocate_cache_of_size(this->memory_cache.begin, this->memory_cache.end_of_cache - this->memory_cache.begin);
		this->memory_cache.begin = temp;
		this->memory_cache.end = temp + old_size;
		this->memory_cache.end_of_cache = this->memory_cache.begin + size;
	  }
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::shrink_to_fit()
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  shrink_to_fit_helper();
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	}
/*
-----------------------------------------------------------------------------
Modifiers                                                                   |
-----------------------------------------------------------------------------
*/
#if __cplusplus >= 201103L
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	template <class InputIterator, typename>
	void vector_space<T,N,Add,Multiply,Allocator>::assign(InputIterator start,InputIterator finish)
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  assign_dispatcher(start,finish,std::false_type());
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	}	
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::assign(std::initializer_list<value_type> ilist)
	{

	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  this->assign_helper(ilist.begin(),ilist.end(),std::random_access_iterator_tag());
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	}
  //a caveat: where type deduction occurs && can bind to rvalues and lvalues hence it is a universal reference
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::assign(size_type && size,const_reference value)
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  this->fill_assign(size,value);
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	}
#else
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	template <class InputIterator>
	void vector_space<T,N,Add,Multiply,Allocator>::assign(InputIterator start,InputIterator finish)
	  {
		assign_dispatcher(start,finish,std::false_type());
	  }

  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::assign(const size_type & size,const_reference value)
	{
	  dimensions_check(std::false_type());
	  this->fill_assign(size,value);
	  dimensions_check(std::true_type(),capacity());
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::assign(const size_type size,const_reference value)
	{
	  dimensions_check(std::false_type());
	  dimensions_check(std::true_type(),capacity());
	  this->fill_assign(size,value);
	  dimensions_check(std::false_type());
	  dimensions_check(std::true_type(),capacity());

	}
#endif
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::clear()
	{
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  erase_at_end(this->memory_cache.begin);
	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	}
#if __cplusplus >= 201103L
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	template<class...Args,typename>
	typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::emplace(const_iterator pos,Args&&...args)
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  const size_type length = pos - begin();
	  if(this->memory_cache.end != this->memory_cache.end_of_cache && pos == end())
	  {
		alloc_traits::construct(this->memory_cache,this->memory_cache.end,std::forward<Args>(args)...);
		++this->memory_cache.end;
	  }
	  else
		insert_helper(begin() + (pos - cbegin()),std::forward<Args>(args)...);
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  return iterator(this->memory_cache.begin + length);
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	template<class...Args>
	typename vector_space<T,N,Add,Multiply,Allocator>::reference vector_space<T,N,Add,Multiply,Allocator>::emplace_back(Args&&...args)
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  if(this->memory_cache.end != this->memory_cache.end_of_cache)
	  {
		alloc_traits::construct(this->memory_cache,this->memory_cache.end,std::forward<T>(args)...);
		++this->memory_cache.end;
	  }
	  else
	  {
		emplace_back_helper(std::forward<Args>(args)...);
	  }
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  return *iterator(end());
	}
#endif
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::insert(const_iterator pos,const_reference value)
	{ 
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  const size_type length = pos - begin();
	  if(this->memory_cache.end != this->memory_cache.end_of_cache )
	  {
		if(pos == end())
		{
		  alloc_traits::construct(this->memory_cache,this->memory_cache.end,value);
		  ++this->memory_cache.end;
		}
		else
		{
#if __cplusplus >= 201103L
		  const_iterator position = begin() + (position - cbegin());
		  value_type value_copy = value;
	  	  insert_helper(pos,std::move(value_copy));
#else
		  insert_helper(position,value);
#endif
		}
	  }
	  else
	  {
#if __cplusplus >= 201103L
		realloc_insert(begin(),+ (pos - cbegin() ),value);
#else
		realloc_insert(pos,value);
#endif
	  }
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  return iterator(this->memory_cache.begin + length);
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::insert(const iterator pos,size_type size,const_reference value)
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  difference_type offset = pos - cbegin();
	  fill_insert(begin() + offset,size,value);
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  return begin() + offset;
	}
#if __cplusplus >= 201103L
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	template<class InputIterator,typename>
	  typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::insert(const_iterator pos,InputIterator begin,InputIterator end)
	  {
		dimensions_check(std::false_type(),space.get(),space.get()+N);
  		dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
		difference_type offset = pos - cbegin();
		insert_dispatcher(this->begin() + offset, begin, end,std::false_type());	
  		dimensions_check(std::false_type(),space.get(),space.get()+N);
  		dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
		return this->begin() + offset;
	  }
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::insert(const_iterator pos, std::initializer_list<value_type> ilist)
	{
	  return this->insert(pos,ilist.begin(),ilist.end());
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::insert(const_iterator pos, value_type && value)
	{
	  return emplace(pos,std::move(value));
	}
#else
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	template<class InputIt>
	void vector_space<T,N,Add,Multiply,Allocator>::insert(iterator pos,InputIt begin,InputIt end)
	{

	} 
#endif
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::erase(const_iterator pos)
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  iterator ret_val = erase_helper(begin() + (pos-cbegin()));
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  return ret_val;
	}
#if __cplusplus >= 201103L
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::erase(const_iterator first, const_iterator last)
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  iterator begin = this->begin();
	  //const_iterator cbegin = this->cbegin();
	  iterator ret_val = erase_helper(begin + (first - cbegin()),begin + (last - cbegin()));
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  return ret_val;
	} 
#else
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  typename vector_space<T,N,Add,Multiply,Allocator>::iterator vector_space<T,N,Add,Multiply,Allocator>::erase(iterator first,iterator last)
	  {
		return erase_helper(first,last);
	  }
#endif
	template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  template<class Type,template<typename> class add,template<typename> class multiply,class Alloc,typename>
	  void vector_space<T,N,Add,Multiply,Allocator>::reshape(const vector_space<Type,1,add,multiply,Alloc> & shape)
	  {
		if(shape != dimensions())
		{
		  dimensions_check(std::true_type(),space.get(),space.get() + order(),capacity());
		  generate_basis(shape);
		  auto requested_capacity = hyper_volume(space.get(),space.get() + order());
		  allocate_and_initialize(requested_capacity, value_type());
		  dimensions_check(std::false_type(),space.get(),space.get() + order());
		  dimensions_check(std::true_type(),space.get(),space.get() + order(),capacity());
		}
	  }
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
   	template<class Type,template<typename> class add,template<typename> class multiply,class Alloc,typename>
	void vector_space<T,N,Add,Multiply,Allocator>::reshape(vector_space<Type,1,add,multiply,Alloc> && shape)
	{ 
	  if(shape != dimensions())
	  {
		dimensions_check(std::true_type(),space.get(),space.get() + order(),capacity());	  
		generate_basis(std::move(shape));
		auto requested_capacity = hyper_volume(space.get(),space.get() + order());
		allocate_and_initialize(requested_capacity, value_type());
		dimensions_check(std::false_type(),space.get(),space.get() + order());
		dimensions_check(std::true_type(),space.get(),space.get() + order(),capacity());
	  }
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	template <class ...Args,typename>
	void vector_space<T,N,Add,Multiply,Allocator>::reshape(const Args&...args)
	{
	  static_assert(sizeof...(Args) == N , "Illegal number of functions args");
	  dimensions_check(std::true_type(),space.get(),space.get() + order(), capacity());
	  generate_basis(std::move(vector_space <T,1,Add,Multiply,Allocator>({args...}) ));
	  auto requested_capacity = hyper_volume(space.get(),space.get() + order());
	  allocate_and_initialize(requested_capacity, value_type());
	  dimensions_check(std::false_type(),space.get(),space.get() + order());
	  dimensions_check(std::true_type(),space.get(),space.get() + order(),capacity());
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	template <class...Args,typename>
	void vector_space<T,N,Add,Multiply,Allocator>::reshape(Args&&...args)
	{
	  static_assert(sizeof...(Args) == N , "Illegal number of functions args");
	  dimensions_check(std::true_type(),space.get(),space.get() + order(),capacity());
	  generate_basis(std::forward<Args>(args)...);
	  auto requested_capacity = hyper_volume(space.get(),space.get() + order());
	  allocate_and_initialize(requested_capacity, value_type());
	  dimensions_check(std::false_type(),space.get(),space.get() + order());
	  dimensions_check(std::true_type(),space.get(),space.get() + order(),capacity());
}
#if __cplusplus >= 201103L
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::resize(size_type && size)
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  if(size > this->size())
	  {
		default_append(size);	
	  }
	  else if(size < this->size())
		erase_at_end(this->memory_cache.begin + size);
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::resize(const size_type &size,const_reference value)
	{
	  if(size > this->size())
	  {
		insert(end(),size-this->size(),value);
	  }
	  else
		erase_at_end(this->memory_cache.begin + size);
	}
#else
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::resize(const size_type & size)
	{
	  if(size > this->size())
		default_append(size-this->size());
	  else if(size < this->size())
		erase_at_end(this->memory_cache.begin + size);
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::resize(const size_type &size,const_reference value)
	{
	  if(size > this->size())
		insert(end(),size-this->size(),value);
	  else
		erase_at_end(this->memory_cache.begin + size);
	}
#endif
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::push_back(const_reference value)
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  if(this->memory_cache.end != this->memory_cache.end_of_cache)
	  {	
		alloc_traits::construct(this->memory_cache,this->memory_cache.end,value);
		++this->memory_cache.end;
	  }
	  else //if(true)
	  {
#if __cplusplus >= 201103L
		
		emplace_back_helper(value);	
#else
  //insert_aux
		insert_helper(end(),value);
#endif
	  }
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	}
#if __cplusplus >= 201103L
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::push_back(value_type && value)
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  emplace_back(std::move(value));
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	}
#else
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::push_back(value_type value)
	{
	}
#endif
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::pop_front()ZINHART_NOEXCEPT
	{
	  //rotate beginning to the back then pop back;
	  //simple right rotation
	  //std::rotate(rbegin(),rbegin()+1,rend());
	  //simple left rotation
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  std::rotate(begin(),begin()+1,end());
	  pop_back();
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	void vector_space<T,N,Add,Multiply,Allocator>::pop_back()ZINHART_NOEXCEPT
	{
  	  dimensions_check(std::false_type(),space.get(),space.get()+N);
  	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  --this->memory_cache.end;
	  alloc_traits::destroy(this->memory_cache,this->memory_cache.end);
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	template<template<typename> class F1, template<typename> class F2>
	void vector_space<T,N,Add,Multiply,Allocator>::swap(vector_space<T,N,F1,F2> & vs)
	{
  	  dimensions_check(std::false_type(),space.get(),space.get()+order());
  	  dimensions_check(std::true_type(),space.get(),space.get()+order(),capacity());
	  this->memory_cache.memory_cache_swap_data( vs.memory_cache);
	  if(alloc_traits::propagate_on_container_swap::value)
		std::swap(get_memory_cache_alloc_type(), vs.get_memory_cache_alloc_type());
	  space.swap(vs.space);
	  basis.swap(vs.basis);
	  dimensions_check(std::false_type(),space.get(),space.get()+order());
  	  dimensions_check(std::true_type(),space.get(),space.get()+order(),capacity());
}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	vector_space<T,N,Add,Multiply,Allocator> & vector_space<T,N,Add,Multiply,Allocator>::operator =(const vector_space &  vs)
	{
	  vs.dimensions_check( std::false_type(), vs.space.get(), vs.space.get() + vs.order() );
  	  vs.dimensions_check( std::true_type(), vs.space.get(), vs.space.get() + vs.order(), vs.capacity() );
	  if(& vs != this)
	  {
#if __cplusplus >= 201103L
		if(alloc_traits::propagate_on_container_copy_assignment::value)
		{
#if __cplusplus >= 201703L
		if(!alloc_traits::is_always_equal::value && get_memory_cache_alloc_type() !=  vs.get_memory_cache_alloc_type())
#else
		  /*not exactly the same as is_always_equal, is_always_equal means
		   * alloc<T1>==alloc<T2>, just an fyi
			* */
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
		space = vs.space;
		basis = vs.basis;
		size_type new_capacity = hyper_volume( space.get(), space.get() + order() );
		pointer new_begin = allocate_and_copy(new_capacity,vs.cbegin(),vs.cend());
		zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
		deallocate_cache_of_size(this->memory_cache.begin,this->memory_cache.end_of_cache - this->memory_cache.begin);
		this->memory_cache.begin = new_begin;
		this->memory_cache.end_of_cache = this->memory_cache.begin + new_capacity;
	  }
	  else if (this->size() >= length)
	  {
		zinhart::destroy(std::copy( vs.cbegin(), vs.cend(),this->begin()),this->end(),this->get_memory_cache_alloc_type());
	  }
	  else
	  {
		std::copy( vs.memory_cache.begin, vs.memory_cache.begin + size(),this->memory_cache.begin);
		zinhart::uninitialized_copy_allocator( vs.memory_cache.begin + this->size(), vs.memory_cache.end,this->memory_cache.end,this->get_memory_cache_alloc_type());
	  }
	  this->memory_cache.end = this->memory_cache.begin + length;
	}
  	  dimensions_check( std::false_type(), space.get(), space.get() + order() );
	  dimensions_check( std::true_type(), space.get(), space.get() + order(), capacity() );
  	  return *this;;
  }
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	vector_space<T,N,Add,Multiply,Allocator>  vector_space<T,N,Add,Multiply,Allocator>::operator +(const vector_space & vs)const
	{
	  Add<vector_space> F;
	  return F(*this,vs); 
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	vector_space<T,N,Add,Multiply,Allocator>  vector_space<T,N,Add,Multiply,Allocator>::operator *(const vector_space & vs)const
	{
	  Multiply<vector_space> G;
	  return G(*this,vs);
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	vector_space<T,N,Add,Multiply,Allocator>  & vector_space<T,N,Add,Multiply,Allocator>::operator +=(const vector_space & vs)
	{
	  Add<vector_space> F;
	  *this = F(*this,vs);
	  return *this;
	}
 template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	vector_space<T,N,Add,Multiply,Allocator>  & vector_space<T,N,Add,Multiply,Allocator>::operator *=(const vector_space & vs)
	{
	  Multiply<vector_space> G;
	  *this = G(*this,vs);
	  return *this;
	}
#if __cplusplus >= 201103L
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	vector_space<T,N,Add,Multiply,Allocator> & vector_space<T,N,Add,Multiply,Allocator>::operator =(vector_space &&  vs) noexcept
	{
	  vs.dimensions_check( std::false_type(), vs.space.get(), vs.space.get() + vs.order() );
	  vs.dimensions_check( std::true_type(), vs.space.get(), vs.space.get() + vs.order(), vs.capacity() );
	  space = vs.space, basis = vs.basis;
  	  vs.space.reset(),vs.basis.reset();
	  vs.allocate_and_initialize_private_members();
	  if(alloc_traits::propagate_on_container_move_assignment::value || get_memory_cache_alloc_type() ==  vs.get_memory_cache_alloc_type())
	  {
		vector_space temp(get_allocator());
		this->memory_cache.memory_cache_swap_data(temp.memory_cache);
		this->memory_cache.memory_cache_swap_data( vs.memory_cache);
		std::swap(get_memory_cache_alloc_type(), vs.get_memory_cache_alloc_type());
	  }
	  else
	  {
		size_type new_capacity = hyper_volume( space.get(), space.get() + order() );
		pointer new_begin(allocate_and_move(new_capacity,vs.cbegin(),vs.cend()));
		zinhart::destroy(this->memory_cache.begin,this->memory_cache.end,this->get_memory_cache_alloc_type());
		deallocate_cache_of_size(this->memory_cache.begin,this->memory_cache.end_of_cache - this->memory_cache.begin);
		this->memory_cache.begin = new_begin;
		this->memory_cache.end = new_begin + size();
		this->memory_cache.end_of_cache = new_begin + new_capacity;
		vs.clear();
	  }
	  dimensions_check( std::false_type(), space.get(), space.get() + order() );
	  dimensions_check( std::true_type(), space.get(), space.get() + order(), capacity() );
	  return *this;
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	vector_space<T,N,Add,Multiply,Allocator> & vector_space<T,N,Add,Multiply,Allocator>::operator =(std::initializer_list<T> ilist)
	{
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  this->assign(ilist.begin(),ilist.end());
	  dimensions_check(std::false_type(),space.get(),space.get()+N);
	  dimensions_check(std::true_type(),space.get(),space.get()+N,capacity());
	  return *this;
	}

  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	vector_space<T,N,Add,Multiply,Allocator>  vector_space<T,N,Add,Multiply,Allocator>::operator +(vector_space && vs)
	{
	  Add<vector_space> F;
	  return F(*this,vs);
	}

  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	vector_space<T,N,Add,Multiply,Allocator>  vector_space<T,N,Add,Multiply,Allocator>::operator *(vector_space && vs)
	{
	  Multiply<vector_space> G;
	  return G(*this,vs);

	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	vector_space<T,N,Add,Multiply,Allocator> & vector_space<T,N,Add,Multiply,Allocator>::operator += (vector_space && vs)
	{
	  Add<vector_space> F;
	  *this = F(*this,vs);
	  return *this;
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	vector_space<T,N,Add,Multiply,Allocator> & vector_space<T,N,Add,Multiply,Allocator>::operator *= (vector_space && vs)
	{
	  Multiply<vector_space> G;
	  *this = G(*this,vs);
	  return *this;
	}
#endif
/*
-----------------------------------------------------------------------------
Non Member Functions                                                        |
-----------------------------------------------------------------------------
*/
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	inline bool operator ==(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs)
	{
	  return (lhs.size() == rhs.size() && std::equal(lhs.cbegin(),lhs.cend(),rhs.cbegin()));
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	inline bool operator !=(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs)
	{
	  return !(lhs == rhs);
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	inline bool operator <(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs)
	{
	  return std::lexicographical_compare(lhs.cbegin(),lhs.cend(),rhs.cbegin(),rhs.cend());
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	inline bool operator <=(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs)
	{
	  return !(rhs < lhs);
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	inline bool operator >(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs)
	{
	  return rhs < lhs;
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	inline bool operator >=(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs)
	{
	  return !(lhs < rhs);
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	inline void swap(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs)
	{
	  lhs.swap(rhs);
	}
  template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator >
	std::ostream & operator <<(std::ostream & os, const vector_space<T,N,Add,Multiply,Allocator> &  vs)
	{
	  typedef typename vector_space<T,N,Add,Multiply,Allocator>::size_type size_type; 
	 // typedef typename vector_space<T,N,Add,Multiply,Allocator>::iterator it;
	  auto dim = vs.dimensions();
	  if(dim.size() > 1)
	  {
	//	if(!vs.empty())
	//	{
		  size_type x = std::accumulate(dim.begin(),dim.end()-2,size_type(1),std::multiplies<size_type>());
		  auto y = vs.cbegin();
		  for(size_type i = 0; i < x;++i )
		  {
			for(size_type j = 0; j< dim[dim.size()-2];++j)
			{
			  for(size_type j = 0; j< dim[dim.size()-1];++j)
			  {
				os<<*y<<" ";
				++y;
			  }
			os<<"\n";
			}
		  os<<"\n";
		  }
	//	}
	  }
	  else
	  {
		std::copy(vs.cbegin(),vs.cend(),std::ostream_iterator<T>(os," "));
		os.clear();
	  }
	  return os;
	}
/*#########################################################################################
 * SUBSPACE<T,N>
 * */
  template <class T,lluint N,template<typename> class Add,template <typename> class Multiply,class Allocator>
	sub_space<T,N,Add,Multiply,Allocator>::sub_space(pointer data, const size_type *dimensions, const size_type *subSpaceLength)
	  :_data(data), space(dimensions),basis(subSpaceLength){}
  template <class T,lluint N,template<typename> class Add,template <typename> class Multiply,class Allocator>
	sub_space<T,N-1,Add,Multiply,Allocator> sub_space<T,N,Add,Multiply,Allocator>::operator [](size_type index)
	{
	  assert(index<space[0]);
	  //static_assert(sub_space<T,N>::indexCheck(i,space[0]),"INVALID INDEX");
	  return sub_space<T,N-1,Add,Multiply,Allocator>(&_data[index *basis[0]],space+1,basis+1);
	} 
  template <class T,lluint N,template<typename> class Add,template <typename> class Multiply,class Allocator>
	const sub_space<T,N-1,Add,Multiply,Allocator> sub_space<T,N,Add,Multiply,Allocator>::operator [](size_type index)const
	{
	  assert(index<space[0]);
	  //static_assert(i<space[0],"INVALID INDEX");
	  return sub_space<T,N-1,Add,Multiply,Allocator>(&_data[index *basis[0]],space+1,basis+1);
	}
/*
 * #########################################################################################
 * SUBSPACE<T,1>
 */
  template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	sub_space<T,1,Add,Multiply,Allocator>::sub_space(pointer data, const size_type * dimensions, const size_type * subSpaceLength)
	  :_data(data), space(dimensions),basis(subSpaceLength){}
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	typename sub_space<T,1,Add,Multiply,Allocator>::reference sub_space<T,1,Add,Multiply,Allocator>::operator [](size_type index)
	{
	  assert(index<space[0]);
	  //static_assert(sub_space<T,1>::indexCheck(i,space),"INVALID INDEX");
	  return _data[index];
	}
	template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  const typename sub_space<T,1,Add,Multiply,Allocator>::reference sub_space<T,1,Add,Multiply,Allocator>::operator [](size_type index)const
	  {
		assert(index<space[0]);
		//static_assert(sub_space<T,1>::indexCheck(i,space),"INVALID INDEX");
		return _data[index];
	  } 
}//end tool

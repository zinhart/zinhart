#ifndef VECTOR_SPACE_H
#define VECTOR_SPACE_H
#include <memory>
#include <iostream>
namespace zinhart
{
  template <class T,class Allocator>
  	class vector_space_base
    {
	  public:
		typedef typename std::allocator_traits<Allocator>::template rebind_alloc<T> type_of_allocator;
		typedef typename std::allocator_traits<Allocator>::pointer pointer;
		typedef typename std::allocator_traits<Allocator>::size_type size_type;
	  private:
		struct Memory_Cache: public type_of_allocator
		{
		  pointer begin;
          pointer end;
		  pointer end_of_cache;
		  Memory_Cache() : type_of_allocator(), begin(), end(), end_of_cache(){}
		  Memory_Cache(type_of_allocator const & alloc)ZINHART_NOEXCEPT
			:type_of_allocator(alloc),begin(),end(),end_of_cache(){}
#if __cplusplus >= 201103L
		  Memory_Cache(type_of_allocator && alloc)ZINHART_NOEXCEPT
			:type_of_allocator(std::move(alloc)),begin(), end(), end_of_cache(){}
#endif
		  void memory_cache_swap_data(Memory_Cache & another) ZINHART_NOEXCEPT
		  {
			std::swap(begin,another.begin);
			std::swap(end,another.end);
			std::swap(end_of_cache,another.end_of_cache);
		  }
		};
	  protected:
		type_of_allocator & get_memory_cache_alloc_type() ZINHART_NOEXCEPT
		{
		  return *static_cast<type_of_allocator*>(&this->memory_cache);
		}
		const type_of_allocator & get_memory_cache_alloc_type()const ZINHART_NOEXCEPT
		{
		  return *static_cast<const type_of_allocator*>(&this->memory_cache);
		}
		void create_cache_of_size(size_type N)
		{
		  this->memory_cache.begin = this->allocate_cache_of_size(N);
		  this->memory_cache.end = this->memory_cache.begin;
		  this->memory_cache.end_of_cache = this->memory_cache.begin+N;
		}
	  public:
		typedef Allocator allocator_type;
		Memory_Cache memory_cache;
		allocator_type get_allocator()const ZINHART_NOEXCEPT
		{
		  return allocator_type(get_memory_cache_alloc_type());
		}
		vector_space_base() : memory_cache(){}
		vector_space_base(const allocator_type & alloc)ZINHART_NOEXCEPT
		  :memory_cache(alloc){}
		template <class ...Args,typename =RequireSimilarTypes<void,typename Allocator::size_type,Args...>>
		  vector_space_base(Args&&...args)
		  :memory_cache()
		  {
			size_type R[] = {args...};//Args is N as in R^N
			create_cache_of_size(std::accumulate(R,R + sizeof...(Args),size_type(1),std::multiplies<size_type>()));
		  }
		/*vector_space_base(size_type N) : memory_cache()
		{
		  create_cache_of_size(N);
		}*/
		vector_space_base(size_type N,const allocator_type &alloc) : memory_cache(alloc)
        {
		  create_cache_of_size(N);
		}
#if __cplusplus >= 201103L
		vector_space_base(type_of_allocator && alloc)noexcept
		  :memory_cache(std::move(alloc)){}
		vector_space_base(vector_space_base && another) noexcept
		  :memory_cache(std::move(another.get_memory_cache_alloc_type()))
		  {
			this->memory_cache.memory_cache_swap_data(another.memory_cache);
		  }
	 	vector_space_base(vector_space_base &&another,const allocator_type & alloc)
		  :memory_cache(alloc)
		{
		  if(another.get_allocator() == alloc)
			this->memory_cache.memory_cache_swap_data(another.memory_cache);
		  else
		  {
			size_type N = another.memory_cache.end - another.memory_cache.begin;
	  		create_cache_of_size(N);
		  }
		}
#endif
		~vector_space_base() ZINHART_NOEXCEPT
		{
		  deallocate_cache_of_size(this->memory_cache.begin ,this->memory_cache.end_of_cache - this->memory_cache.begin);
		}
		pointer allocate_cache_of_size(size_type N)
		{
		  typedef std::allocator_traits<type_of_allocator> local_allocator;
		  return  N != 0 ? local_allocator::allocate(memory_cache,N) : pointer();
		}
		void deallocate_cache_of_size(pointer cache, size_type cache_size)
		{
		  typedef std::allocator_traits<type_of_allocator> local_allocator;
		  if(cache)
			local_allocator::deallocate(memory_cache,cache,cache_size);
		}
	};
    template<class T, template<typename> class Add,template <typename> class Multiply>
  	  class vector_space<T,0,Add,Multiply>
	  {
	  //Concepts
#if __cplusplus >= 201703L

#else
		public:
		  static_assert(std::is_default_constructible<T>::value,"Type T is Not Default Constructible");
		  static_assert(std::is_copy_constructible<T>::value,"Type T is Not Copy Constructible");
		  static_assert(std::is_copy_assignable<T>::value,"Type T is Not Copy Assignable");
		  static_assert(std::is_move_constructible<T>::value,"Type T is Not Move Constructible");
		  static_assert(std::is_move_assignable<T>::value,"Type T is Not Move Assignable");
#endif
//----------Member Types
    	  typedef T value_type;
		  typedef value_type & reference;
		  typedef const value_type & const_reference;
		  typedef value_type * pointer;
		  typedef const value_type * const_pointer;
//----------Constructors
		  vector_space()ZINHART_NOEXCEPT;
		  vector_space(vector_space<T,0,Add,Multiply> & another)ZINHART_NOEXCEPT;
		  vector_space(vector_space<T,0,Add,Multiply> && another)ZINHART_NOEXCEPT;
//----------Destructor
		  ~vector_space()ZINHART_NOEXCEPT;
//----------Modifiers
		  void set(const_reference value)ZINHART_NOEXCEPT;
		  void set(value_type && value)ZINHART_NOEXCEPT;
		  vector_space<T,0,Add,Multiply> & operator =(const vector_space<T,0,Add,Multiply> & another)ZINHART_NOEXCEPT;
		  vector_space<T,0,Add,Multiply> & operator =(vector_space<T,0,Add,Multiply> && another) ZINHART_NOEXCEPT;
		  void swap(vector_space<T,0,Add,Multiply> &another)ZINHART_NOEXCEPT;
			//void operator +(vector_space vs<)
//----------Accessors
		  const_reference get()const ZINHART_NOEXCEPT;
		private:
		  T _data;
	  };
	template <class T,template<typename> class Add,template<typename> class Multiply>
	  inline bool operator ==(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs);
    template <class T,template<typename> class Add,template<typename> class Multiply>
	  inline bool operator !=(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs);
    template <class T,template<typename> class Add,template<typename> class Multiply>
	  inline bool operator <(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs);
    template <class T,template<typename> class Add,template<typename> class Multiply>
	  inline bool operator <=(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs);
    template <class T,template<typename> class Add,template<typename> class Multiply>
	  inline bool operator >(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs);
    template <class T,template<typename> class Add,template<typename> class Multiply>
	  inline bool operator >=(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs);
    template <class T,template<typename> class Add,template<typename> class Multiply>
	  inline void swap(const vector_space<T,0,Add,Multiply> &lhs, const vector_space<T,0,Add,Multiply> &rhs);
    //End vector_space<T,0,Add,Multiply>
    template<class T, template<typename> class Add,template <typename> class Multiply,class Allocator>
	  class vector_space<T,1,Add,Multiply,Allocator> : protected vector_space_base<T,Allocator>
	  {
#if __cplusplus >= 202003L
            //Concepts   
#elif __cplusplus >= 201103L
		  static_assert(std::is_default_constructible<T>::value || std::is_trivially_default_constructible<T>::value || std::is_copy_constructible<T>::value ,"Type T Must be Trivially, Default, or Copy Constructible");
		  static_assert(std::is_copy_assignable<T>::value,"Type T is Not Copy Assignable");
		  static_assert(std::is_move_constructible<T>::value,"Type T is Not Move Constructible");
		  static_assert(std::is_move_assignable<T>::value,"Type T is Not Move Assignable");
		  static_assert(std::is_same<T,typename Allocator::value_type>::value,"Type T is Not The Same as Allocator::value_type");
#else

#endif
		  typedef vector_space_base<T,Allocator> Base;
		  typedef typename Base::type_of_allocator type_of_allocator;
		  typedef std::allocator_traits<type_of_allocator> alloc_traits;
		protected:
		  using Base::allocate_cache_of_size;
          using Base::deallocate_cache_of_size;
          using Base::memory_cache;
          using Base::get_memory_cache_alloc_type;
		public:
//----------Member Types
          typedef typename alloc_traits::allocator_type allocator_type;
 		  typedef typename alloc_traits::value_type value_type;
		  typedef value_type & reference;
		  typedef const value_type & const_reference;
		  typedef typename Base::pointer pointer;
		  typedef typename alloc_traits::const_pointer const_pointer;
		  typedef generic_iterator<pointer,vector_space<T,1,Add,Multiply,Allocator>> iterator;
		  typedef generic_iterator<const_pointer,vector_space<T,1,Add,Multiply,Allocator>>  const_iterator;
		  typedef typename alloc_traits::size_type size_type;
		  typedef typename alloc_traits::difference_type difference_type;
		  typedef std::reverse_iterator<iterator> reverse_iterator;
		  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
		  typedef Add<T> scalar_addition;
		  typedef Multiply<T> scalar_multiplication;
//----------Member Functions
//----------Constructors
          vector_space()
#if __cplusplus >= 201103L
			noexcept(std::is_nothrow_default_constructible<Allocator>::value);
#endif
		  explicit vector_space(const allocator_type & alloc)ZINHART_NOEXCEPT;
#if __cplusplus >= 201103L
		  explicit vector_space(const size_type & size,const_reference value= T(),const allocator_type & alloc = allocator_type());
		  explicit vector_space(size_type && size,value_type && value=T(),const allocator_type && alloc = allocator_type());
#else
		  explicit vector_space(const size_type  size,value_type value= value_type(),const allocator_type & alloc = allocator_type());
		  explicit vector_space(const size_type & size,const reference value= value_type(),const allocator_type & alloc = allocator_type());
#endif
		  vector_space(const vector_space &another);
#if __cplusplus >= 201103L
		  vector_space(vector_space &&another)noexcept;
		  vector_space(const vector_space &another,const allocator_type & alloc);
		  vector_space(vector_space &&, const allocator_type & alloc)noexcept;
		  vector_space(std::initializer_list<value_type> ilist,const allocator_type & alloc = allocator_type());
#endif
#if __cplusplus >= 201103L
		  template <typename InputIterator,typename = RequireAnInputIter<InputIterator>>
			//typename std::enable_if<std::is_convertible< typename std::iterator_traits<InputIterator>::iterator_category,std::input_iterator_tag  >::value >::type>
            vector_space(InputIterator first, InputIterator last, const allocator_type & alloc = allocator_type());
#else
		  template <InputIterator>
			vector_space(InputIterator first, InputIterator last,const allocator_type & alloc=allocator_type());
#endif
//----------Destructor
            ~vector_space()ZINHART_NOEXCEPT;
//----------Accessors
            reference operator[](size_type index) ZINHART_NOEXCEPT;
            const_reference operator[](size_type index)const ZINHART_NOEXCEPT;
            reference front()ZINHART_NOEXCEPT;
            reference back()ZINHART_NOEXCEPT;
            const_reference front()const ZINHART_NOEXCEPT;
            const_reference back()const ZINHART_NOEXCEPT;
            reference at(const size_type index)ZINHART_NOEXCEPT;
            const_reference at(const size_type index)const ZINHART_NOEXCEPT;
            pointer data()ZINHART_NOEXCEPT;
            const_pointer data()const ZINHART_NOEXCEPT;
			template<class IntegralType>
			  vector_space<T,1,Add,Multiply> slice(IntegralType && start, IntegralType && size, IntegralType && step) ZINHART_NOEXCEPT;
			template<class IntegralType>
			  vector_space<T,1,Add,Multiply> gslice(const vector_space<IntegralType,1> & start, const vector_space<IntegralType,1> & sizes, const vector_space<IntegralType,1> & strides);
			//notice that with this, get_allocator can be used in functions,outside of the derived class
            using Base::get_allocator;
//----------Iterators
            iterator begin()ZINHART_NOEXCEPT;
            iterator end()ZINHART_NOEXCEPT;
            const_iterator cbegin()const ZINHART_NOEXCEPT;
            const_iterator cend()const ZINHART_NOEXCEPT;
            reverse_iterator rbegin()ZINHART_NOEXCEPT;
            reverse_iterator rend()ZINHART_NOEXCEPT;
            const_reverse_iterator crbegin()const ZINHART_NOEXCEPT;
            const_reverse_iterator crend()const ZINHART_NOEXCEPT;
//----------Capacity
            bool empty()const ZINHART_NOEXCEPT;
            size_type size()const ZINHART_NOEXCEPT;
            size_type capacity()const ZINHART_NOEXCEPT;
            constexpr size_type order() const ZINHART_NOEXCEPT;
            //consider using constexpr for order of vector space instead of  size type
            //constexpr lluint order(){return 1;}
            size_type max_size()const ZINHART_NOEXCEPT;
            //requests container shrinks to size()
            void shrink_to_fit();
            //increases capacity, throws length error if new_capacity > max_size()
            void reserve(size_type new_capacity);
			template<class Type=lluint,class Alloc = std::allocator<Type>, typename = RequireUnsignedReal<void,Type,Type>>
			  vector_space<Type,1,Add,Multiply,Alloc> dimensions()const ZINHART_NOEXCEPT;
//----------Modifiers
            void assign(const size_type & size,const_reference value);

#if __cplusplus >= 201103L
			template <class InputIterator, typename = RequireAnInputIter<InputIterator>>
  			  void assign(InputIterator start,InputIterator finish);
  			void assign(std::initializer_list<value_type> ilist);           
  			void assign(size_type && size,const_reference value);
#else
			template <class InputIterator>
  			  void assign(InputIterator start,InputIterator finish);
            void assign(const size_type size,const_reference value);
#endif
            void clear()ZINHART_NOEXCEPT;
#if __cplusplus >= 201103L
			template <class...Args>
  			  iterator emplace(const_iterator pos,Args&&...args);
            template<class...Args>
			  reference emplace_back(Args&&...args);
#endif
            iterator insert(const_iterator pos,const_reference value);
            iterator insert(const iterator pos,size_type size,const_reference value);
#if __cplusplus >= 201103L
			template<class InputIterator,typename = RequireAnInputIter<InputIterator>>
  			  iterator insert(const_iterator pos,InputIterator begin,InputIterator end);
  			iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);
            iterator insert(const_iterator pos, value_type && value);
#else
			template<class InputIt>
  			  void insert(iterator pos,InputIt begin,InputIt end);
#endif
            iterator erase(const_iterator pos);
#if __cplusplus >= 201103L
            iterator erase(const_iterator first, const_iterator last);
#else
            iterator erase(iterator first,iterator last);
#endif
#if __cplusplus >= 201103L
            void resize(size_type && size);
            void resize(const size_type & size,const_reference value = T());
#else
            void resize(const size_type & size);
            void resize(const size_type &size,const_reference value = T());
#endif
			void push_back(const_reference value);
#if __cplusplus >= 201103L
            void push_back(value_type && value);
#else
            void push_back(value_type value);
#endif
            void pop_front()ZINHART_NOEXCEPT;
            void pop_back()ZINHART_NOEXCEPT;
            void swap(vector_space &another)ZINHART_NOEXCEPT;
            vector_space<T,1,Add,Multiply,Allocator> & operator =(const vector_space & another);
			vector_space<T,1,Add,Multiply,Allocator>  operator +(const vector_space & another)const;
		  	vector_space<T,1,Add,Multiply,Allocator>  operator *(const vector_space & another)const;
			vector_space<T,1,Add,Multiply,Allocator> & operator += (const vector_space & vs);
			vector_space<T,1,Add,Multiply,Allocator> & operator *= (const vector_space & vs);
#if __cplusplus >= 201103L
			vector_space<T,1,Add,Multiply,Allocator> & operator =(vector_space && another) noexcept;
            vector_space<T,1,Add,Multiply,Allocator> & operator =(std::initializer_list<value_type> ilist);
			vector_space<T,1,Add,Multiply,Allocator> & operator +(vector_space && another);
			vector_space<T,1,Add,Multiply,Allocator> & operator *(vector_space && another);
			vector_space<T,1,Add,Multiply,Allocator> & operator += (vector_space && vs);
			vector_space<T,1,Add,Multiply,Allocator> & operator *= (vector_space && vs);
#endif           
		private:
//----------Internal Helper Functions
            template <class Integer>
  			  void initialize_dispatcher(Integer size, const_reference value, std::true_type);
            template <class InputIterator>
			  void initialize_dispatcher(InputIterator first, InputIterator last , std::false_type);
            template <class ForwardIterator>
			  ForwardIterator initialize(ForwardIterator start,size_type size, const value_type & value);
            void default_initialize(const size_type & size);
            void fill_initialize(const size_type & size, const_reference value);
            template <class InputIterator,class ForwardIterator>
			  ForwardIterator initialize(InputIterator start,InputIterator end,ForwardIterator result);
            template <class InputIterator>
			  void range_initialize(InputIterator first,InputIterator last,std::input_iterator_tag);
            template <class ForwardIterator>
			  void range_initialize(ForwardIterator first,ForwardIterator last,std::forward_iterator_tag);
            template <class Integer>
			  void assign_dispatcher(Integer size,const_reference values,std::true_type);
            template <class InputIterator>
			  void assign_dispatcher(InputIterator first, InputIterator last,std::false_type);
            void fill_assign(size_type & size, const_reference value);
            template <class InputIterator>
			  void assign_helper(InputIterator first, InputIterator last , std::input_iterator_tag);
            template <class ForwardIterator>
			  void assign_helper(ForwardIterator first, ForwardIterator last,std::forward_iterator_tag);
            template <class Integer>
			  void insert_dispatcher(iterator pos, const_reference value, Integer size,std::true_type);
            template <class InputIterator>
			  void insert_dispatcher(iterator pos,InputIterator first, InputIterator last , std::false_type);
            void fill_insert(iterator pos, const size_type & size,const_reference value);
            template <class InputIterator>
			  void range_insert(iterator pos, InputIterator first, InputIterator last,std::input_iterator_tag);
            template <class ForwardIterator>
			  void range_insert(iterator pos, ForwardIterator first, ForwardIterator last,std::forward_iterator_tag);
            size_type check_length(const size_type & size);
            void check_range(size_type size);
#if __cplusplus >= 201103L
            template<class...Args>
			  void insert_helper(iterator pos,Args&&...args);
#else
			template<class T, class Allocator>
  			  void insert_helper(iterator pos,const_reference value);
#endif
#if __cplusplus >= 201103L
            template<class...Args>
  			  void emplace_back_helper(Args&&...args);
#endif
            iterator erase_helper(iterator pos);
            iterator erase_helper(iterator first,iterator last);
            void erase_at_end(pointer pos) ZINHART_NOEXCEPT;
            void default_append(size_type  size);
            template <class ForwardIterator>
              pointer allocate_and_copy(size_type size, ForwardIterator first, ForwardIterator last);
            bool shrink_to_fit_helper();
#if __cplusplus >= 201103L
            template<class UPtr>
              UPtr * data_ptr(UPtr * ptr) const;
            template<class Ptr>
	          typename std::pointer_traits<Ptr>::element_type* data_ptr(Ptr  ptr) const;
#else
            template<class Ptr>
              Ptr data_ptr (Ptr ptr) const;
#endif
            void move_assign(vector_space<T,1,Add,Multiply,Allocator> && vs,std::true_type)noexcept;
            void move_assign(vector_space<T,1,Add,Multiply,Allocator> && vs,std::false_type);
    };
  	template <class T,template<typename> class Add,template<typename> class Multiply,class Allocator>
      inline bool operator ==(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs);
    template <class T,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  inline bool operator !=(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs);
    template <class T,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  inline bool operator <(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs);
    template <class T,template<typename> class Add,template<typename> class Multiply,class Allocator>
      inline bool operator <=(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs);
    template <class T,template<typename> class Add,template<typename> class Multiply,class Allocator>
      inline bool operator >(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs);
    template <class T,template<typename> class Add,template<typename> class Multiply,class Allocator>
      inline bool operator >=(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs);
    template <class T,template<typename> class Add,template<typename> class Multiply,class Allocator>
      inline void swap(const vector_space<T,1,Add,Multiply,Allocator> &lhs, const vector_space<T,1,Add,Multiply,Allocator> &rhs);
    //End vector_space<T,1>
	template<class T,lluint N ,template<typename> class Add,template <typename> class Multiply,class Allocator /*=std::allocator<T>*/>
	  class vector_space : protected vector_space_base<T,Allocator>
	  {
#if __cplusplus >= 202003L
			//Concepts            
			
#elif __cplusplus >= 201103L
		  static_assert( (std::is_default_constructible<T>::value || std::is_trivially_default_constructible<T>::value) || std::is_copy_constructible<T>::value ,"Type T Must be Trivially, Default, or Copy Constructible");
		  static_assert(std::is_copy_assignable<T>::value,"Type T is Not Copy Assignable");
		  static_assert(std::is_move_constructible<T>::value,"Type T is Not Move Constructible");
		  static_assert(std::is_move_assignable<T>::value,"Type T is Not Move Assignable");
		  static_assert(std::is_same<T,typename Allocator::value_type>::value,"Type T is Not The Same as Allocator::value_type");
#else
	
#endif
		  typedef vector_space_base<T,Allocator> Base;
		  typedef typename Base::type_of_allocator type_of_allocator;
		  typedef std::allocator_traits<type_of_allocator> alloc_traits;
		protected:
	   	  using Base::allocate_cache_of_size;
          using Base::deallocate_cache_of_size;
          using Base::memory_cache;
		  using Base::get_memory_cache_alloc_type;
        public:
//----------Member Types
          typedef typename alloc_traits::allocator_type allocator_type;
		  typedef typename alloc_traits::value_type value_type;
		  typedef value_type & reference;
		  typedef const value_type & const_reference;
		  typedef typename Base::pointer pointer;
		  typedef typename alloc_traits::const_pointer const_pointer;
		  typedef generic_iterator<pointer,vector_space<T,N,Add,Multiply,Allocator>> iterator;
		  typedef generic_iterator<const_pointer,vector_space<T,N,Add,Multiply,Allocator>>  const_iterator;
		  typedef typename alloc_traits::size_type size_type;
		  typedef typename alloc_traits::difference_type difference_type;
		  typedef std::reverse_iterator<iterator> reverse_iterator;
		  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
		  typedef Add<T> scalar_addition;
		  typedef Multiply<T> scalar_multiplication;
//---------Member Functions
//--------- Constructors
          vector_space()
#if __cplusplus >= 201103L
			noexcept(std::is_nothrow_default_constructible<Allocator>::value);
#endif
		  explicit vector_space(const allocator_type & alloc)ZINHART_NOEXCEPT;
          template <class Type,template<typename> class F1,template <typename> class F2,class Alloc /*=std::allocator<T>*/>
		   vector_space(vector_space<Type, 1, F1, F2, Alloc> && dim);
		  vector_space(const vector_space & vs);
#if __cplusplus >= 201103L
		  template <class ...Args,typename = RequireUnsignedReal<void,size_type,Args...>>
			explicit vector_space(Args&&...args)noexcept;
		  template <class ...Args,typename = RequireUnsignedReal<void,size_type,Args...>>
			explicit vector_space(Args&...args)noexcept;
		  vector_space(vector_space &&another)noexcept;
		  vector_space(const vector_space & another,const allocator_type & alloc);
		  vector_space(vector_space &&, const allocator_type & alloc)noexcept;
		  vector_space(std::initializer_list<value_type> ilist,const allocator_type & alloc = allocator_type());
#endif
#if __cplusplus >= 201103L
		  template <typename InputIterator,typename = RequireAnInputIter<InputIterator>>
            //typename std::enable_if<std::is_convertible< typename std::iterator_traits<InputIterator>::iterator_category,std::input_iterator_tag  >::value >::type>
		   vector_space(InputIterator first, InputIterator last,vector_space<T,1,Add,Multiply,Allocator> && dimensions ,const allocator_type & alloc = allocator_type());
		  template <typename InputIterator,typename = RequireAnInputIter<InputIterator>>
            //typename std::enable_if<std::is_convertible< typename std::iterator_traits<InputIterator>::iterator_category,std::input_iterator_tag  >::value >::type>
		   vector_space(InputIterator first, InputIterator last,std::initializer_list<size_type> dimensions ,const allocator_type & alloc = allocator_type());
#else
		  template <InputIterator>
            vector_space(InputIterator first, InputIterator last,const allocator_type & alloc=allocator_type());
#endif
//----------Destructor
            ~vector_space()ZINHART_NOEXCEPT;
//----------Accessors
		    sub_space<T,N-1,Add,Multiply,Allocator> operator[](size_type index);
	  		const sub_space<T,N-1,Add,Multiply,Allocator> operator[](size_type index)const;
  			reference front()ZINHART_NOEXCEPT;
			reference back()ZINHART_NOEXCEPT;
			template <class ...Args>
			  reference at(Args&&...args)ZINHART_NOEXCEPT;
            template <class ...Args>
			  const_reference at(const Args&...args)const ZINHART_NOEXCEPT;
			pointer data()ZINHART_NOEXCEPT;
		    const_pointer data()const ZINHART_NOEXCEPT;
			template<class IntegralType>
			  vector_space<T,1,Add,Multiply> slice(IntegralType && start, IntegralType && size, IntegralType && step) ZINHART_NOEXCEPT;
			template<class IntegralType>
			  vector_space<T,1,Add,Multiply> gslice(const vector_space<IntegralType,1> & start, const vector_space<IntegralType,1> & sizes, const vector_space<IntegralType,1> & strides);
			template<class Type=lluint,class Alloc = std::allocator<Type>, typename = RequireUnsignedReal<void,Type,Type>>
			  vector_space<Type,1,Add,Multiply,Alloc> dimensions()ZINHART_NOEXCEPT;
            template<class Type=lluint,class Alloc = std::allocator<Type>, typename = RequireUnsignedReal<void,Type,Type>>
			  vector_space<Type,1,Add,Multiply,Alloc> dimensions()const ZINHART_NOEXCEPT;
			template<class Type=lluint,class Alloc = std::allocator<Type>, typename = RequireUnsignedReal<void,Type,Type>>
			  vector_space<Type,1,Add,Multiply,Alloc> request_basis()ZINHART_NOEXCEPT;
			template<class Type=lluint,class Alloc = std::allocator<Type>, typename = RequireUnsignedReal<void,Type,Type>>
			  const vector_space<Type,1,Add,Multiply,Alloc> request_basis() const ZINHART_NOEXCEPT;
            using Base::get_allocator;
            template<class...Args,typename = RequireUnsignedWhole<void,size_type,Args...>>
			  size_type map(Args...args);
//----------Iterators
            iterator begin()ZINHART_NOEXCEPT;
            iterator end()ZINHART_NOEXCEPT;
            const_iterator cbegin()const ZINHART_NOEXCEPT;
            const_iterator cend()const ZINHART_NOEXCEPT;
            reverse_iterator rbegin()ZINHART_NOEXCEPT;
            reverse_iterator rend()ZINHART_NOEXCEPT;
            const_reverse_iterator crbegin()const ZINHART_NOEXCEPT;
            const_reverse_iterator crend()const ZINHART_NOEXCEPT;
//----------Capacity
            bool empty()const ZINHART_NOEXCEPT;
            size_type size()const ZINHART_NOEXCEPT;
            size_type capacity()const ZINHART_NOEXCEPT;
			constexpr size_type order()const ZINHART_NOEXCEPT;
            size_type max_size()const ZINHART_NOEXCEPT;
            //increases capacity, throws length error if new_capacity > max_size()
            void reserve(size_type size);
            //requests container shrinks to size()
            void shrink_to_fit();
//----------Modifiers
            void assign(const size_type & size,const_reference value);
#if __cplusplus >= 201103L
            template <class InputIterator, typename = RequireAnInputIter<InputIterator>>
            void assign(InputIterator start,InputIterator finish);
            void assign(std::initializer_list<value_type>);
            void assign(size_type && size,const_reference value);
#else
            template <class InputIterator>
	          void assign(InputIterator start,InputIterator finish);
            void assign(const size_type size,const_reference value);
#endif
            void clear();
#if __cplusplus >= 201103L
            template <class...Args,typename = RequireUnsignedReal<void,size_type,Args...>>
	          iterator emplace(const_iterator pos,Args&&...args);
            template<class...Args>
		      reference emplace_back(Args&&...args);
#endif
            iterator insert(const_iterator pos,const_reference value);
            iterator insert(const iterator pos,size_type size,const_reference value);
#if __cplusplus >= 201103L
            template<class InputIterator,typename = RequireAnInputIter<InputIterator>>
		      iterator insert(const_iterator pos,InputIterator begin,InputIterator end);
            iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);
            iterator insert(const_iterator pos, value_type && value);
#else
            template<class InputIt>
            void insert(iterator pos,InputIt begin,InputIt end);
#endif
            iterator erase(const_iterator pos);
#if __cplusplus >= 201103L
            iterator erase(const_iterator first, const_iterator last);
#else
            iterator erase(iterator first,iterator last);
#endif
#if __cplusplus >= 201103L
            void resize(size_type && size);
            void resize(const size_type &size,const_reference value = T());
#else
            void resize(const size_type & size);
            void resize(const size_type &size,const_reference value = T());
#endif
            template<class Type = lluint,template<typename> class add,template<typename> class multiply,class Alloc,typename = RequireUnsignedWhole<void,Type,Type>>
              void reshape(const vector_space<Type,1,add,multiply,Alloc> & shape);
			template<class Type = lluint,template<typename> class add,template<typename> class multiply,class Alloc,typename = RequireUnsignedWhole<void,Type,Type>>
	          void reshape(vector_space<Type,1,add,multiply,Alloc> && shape);
            template <class ...Args,typename = RequireUnsignedWhole<void,size_type,Args...>>
	          void reshape(Args&&...args);    
            template <class ...Args,typename = RequireUnsignedWhole<void,size_type,Args...>>
	          void reshape(const Args&...args);
            void push_back(const_reference value);
#if __cplusplus >= 201103L
            void push_back(value_type && value);
#else
            void push_back(value_type value);
#endif
            void pop_front()ZINHART_NOEXCEPT;
            void pop_back()ZINHART_NOEXCEPT;
			template<template<typename> class F1, template<typename> class F2>
            void swap(vector_space<T,N, F1, F2> &another);
            vector_space<T,N,Add,Multiply,Allocator> & operator = (const vector_space & another);
			vector_space<T,N,Add,Multiply,Allocator>  operator +(const vector_space & another)const;
		  	vector_space<T,N,Add,Multiply,Allocator>  operator *(const vector_space & another)const;

			vector_space<T,N,Add,Multiply,Allocator> & operator += (const vector_space & vs);
			vector_space<T,N,Add,Multiply,Allocator> & operator *= (const vector_space & vs);
#if __cplusplus >= 201103L
            vector_space<T,N,Add,Multiply,Allocator> & operator = (vector_space && another) noexcept;
            vector_space<T,N,Add,Multiply,Allocator> & operator = (std::initializer_list<T> ilist);
			vector_space<T,N,Add,Multiply,Allocator>  operator +(vector_space && another);
		  	vector_space<T,N,Add,Multiply,Allocator>  operator *(vector_space && another);
			vector_space<T,N,Add,Multiply,Allocator> & operator += (vector_space && vs);
			vector_space<T,N,Add,Multiply,Allocator> & operator *= (vector_space && vs);
#endif
        private:
			std::shared_ptr<size_type[]> space;
			std::shared_ptr<size_type[]> basis;
			void allocate_and_initialize_private_members();
			template<class ForwardIterator>
  			  size_type hyper_volume(ForwardIterator begin,ForwardIterator end)const;
			template<class ForwardIterator>
  			  void dimensions_check(std::true_type,ForwardIterator start,ForwardIterator finish,size_type && capacity = size_type())const;
			template<class ForwardIterator>
  			  void dimensions_check(std::false_type,ForwardIterator start,ForwardIterator finish)const;
			void stack(const size_type & size);
			void unstack(const size_type & size);
			size_type stack_factor(const size_type & length)const;
			size_type unstack_factor(const size_type & length)const;
#if __cplusplus >= 201103L
            template<class Type=lluint,template<typename> class add,template<typename> class multiply,class Alloc = std::allocator<Type>, typename = RequireUnsignedWhole<void,Type,Type>>
              void generate_basis(const vector_space<Type,1,add,multiply,Alloc> &another);
            template<class Type=lluint,template<typename> class add,template<typename> class multiply,class Alloc = std::allocator<Type>, typename = RequireUnsignedWhole<void,Type,Type>>
	          void generate_basis(vector_space<Type,1,add,multiply,Alloc> &&another);
            template<class...Args,typename = RequireUnsignedWhole<void,size_type,Args...>>
	          void generate_basis(Args&&...);
            #endif
//----------Internal Helper Functions       
            template <class Integer>
		      void initialize_dispatcher(Integer size, const_reference value, std::true_type);
            template <class InputIterator>
		      void initialize_dispatcher(InputIterator first, InputIterator last , std::false_type);
            template <class ForwardIterator>
			  ForwardIterator initialize(ForwardIterator start,size_type size, const value_type & value);
            void default_initialize(const size_type & size);
            void fill_initialize(const size_type & size, const_reference value);
			void allocate_and_initialize(const size_type & requested_capacity, const value_type & value);
            template <class InputIterator,class ForwardIterator>
			  ForwardIterator initialize(InputIterator start,InputIterator end,ForwardIterator result);
            template <class InputIterator>
			  void range_initialize(InputIterator first,InputIterator last,std::input_iterator_tag);
            template <class ForwardIterator>
			  void range_initialize(ForwardIterator first,ForwardIterator last,std::forward_iterator_tag);
            template <class ForwardIterator>
			  void range_initialize(ForwardIterator first,ForwardIterator last);
            template <class Integer>
			  void assign_dispatcher(Integer size,const_reference values,std::true_type);
            template <class InputIterator>
			  void assign_dispatcher(InputIterator first, InputIterator last,std::false_type);
            void fill_assign(size_type & size, const_reference value);
            template <class InputIterator>
			  void assign_helper(InputIterator first, InputIterator last , std::input_iterator_tag);
            template <class ForwardIterator>
			  void assign_helper(ForwardIterator first, ForwardIterator last,std::forward_iterator_tag);
            template <class Integer>
			  void insert_dispatcher(iterator pos, const_reference value, Integer size,std::true_type);
            template <class InputIterator>
			  void insert_dispatcher(iterator pos,InputIterator first, InputIterator last , std::false_type);
            void fill_insert(iterator pos, const size_type & size,const_reference value);
            template <class InputIterator>
			  void range_insert(iterator pos, InputIterator first, InputIterator last,std::input_iterator_tag);
            template <class ForwardIterator>
			  void range_insert(iterator pos, ForwardIterator first, ForwardIterator last,std::forward_iterator_tag);
			void check_length(const size_type & size,std::true_type);
            size_type check_length(const size_type & size);
            void check_range(size_type size);
#if __cplusplus >= 201103L
            template<class Arg>
			  void insert_helper(iterator pos,Arg && args);
#else
            template<class T, class Allocator>
              void insert_helper(iterator pos,const_reference value);
#endif
#if __cplusplus >= 201103L
			template <class ...Args>
  			  void realloc_insert(iterator pos,Args&&...args);
#else
			void realloc_insert(iterator pos, const value_type & val);
#endif
#if __cplusplus >= 201103L
            template<class...Args>
              void emplace_back_helper(Args&&...args);
#endif
            iterator erase_helper(iterator pos);
            iterator erase_helper(iterator first,iterator last);
            void erase_at_end(pointer pos) ZINHART_NOEXCEPT;
            void default_append(size_type  size);
            template <class ForwardIterator>
              pointer allocate_and_copy(size_type size, ForwardIterator first, ForwardIterator last);
			template<class ForwardIterator>
		     pointer allocate_and_move( size_type size, ForwardIterator first, ForwardIterator last);
            bool shrink_to_fit_helper();
#if __cplusplus >= 201103L
            template<class UPtr>
  			  UPtr * data_ptr(UPtr * ptr) const;
            template<class Ptr>
			  typename std::pointer_traits<Ptr>::element_type* data_ptr(Ptr  ptr) const;
#else
            template<class Ptr>
  			  Ptr data_ptr (Ptr ptr) const;
#endif
            void move_assign(vector_space<T,1,Add,Multiply,Allocator> && vs,std::true_type)noexcept;
            void move_assign(vector_space<T,1,Add,Multiply,Allocator> && vs,std::false_type);
    };
    template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  inline bool operator ==(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs);
    template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  inline bool operator !=(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs);
    template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  inline bool operator <(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs);
    template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  inline bool operator <=(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs);
    template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  inline bool operator >(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs);
    template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  inline bool operator >=(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs);
    template <class T,lluint N,template<typename> class Add,template<typename> class Multiply,class Allocator>
	  inline void swap(const vector_space<T,N,Add,Multiply,Allocator> &lhs, const vector_space<T,N,Add,Multiply,Allocator> &rhs);
    template <class T,lluint N,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  class sub_space
	  {
        public:
		  friend class vector_space<T,N+1,Add,Multiply,Allocator>;
          friend class sub_space<T,N+1,Add,Multiply,Allocator>;
          typedef typename vector_space<T,N+1,Add,Multiply,Allocator>::size_type size_type;
		  typedef typename vector_space<T,N+1,Add,Multiply,Allocator>::pointer pointer;
          sub_space<T,N-1,Add,Multiply,Allocator> operator [](size_type i);
          const sub_space<T,N-1,Add,Multiply,Allocator> operator [](size_type i)const;
		  static constexpr bool indexCheck(const size_type & i, const size_type & j);
        private:
		  sub_space(pointer data, const size_type * dimensions,const size_type * basis);
		  pointer const _data;
		  const size_type * const space;
		  const size_type * const basis;
  	  };
    template <class T,template<typename> class Add,template <typename> class Multiply,class Allocator>
	  class sub_space<T,1,Add,Multiply,Allocator>
	  {
        public:
		  friend class vector_space<T,2,Add,Multiply,Allocator>;
          friend class sub_space<T,2,Add,Multiply,Allocator>;
          typedef typename vector_space<T,2,Add,Multiply,Allocator>::size_type size_type;
		  typedef typename vector_space<T,2,Add,Multiply,Allocator>::pointer pointer;
		  typedef typename vector_space<T,2,Add,Multiply,Allocator>::reference reference;
		  reference operator [](size_type i);
		  const reference operator [](size_type i)const;
		private:
		  static constexpr bool indexCheck(const size_type &i,const size_type &j);
		  sub_space<T,1,Add,Multiply,Allocator>(pointer data, const size_type * dimensions,const size_type * basis);
		  pointer const _data;
		  const size_type * const space;
		  const size_type * const basis;
  	  };
}//End Tool
#endif /*VECTOR_SPACE_H*/

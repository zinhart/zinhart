#ifndef TESTALLOCATOR_H
#define TESTALLOCATOR_H
//using namespace std;
//#include <cstdlib>
#include <memory>
template<typename _Tp>
class TestAllocator
{
public:
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef _Tp*       pointer;
    typedef const _Tp* const_pointer;
    typedef _Tp&       reference;
    typedef const _Tp& const_reference;
    typedef _Tp        value_type;
    template<typename _Tp1>
    struct rebind{ typedef TestAllocator<_Tp1> other; };
    typedef std::true_type propagate_on_container_move_assignment;
    TestAllocator()   { }
    TestAllocator(const TestAllocator&)   { }
    template<typename _Tp1>
      TestAllocator(const TestAllocator<_Tp1>&)   { }
	TestAllocator(TestAllocator &&){}

    ~TestAllocator()   { }

    pointer
    address(reference __x) const
    { return std::__addressof(__x); }

    const_pointer
    address(const_reference __x) const
    { return std::addressof(__x); }

    // NB: __n is permitted to be 0.  The C++ standard says nothing
    // about what the return value is when __n == 0.
    pointer allocate(size_type __n, const void* = 0)
    {
        if (__n > this->max_size())
        std::__throw_bad_alloc();
        return static_cast<_Tp*>(::operator new(__n * sizeof(_Tp)));
    }
void deallocate(pointer __p, size_type){ ::operator delete(__p); }
size_type max_size() const { return std::size_t(-1) / sizeof(_Tp); }
template<typename _Up,typename... _Args>
void construct(_Up* __p ,_Args&&... __args) { ::new((void *)__p) _Up(std::forward<_Args>(__args)...); }
template<typename _Up>
void destroy(_Up* __p) { __p->~_Up(); }
};
template<typename _Tp>
inline bool operator==(const TestAllocator<_Tp>&, const TestAllocator<_Tp>&)
{ return true; }
template<typename _Tp>
inline bool operator!=(const TestAllocator<_Tp>&, const TestAllocator<_Tp>&){ return false; }
#endif // TESTALLOCATOR_H

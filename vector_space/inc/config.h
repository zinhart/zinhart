#ifndef CONFIG_H
#define CONFIG_H
#include <stdlib.h>
namespace zinhart
{
    #if __cplusplus
    #define DEBUG 0
    #define COMPILE_TIME_SAFETY 0
    #define VERBOSE 1
    //DEFINES A COMPILE TIME REQUIREMENT FOR C++ 03
    template<bool>
    struct _requires;
    template<>
    struct _requires<true>{};
    #define JOIN(lhs, rhs)    JOIN_1(lhs, rhs)
    #define JOIN_1(lhs, rhs)  JOIN_2(lhs, rhs)
    #define JOIN_2(lhs, rhs)  lhs##rhs
    #define REQUIRES(expression, message)\
    struct JOIN(__static_assertion_at_line_, __LINE__)\
    {\
        _requires<static_cast<bool>((expression))> JOIN(JOIN(JOIN(STATIC_ASSERTION_FAILED_AT_LINE_, __LINE__), _), message);\
    };\
        typedef _requires<sizeof(JOIN(__static_assertion_at_line_, __LINE__))> JOIN(__static_assertion_test_at_line_, __LINE__)
    //simple test
    const bool t=true;
    constexpr int arry[3]={0,1,2};
    //REQUIRES(!t,requirement_test_successfull);
    //REQUIRES((arry[0]==arry[1]),requirement_test_successfull);
    #endif

	//Macros
    #if !__cpp_exceptions
    #define ZINHART_TRY if (true)
    #define ZINHART_CATCH(X) if (false)
    #define ZINHART_THROW_EXCEPTION_AGAIN
    #else
    #define ZINHART_TRY try
    #define ZINHART_CATCH(X) catch(X)
    #define ZINHART_THROW_EXCEPTION_AGAIN throw
    #endif
    #define MSG(msgid) (msgid)

    #ifndef ZINHART_NOEXCEPT
    #if __cplusplus >= 201103L
    #define ZINHART_CONSTEXPR constexpr
    #define ZINHART_MOVE(val) std::move(val)
    #define ZINHART_MOVE3(begin,end,des) std::move(begin,end,des)
    #define ZINHART_MAKE_MOVE_ITERATOR(Iter) std::make_move_iterator(Iter)
    #define ZINHART_MOVE_BACKWARD3(begin,end,des) std::move_backward(begin,end,des)
    #define ZINHART_NOEXCEPT noexcept
    #else
    #define ZINHART_CONSTEXPR const
    #define ZINHART_MOVE(val) (val)
    #define ZINHART_MOVE3(begin,end,des) std::copy(begin,end,des)
    #define ZINHART_MOVE3_ITERATOR(Iter) Iter
    #define ZINHART_MOVE_BACKWARD3(begin,end,des) std::copy_backward(begin,end,des)
    #define ZINHART_NOEXCEPT throw()
    #endif
    #endif


}
#endif




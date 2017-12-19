#include <gtest/gtest.h>
#include "../inc/vector_space_test.h"
#include "../inc/testallocator.h"
#include <vector>
#include <random>
/*
 * There are 10 constructors to be tested in a R^1 space assuming __cplusplus > 201103l (listed in same order as in header file)
 * vector_space<lluint,1> s1_default;
 * vector_space<lluint,1> s1_alt_alloc;
 * vector_space<lluint,1> s1n_rval;
 * vector_space<lluint,1> s1n_lval;
 * vector_space<lluint,1> s1_cpy_ctor;
 * vector_space<lluint,1> s1_mv_ctor;
 * vector_space<lluint,1> s1_cpy_alt_alloc;
 * vector_space<lluint,1> s1_mv_alt_alloc;
 * vector_space<lluint,1> s1_ilist_ctor;
 * vector_space<lluint,1> s1_rng_ctor;
 * */

/*
 * Plan of attack to avoid redundancy in tests
 * 1.Validate first 4 Ctors and ilist ctor b/c they don't involve another space object or a range
 * 2.Validate those member functions that modify the container state,with member functions that return information on containers state, using static member s1 in fixture
 * 3.Validate rest of ctors
 * I chose this order because it's not realistic to code every permutation of function calls if I were to do that total number of ctors ^ total number of member functions 
 * */
/*
 * constructor doesn't really make sense, is it neccesary?
 * Alt Alloc Ctor Test
 * size(),capacity() ret 0
 * empty() ret 1
 * data()==nullptr
 **/
TEST(s1_alt_alloc,test_s1_alt_alloc)
{
  TestAllocator<lluint> a;
  vector_space<lluint,1,std::plus,std::multiplies,TestAllocator<lluint>> s1_alt_alloc(a);
  ASSERT_EQ(s1_alt_alloc.size(),0);
  ASSERT_EQ(s1_alt_alloc.capacity(),0);
  ASSERT_EQ(s1_alt_alloc.empty(),1);
  ASSERT_EQ(s1_alt_alloc.data(),nullptr);
  ASSERT_EQ(std::addressof(*s1_alt_alloc.begin()),std::addressof(*s1_alt_alloc.end() ) );
  ASSERT_EQ(std::addressof(*s1_alt_alloc.cbegin()),std::addressof(*s1_alt_alloc.cend() ) );
}
/*
 * Initialize N Vals Rval Ctor Test
 * size(),capacity() ret val != 0
 * empty() ret 0
 * data()!=nullptr
 * also check constant amortized growth i.e. space is doubling in size
 **/
TEST(s1n_rval,test_s1n_rval)
{
  unsigned size = 9;
  vector_space<lluint,1> s1n_rval(size);
  ASSERT_NE(s1n_rval.size(),0);
  ASSERT_NE(s1n_rval.capacity(),0);
  ASSERT_EQ(s1n_rval.empty(),0);
  ASSERT_NE(s1n_rval.data(),nullptr);
  ASSERT_PRED_FORMAT1(assert_const_amortized_growth<1>,s1n_rval);
  ASSERT_NE(std::addressof( *s1n_rval.begin()),std::addressof( *s1n_rval.end()) );
  ASSERT_NE(std::addressof( *s1n_rval.cbegin()),std::addressof( *s1n_rval.cend()) );
}

/*
 * Initialize N Vals :val Ctor Test
 * size(),capacity() ret val != 0
 * empty() ret 0
 * data()!=nullptr
 * also check constant amortized growth i.e. space is doubling in size
 **/
TEST(s1n_rval,test_s1n_lval)
{
  vector_space<lluint,1> s1n_lval(7);
  ASSERT_NE(s1n_lval.size(),0);
  ASSERT_NE(s1n_lval.capacity(),0);
  ASSERT_EQ(s1n_lval.empty(),0);
  ASSERT_NE(s1n_lval.data(),nullptr);
  //ASSERT_PRED1(constant_amortized_growth_pred<lluint,1>,s1n_lval);
  ASSERT_NE(std::addressof( *s1n_lval.begin()),std::addressof( *s1n_lval.end()) );
  ASSERT_NE(std::addressof( *s1n_lval.cbegin()),std::addressof( *s1n_lval.cend()) );
}

/*
 * Initialize N Vals :val Ctor Test
 * size(),capacity() ret val != 0
 * empty() ret 0
 * data()!=nullptr
 * also check constant amortized growth i.e. capacity doubles when size > capacity
 **/
TEST(s1_ilist_ctor,test_s1_ilist_ctor)
{
  vector_space<lluint,1> s1n_lval({1,2,3,4,5});
  ASSERT_NE(s1n_lval.size(),0);
  ASSERT_NE(s1n_lval.capacity(),0);
  ASSERT_EQ(s1n_lval.empty(),0);
  ASSERT_NE(s1n_lval.data(),nullptr);
  std::pair<lluint,lluint> params(0,1);
  //ASSERT_PRED1(constant_amortized_growth_pred<lluint,1>,s1n_lval);
  ASSERT_NE(std::addressof( *s1n_lval.begin()),std::addressof( *s1n_lval.end()) );
  ASSERT_NE(std::addressof( *s1n_lval.cbegin()),std::addressof( *s1n_lval.cend()) );
}
/*
 * Testing copy ctor
 * */
TEST_F(vector_space_test,test_copy_ctor)
{
  vector_space<lluint,1> s1_copy_ctor(s1);
  ASSERT_EQ(s1.size(),s1_copy_ctor.size());
  ASSERT_EQ(s1.empty(),s1_copy_ctor.empty());
  for(unsigned i =0; i<s1.size();++i)
  {
	ASSERT_EQ(s1.data()[i],s1_copy_ctor.data()[i]);
  }
}

/*
 * Testing move ctor
 * */
TEST_F(vector_space_test,test_move_ctor)
{
  auto s1_size = s1.size();
  auto s1_capacity = s1.capacity();
  auto s1_data = s1.data();
  vector_space<lluint,1> s1_move_ctor(std::move(s1));
  ASSERT_EQ(s1_size,s1_move_ctor.size());
  ASSERT_EQ(s1_capacity,s1_move_ctor.capacity());
  for(unsigned i = 0; i<s1_move_ctor.size();++i)
  {
	ASSERT_EQ(s1_data[i],s1_move_ctor.data()[i] );
  }
}

/*
 * Testing range ctor
 * */
TEST_F(vector_space_test,test_rng_ctor)
{

  s1.assign(67,69);
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,s1.size());
  vector_space<lluint,1> s1_rng_ctor(s1.begin(),s1.begin()+dist(mt));
  for(unsigned i=0; i<s1_rng_ctor.size();++i)
  {
	ASSERT_EQ(s1_rng_ctor[i],s1[i]);
  }
}

/*
 * Test member clear
 * capacity() ret val should be unmodified
 * size() should be 0 
 * */
TEST(member_clear,test_member_clear)
{
  vector_space<lluint,1> s1n(9);
  auto old_capacity = s1n.capacity();
  s1n.clear();
  ASSERT_EQ(s1n.size(),0);
  ASSERT_EQ(s1n.capacity(),old_capacity);
}
/*
 * Testing method push_back
 * size() before call != size() after call
 * check amortized growth
 * after call size != zero
 * */
TEST(member_push_back,test_member_push_back)
{
  vector_space<lluint,1> s1(9);
  //increase current size to trigger a resize
  auto old_capacity = s1.capacity();
  for(unsigned i = s1.size(); i <= old_capacity;++i)
	s1.push_back(lluint());
  ASSERT_TRUE( (s1.size() -1) == s1.capacity()/2) ; 
  ASSERT_TRUE(s1.size() != 0);
}

/*
 * Testing operator [] both reference and const_reference
 * size(),capacity(),empty(),data(),front(),back() unmodified after function call
 * s1[i] == *(s1.begin() + i) 
 * s1[i] == *(s1.cbegin() + i) 
 * */
TEST(method_subscript_operator,test_method_subscript_operator)
{
  vector_space<lluint,1> s1(9);
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
  auto front = s1.front();
  auto back = s1.back();
  for(unsigned i = 0; i < s1.size();++i )
  {
	ASSERT_EQ(s1[i], *(s1.begin()+i));
	ASSERT_EQ(s1[i], *(s1.cbegin()+i));
	ASSERT_EQ(data[i], s1.data()[i]);
  }
  ASSERT_EQ(front,s1.front());
  ASSERT_EQ(back,s1.back());
  ASSERT_EQ(size,s1.size());
  ASSERT_EQ(capacity,s1.capacity());
  ASSERT_EQ(empty,s1.empty());
}
/*
 * Test member front()
 * s1[0]==s1.front();
 * size(),capacity(),empty(),data(),back() unmodified after function call
 * the data begin() and end() point to should not be modified 
 * */
TEST(member_front,test_member_front)
{
  vector_space<lluint,1> s1(9);
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
  auto back = s1.back();
  auto front = s1.front();
  
  ASSERT_EQ(front,s1[0]);
  
  ASSERT_EQ(back,s1.back());
  ASSERT_EQ(size,s1.size());
  ASSERT_EQ(capacity,s1.capacity());
  ASSERT_EQ(empty,s1.empty());
  for(unsigned i = 0; i<s1.size();++i)
  {
	ASSERT_EQ(s1[i], *(s1.begin()+i));
	ASSERT_EQ(s1[i], *(s1.cbegin()+i));
	ASSERT_EQ(data[i],s1.data()[i]);
  }
}
/*
 * Test member back()
 * s1[s1.size()-1] == s1.back();
 * size(),capacity(),empty(),data() unmodified
 * the data begin() and end() point to should not be modified 
 * */
TEST(member_back,test_member_back)
{
  vector_space<lluint,1> s1(9);
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
  auto back = s1.back();
  auto front = s1.front();

  ASSERT_EQ(back,s1[size-1]);
 
  ASSERT_EQ(front,s1.front());
  ASSERT_EQ(size,s1.size());
  ASSERT_EQ(capacity,s1.capacity());
  ASSERT_EQ(empty,s1.empty());
  for(unsigned i = 0; i<s1.size();++i)
  {
    ASSERT_EQ(s1[i], *(s1.begin()+i));
	ASSERT_EQ(s1[i], *(s1.cbegin()+i));
	ASSERT_EQ(data[i], s1.data()[i]);
  }
}
/*
 * Testing member at
 * size(),capacity(),empty(),data(),front(),back() unmodified
 * s1.at(i) == *(s1.begin() + i)
 * s1.at(i) == *(s1.cbegin() + i)
 * s1[i] == *(s1.begin() + i)
 * s1[i] == *(s1.cbegin() + i)
 * */
TEST(member_at,test_member_at)
{
  vector_space<lluint,1> s1(9);
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
  auto back = s1.back();
  auto front = s1.front();

  for(unsigned i = 0; i<s1.size();++i)
  {
	ASSERT_EQ(s1.at(i),*(s1.begin()+i));
	ASSERT_EQ(s1.at(i),*(s1.cbegin()+i));
	ASSERT_EQ(s1[i],*(s1.begin()+i));
	ASSERT_EQ(s1[i],*(s1.cbegin()+i));
	ASSERT_EQ(data[i],s1.data()[i]);
  }

  ASSERT_EQ(size,s1.size());
  ASSERT_EQ(capacity,s1.capacity());
  ASSERT_EQ(empty,s1.empty());
  ASSERT_EQ(front,s1.front());
  ASSERT_EQ(back,s1.back());

}

/*
 * Testing method both begin() and cbegin()
 * size(),capacity(),empty(),data(),front(),back() unmodified
 * s1[i] == *(begin() + i);
 * s1[i] == *(cbegin() + i);
 * s1.at(i) == *(begin() + i);
 * s1.at(i) == *(cbegin() + i);
 * std::addressof(*s1.begin()) != std::addressof(*s1.end()) 
 * std::addressof(s1.cbegin()) != std::addressof(*s1.cend())
 * */
TEST(method_begin,test_method_begin)
{
  vector_space<lluint,1> s1(9);
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
  auto back = s1.back();
  auto front = s1.front();
   
  for(unsigned i = 0; i<s1.size();++i)
  {
    ASSERT_EQ(s1[i], *(s1.begin() + i));
    ASSERT_EQ(s1[i],  *(s1.cbegin() + i));
	ASSERT_EQ(s1.at(i),*(s1.begin()+i));
	ASSERT_EQ(s1.at(i),*(s1.cbegin()+i));
	ASSERT_EQ(data[i],s1.data()[i]);
  }

  ASSERT_EQ(front,s1.front());
  ASSERT_EQ(back,s1.back());
  ASSERT_EQ(size,s1.size());
  ASSERT_EQ(capacity,s1.capacity());
  ASSERT_EQ(empty,s1.empty());

  ASSERT_NE(std::addressof( *s1.begin()),std::addressof( *s1.end()) );
  ASSERT_NE(std::addressof( *s1.cbegin()),std::addressof( *s1.cend()) );
  /*If the pointers were not invalidated during an increase or decrease*/
  vector_space<lluint,1> test;
  ASSERT_EQ(std::addressof( *test.begin()),std::addressof( *test.end()) );
  ASSERT_EQ(std::addressof( *test.cbegin()),std::addressof( *test.cend()) );
}

/*
 * Testing method both end() and cend()
 * size(),capacity(),empty(),data(),front(),back() unmodified
 * s1[size - i -1] == *(s1.end() - i -1))
 * s1[size - i -1] == *(s1.cend() - i -1)
 * s1.at(size - i -1) == *(s1.end() - i -1)
 * s1.at(size - i -1) ==  *(s1.end() - i -1)
 * std::addressof(*s1.end()) != std::addressof(*s1.begin()) 
 * std::addressof(*s1.cend()) != std::addressof(*s1.cbegin())
 * */
TEST(member_end,test_member_end)
{
  vector_space<lluint,1> s1(9);
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
  auto back = s1.back();
  auto front = s1.front();
  
  for(unsigned i = 0; i<s1.size();++i)
  {
	ASSERT_EQ(s1.at(size - i - 1),*(s1.end() - i -1));
	ASSERT_EQ(s1.at(size - i - 1),*(s1.cend() - i -1));
	ASSERT_EQ(s1[size - i - 1],*(s1.end() - i -1));
	ASSERT_EQ(s1[size - i - 1],*(s1.cend() - i -1));
	ASSERT_EQ(data[i],s1.data()[i]);
  }

  ASSERT_EQ(front,s1.front());
  ASSERT_EQ(back,s1.back());
  ASSERT_EQ(size,s1.size());
  ASSERT_EQ(capacity,s1.capacity());
  ASSERT_EQ(empty,s1.empty());
  ASSERT_NE( std::addressof(*s1.end()) ,std::addressof(*s1.begin()) );
  ASSERT_NE( std::addressof(*s1.cend()) ,std::addressof(*s1.cbegin()) );
}
/*
 * Testing method both rbegin() and crbegin()
 * size(),capacity(),empty(),data(),front(),back() unmodified
 * s1[s1.size() - i -1] == *(s1.rbegin() + i)
 * s1[s1.size() -i -1] == *(s1.crbegin()+ i)
 * s1.at(size - i -1) == *(s1.rbegin() + i)
 * s1.at(size - i -1) ==  *(s1.crend() + i)
 * std::addressof(*s1.rbegin()) != std::addressof(*s1.begin()
 * std::addressof(*s1.crbegin()) != std::addressof(*s1.cbegin())
 * */
TEST(method_rbegin,test_method_rbegin)
{
  vector_space<lluint,1> s1(9);
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
  auto back = s1.back();
  auto front = s1.front();
 
  for(unsigned i = 0; i<s1.size();++i)
  {

    ASSERT_EQ(s1.at(size - i - 1),*(s1.rbegin() +  i));
	ASSERT_EQ(s1.at(size - i - 1),*(s1.cend() - i -1));
	ASSERT_EQ(s1[size - i - 1],*(s1.end() - i -1));
	ASSERT_EQ(s1[size - i - 1],*(s1.cend() - i -1));
	ASSERT_EQ(data[i],s1.data()[i]);

  }
  ASSERT_EQ(front,s1.front());
  ASSERT_EQ(back,s1.back());
  ASSERT_EQ(size,s1.size());
  ASSERT_EQ(capacity,s1.capacity());
  ASSERT_EQ(empty,s1.empty());
  ASSERT_NE(std::addressof( *s1.begin()),std::addressof( *s1.end()) );
  ASSERT_NE(std::addressof( *s1.cbegin()),std::addressof( *s1.cend()) );
}

/*
 * Testing method both rend() and crend()
 * size(),capacity(),empty(),data(),front(),back() unmodified
 * s1[i] == *(s1.rend() + i)
 * s1[i] == *(s1.crend()+ i)
 * s1.at(i) == *(s1.rend() + i)
 * s1.at(i) ==  *(s1.crend() + i)
 * std::addressof(*s1.rend()) != std::addressof(*s1.end()) 
 * std::addressof(*s1.crend()) != std::addressof(*s1.cbegin())
 * */
TEST(method_rend,test_method_rend)
{
  vector_space<lluint,1> s1(9);
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
  auto back = s1.back();
  auto front = s1.front();
 
  for(unsigned i = 0; i<s1.size();++i)
  {

    ASSERT_EQ(s1.at(i),*(s1.rend() - i - 1));
	ASSERT_EQ(s1.at(i),*(s1.crend()  - i - 1));
	ASSERT_EQ(s1[i],*(s1.end() - i - 1));
	ASSERT_EQ(s1[i],*(s1.cend() - i - 1));
	ASSERT_EQ(data[i],s1.data()[i]);

  }
  ASSERT_EQ(front,s1.front());
  ASSERT_EQ(back,s1.back());
  ASSERT_EQ(size,s1.size());
  ASSERT_EQ(capacity,s1.capacity());
  ASSERT_EQ(empty,s1.empty());
  ASSERT_NE( std::addressof(*s1.rend()) ,std::addressof(*s1.begin()) );
  ASSERT_NE( std::addressof(*s1.crend()) ,std::addressof(*s1.cbegin()) );
}
/*
 * SPACE ORDER >= 2
 */





int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc,argv);
  return RUN_ALL_TESTS();
}

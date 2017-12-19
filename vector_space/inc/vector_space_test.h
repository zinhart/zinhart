#ifndef SPACE_TEST_H
#define SPACE_TEST_H
#include <gtest/gtest.h>
#include <random>
#include "../vector_space"
#include "../inc/testallocator.h"
using namespace zinhart;
class vector_space_test : public ::testing::Test
{
  protected:
	static vector_space<lluint,1> s1;
	static vector_space<lluint,3,scalar_addition,scalar_multiplication> sn;
	static void SetUpTestCase(){ }
	static void TearDownTestCase(){}
	virtual void SetUp(){}
	//virtual void TearDown(){} //for completeness sake
};
//unfortunately on line 91 in gtest_pred_imple const is used so can't pass by reference, could use pointer but don't feel like typing
vector_space<lluint,1> vector_space_test::s1;
vector_space<lluint,3,scalar_addition,scalar_multiplication> vector_space_test::sn;
template<typename T,lluint N,typename  = RequireUnsignedWhole<void,T,T>>
bool constant_amortized_growth_pred(vector_space<T,N> &s)
{
  if(s.size() == s.capacity())
	s.push_back(T());
  else 
  {
	//increase current size to trigger a resize
	auto old_capacity = s.capacity();
	for(unsigned i = s.size(); i <= old_capacity;++i)
	  s.push_back(T());
  }
  return (s.size() -1) == s.capacity()/2; 
}
template<typename T,lluint N,typename  = RequireUnsignedWhole<void,T,T>>
bool constant_amortized_growth_pred(vector_space<T,N,scalar_addition,scalar_multiplication> &s)
{
  if(s.size() == s.capacity())
	s.push_back(T());
  else 
  {
	//increase current size to trigger a resize
	auto old_capacity = s.capacity();
	for(unsigned i = s.size(); i <= old_capacity;++i)
	  s.push_back(T());
  }
  return (s.size() -1) == s.capacity()/2; 
}
template<lluint N>
::testing::AssertionResult assert_const_amortized_growth(const char*msg1,vector_space<lluint,N> & s)
{
  if(constant_amortized_growth_pred(s))
	return ::testing::AssertionSuccess();
  return ::testing::AssertionFailure()
	<<msg1<<" does not have constant amortized growth";
}

template<lluint N>
::testing::AssertionResult assert_const_amortized_growth(const char*msg1,vector_space<lluint,N,scalar_addition,scalar_multiplication> & s)
{
  if(constant_amortized_growth_pred(s))
	return ::testing::AssertionSuccess();
  return ::testing::AssertionFailure()
	<<msg1<<" does not have constant amortized growth";
}

//####################################################
// TEST SPACE ORDER  1  							 #
//####################################################
/*
 * Default Ctor Test
 * size(),capacity() ret 0
 * empty() ret 1
 * data()==nullptr
 **/
TEST_F(vector_space_test,default_ctor)
{
  ASSERT_EQ(vector_space_test::s1.size(),0);
  ASSERT_EQ(vector_space_test::s1.capacity(),0);
  ASSERT_EQ(vector_space_test::s1.empty(),1);
  ASSERT_EQ(vector_space_test::s1.data(),nullptr);
  ASSERT_EQ(std::addressof( *s1.begin()),std::addressof( *s1.end()) );
  ASSERT_EQ(std::addressof( *s1.cbegin()),std::addressof( *s1.cend()) );
}


/*
 * Testing operator [] both reference and const_reference
 * size(),capacity(),empty(),data() unmodified after function call
 * s1[i] == *(s1.begin() + i) 
 * s1[i] == *(s1.cbegin() + i) 
 * */
TEST_F(vector_space_test,test_method_subscript_operator)
{

  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
  for(unsigned i = 0; i < s1.size();++i )
  {
	ASSERT_EQ(s1[i], *(s1.begin()+i));
	ASSERT_EQ(s1[i], *(s1.cbegin()+i));
	ASSERT_EQ(data[i], s1.data()[i]);
  }
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
TEST_F(vector_space_test,test_member_front)
{
  
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
  if(!s1.empty())
  {
  	auto back = s1.back();
	auto front = s1.front();
	ASSERT_EQ(front,s1[0]);
    ASSERT_EQ(back,s1.back());
  }
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
TEST_F(vector_space_test,test_member_back)
{
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
  if(!s1.empty())
  {	
	auto back = s1.back();
	auto front = s1.front();
	ASSERT_EQ(back,s1[size-1]);
	ASSERT_EQ(front,s1.front());
  }
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
 * size(),capacity(),empty(),data() unmodified
 * s1.at(i) == *(s1.begin() + i)
 * s1.at(i) == *(s1.cbegin() + i)
 * s1[i] == *(s1.begin() + i)
 * s1[i] == *(s1.cbegin() + i)
 * */
TEST_F(vector_space_test,test_member_at)
{
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();

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
}

/*
 * Testing method both begin() and cbegin()
 * size(),capacity(),empty(),data(), unmodified
 * s1[i] == *(begin() + i);
 * s1[i] == *(cbegin() + i);
 * s1.at(i) == *(begin() + i);
 * s1.at(i) == *(cbegin() + i);
 * std::addressof(*s1.begin()) != std::addressof(*s1.end()) 
 * std::addressof(s1.cbegin()) != std::addressof(*s1.cend())
 * */
TEST_F(vector_space_test,test_method_begin)
{
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
   
  for(unsigned i = 0; i<s1.size();++i)
  {
    ASSERT_EQ(s1[i], *(s1.begin() + i));
    ASSERT_EQ(s1[i],  *(s1.cbegin() + i));
	ASSERT_EQ(s1.at(i),*(s1.begin()+i));
	ASSERT_EQ(s1.at(i),*(s1.cbegin()+i));
	ASSERT_EQ(data[i],s1.data()[i]);
  }
  ASSERT_EQ(size,s1.size());
  ASSERT_EQ(capacity,s1.capacity());
  ASSERT_EQ(empty,s1.empty());

  ASSERT_EQ(std::addressof( *s1.begin()),std::addressof( *s1.end()) );
  ASSERT_EQ(std::addressof( *s1.cbegin()),std::addressof( *s1.cend()) );
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
TEST_F(vector_space_test,test_method_end)
{
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
  
  for(unsigned i = 0; i<s1.size();++i)
  {
	ASSERT_EQ(s1.at(size - i - 1),*(s1.end() - i -1));
	ASSERT_EQ(s1.at(size - i - 1),*(s1.cend() - i -1));
	ASSERT_EQ(s1[size - i - 1],*(s1.end() - i -1));
	ASSERT_EQ(s1[size - i - 1],*(s1.cend() - i -1));
	ASSERT_EQ(data[i],s1.data()[i]);
  }

  ASSERT_EQ(size,s1.size());
  ASSERT_EQ(capacity,s1.capacity());
  ASSERT_EQ(empty,s1.empty());
  ASSERT_EQ( std::addressof(*s1.end()) ,std::addressof(*s1.begin()) );
  ASSERT_EQ( std::addressof(*s1.cend()) ,std::addressof(*s1.cbegin()) );
}
/*
 * Testing method both rbegin() and crbegin()
 * size(),capacity(),empty(),data() unmodified
 * s1[s1.size() - i -1] == *(s1.rbegin() + i)
 * s1[s1.size() -i -1] == *(s1.crbegin()+ i)
 * s1.at(size - i -1) == *(s1.rbegin() + i)
 * s1.at(size - i -1) ==  *(s1.crend() + i)
 * std::addressof(*s1.rbegin()) != std::addressof(*s1.begin()
 * std::addressof(*s1.crbegin()) != std::addressof(*s1.cbegin())
 * */
TEST_F(vector_space_test,test_method_rbegin)
{
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
 
  for(unsigned i = 0; i<s1.size();++i)
  {

    ASSERT_EQ(s1.at(size - i - 1),*(s1.rbegin() +  i));
	ASSERT_EQ(s1.at(size - i - 1),*(s1.cend() - i -1));
	ASSERT_EQ(s1[size - i - 1],*(s1.end() - i -1));
	ASSERT_EQ(s1[size - i - 1],*(s1.cend() - i -1));
	ASSERT_EQ(data[i],s1.data()[i]);

  }
  ASSERT_EQ(size,s1.size());
  ASSERT_EQ(capacity,s1.capacity());
  ASSERT_EQ(empty,s1.empty());
  ASSERT_NE( std::addressof(*s1.rbegin()) ,std::addressof(*s1.begin()) );
  ASSERT_NE( std::addressof(*s1.crbegin()) ,std::addressof(*s1.cbegin()) );
}

/*
 * Testing method both rend() and crend()
 * size(),capacity(),empty(),data() unmodified
 * s1[i] == *(s1.rend() + i)
 * s1[i] == *(s1.crend()+ i)
 * s1.at(i) == *(s1.rend() + i)
 * s1.at(i) ==  *(s1.crend() + i)
 * std::addressof(*s1.rend()) != std::addressof(*s1.end()) 
 * std::addressof(*s1.crend()) != std::addressof(*s1.cbegin())
 * */
TEST_F(vector_space_test,test_method_rend)
{
  auto size = s1.size();
  auto capacity = s1.capacity();
  auto empty = s1.empty();
  auto data = s1.data();
 
  for(unsigned i = 0; i<s1.size();++i)
  {

    ASSERT_EQ(s1.at(i),*(s1.rend() - i - 1));
	ASSERT_EQ(s1.at(i),*(s1.crend()  - i - 1));
	ASSERT_EQ(s1[i],*(s1.end() - i - 1));
	ASSERT_EQ(s1[i],*(s1.cend() - i - 1));
	ASSERT_EQ(data[i],s1.data()[i]);

  }
  ASSERT_EQ(size,s1.size());
  ASSERT_EQ(capacity,s1.capacity());
  ASSERT_EQ(empty,s1.empty());
  ASSERT_NE( std::addressof(*s1.rend()) ,std::addressof(*s1.begin()) );
  ASSERT_NE( std::addressof(*s1.crend()) ,std::addressof(*s1.cbegin()) );
}
/*
 * Testing member reserve
 * old_capacity != capacity()
 * size() should be unmodified
 * */
TEST_F(vector_space_test,test_method_reserve)
{
  auto old_size = s1.size();
  auto old_capacity = s1.capacity();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned int> dist(10,20);
  s1.reserve(old_capacity+dist(mt));
  ASSERT_EQ(old_size,s1.size());
  ASSERT_NE(old_capacity,s1.capacity());
}
/*
 * Testing member shrink_to_fit
 * capacity() == size()
 * */
TEST_F(vector_space_test,test_method_shrink_to_fit)
{
  s1.shrink_to_fit();
  ASSERT_EQ(s1.size(),s1.capacity());
}
/*
 * Testing assign(size_type & size,const_reference value)
 * old_size != size()
 * old_capacity != capacity()
 * */
TEST_F(vector_space_test,test_method_assign_1)
{
  auto old_size = s1.size();
  auto old_capacity = s1.capacity();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned int> dist(old_size + 1 ,old_capacity + 1);
  s1.assign(dist(mt),dist(mt));
  ASSERT_NE(old_size,s1.size());
}
/*
 * Testing assign(InputIterator start,InputIterator begin)
 * old_size != size()
 * old_capacity != capacity()
 * All iterators are invalidated
 * */
TEST_F(vector_space_test,test_method_assign_rng)
{
  auto old_size = s1.size();
  auto old_capacity = s1.capacity();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned int> dist(0 ,old_capacity + 1);
  vector_space<lluint,1> ts1;
  for(unsigned i =0; i < old_size;++i)
  {
	s1.push_back(dist(mt));
  }
  s1.assign(ts1.begin(),ts1.end());
  ASSERT_NE(old_size,s1.size());
}
/*
 * Testing assign(initializerlist)
 * old_size != size()
 * old_capacity != capacity() 
 * All iterators are invalidated
 * */
TEST_F(vector_space_test,test_method_assign_ilist)
{
  s1.clear();
  auto old_size = s1.size();
  auto old_capacity = s1.capacity();
  s1.assign({1215434,152635,65656565,6767676,898989});
  ASSERT_NE(old_size,s1.size());
  ASSERT_NE(old_capacity,s1.capacity());
  ASSERT_NE( std::addressof(*s1.end()) ,std::addressof(*s1.begin()) );
  ASSERT_NE( std::addressof(*s1.cend()) ,std::addressof(*s1.cbegin()) );
  ASSERT_NE( std::addressof(*s1.rend()) ,std::addressof(*s1.begin()) );
  ASSERT_NE( std::addressof(*s1.crend()) ,std::addressof(*s1.cbegin()) );
}
/*
 * Testing assign(size_type & size,const_reference value)
 * old_size != size()
 * old_capacity != capacity()
 * All iterators are invalidated
 * */
TEST_F(vector_space_test,test_method_assign_2)
{
  
  auto old_size = s1.size();
  auto old_capacity = s1.capacity();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned int> dist(old_size + 1 ,old_capacity + 1);
  s1.assign(100,dist(mt));
  ASSERT_NE(old_size,s1.size());
  ASSERT_NE( std::addressof(*s1.end()) ,std::addressof(*s1.begin()) );
  ASSERT_NE( std::addressof(*s1.cend()) ,std::addressof(*s1.cbegin()) );
  ASSERT_NE( std::addressof(*s1.rend()) ,std::addressof(*s1.begin()) );
  ASSERT_NE( std::addressof(*s1.crend()) ,std::addressof(*s1.cbegin()) );
}
/*
 * Testing clear()
 * size() == 0
 * old_capacity == capacity()
 * */
TEST_F(vector_space_test,test_method_clear)
{
  auto old_capacity = s1.capacity();
  s1.clear();
  ASSERT_EQ(s1.size(),0);
  ASSERT_EQ(old_capacity,s1.capacity());
}
/*
 * Testing emplace(const it pos,args)
 * will check emplacing at end and anywhere other than the end, i.e the if else in the body of emplace
 * */
TEST_F(vector_space_test, test_method_emplace_ptr)
{
  s1.assign(s1.capacity()+100,73);
  auto old_size = s1.size();
  auto it = s1.emplace(s1.end(),888888);  
  ASSERT_EQ(old_size + 1 ,s1.size());
  ASSERT_EQ(s1[s1.size()-1],*(it));
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,s1.size()-2);
  auto pos_index = dist(mt);
  it = s1.emplace(s1.begin()+pos_index,999999); 
  ASSERT_EQ(old_size + 2,s1.size());
  ASSERT_EQ(s1[pos_index],*(it));
}
/*
 *Testing member emplace back
 **/
TEST_F(vector_space_test, test_method_emplace_back)
{
  s1.assign(s1.capacity()+100,73);
  auto old_size = s1.size();
  s1.emplace_back(888888);  
  ASSERT_EQ(old_size + 1 ,s1.size());
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,s1.size()-2);
  s1.emplace_back(999999); 
  ASSERT_EQ(old_size + 2,s1.size());
}
/*
 * Testing insert(const it pos,const ref pos)
 * will check insert at end and anywhere other than the end, i.e the if else in the body of insert
 * size()==size()+1
 * */
TEST_F(vector_space_test, test_method_insert_ptr)
{
  auto old_size = s1.size();
  auto it = s1.insert(s1.end(),222222);  
  ASSERT_EQ(old_size + 1 ,s1.size());
  ASSERT_EQ(s1[s1.size()-1],*(it));
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,s1.size()-2);
  auto pos_index = dist(mt);
  it = s1.insert(s1.begin()+pos_index,444444); 
  ASSERT_EQ(old_size + 2,s1.size());
  ASSERT_EQ(s1[pos_index],*(it));
}
/*
 * Testing insert(const it pos,n copies,const ref pos)
 * will check insert at end and anywhere other than the end, i.e the if else in the body of emplace
 * size()==size()+n
 * */
TEST_F(vector_space_test, test_method_insert_ncopies_ptr)
{
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,s1.size()-2);
  auto old_size = s1.size();
  auto n_copies = dist(mt);
  s1.insert(s1.end(),n_copies,121212);  
  ASSERT_EQ(old_size + n_copies ,s1.size());
  unsigned count = 0;
  for(unsigned i=n_copies -1; i<s1.size();++i)
  {
	if(s1[i] == 121212)
	  ++count;
  }
  ASSERT_EQ(count,n_copies);
}
/*
 * Testing insert(InputIterator start,InputIterator begin)
 * old_size != size()
 * old_capacity != capacity()
 * All iterators are invalidated
 * */
TEST_F(vector_space_test,test_method_insert_rng)
{
  auto old_size = s1.size();
  auto old_capacity = s1.capacity();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned int> dist(0 ,old_capacity + 1);
  vector_space<lluint,1> s1n;
  for(unsigned i =0; i < old_size;++i)
  {
	s1n.push_back(dist(mt));
  }
  auto pos = s1.begin();
  s1.insert(pos,s1n.begin(),s1n.end());
  ASSERT_NE(old_size,s1.size());
  
  ASSERT_NE( std::addressof(*s1.end()) ,std::addressof(*s1.begin()) );
  ASSERT_NE( std::addressof(*s1.cend()) ,std::addressof(*s1.cbegin()) );
  ASSERT_NE( std::addressof(*s1.rend()) ,std::addressof(*s1.begin()) );
  ASSERT_NE( std::addressof(*s1.crend()) ,std::addressof(*s1.cbegin()) );

}
/*
 * Testing insert(initializer list)
 * old_size != size()
 * old_capacity != capacity()
 * All iterators are invalidated
 * */
TEST_F(vector_space_test,test_method_ilist)
{
  s1.clear();
  auto old_size = s1.size();
  auto old_capacity = s1.capacity();
  s1.insert(s1.begin(),{1,2,3,4,5,6});
  ASSERT_NE(old_size,s1.size());
  ASSERT_EQ(old_capacity,s1.capacity());

  ASSERT_NE( std::addressof(*s1.end()) ,std::addressof(*s1.begin()) );
  ASSERT_NE( std::addressof(*s1.cend()) ,std::addressof(*s1.cbegin()) );
  ASSERT_NE( std::addressof(*s1.rend()) ,std::addressof(*s1.begin()) );
  ASSERT_NE( std::addressof(*s1.crend()) ,std::addressof(*s1.cbegin()) );

}
/*
 * Testing member erase
 * size()==size()-1
 * capacity() unchanged
 * */
TEST_F(vector_space_test,test_member_erase)
{
  auto old_size = s1.size();
  auto old_capacity = s1.capacity();
  s1.erase(s1.begin());
  ASSERT_EQ(old_size - 1,s1.size());
  ASSERT_EQ(old_capacity,s1.capacity());
}
/*
 * Testing member erase_rng
 * size()==size()-n
 * capacity() unchanged
 * */
TEST_F(vector_space_test,test_member_erase_rng)
{
  s1.clear();
  s1.assign(100,69);
  auto old_size = s1.size();
  auto old_capacity = s1.capacity();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,s1.size()-2);
  auto elems_to_erase = dist(mt);
  s1.erase(s1.begin(),s1.begin()+elems_to_erase);
  ASSERT_EQ(old_size - elems_to_erase,s1.size());
  ASSERT_EQ(old_capacity,s1.capacity());
}
/*
 * Testing member resize_rval
 * resize() up i.e increase size > capacity
 * old_size!=size()
 * old_capacity!=capacity()
 * resize() down
 * old_size!=size()
 * capacity unchanged
 * */
TEST_F(vector_space_test,test_member_resize_rval)
{
  auto old_capacity = s1.capacity();
  auto old_size = s1.size();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(s1.capacity()+1,s1.capacity()+s1.size());
  auto resize_factor = dist(mt);
  s1.resize(resize_factor);
  ASSERT_NE(old_size,s1.size());
  ASSERT_NE(old_capacity,s1.capacity());
  old_capacity = s1.capacity();
  old_size = s1.size();
  decltype(dist.param()) new_range(0,s1.size());
  dist.param(new_range);
  resize_factor = dist(mt);
  s1.resize(resize_factor);
  ASSERT_NE(old_size,s1.size());
  ASSERT_EQ(old_capacity,s1.capacity());
}
/*
 * Testing method push_back
 * size() before call != size() after call
 * check amortized growth
 * after call size != zero
 * */
TEST_F(vector_space_test,test_member_push_back)
{

  if(s1.size() == s1.capacity())
	s1.push_back(lluint());
  else if (s1.size() < s1.capacity()) 
  {
	//increase current size to trigger a resize
	auto old_capacity = s1.capacity();
	for(unsigned i = s1.size(); i <= old_capacity;++i)
	  s1.push_back(lluint());
  }
  ASSERT_TRUE( (s1.size() -1) == s1.capacity()/2) ; 
  ASSERT_TRUE(s1.size() != 0);
}
/*
 * Test method slice
 * */
TEST_F(vector_space_test,test_member_slice)
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<lluint> dist(0,s1.size());
  for(lluint i = 0; i < s1.size(); ++i)
  {
	s1[i] = dist(mt);
  }
  decltype (dist.param()) new_range(0,5);
  dist.param(new_range);
  auto start = dist(mt);
  auto size = dist(mt);
  auto stride = dist(mt);
  auto slice = s1.slice(start, size, stride);
  auto beg = s1.cbegin() + start;
  ASSERT_EQ(slice.size(),size);
  for(unsigned i =0; i<slice.size();++i)
  {
	ASSERT_EQ((*beg),slice[i]);
	beg+= stride;
  }
}
/*
 * Test method gslice - not the same as std::gslice, but can be manipulated to spit out the same results, which I won't do here
 * */
TEST_F(vector_space_test,test_member_gslice)
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<lluint> dist(0,s1.size());
  for(lluint i = 0; i < s1.size(); ++i)
  {
	s1[i] = dist(mt);
  }
  decltype (dist.param()) new_range(0,5);
  dist.param(new_range);
  auto starts = decltype(s1)({dist(mt),dist(mt)});
  auto sizes = decltype(s1)({dist(mt),dist(mt)});
  auto strides = decltype(s1)({dist(mt),dist(mt)});
  auto gslice = s1.gslice(starts,sizes,strides);
  for(lluint i = 0, offset = 0; i < starts.size(); ++i)
  {
	auto temp = s1.slice(starts[i], sizes[i], strides[i]);
    auto begin = (s1.cbegin() + starts[i]);
	for(lluint j = 0; j < temp.size(); ++j)
	{
	  ASSERT_EQ(temp[j],*(begin + offset));
	  offset += strides[i];
	}
	offset = 0;
  }
}
/*
 * Test method pop front
 * size - 1 = size() 
 * */
TEST_F(vector_space_test,test_member_pop_front)
{
  EXPECT_TRUE(s1.size() >= 1);
  auto size = s1.size();
  s1.pop_front();
  ASSERT_EQ(s1.size(),size - 1);
}
/*
 * Test method pop back
 * size - 1 = size() 
 * */
TEST_F(vector_space_test,test_member_pop_back)
{

  EXPECT_TRUE(s1.size() >= 1);
  auto size = s1.size();
  s1.pop_front();
  ASSERT_EQ(size - 1,s1.size());
}
/*
 * Test member swap
 * s1.swap(temp)
 * */
TEST_F(vector_space_test,test_member_swap)
{
  vector_space<lluint,1> ts1({1,2,3,4,5});
  auto s1_size = s1.size();
  auto s1_capacity = s1.capacity();
  auto ts1_size = ts1.size();
  auto ts1_capacity = ts1.capacity();

  s1.swap(ts1);
  ASSERT_EQ(s1.size(),ts1_size);
  ASSERT_EQ(s1.capacity(),ts1_capacity);

  ASSERT_EQ(ts1.size(),s1_size);
  ASSERT_EQ(ts1.capacity(),s1_capacity);
}
/*
 * Test copy assignment
 * s1[i] == cs1[i] 
 * s1.size == cs1.size
 * s1.capacity == cs1.capacity
 * */
TEST_F(vector_space_test,test_copy_assignment)
{
  vector_space<lluint,1> cs1(30,12345);
  auto cs1_size = cs1.size();
  auto cs1_capacity = cs1.capacity();
  s1 = cs1;
  ASSERT_EQ(s1.size(),cs1_size);
  ASSERT_EQ(s1.capacity(),cs1_capacity);
}
/*
 * Test move assignment
 * */
TEST_F(vector_space_test,test_member_move_assignment)
{
  vector_space<lluint,1> ms1(30,12345);
  auto ms1_size = ms1.size();
  auto ms1_capacity = ms1.capacity();
  s1 = std::move(ms1);
  ASSERT_EQ(s1.size(),ms1_size);
  ASSERT_EQ(s1.capacity(),ms1_capacity);
  ASSERT_EQ(ms1.size(),0);
  ASSERT_EQ(ms1.capacity(),0);
}

/*
 * Test ilist assignment
 * */
TEST_F(vector_space_test,test_member_ilist_assignment)
{
  auto s1_capacity = s1.capacity();
  s1 = {1,2,3,4,5};
  EXPECT_TRUE(s1.capacity() > 5);
  ASSERT_EQ(s1.size(),5);
  ASSERT_EQ(s1_capacity,s1.capacity());
}
/*
 * Test Scalar Addition
 * lhs is modified, rhs is not
 */
TEST_F(vector_space_test,test_scalar_addition)
{
  vector_space<lluint,1,scalar_addition> vs1,vs2,vs3;
  vs1.assign(3,2);
  vs2.assign(3,3);
  auto old_vs2_size = vs2.size();
  auto old_vs2_capacity = vs2.capacity();
  auto vs1_copy = vs1;
  auto vs2_copy = vs2;
  vs3  = vs1 + vs2;
  for(lluint i = 0; i<vs1.size();++i)
  {
	ASSERT_EQ(vs1[i]+vs2[i],vs3[i]);
  }
  for(lluint i = 0; i<vs1.size(); ++i)
  {
	ASSERT_EQ(vs1[i], vs1_copy[i]);
	ASSERT_EQ(vs2[i],vs2_copy[i]);
  }
  ASSERT_EQ(old_vs2_size,vs2.size());
  ASSERT_EQ(old_vs2_capacity,vs2.capacity());
}
/*
 * Test Scalar Multiplication
 * lhs is modified, rhs is not
 */
TEST_F(vector_space_test,test_scalar_multiplication)
{
  vector_space<lluint,1,null_functor,scalar_multiplication> vs1,vs2,vs3;//vs1_copy;
  vs1.assign(3,2);
  vs2.assign(3,3);
  auto old_vs2_size = vs2.size();
  auto old_vs2_capacity = vs2.capacity();
  auto vs1_copy = vs1;
  auto vs2_copy = vs2;
  vs3 = vs1 * vs2;
  for(lluint i = 0; i<vs1.size();++i)
  {
 	ASSERT_EQ(vs1_copy[i]*vs2[i],vs3[i]);
  } 
  for(lluint i = 0; i<vs1.size(); ++i)
  {
	ASSERT_EQ(vs1[i], vs1_copy[i]);
	ASSERT_EQ(vs2[i],vs2_copy[i]);
  }
  ASSERT_EQ(old_vs2_size,vs2.size());
  ASSERT_EQ(old_vs2_capacity,vs2.capacity());
}
/*
 * Test Scalar Addition +=
 * lhs is modified, rhs is not
 */
TEST_F(vector_space_test,test_scalar_addition_plus_equal)
{
  vector_space<lluint,1,scalar_addition> vs1,vs2,vs3;
  vs1.assign(3,2);
  vs2.assign(3,3);
  auto old_vs2_size = vs2.size();
  auto old_vs2_capacity = vs2.capacity();
  vs3 += vs1 + vs2;
  for(lluint i = 0; i<vs1.size();++i)
  {
	ASSERT_EQ(vs1[i]+vs2[i],vs3[i]);
  }
  auto old_vs3 = vs3;
  vs3 += old_vs3;
  for(lluint i = 0; i<vs3.size();++i)
  {
	ASSERT_EQ(vs3[i], old_vs3[i] + old_vs3[i]);
  }
  ASSERT_EQ(old_vs2_size,vs2.size());
  ASSERT_EQ(old_vs2_capacity,vs2.capacity());
}
/*
 * Test Scalar Addition *=
 * lhs is modified, rhs is not
 */
TEST_F(vector_space_test,test_scalar_addition_times_equal)
{
  vector_space<lluint,1,null_functor,scalar_multiplication> vs1,vs2,vs3;
  vs1.assign(3,2);
  vs2.assign(3,3);
  auto old_vs2_size = vs2.size();
  auto old_vs2_capacity = vs2.capacity();
  vs3 *= vs1 * vs2;
  for(lluint i = 0; i<vs1.size();++i)
  {
	ASSERT_EQ(0,vs3[i]);
  }
  std::fill(vs3.begin(),vs3.end(),2);
  auto old_vs3 = vs3;
  vs3 *= old_vs3;
  for(lluint i = 0; i<vs3.size();++i)
  {
	ASSERT_EQ(vs3[i], old_vs3[i] * old_vs3[i]);
  }
  ASSERT_EQ(old_vs2_size,vs2.size());
  ASSERT_EQ(old_vs2_capacity,vs2.capacity());
}
//####################################################
// TEST SPACE ORDER >= 2							 #
//####################################################
/*
 * Default Ctor Test
 * size(),capacity() ret 0
 * empty() ret 1
 * data()==nullptr
 * addressof(begin) == addressof(end)
 * dimensions() should result in an exception 
 **/
TEST_F(vector_space_test,default_ctor_sN)
{
  ASSERT_EQ(sn.size(),0);
  ASSERT_EQ(sn.capacity(),0);
  ASSERT_EQ(sn.empty(),1);
  ASSERT_EQ(sn.data(),nullptr);
  ASSERT_EQ(std::addressof( *sn.begin()),std::addressof( *sn.end()) );
  ASSERT_EQ(std::addressof( *sn.cbegin()),std::addressof( *sn.cend()) );
  //sn.dimensions(); correctly determins that dimensions is 0;
}
/*
 * Test method clear()
 * capacity() unchanged
 * size() == 0
 * empty() == 1 
 * */
TEST_F(vector_space_test,test_member_clear_sn)
{
  auto old_capacity = sn.capacity();
  sn.clear();
  ASSERT_EQ(sn.size(),0);
  ASSERT_EQ(sn.capacity(),old_capacity);
  ASSERT_EQ(sn.empty(),1);
}
/*
 * Test Method Reshape - reshape above capacity and below it
 * vector_space<T,1> rval
 * size() unmodified if new shape exceeds capacity
 * capacity() == pi(sn.dimensions())
 * empty() ret 0
 **/
TEST_F(vector_space_test,test_member_reshape_rval)
{
  vector_space<lluint,1,scalar_addition,scalar_multiplication> new_dimensions(3,2);
  auto expected_capacity = std::accumulate(new_dimensions.begin(),new_dimensions.end(),lluint(1),std::multiplies<lluint>());
  auto old_size = sn.size();
  sn.reshape(new_dimensions);
  new_dimensions = sn.dimensions();
  ASSERT_NE(old_size, sn.size());
  ASSERT_EQ(sn.size(), sn.capacity());
  ASSERT_EQ(sn.capacity(),expected_capacity);
  ASSERT_EQ(sn.empty(),0);
  vector_space<lluint,1,scalar_addition,scalar_multiplication> new_dimensions1(3,1);
  sn.reshape(new_dimensions1);
  expected_capacity = std::accumulate(new_dimensions1.begin(), new_dimensions1.end(), lluint(1), std::multiplies<lluint>());
  ASSERT_EQ(sn.capacity(), expected_capacity);
}
/*
 * Test Method Reshape - reshape above capacity and below it
 * vector_space<T,1> rval
 * size() unmodified if new shape exceeds capacity 
 * capacity() == pi(sn.dimensions())
 * empty() ret 0
 * */
TEST_F(vector_space_test,test_member_reshape_lval)
{
  vector_space<lluint,1,scalar_addition,scalar_multiplication> new_dimensions;
  sn.clear();
  sn.reshape(vector_space<lluint,1,scalar_addition,scalar_multiplication>(3,3));
  new_dimensions = sn.dimensions();
  ASSERT_EQ(sn.capacity(),std::accumulate(new_dimensions.begin(),new_dimensions.end(),lluint(1),std::multiplies<lluint>()));
  ASSERT_EQ(sn.empty(),0);
  vector_space<lluint,1,scalar_addition,scalar_multiplication> new_dimensions1(3,2);
  sn.reshape(new_dimensions1);
  ASSERT_EQ(sn.capacity(),std::accumulate(new_dimensions1.begin(),new_dimensions1.end(),lluint(1),std::multiplies<lluint>()));
  ASSERT_EQ(sn.empty(),0);
}
/*
 * Test Method Reshape
 * vector_space<T,1> variadic
 * size() != old_size
 * capacity() == pi(sn.dimensions())
 * empty() ret 0
 * */
TEST_F(vector_space_test,test_member_reshape_variadic)
{
  vector_space<lluint,1,scalar_addition,scalar_multiplication> new_dimensions;
  sn.clear();
  sn.reshape(4,4,4);
  new_dimensions = sn.dimensions();
  ASSERT_EQ(sn.capacity(),std::accumulate(new_dimensions.begin(),new_dimensions.end(),lluint(1),std::multiplies<lluint>()));
  sn.reshape(2,2,2);
  vector_space<lluint,1,scalar_addition,scalar_multiplication> new_dimensions1(sn.dimensions());
  ASSERT_EQ(sn.capacity(),std::accumulate(new_dimensions1.begin(),new_dimensions1.end(),lluint(1),std::multiplies<lluint>()));
}
/*
 * Testing method push_back
 * size() before call != size() after call
 * check amortized growth
 * after call size != zero
 * */
TEST_F(vector_space_test,test_member_push_back_sN)
{
  auto old_size = sn.size();
  auto old_capacity = sn.capacity();
  auto z = sn.dimensions()[0];
  ASSERT_PRED_FORMAT1(assert_const_amortized_growth<3>,sn);
  ASSERT_NE(old_size,sn.size());
  ASSERT_EQ(old_capacity*2,sn.capacity());
  ASSERT_EQ(z*2,sn.dimensions()[0]);
  old_size = sn.size();
  old_capacity = sn.capacity();
  z = sn.dimensions()[0];
  ASSERT_PRED_FORMAT1(assert_const_amortized_growth<3>,sn);
  ASSERT_NE(old_size,sn.size());
  ASSERT_EQ(old_capacity*2,sn.capacity());
  ASSERT_EQ(z*2,sn.dimensions()[0]);
}

/*
 * Test method map
 * transform R^n coordinate vector to R1
 * Given a vector in R^n where n > 1, 
 * *(begin() + map(i,j,k)) ==  sn[i][j][k]
 * */
TEST_F(vector_space_test,test_method_map_sN)
{
  sn.reshape(6,5,5);
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,sn.capacity()*2);
  vector_space<lluint,1,scalar_addition,scalar_multiplication> dim = sn.dimensions();
  for(unsigned i =0;i<dim[0];++i)
  {
	for(unsigned j = 0;j<dim[1];++j)
	{
	  for(unsigned k = 0; k<dim[2];++k)
	  {
		sn.push_back(dist(mt));
		ASSERT_EQ( *(sn.begin()+sn.map(i,j,k)) ,sn[i][j][k]);
	  }
	}
  }
}

/*
 * Testing member resize_rval
 * resize() up i.e increase size > capacity
 * old_size!=size()
 * old_capacity!=capacity()
 * resize() down
 * old_size!=size()
 * capacity unchanged
 * */
TEST_F(vector_space_test,test_member_resize_rval_sN)
{
  auto old_capacity = sn.capacity();
  auto old_size = sn.size();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(sn.capacity()+1,sn.capacity()+sn.size());
  auto resize_factor = dist(mt);
  sn.resize(resize_factor);
  ASSERT_NE(old_size,sn.size());
  ASSERT_NE(old_capacity,sn.capacity());
  old_capacity = sn.capacity();
  old_size = sn.size();
  decltype(dist.param()) new_range(0,sn.size());
  dist.param(new_range);
  resize_factor = dist(mt);
  sn.resize(resize_factor);
  ASSERT_NE(old_size,sn.size());
  ASSERT_EQ(old_capacity,sn.capacity());
}
/*
 * Testing operator [] both reference and const_reference
 * size(),capacity(),empty(),data() unmodified after function call
 * sn[i] == *(sn.begin() + i) 
 * sn[i] == *(sn.cbegin() + i) 
 * */
TEST_F(vector_space_test,test_method_subscript_operator_sN)
{
  sn.reshape(6,6,7);
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,sn.capacity()*2);
  vector_space<lluint,1,scalar_addition,scalar_multiplication> dim = sn.dimensions();
  for(unsigned i =0;i<dim[0];++i)
  {
	for(unsigned j = 0;j<dim[1];++j)
	{
	  for(unsigned k = 0; k<dim[2];++k)
	  {
		sn.push_back(dist(mt));
		ASSERT_EQ( *(sn.begin()+sn.map(i,j,k)) ,sn[i][j][k]);
	  }
	}
  }
}
/*
 * Test member front()
 * sn[0]==sn.front();
 * size(),capacity(),empty(),data(),back() unmodified after function call
 * the data begin() and end() point to should not be modified 
 * */
TEST_F(vector_space_test,test_member_front_sN)
{
  auto size = sn.size();
  auto capacity = sn.capacity();
  auto empty = sn.empty();
  auto data = sn.data();
  if(!sn.empty())
  {
  	auto back = sn.back();
	auto front = sn.front();
	ASSERT_EQ(front,sn[0][0][0]);
    ASSERT_EQ(back,sn.back());
  }
  ASSERT_EQ(size,sn.size());
  ASSERT_EQ(capacity,sn.capacity());
  ASSERT_EQ(empty,sn.empty());
  vector_space<lluint,1,scalar_addition,scalar_multiplication> dim = sn.dimensions();
  for(unsigned i =0;i<dim[0];++i)
  {
	for(unsigned j = 0;j<dim[1];++j)
	{
	  for(unsigned k = 0; k<dim[2];++k)
	  {
		ASSERT_EQ( *(sn.begin()+sn.map(i,j,k)) ,sn[i][j][k]);
		ASSERT_EQ( *(sn.cbegin()+sn.map(i,j,k)) ,sn[i][j][k]);
  		ASSERT_EQ(data[i],sn.data()[i]);
	  }
	}
  }
}
/*
 * Test member back()
 * sn[sn.size()-1] == sn.back();
 * size(),capacity(),empty(),data() unmodified
 * the data begin() and end() point to should not be modified 
 * */
TEST_F(vector_space_test,test_member_back_sN)
{
  auto size = sn.size();
  auto capacity = sn.capacity();
  auto empty = sn.empty();
  auto data = sn.data();
  if(!sn.empty())
  {	
	auto back = sn.back();
	auto front = sn.front();
	ASSERT_EQ(back,*(sn.end()-1));
	ASSERT_EQ(front,sn.front());
  }
  ASSERT_EQ(size,sn.size());
  ASSERT_EQ(capacity,sn.capacity());
  ASSERT_EQ(empty,sn.empty());
  vector_space<lluint,1,scalar_addition,scalar_multiplication> dim = sn.dimensions();
  for(unsigned i =0;i<dim[0];++i)
  {
	for(unsigned j = 0;j<dim[1];++j)
	{
	  for(unsigned k = 0; k<dim[2];++k)
	  {
		ASSERT_EQ( *(sn.begin()+sn.map(i,j,k)) ,sn[i][j][k]);
		ASSERT_EQ( *(sn.cbegin()+sn.map(i,j,k)) ,sn[i][j][k]);
  		ASSERT_EQ(data[i],sn.data()[i]);
	  }
	}
  }
}
/*
 * Testing member at
 * size(),capacity(),empty(),data() unmodified
 * sn.at(i) == *(sn.begin() + i)
 * sn.at(i) == *(sn.cbegin() + i)
 * sn[i] == *(sn.begin() + i)
 * sn[i] == *(sn.cbegin() + i)
 * */
TEST_F(vector_space_test,test_member_at_sN)
{
  auto size = sn.size();
  auto capacity = sn.capacity();
  auto empty = sn.empty();
  auto data = sn.data();
  vector_space<lluint,1,scalar_addition,scalar_multiplication> dim = sn.dimensions();
  for(unsigned i =0;i<dim[0];++i)
  {
	for(unsigned j = 0;j<dim[1];++j)
	{
	  for(unsigned k = 0; k<dim[2];++k)
	  {
		ASSERT_EQ(sn.at(i,j,k) ,sn[i][j][k]);
  		ASSERT_EQ(data[i],sn.data()[i]);
		ASSERT_EQ( *(sn.begin()+sn.map(i,j,k))  , sn.at(i,j,k) );
		ASSERT_EQ( *(sn.cbegin()+sn.map(i,j,k))  , sn.at(i,j,k) );
	  }
	}
  }
  ASSERT_EQ(size,sn.size());
  ASSERT_EQ(capacity,sn.capacity());
  ASSERT_EQ(empty,sn.empty());
}

/*
 * Testing method both begin() and cbegin()
 * size(),capacity(),empty(),data(), unmodified
 * sn[i] == *(begin() + i);
 * sn[i] == *(cbegin() + i);
 * sn.at(i) == *(begin() + i);
 * sn.at(i) == *(cbegin() + i);
 * std::addressof(*sn.begin()) != std::addressof(*sn.end()) 
 * std::addressof(sn.cbegin()) != std::addressof(*sn.cend())
 * */
TEST_F(vector_space_test,test_method_begin_sN)
{
  auto size = sn.size();
  auto capacity = sn.capacity();
  auto empty = sn.empty();
   
  vector_space<lluint,1,scalar_addition,scalar_multiplication> dim = sn.dimensions();
  for(unsigned i =0;i<dim[0];++i)
  {
	for(unsigned j = 0;j<dim[1];++j)
	{
	  for(unsigned k = 0; k<dim[2];++k)
	  {
		ASSERT_EQ(sn.at(i,j,k) ,sn[i][j][k]);
		ASSERT_EQ( *(sn.begin()+sn.map(i,j,k))  , sn.at(i,j,k) );
		ASSERT_EQ( *(sn.cbegin()+sn.map(i,j,k))  , sn.at(i,j,k) );
		ASSERT_EQ( *(sn.begin()+sn.map(i,j,k))  , sn[i][j][k] );
		ASSERT_EQ( *(sn.cbegin()+sn.map(i,j,k))  , sn[i][j][k] );

	  }
	}
  }
  ASSERT_EQ(size,sn.size());
  ASSERT_EQ(capacity,sn.capacity());
  ASSERT_EQ(empty,sn.empty());
  ASSERT_NE(std::addressof( *sn.begin()),std::addressof( *sn.end()) );
  ASSERT_NE(std::addressof( *sn.cbegin()),std::addressof( *sn.cend()) );
}

/*
 * Testing method both end() and cend()
 * size(),capacity(),empty(),data(),front(),back() unmodified
 * sn[size - i -1] == *(sn.end() - i -1))
 * sn[size - i -1] == *(sn.cend() - i -1)
 * sn.at(size - i -1) == *(sn.end() - i -1)
 * sn.at(size - i -1) ==  *(sn.end() - i -1)
 * std::addressof(*sn.end()) != std::addressof(*sn.begin()) 
 * std::addressof(*sn.cend()) != std::addressof(*sn.cbegin())
 * */ 
TEST_F(vector_space_test,test_method_end_sN)
{
  auto size = sn.size();
  auto capacity = sn.capacity();
  auto empty = sn.empty();
  
  vector_space<lluint,1,scalar_addition,scalar_multiplication> dim = sn.dimensions();
  lluint count = 1;
  for(unsigned i =0;i<dim[0];++i)
  {
	for(unsigned j = 0;j<dim[1];++j)
	{
	  for(unsigned k = 0; k<dim[2];++k)
	  {
		ASSERT_EQ(sn.at(i,j,k) ,sn[i][j][k]);
		ASSERT_EQ( *(sn.end() - sn.map(i,j,k) - 1)  , *(sn.end()-count) );
		ASSERT_EQ( *(sn.cend() - sn.map(i,j,k)- 1)  , *(sn.cend()-count) );
		++count;
	  }
	}
  }
  ASSERT_EQ(size,sn.size());
  ASSERT_EQ(capacity,sn.capacity());
  ASSERT_EQ(empty,sn.empty());
  ASSERT_NE( std::addressof(*sn.end()) ,std::addressof(*sn.begin()) );
  ASSERT_NE( std::addressof(*sn.cend()) ,std::addressof(*sn.cbegin()) );
}
/*
 * Testing method both rbegin() and crbegin()
 * size(),capacity(),empty(),data() unmodified
 * sn[sn.size() - i -1] == *(sn.rbegin() + i)
 * sn[sn.size() -i -1] == *(sn.crbegin()+ i)
 * sn.at(size - i -1) == *(sn.rbegin() + i)
 * sn.at(size - i -1) ==  *(sn.crend() + i)
 * std::addressof(*sn.rbegin()) != std::addressof(*sn.begin()
 * std::addressof(*sn.crbegin()) != std::addressof(*sn.cbegin())
 * */ 
TEST_F(vector_space_test,test_method_rbegin_sN)
{
  auto size = sn.size();
  auto capacity = sn.capacity();
  auto empty = sn.empty();
  vector_space<lluint,1,scalar_addition,scalar_multiplication> dim = sn.dimensions();
  long count = 1;
  for(unsigned i =0;i<dim[0];++i)
  {
	for(unsigned j = 0;j<dim[1];++j)
	{
	  for(unsigned k = 0; k<dim[2];++k)
	  {
		ASSERT_EQ(sn.at(i,j,k) ,sn[i][j][k]);
		ASSERT_EQ( *( sn.rbegin()+sn.map(i,j,k) )  , *(sn.end()-count) );
		ASSERT_EQ( *( sn.crbegin() + sn.map(i,j,k) )  , *(sn.cend()-count));
		++count;
	  }
	}
  }
  ASSERT_EQ(size,sn.size());
  ASSERT_EQ(capacity,sn.capacity());
  ASSERT_EQ(empty,sn.empty());
  ASSERT_NE( std::addressof(*sn.rbegin()) ,std::addressof(*sn.begin()) );
  ASSERT_NE( std::addressof(*sn.crbegin()) ,std::addressof(*sn.cbegin()) );
}

/*
 * Testing method both rend() and crend()
 * size(),capacity(),empty(),data() unmodified
 * sn[i] == *(sn.rend() + i)
 * sn[i] == *(sn.crend()+ i)
 * sn.at(i) == *(sn.rend() + i)
 * sn.at(i) ==  *(sn.crend() + i)
 * std::addressof(*sn.rend()) != std::addressof(*sn.end()) 
 * std::addressof(*sn.crend()) != std::addressof(*sn.cbegin())
 * */
TEST_F(vector_space_test,test_method_rend_sN)
{
  auto size = sn.size();
  auto capacity = sn.capacity();
  auto empty = sn.empty();
  vector_space<lluint,1,scalar_addition,scalar_multiplication> dim = sn.dimensions();
  for(unsigned i =0;i<dim[0];++i)
  {
	for(unsigned j = 0;j<dim[1];++j)
	{
	  for(unsigned k = 0; k<dim[2];++k)
	  {
		ASSERT_EQ(sn.at(i,j,k) ,sn[i][j][k]);
		ASSERT_EQ( *(sn.rend() - sn.map(i,j,k) -1)  , sn.at(i,j,k) );
		ASSERT_EQ( *(sn.crend()-sn.map(i,j,k) -1)  , sn.at(i,j,k) );
		ASSERT_EQ( *(sn.rend()-sn.map(i,j,k) -1)  , sn[i][j][k] );
		ASSERT_EQ( *(sn.crend()-sn.map(i,j,k) -1)  , sn[i][j][k] );

	  }
	}
  }
  ASSERT_EQ(size,sn.size());
  ASSERT_EQ(capacity,sn.capacity());
  ASSERT_EQ(empty,sn.empty());
  ASSERT_NE( std::addressof(*sn.rend()) ,std::addressof(*sn.begin()) );
  ASSERT_NE( std::addressof(*sn.crend()) ,std::addressof(*sn.cbegin()) );
}
/*
 * Testing member reserve
 * old_capacity != capacity()
 * size() should be unmodified
 * */
TEST_F(vector_space_test,test_method_reserve_sN)
{
  auto old_size = sn.size();
  auto old_capacity = sn.capacity();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned int> dist(10,20);
  sn.reserve(old_capacity+dist(mt));
  ASSERT_EQ(old_size,sn.size());
  ASSERT_NE(old_capacity,sn.capacity());
  ASSERT_TRUE(old_capacity < sn.capacity());
}
/*
 * Testing member shrink_to_fit
 * capacity() == size()
 * */
TEST_F(vector_space_test,test_method_shrink_to_fit_sN)
{
  auto size = sn.size();
  auto capacity = sn.capacity();
  //std::cout<<"capacity: "<<sn.capacity()<<" size: "<<sn.size()<<"\n";
  sn.shrink_to_fit();
  if(capacity != sn.capacity())
	ASSERT_EQ(sn.size(),sn.capacity());
  else
	ASSERT_TRUE( (size == sn.size()) && (capacity == sn.capacity()) );
  //std::cout<<sn.dimensions();
  sn.reshape(4,4,4); 
  sn.resize(16);
  sn.shrink_to_fit();
  ASSERT_EQ(sn.size(),sn.capacity());
 // std::cout<<"capacity: "<<sn.capacity()<<" size: "<<sn.size()<<"\n";
  //std::cout<<sn.dimensions();
}
/*
 * Testing assign(size_type & size,const_reference value)
 * could be 3 parts to this test because 3 parts to fill assign
 * part 1 assign # of elements > capacity
 * part 2 assign # of elements >  size nope because capacity is not increased so neither is the dimensions
 * part 3 assign # of elements <  size nope because capacity is not increase so neither is the dimensions
 * */  
TEST_F(vector_space_test,test_method_assign_lval_sN)
{
  //setup
  sn.clear();
  //sn.shrink_to_fit();
  sn.reshape(2,2,2);
  //part 1
  auto old_size = sn.size();
  auto old_capacity = sn.capacity();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned int> dist(sn.capacity()*sn.capacity() , sn.capacity() * 10);
  std::uniform_int_distribution<unsigned int> new_values_dist(0 ,old_capacity * 43 );
  auto z = dist(mt);
  sn.assign(z+1,new_values_dist(mt));
  ASSERT_NE(old_capacity,sn.capacity());
  ASSERT_NE(old_size,sn.size());
  ASSERT_TRUE(sn.capacity()>= sn.size());
}
/*
 * Testing assign(InputIterator start,InputIterator begin)
 * old_size != size()
 * old_capacity != capacity()
 * All iterators are invalidated
 * */ 
TEST_F(vector_space_test,test_method_assign_rng_sN)
{
  auto old_size = sn.size();
  auto old_capacity = sn.capacity();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned int> dist(0 ,old_capacity + 1);
  vector_space<lluint,3> tsn;
  tsn.reshape(4,4,4);
  tsn.resize(0);
  for(unsigned i = 0; i < tsn.dimensions()[0];++i)
  {
	for(unsigned j = 0; j < tsn.dimensions()[1];++j)
	{
	  for(unsigned k = 0; k < tsn.dimensions()[2];++k)
	  {
		tsn.push_back(dist(mt));
	  }
	} 
  }
  sn.assign(tsn.begin(),tsn.end());
  ASSERT_NE(old_size,sn.size());
  ASSERT_NE(std::addressof(*sn.end()) ,std::addressof(*sn.begin()) );
  ASSERT_NE(std::addressof(*sn.cend()) ,std::addressof(*sn.cbegin()) );
  ASSERT_NE(std::addressof(*sn.rend()) ,std::addressof(*sn.begin()) );
  ASSERT_NE(std::addressof(*sn.crend()) ,std::addressof(*sn.cbegin()) );
  ASSERT_TRUE(sn.capacity()>= sn.size());
}
/*
 * Testing assign(initializerlist)
 * old_size != size()
 * old_capacity != capacity() 
 * All iterators are invalidated
 * */
TEST_F(vector_space_test,test_method_assign_ilist_sN)
{
  sn.clear();
  auto old_size = sn.size();
  auto old_capacity = sn.capacity();
  sn.assign({1,2,3,4,5});
  ASSERT_NE(old_size,sn.size());
  ASSERT_EQ(old_capacity,sn.capacity());
  ASSERT_NE( std::addressof(*sn.end()) ,std::addressof(*sn.begin()) );
  ASSERT_NE( std::addressof(*sn.cend()) ,std::addressof(*sn.cbegin()) );
  ASSERT_NE( std::addressof(*sn.rend()) ,std::addressof(*sn.begin()) );
  ASSERT_NE( std::addressof(*sn.crend()) ,std::addressof(*sn.cbegin()) );
  ASSERT_TRUE(sn.capacity()>= sn.size());
}
/*
 *
 * Testing assign(size_type & size,const_reference value)
 * old_size != size()
 * old_capacity != capacity()
 * All iterators are invalidated
 * */
TEST_F(vector_space_test,test_method_assign_rval_sN)
{
  auto old_size = sn.size();
  auto old_capacity = sn.capacity();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned int> dist(old_size + 1 ,old_capacity + 1);
  lluint rval = 100;
  sn.assign(rval,dist(mt));
  ASSERT_NE(old_size,sn.size());
  ASSERT_NE( std::addressof(*sn.end()) ,std::addressof(*sn.begin()) );
  ASSERT_NE( std::addressof(*sn.cend()) ,std::addressof(*sn.cbegin()) );
  ASSERT_NE( std::addressof(*sn.rend()) ,std::addressof(*sn.begin()) );
  ASSERT_NE( std::addressof(*sn.crend()) ,std::addressof(*sn.cbegin()) );
  ASSERT_TRUE(sn.capacity()>= sn.size());
}
/*
 * Testing clear()
 * size() == 0
 * old_capacity == capacity()
 * */
TEST_F(vector_space_test,test_method_clear_sN)
{
  auto old_capacity = sn.capacity();
  sn.clear();
  ASSERT_EQ(sn.size(),0);
  ASSERT_EQ(old_capacity,sn.capacity());
  ASSERT_TRUE(sn.capacity()>= sn.size());
}
/*
 * Testing emplace(const it pos,args)
 * will check emplacing at end and anywhere other than the end, i.e the if else in the body of emplace
 * */
TEST_F(vector_space_test, test_method_emplace_ptr_sN)
{
  sn.assign(sn.capacity()+100,73);
  auto old_size = sn.size();
  auto it = sn.emplace(sn.end(),888888);  
  ASSERT_EQ(old_size + 1 ,sn.size());
  ASSERT_EQ(*(sn.end()-1),(*it) );
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,sn.size()-2);
  auto pos_index = dist(mt);
  it = sn.emplace(sn.begin()+pos_index,999999); 
  ASSERT_EQ(old_size + 2,sn.size());
  ASSERT_EQ(*(sn.begin()+pos_index),(*it) );
  ASSERT_TRUE(sn.capacity()>= sn.size());
}
/*
 *Testing member emplace back
 **/
TEST_F(vector_space_test, test_method_emplace_back_sN)
{
  sn.assign(s1.capacity()+100,73);
  auto old_size = sn.size();
  sn.emplace_back(888888);  
  ASSERT_EQ(old_size + 1 ,sn.size());
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,sn.size()-2);
  sn.emplace_back(999999); 
  ASSERT_EQ(old_size + 2,sn.size());
  ASSERT_TRUE(sn.capacity()>= sn.size());
}
/*
 * Testing insert(const it pos,const ref pos)
 * will check insert at end and anywhere other than the end, i.e the if else in the body of insert
 * size()==size()+1
 * */
TEST_F(vector_space_test, test_method_insert_ptr_sN)
{
  auto old_size = sn.size();
  auto it = sn.insert(sn.end(),222222);  
  ASSERT_EQ(old_size + 1 ,sn.size());
  ASSERT_EQ(*(sn.end()-1),(*it) );
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,sn.size()-2);
  auto pos_index = dist(mt);
  it = sn.insert(sn.begin()+pos_index,444444); 
  ASSERT_EQ(old_size + 2,sn.size());
  ASSERT_EQ(*(sn.begin()+pos_index),(*it) );
  ASSERT_TRUE(sn.capacity()>= sn.size());
  ASSERT_TRUE(sn.capacity()>= sn.size()); 
}
/*
 * Testing insert(const it pos,n copies,const ref pos)
 * will check insert at end and anywhere other than the end, i.e the if else in the body of emplace
 * size()==size()+n
 * */
TEST_F(vector_space_test, test_method_insert_ncopies_ptr_sN)
{
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,sn.capacity() - sn.size() - 1);
  EXPECT_TRUE(sn.capacity() - sn.size() - 1);
  auto old_size = sn.size();
  auto old_capacity = sn.capacity();
  unsigned  n_copies = dist(mt);
  sn.insert(sn.begin(),n_copies,121212);
  ASSERT_TRUE(sn.capacity()>= sn.size());
  ASSERT_NE(old_size,sn.size());
  ASSERT_EQ(old_capacity,sn.capacity());
  decltype(dist.param()) new_range(sn.capacity()+10,sn.capacity()*2);
  dist.param(new_range);
  n_copies = dist(mt);
  old_size= sn.size();
  sn.insert(sn.begin(),n_copies,121212);
  ASSERT_NE(old_size,sn.size());
  ASSERT_NE(old_capacity,sn.capacity());
  ASSERT_TRUE(sn.capacity()>= sn.size());
}
/*
 * Testing insert(InputIterator start,InputIterator begin) This method internally call fill_insert like the one directly above so it's a 2 for 1!
 * old_size != size()
 * old_capacity != capacity()
 * All iterators are invalidated
 * */
TEST_F(vector_space_test,test_method_insert_rng_sN)
{
  
  auto old_size = sn.size();
  auto old_capacity = sn.capacity();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned int> dist(0 ,old_capacity + 1);
  vector_space<lluint,1> snn;
  for(unsigned i =0; i < old_size;++i)
  {
	snn.push_back(dist(mt));
  }
  auto pos = sn.begin();
  sn.insert(pos,snn.begin(),snn.end());
  ASSERT_NE(old_size,sn.size());
  ASSERT_TRUE(sn.capacity()>= sn.size()); 
  ASSERT_NE( std::addressof(*sn.end()) ,std::addressof(*sn.begin()) );
  ASSERT_NE( std::addressof(*sn.cend()) ,std::addressof(*sn.cbegin()) );
  ASSERT_NE( std::addressof(*sn.rend()) ,std::addressof(*sn.begin()) );
  ASSERT_NE( std::addressof(*sn.crend()) ,std::addressof(*sn.cbegin()) );
  ASSERT_TRUE(sn.capacity()>= sn.size()); 
}
/*
 * Testing insert(initializer list)
 * old_size != size()
 * old_capacity != capacity()
 * All iterators are invalidated
 * */
TEST_F(vector_space_test,test_method_ilist_sN)
{
  sn.clear();
  auto old_size = sn.size();
  auto old_capacity = sn.capacity();
  sn.insert(sn.begin(),{1,2,3,4,5,6});
  ASSERT_NE(old_size,sn.size());
  ASSERT_EQ(old_capacity,sn.capacity());
  ASSERT_NE( std::addressof(*sn.end()) ,std::addressof(*sn.begin()) );
  ASSERT_NE( std::addressof(*sn.cend()) ,std::addressof(*sn.cbegin()) );
  ASSERT_NE( std::addressof(*sn.rend()) ,std::addressof(*sn.begin()) );
  ASSERT_NE( std::addressof(*sn.crend()) ,std::addressof(*sn.cbegin()) );
  ASSERT_TRUE(sn.capacity()>= sn.size()); 
}
/*
 * Testing member erase
 * size()==size()-1
 * capacity() unchanged
 * */
TEST_F(vector_space_test,test_member_erase_sN)
{
  auto old_size = sn.size();
  auto old_capacity = sn.capacity();
  sn.erase(sn.begin());
  ASSERT_EQ(old_size - 1,sn.size());
  ASSERT_EQ(old_capacity,sn.capacity());
  ASSERT_TRUE(sn.capacity()>= sn.size()); 
}
/*
 * Testing member erase_rng
 * size()==size()-n
 * capacity() unchanged
 * */
TEST_F(vector_space_test,test_member_erase_rng_sN)
{
  sn.clear();
  sn.assign(100,69);
  auto old_size = sn.size();
  auto old_capacity = sn.capacity();
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,sn.size()-2);
  auto elems_to_erase = dist(mt);
  sn.erase(sn.begin(),sn.begin()+elems_to_erase);
  ASSERT_EQ(old_size - elems_to_erase,sn.size());
  ASSERT_EQ(old_capacity,sn.capacity());
  ASSERT_TRUE(sn.capacity()>= sn.size()); 
}

/*
 * Test method pop front
 * size - 1 = size() 
 * */
TEST_F(vector_space_test,test_member_pop_front_sN)
{
  EXPECT_TRUE(sn.size() >= 1);
  auto size = sn.size();
  sn.pop_front();
  ASSERT_EQ(sn.size(),size - 1);
  ASSERT_TRUE(sn.capacity()>= sn.size()); 
}
/*
 * Test method pop back
 * size - 1 = size() 
 * */
TEST_F(vector_space_test,test_member_pop_back_sN)
{
  EXPECT_TRUE(sn.size() >= 1);
  auto size = sn.size();
  sn.pop_front();
  ASSERT_EQ(size - 1,sn.size());
  ASSERT_TRUE(sn.capacity()>= sn.size()); 

}
/*
 * Test member swap
 * sn.swap(temp)
 * */
TEST_F(vector_space_test,test_member_swap_sN)
{
  vector_space<lluint,3,scalar_addition,scalar_multiplication> tsn;
  tsn.reshape(3,3,3);
  std::mt19937 mt;
  std::uniform_int_distribution<lluint> dist (0,199999);
  for(unsigned i = 0; i < sn.capacity();++i)
  {
	tsn.push_back(dist(mt));
  }
  auto sn_size = sn.size();
  auto sn_capacity = sn.capacity();
  auto tsn_size = tsn.size();
  auto tsn_capacity = tsn.capacity();

  sn.swap(tsn);
  ASSERT_EQ(sn.size(),tsn_size);
  ASSERT_EQ(sn.capacity(),tsn_capacity);

  ASSERT_EQ(tsn.size(),sn_size);
  ASSERT_EQ(tsn.capacity(),sn_capacity);
  ASSERT_TRUE(sn.capacity()>= sn.size()); 
}
/*
 * Test method slice
 * */
TEST_F(vector_space_test,test_member_slice_sN)
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<lluint> dist(0,sn.size());
  //std::uniform
  for(lluint i = 0; i < sn.size(); ++i)
  {
	*(sn.begin() + i) = dist(mt);
  }
  decltype (dist.param()) new_range(0,5);
  dist.param(new_range);
  auto start = dist(mt);
  auto size = dist(mt);
  auto stride = dist(mt);
  auto slice = sn.slice(start, size, stride);
  auto beg = sn.begin() + start;
  ASSERT_EQ(slice.size(),size);
  for(lluint i =0; i<slice.size();++i)
  {
	ASSERT_EQ((*beg),slice[i]);
	beg+= stride;
  }
}
/*
 * Test method gslice - not the same as std::gslice, but can be manipulated to spit out the same results, which I won't do here
 * */
TEST_F(vector_space_test,test_member_gslice_sN)
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<lluint> dist(sn.size(), sn.size() * 4);
  sn.clear();
  for(lluint i = 0; i < dist(mt); ++i)
  {
	s1.push_back( dist(mt));
  }
  decltype (dist.param()) new_range(0,5);
  dist.param(new_range);
  auto starts = decltype(s1)({dist(mt),dist(mt)});
  auto sizes = decltype(s1)({dist(mt),dist(mt)});
  auto strides = decltype(s1)({dist(mt),dist(mt)});
  auto gslice = sn.gslice(starts,sizes,strides);
  for(lluint i = 0, offset = 0; i < starts.size(); ++i)
  {
	auto temp = sn.slice(starts[i], sizes[i], strides[i]);
    auto begin = (sn.cbegin() + starts[i]);
	for(lluint j = 0; j < temp.size(); ++j)
	{
	  ASSERT_EQ(temp[j],*(begin + offset));
	  offset += strides[i];
	}
	offset = 0;
  }
}
/*
 * Test copy assignment
 * sn[i] == csn[i] 
 * sn.size == csn.size
 * sn.capacity == csn.capacity
 * */
TEST_F(vector_space_test,test_copy_assignment_sN)
{
  vector_space<lluint,3,scalar_addition,scalar_multiplication> csn;
  csn.reshape(vector_space<lluint,1,scalar_addition,scalar_multiplication>(3,100));//1,000,000 elements
  csn.resize(0);
  int x = 100;
  sn.reshape(x,x,x); 
  //sn.reshape(100,100,100);
  std::mt19937 mt;
  std::uniform_int_distribution<lluint>(0,csn.capacity());
  auto csn_size = csn.size();
  auto csn_capacity = csn.capacity();
  for(lluint i = 0; i<csn.capacity(); ++i)
  {
	csn.emplace_back(i);
  }
  csn_size = csn.size();
  csn_capacity = csn.capacity();
  sn = csn;
  for(lluint i = 0; i<csn.capacity(); ++i)
  {
	ASSERT_EQ( *(csn.begin() + i), *(sn.begin() + i));
  }
  ASSERT_EQ(sn.size(),csn_size);
  ASSERT_EQ(sn.capacity(),csn_capacity);
}
/*
 * Test move assignment
 * */
TEST_F(vector_space_test,test_member_move_assignment_sN)
{
  vector_space<lluint,3,scalar_addition,scalar_multiplication> msn;
  msn.reshape(vector_space<lluint,1,scalar_addition,scalar_multiplication>(3,100));//1000 = 1,000,000,1000 elements, 100 = 1,000,000
  auto msn_size = msn.size();
  auto msn_capacity = msn.capacity();
  sn = std::move(msn);
  ASSERT_EQ(sn.size(),msn_size);
  ASSERT_EQ(sn.capacity(),msn_capacity);
  ASSERT_EQ(msn.size(),0);
  ASSERT_EQ(msn.capacity(),0);
}

/*
 * Test ilist assignment
 * */
TEST_F(vector_space_test,test_member_ilist_assignment_sN)
{
  auto sn_capacity = sn.capacity();
  sn = {1,2,3,4,5};
  EXPECT_TRUE(sn.capacity() > 5);
  ASSERT_EQ(sn.size(),5);
  ASSERT_EQ(sn_capacity,sn.capacity());
}

TEST_F(vector_space_test,test_sN_alt_alloc)
{
  TestAllocator<lluint> a;
  vector_space<lluint,3,std::plus,std::multiplies,TestAllocator<lluint>> sn_alt_alloc(a);
  ASSERT_EQ(sn_alt_alloc.size(),0);
  ASSERT_EQ(sn_alt_alloc.capacity(),0);
  ASSERT_EQ(sn_alt_alloc.empty(),1);
  ASSERT_EQ(sn_alt_alloc.data(),nullptr);
  ASSERT_EQ(std::addressof(*sn_alt_alloc.begin()),std::addressof(*sn_alt_alloc.end() ) );
  ASSERT_EQ(std::addressof(*sn_alt_alloc.cbegin()),std::addressof(*sn_alt_alloc.cend() ) );
}
/*
 * vs1 Ctor Test
 * */
TEST_F(vector_space_test,test_v1_ctor)
{
  vector_space<lluint,1>v1{1,2,3};
  vector_space<lluint,3> vs3(sn.dimensions());
  ASSERT_EQ(vs3.capacity(),sn.capacity());
}
/*
 * Varaidic Ctor Test
 * size() == 0
 * capacity() product of args
 * empty() ret 1
 * data()!=nullptr
 * also check constant amortized growth i.e. space is doubling in size
 **/
TEST_F(vector_space_test,test_variadic_ctor)
{
  vector_space<lluint,3> snn_lval(7,7,7);
  ASSERT_EQ(snn_lval.size(),343);
  ASSERT_EQ(snn_lval.capacity(),343);
  ASSERT_EQ(snn_lval.empty(),0);
  ASSERT_NE(snn_lval.data(),nullptr);
  //ASSERT_PRED1(constant_amortized_growth_pred<3>,snn_lval);
  unsigned x = 7*7*7*7;
  for(unsigned i =0; i< x;++i)
	snn_lval.push_back(x);
  x*=49*7;
  for(unsigned i =0; i< x;++i)
	snn_lval.emplace_back(i);
  ASSERT_NE(std::addressof( *snn_lval.begin()),std::addressof( *snn_lval.end()) );
  ASSERT_NE(std::addressof( *snn_lval.cbegin()),std::addressof( *snn_lval.cend()) );
  unsigned a = 4, b = 5 ,c = 6, d = 8;
  vector_space<lluint,4> s4(a,b,c,d);
  x = s4.capacity();
  for(unsigned i =0; i< x;++i)
	snn_lval.emplace_back(i);
  
  ASSERT_NE(snn_lval.size(),0);
  ASSERT_NE(snn_lval.capacity(),0);
  ASSERT_EQ(snn_lval.empty(),0);
  ASSERT_NE(snn_lval.data(),nullptr);
}

/*
 * Initialize N Vals :val Ctor Test
 * size(),capacity() ret val != 0
 * empty() ret 0
 * data()!=nullptr
 * also check constant amortized growth i.e. capacity doubles when size > capacity
 **/

/*
 * Testing copy ctor
 * */
TEST_F(vector_space_test,test_sn_copy_ctor)
{
  vector_space<lluint,3,scalar_addition,scalar_multiplication> sn_copy_ctor(sn);
  ASSERT_EQ(sn.size(),sn_copy_ctor.size());
  ASSERT_EQ(sn.empty(),sn_copy_ctor.empty());
  for(unsigned i =0; i<sn.size();++i)
  {
	ASSERT_EQ(sn.data()[i],sn_copy_ctor.data()[i]);
  }
  for(unsigned i =0; i< sn.size()*10000;++i)
  {
	sn_copy_ctor.emplace_back(i);
  }

}

/*
 * Testing move ctor
 * */
TEST_F(vector_space_test,test__sn_move_ctor)
{
  auto sn_size = sn.size();
  auto sn_capacity = sn.capacity();
  auto sn_data = sn.data();
  vector_space<lluint,3,scalar_addition,scalar_multiplication> sn_move_ctor(std::move(sn));
  ASSERT_EQ(sn_size,sn_move_ctor.size());
  ASSERT_EQ(sn_capacity,sn_move_ctor.capacity());
  for(unsigned i = 0; i<sn_move_ctor.size();++i)
  {
	ASSERT_EQ(sn_data[i],sn_move_ctor.data()[i] );
  }
  ASSERT_TRUE(sn.empty()==true);
  sn_move_ctor.assign(100000,456);
}

/*
TEST_F(vector_space_test,test_sn_ilist_ctor)
{
  
  vector_space<lluint,3> snn_lval({1,2,3,4,5});
  ASSERT_NE(snn_lval.size(),0);
  ASSERT_NE(snn_lval.capacity(),0);
  ASSERT_EQ(snn_lval.empty(),0);
  ASSERT_NE(snn_lval.data(),nullptr);
  std::pair<lluint,lluint> params(0,1);
  //ASSERT_PRED1(constant_amortized_growth_pred<lluint,1>,snn_lval);
  ASSERT_NE(std::addressof( *snn_lval.begin()),std::addressof( *snn_lval.end()) );
  ASSERT_NE(std::addressof( *snn_lval.cbegin()),std::addressof( *snn_lval.cend()) );
}*/


/*
 * Testing range ctor
 * */
TEST_F(vector_space_test,test_sn_rng_ctor)
{
  sn.reshape(4,4,4);
  sn.assign(64,20);
  std::mt19937 mt;
  std::uniform_int_distribution<unsigned> dist(0,sn.size());
  vector_space<lluint,3,scalar_addition,scalar_multiplication> sn_rng_ctor(sn.begin(),sn.begin()+dist(mt),sn.dimensions());
  for(unsigned i=0; i<sn_rng_ctor.size();++i)
  {
	ASSERT_EQ( *(sn_rng_ctor.begin() + i), *(sn.begin() + i) );
  }

}
/*
 * Test Scalar Addition
 * lhs is modified, rhs is not
 */
TEST_F(vector_space_test,test_scalar_addition_sN)
{
  vector_space<lluint,3,scalar_addition,scalar_multiplication> vs1(3,3,3),vs2(5,5,5);
  vs1.assign(27,2);
  vs2.assign(125,3);
  auto old_vs2_size = vs2.size();
  auto old_vs2_capacity = vs2.capacity();
  sn = vs1 + vs2;
  for(lluint i = 0; i<sn.size();++i)
  {
	ASSERT_EQ( *(sn.begin() + i), *(vs1.begin() + i)   +  *(vs2.begin() + i) );
  }
  ASSERT_EQ(old_vs2_size,vs2.size());
  ASSERT_EQ(old_vs2_capacity,vs2.capacity());
}
/*
 * Test Scalar Multiplication
 * lhs is modified, rhs is not
 */
TEST_F(vector_space_test,test_scalar_multiplication_sN)
{
  sn.clear(); 
  vector_space<lluint,3,scalar_addition,scalar_multiplication> vs1(3,3,3),vs2(5,5,5);
  vs1.assign(27,2);
  vs2.assign(125,3);
  auto old_vs2_size = vs2.size();
  auto old_vs2_capacity = vs2.capacity();
  sn = vs1 + vs2;
  for(lluint i = 0; i<sn.size();++i)
  {
	ASSERT_EQ( *(sn.begin() + i), *(vs1.begin() + i)   +  *(vs2.begin() + i) );
  }
  ASSERT_EQ(old_vs2_size,vs2.size());
  ASSERT_EQ(old_vs2_capacity,vs2.capacity());
}
/*
 * Test Scalar Addition +=
 * lhs is modified, rhs is not
 */
TEST_F(vector_space_test,test_scalar_addition_plus_equal_sN)
{
  vector_space<lluint,2,scalar_addition> vs1(2,3),vs2(2,3),vs3(2,3);
  vs1.assign(6,40);
  vs2.assign(6,50);
  auto old_vs2_size = vs2.size();
  auto old_vs2_capacity = vs2.capacity();
  vs3 += vs1 + vs2;
  for(lluint i = 0; i<vs1.dimensions()[0];++i)
  {
	for(lluint j = 0; j < vs1.dimensions()[1];++j)
	{
	  ASSERT_EQ(vs1[i][j]+vs2[i][j],vs3[i][j]);
	}
  }
  auto old_vs3 = vs3;
  vs3 += old_vs3;
  for(lluint i = 0; i<vs3.dimensions()[0];++i)
  {
	for(lluint j = 0; j < vs3.dimensions()[1]; ++j)
	{
	  ASSERT_EQ(vs3[i][j], old_vs3[i][j] + old_vs3[i][j]);
	}
  }
  ASSERT_EQ(old_vs2_size,vs2.size());
  ASSERT_EQ(old_vs2_capacity,vs2.capacity());
}
/*
 * Test Scalar Addition *=
 * lhs is modified, rhs is not
 */
TEST_F(vector_space_test,test_scalar_addition_times_equal_sN)
{
  vector_space<lluint,2,null_functor,scalar_multiplication> vs1(4,5),vs2(4,5),vs3(4,5);
  vs1.assign(3,2);
  vs2.assign(3,3);
  auto old_vs2_size = vs2.size();
  auto old_vs2_capacity = vs2.capacity();
  vs3 *= vs1 * vs2;
  for(lluint i = 0; i<vs1.dimensions()[0];++i)
  {
	for(lluint j = 0; j < vs1.dimensions()[1];++j)
	{
	  ASSERT_EQ(0,vs3[i][j]);
	}
  }
  std::fill(vs3.begin(),vs3.end(),2);
  auto old_vs3 = vs3;
  vs3 *= old_vs3;
  for(lluint i = 0; i<vs3.dimensions()[0];++i)
  {
	for(lluint j = 0; j < vs3.dimensions()[1]; ++j)
	{
	  ASSERT_EQ(vs3[i][j], old_vs3[i][j] * old_vs3[i][j]);
	}
  }
  ASSERT_EQ(old_vs2_size,vs2.size());
  ASSERT_EQ(old_vs2_capacity,vs2.capacity());
}






/*
 * Test Matrix Addition
 * Note default of vector_space is row_major
 * */
TEST_F(vector_space_test,test_matrix_addition)
{
  vector_space<lluint,2,matrix_sum> vs1(2,2),vs2(2,2),vs3;
  std::mt19937 mt;
  std::uniform_int_distribution<int> dist(0,40);
  auto capacity = vs1.capacity();
  for(lluint i = 0; i<capacity;++i)
  {
	vs1.emplace_back(dist(mt));
	vs2.emplace_back(dist(mt));
  }
  vs3 = vs1 + vs2;
  for(lluint i = 0; i < vs3.dimensions()[0]; ++i)
  {
	for(lluint j =0; j < vs3.dimensions()[1];++j)
	{
	  ASSERT_EQ(vs3[i][j],vs1[i][j] + vs2[i][j]);
	}
  }
  
}
/*
 * Test Matrix Multiplication
 */
TEST_F(vector_space_test,test_matrix_multiplication)
{
  std::mt19937 mt;
  std::uniform_int_distribution<int> dist(0,40);
  vector_space<lluint,4,matrix_sum,matrix_product> vs4(2,2,5,7),vs5(2,2,7,5),vs6;
  auto end = vs4.capacity() ;
  for(lluint i = 0; i < end;++i)
  {
	vs4.emplace_back(dist(mt));
  }
  end = vs5.capacity();
  for(lluint i = 0; i < end;++i)
  {
	vs5.emplace_back(dist(mt));
  }
  vs6 = vs4 * vs5; 
  typedef vector_space<lluint,4,matrix_sum,matrix_product>::size_type stype;
  stype lhs_dim_size = vs4.dimensions().size();
  stype rhs_dim_size = vs5.dimensions().size();
  stype lhs_row = vs4.dimensions()[lhs_dim_size - 2];// M
  stype lhs_col = vs4.dimensions()[lhs_dim_size - 1];// N
  stype rhs_row = vs5.dimensions()[rhs_dim_size - 2];// N
  stype rhs_col = vs5.dimensions()[rhs_dim_size - 1];// P
  decltype(vs4.dimensions()) dim(vs4.dimensions());
  auto new_capacity = std::accumulate(dim.begin(),dim.end()-2,stype(1),std::multiplies<lluint>());
  ASSERT_EQ(new_capacity * lhs_row * rhs_col,vs6.capacity());
  stype lhs_start = 0,lhs_size = lhs_row * lhs_col, rhs_start = 0,rhs_size = rhs_row * rhs_col ,stride = 1;
  lluint count =0;
  for(stype i = 0; i < new_capacity;++i,lhs_start += lhs_row * lhs_col,rhs_start += rhs_row * rhs_col)
  {
	decltype(vs4.slice(lhs_start,lhs_size,stride)) lhs_slice = vs4.slice(lhs_start,lhs_size,stride );
	decltype(vs5.slice(rhs_start,rhs_size,stride)) rhs_slice = vs5.slice(rhs_start,rhs_size,stride);
	vector_space<lluint,2,matrix_sum,matrix_product> lhs(lhs_slice.begin(),lhs_slice.end(),{lhs_row,lhs_col});
	vector_space<lluint,2,matrix_sum,matrix_product> rhs(rhs_slice.begin(),rhs_slice.end(),{rhs_row,rhs_col});
	for(stype i =0; i < lhs_row;++i)
	{
	  for(stype j = 0; j < rhs_col;++j)
	  {
		lluint sum = 0;
		for(stype k = 0; k < rhs_row;++k)
		{
		  sum += lhs[i][k] * rhs[k][j];
		}
		ASSERT_EQ(sum, *(vs6.begin()+count) );
		++count;
	  }
	}
  }/**/
/*  std::cout<<"vs4\n";
  std::cout<<vs4;
  std::cout<<"vs5\n";
  std::cout<<vs5;
  std::cout<<"vs6\n";
  std::cout<<vs6;*/
}
TEST_F(vector_space_test,hadamard_product)
{
  hadamard_product<vector_space<lluint,2>> h;
  vector_space<lluint, 2> v1(2,2);
  vector_space<lluint, 2> v2(2,2);
  vector_space<lluint, 2> v3;
  ASSERT_EQ(v1.dimensions()[0],v2.dimensions()[0]);
  ASSERT_EQ(v1.dimensions()[1],v2.dimensions()[1]);
  for(lluint i = 0; i< v1.dimensions()[0];++i)
  {
	for(lluint j = 0; j < v1.dimensions()[1];++j)
	{
	  v1[i][j] = i * j;
	  v2[i][j] = i * j + 1;
	}
	
  }
  v3 = h(v1,v2);
  ASSERT_EQ(v1.dimensions()[0],v3.dimensions()[0]);
  ASSERT_EQ(v1.dimensions()[1],v3.dimensions()[1]);
  for(lluint i = 0; i< v1.dimensions()[0];++i)
  {
	for(lluint j = 0; j < v1.dimensions()[1];++j)
	{
	  ASSERT_EQ(v3[i][j], v1[i][j] *  v2[i][j]);
	}
	
  }
 // std::cout<<"v1:\n"<<v1<<"\n";
 // std::cout<<"v2:\n"<<v2<<"\n";
  //std::cout<<"v3:\n"<<v3<<"\n";
}
#endif

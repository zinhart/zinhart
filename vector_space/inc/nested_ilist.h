#ifndef NESTEDILIST_H
#define NESTEDILIST_H
#include "./declarations.h"
#include <algorithm>
namespace zinhart
{
  template<class T, lluint First,lluint ...Rest>
	struct nested_ilist_contents<T,First,Rest...> 
	{
	  using nilist = nested_ilist<T, 1 + sizeof ...(Rest)>;
	  template<class Function>
		static void init(nilist elements,char &a,vector_space<T,1> & vs,Function f)
		{
		  if(vs.size() < elements.size())
		  {
			vs.push_back(First);
		  }
		  if(elements.size() > First)
			throw std::logic_error("invalid argument size");
		  std::for_each(elements.begin(),elements.end(),[&a,&vs,f](T & val){nested_ilist_contents<T,Rest...>::init(val,a,vs,f);});
		  /*if(elements.size() < First)
		  {
			lluint pi;
			for(;pi>0;--pi)
			  f(T());
		  }*/
		}
	};
  template<typename T,lluint Last>
	struct nested_ilist_contents<T,Last>
	{
	  using nilist = nested_ilist<T, 1>;
	  template<class Function>
		static void init(nilist elements,char &a,vector_space<T,1> & vs,Function f)
		{
		  if(a < 1)
		  {
			vs.push_back(Last);
			++a;
		  }
		  if(elements.size() > Last)
			throw std::logic_error("invalid argument size");
		  std::for_each(elements.begin(),elements.end(),f);
		  if(elements.size() < Last)
		  {
			lluint zero_elements = Last - elements.size();
			for(; zero_elements>0; --zero_elements)
			  f(T());
		  }
		}
	};

}
#endif

#include "kernel.h"

#include <iostream>

double testfun(int a, int b)
{
	return (double)(a+b);
}

int main()
{

	// vm interface between languages
	// vm objects can convert into managed C++ for .NET, v8 for Node.js and 
	// Objective C
	
	// primitives are SIMD optimized?

	// aw::Application : public aw::Object()
	// aw::Window : public aw::Object()

	// Appware primitives
	// aw::Object();
	// aw::Value();
	// aw::Number();
	// aw::Array();
	// aw::Function();


	// object.setMethod(aw::Function<double,int,int>(testfun));
	// object.setProperty<aw::Number>(number);
	


	auto function = aw::Function<double, int, int>(testfun);

	auto object = aw::Object();
	object.set("test",function);

	// object.cast_Obj_C()
	// object.cast_v8()

	
	
	return 0;
}
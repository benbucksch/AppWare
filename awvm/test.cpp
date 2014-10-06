#include "appware.h"

#include <iostream>

using namespace aw;



ClassTemplate exportRect()
{
	ClassTemplate Rect = ClassTemplate();
	Rect.defProperty("origin");
	Rect.defProperty("size");
	// .setMethod(Function<>())
		// .setConstructor(Function<>())
		// .setProperty(Value())
		// .setMethod(Function<>())

	return Rect;
}



int main()
{
	ClassTemplate Rect = exportRect();


	Object rect = Rect.init(Arguments());


	auto object = Object();

	object["test1"] = Function();
	object["test2"] = Function();
	





	// Handle<Object> rect = Rect.init( Arguments() );

	// Window properties
	// Window::frame
	// Window::geometry
	// Window::size
		// Object::width
		// Object::height
	// Window::maximized
	// Window::position
		// Object::top
		// Object::left
	// Window::visible
	// Window::baseSize
	// Window::fullScreen
	// Window::focus
	// Window::children

	// Window methods
	// Window::init( Rect rect )
	// Window::acceptDrops( bool active )
	// Window::resize( int witdth, int height )
	// Window::setFocus( bool active )
	// Window::show()

	

	
	
	// Window.setConstructor("",Function<>());


	// Virtual<Object> exportWindow ()
	// {
	// 	ClassTemplate Window = ClassTemplate();
	// 	// Window.setConstructor("",Function<>());
	// }

	// Virtual<Object> window = Window.init( Arguments() );



	// Object::init()

	// (*persists)->

	// window.set("")

	// window::init()



	
	return 0;
}
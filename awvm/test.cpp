#include "appware.h"

#include <iostream>

using namespace aw;



ClassTemplate exportRect()
{
	ClassTemplate Rect = ClassTemplate();
	Rect.defProperty("origin");
	Rect.defProperty("size");
	return Rect;
}

ClassTemplate exportWindow()
{
	ClassTemplate Window = ClassTemplate();
	// Window.defProperty("origin");
	// Window.defProperty("size");
	// Window.defConstructor(Function());

	// Window.defMethod("name",FunctionWrap());
	// Window.defMethod("name",FunctionWrap());
	// Window.defMethod("name",FunctionWrap());
	return Window;
}



int main()
{

	auto function = Function<void()>();

	// ClassTemplate Rect = exportRect();
	// ClassTemplate Window = exportWindow();

	// // TODO
	// // ClassTemplate Window = WindowTemplate::export();

	// Object rect = Rect.create();
	// Object window = Window.create(Arguments({rect}));

	


	// auto object = Object();

	// object["test1"] = Function();
	// object["test2"] = Function();
	





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
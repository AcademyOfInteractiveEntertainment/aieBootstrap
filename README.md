# aieBootstrap - NOT FINAL
The base project code used for the Advanced Diploma of Professional Game Development to aid in graphical real-time applications.

Bootstrap is a static library that can be linked against to implement real-time graphical applications with OpenGL as the base API.

To create an application simply derive from the ```aie::Application``` class like so:
```c++
class MyApplication : public aie::Application {
public:

	MyApplication();
	virtual ~MyApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

};
```
Then implement a ```main()``` function to create an instance of your application:
```c++
int main() {
	
	auto app = new MyApplication();
	// window title, dimensions and if it is fullscreen
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}
```
The ```run()``` method implements a game loop that executes in the following manner:
```
  if startup() is true then
    while quit is false
      update()
      draw()
  shutdown()
```
Provided with Bootstrap is an example of a project creating an application to implement 2D tasks, and another that implements a 3D environment. Both are near identical and just make use of different provided classes.

Also provided is a VS2015 project template that can create a basic 2D project, and another for a 3D project.

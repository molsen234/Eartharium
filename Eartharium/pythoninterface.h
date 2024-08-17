#pragma once

// For the Python interface. Split into separate file if possible.

class Application;

class PyIface {
public:
	void runscript(std::string& pythonscript);
	static Application* getApplication();
};
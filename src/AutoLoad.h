/*
 * AutoLoad
 * ========
 *
 * AutoGen can compile code and load the resulting dynamic library at runtime.
 *
 * Usage:
 * ```
 * if(!buildLibrary(code, libName, error)) {
 *		compute_extern* fnc = loadLibrary(libName);
 *		fnc(x, y); // use function!
 * }
 * ```
 *
 * Currently this only works with g++ and on Linux. However, this should also
 * be possible on Windows and with the Visual compiler.
 *
 * Some online references:
 * http://tldp.org/HOWTO/Program-Library-HOWTO/dl-libraries.html
 *
 */

#pragma once

#include <dlfcn.h>
#include <fstream>
#include <cstdlib>
#include <chrono>

#include "execCmd.h"

namespace AutoGen {

// general function for code generator that is loaded during runtime
//                            in     out
typedef void compute_extern(double*,double*);

// general function for code generator that is loaded during runtime
//                            in     out
typedef void compute_extern2(double*,double*,double*);

bool buildLibrary(const std::string &code, const std::string &libName, std::string &error) {

	// make dir
	std::string out;
	if(exec("mkdir -p " + libName, out) != 0){
		error = "Could not create directory: '" + libName + "'.";
		return false;
	}

	// create cpp file
	std::ofstream cppFile(libName+"/"+libName+".cpp");
	cppFile << code;
	cppFile.close();

#if defined(__clang__)
	std::cout << "Using `clang++` to compile '" << libName << "'." << std::endl;
	std::string compile_cmd = "clang++ -fPIC -std=c++14 -shared -o "+libName+"/lib"+libName+".so "+libName+"/"+libName+".cpp";
#elif defined(__GNUC__) || defined(__GNUG__)
	std::cout << "Using `g++` to compile '" << libName << "'." << std::endl;
	std::string compile_cmd = "g++ -fPIC -I" AUTOGEN_SRC_DIR " -shared -o "+libName+"/lib"+libName+".so "+libName+"/"+libName+".cpp";
#else
	std::cout << "Using `cmake --build` to compile '" << libName << "'." << std::endl;
	// create CMakeLists.txt
	std::ofstream cmakeFile(libName+"/CMakeLists.txt");
	cmakeFile <<
				 "cmake_minimum_required(VERSION 3.5 FATAL_ERROR)\n"
				 "project(" << libName << ")\n"
				 "include_directories(" AUTOGEN_SRC_DIR ")\n"
				 "file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.cpp)\n"
				 "add_library(${PROJECT_NAME} SHARED ${sources})";
	cmakeFile.close();

	std::string compile_cmd = "cmake "+libName+"/CMakeLists.txt && cmake --build "+libName;
#endif

	if(exec(compile_cmd, out) != 0){
		error += "Failed to compile '" + libName + "'.";
		return false;
	}

	return true;
}

template<class F>
bool loadLibrary(std::string libName, F* (&fncPtr), std::string fncName = "compute_extern") {
	// load the library
	void* libCompute = dlopen((libName+"/lib"+ libName + ".so").c_str(), RTLD_LAZY);
	if (!libCompute) {
		std::cerr << "Cannot load library: " << dlerror() << '\n';
		return false;
	}

	// reset errors
	dlerror();

	// load the symbols
	fncPtr = (F*) dlsym(libCompute, fncName.c_str());
	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Cannot load symbol create: " << dlsym_error << '\n';
		return false;
	}

	return true;
}

std::string getPseudoUniqueLibName(std::string name = "") {
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
	std::srand(ms.count()); // use current time as seed for random generator
	int random_variable = std::rand();
	return "lib_"+name+"_"+std::to_string(random_variable);
}

template<class F>
bool buildAndLoad(const std::string &code, F* (&fnc), const std::string &name, const std::string &fncname, std::string &error) {

	std::string libName = getPseudoUniqueLibName(name);

	if(!buildLibrary(code, libName, error))
		return false;

	if(!loadLibrary(libName, fnc, fncname))
		return false;

	return true;
}

template<class F>
bool buildAndLoad(const std::string &code, F* (&fnc), std::string &error) {
	return buildAndLoad(code, fnc, "", "compute_extern", error);
}

}


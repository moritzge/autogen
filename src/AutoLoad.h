/*
 * AutoLoad
 * ========
 *
 * AutoGen can compile code and load the resulting dynamic library at runtime.
 *
 *
 * Currently this only works with g++ and on Linux. However, this should also
 * be possible on Windows and with the Visual compiler.
 *
 * Some online references:
 * http://tldp.org/HOWTO/Program-Library-HOWTO/dl-libraries.html
 *
 */

#include "execCmd.h"
#include <dlfcn.h>

namespace CodeGen {

// general function for code generator that is loaded during runtime
//                            in     out
typedef void compute_extern(double*,double*);

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
	std::string compile_cmd = "clang++ -fPIC -shared -o "+libName+"/lib"+libName+".so "+libName+"/"+libName+".cpp";
#elif defined(__GNUC__) || defined(__GNUG__)
	std::cout << "Using `g++` to compile '" << libName << "'." << std::endl;
	std::string compile_cmd = "g++ -fPIC -shared -o "+libName+"/lib"+libName+".so "+libName+"/"+libName+".cpp";
#else
	std::cout << "Using `cmake --build` to compile '" << libName << "'." << std::endl;
	// create CMakeLists.txt
	std::ofstream cmakeFile(libName+"/CMakeLists.txt");
	cmakeFile <<
				 "cmake_minimum_required(VERSION 3.5 FATAL_ERROR)\n"
				 "project(" << libName << ")\n"
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

compute_extern* loadLibrary(std::string libName) {
	// load the library
	void* libCompute = dlopen((libName+"/lib"+ libName + ".so").c_str(), RTLD_LAZY);
	if (!libCompute) {
		std::cerr << "Cannot load library: " << dlerror() << '\n';
	}

	// reset errors
	dlerror();

	// load the symbols
	compute_extern* comp_y = (compute_extern*) dlsym(libCompute, "compute_extern");
	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Cannot load symbol create: " << dlsym_error << '\n';
	}

	return comp_y;
}

}


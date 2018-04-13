#pragma once

#include "VarDef.h"
#include "RecType.h"
#include "CodeGenerator.h"

#include <experimental/filesystem>

namespace AutoGen {

// Helper functions
template <typename A>
void addNameToList(std::vector<std::string> &typeList, A arg) {
	typeList.push_back(arg.getName());
}

template <typename A>
void addTypeToList(std::vector<std::string> &typeList, A arg) {
	typeList.push_back(arg.getGeneratedType());
}

const std::string energyName = "E";
const std::string gradName = "grad";
#define getGradType(type, size)	("Eigen::Matrix<" + type + ", " + std::to_string(size) + ", 1>")
const std::string hessName = "hess";
#define getHessType(type, size)	("Eigen::Matrix<" + type + ", " + std::to_string(size) + ", " + std::to_string(size) + ">")
const std::string jacobianName = "J";
#define getJacobianType(type, size1, size2)	("Eigen::Matrix<" + type + ", " + std::to_string(size1) + ", " + std::to_string(size2) + ">")

template<typename... A>
void writeCodeToFile(const std::string &fileName, const std::string &functionName, const std::vector<std::string> &returnTypes, const std::vector<std::string> &returnNames, const std::string &code, A&&... a)
{
	// Create Directory
	std::string folderName = "./GeneratedCode";
	std::experimental::filesystem::create_directories(folderName);

	// Create file
	std::ofstream file;
	file.open(folderName + "/" + fileName + "_AutoGen_" + functionName + ".h");

	// Fetch argument names and types
	std::vector<std::string> nameList;
	auto tmpname = { (addNameToList(nameList, a),0)... };
	std::vector<std::string> typeList;
	auto tmptype = { (addTypeToList(typeList, a),0)... };

	// Namespace
	file << "namespace " << fileName << "_AutoGen{\n";
	// Write function name
	file << "void " << functionName << "(";
	// Write function arguments
	for (size_t i = 0; i < nameList.size(); i++)
	{
		file << "const " << typeList[i] << " &" << nameList[i];
		if (i< nameList.size() - 1)
			file << ", ";
	}
	// Write function return type and name
	for (size_t i = 0; i < returnTypes.size(); i++)
	{
		if (i == 0)
			file << ", ";
		file << returnTypes[i] << " &" << returnNames[i];
		if (i< returnTypes.size() - 1)
			file << ", ";
	}
	// Finish function signature and write generted code
	file << ")\n{\n";
	file << code;
	file << "}\n";
	// close namespace
	file << "}\n";

	// Close file
	file.close();
}

template<typename... A>
void writeCodeToFile(const std::string &fileName, const std::string &functionName, std::string &returnType, const std::string &returnName,  const std::string &code, A&&... a)
{
	std::vector<std::string> returnTypes, returnNames;
	returnTypes.push_back(returnType);
	returnNames.push_back(returnName);
	writeCodeToFile(fileName, functionName, returnTypes, returnNames, code, a...);
}

template<typename F, typename... A>
std::string generateEnergyCodeADR(F &&f, A&&... a)
{
	ADR energy = std::forward<F>(f)(std::forward<A>(a)...);
	RecType<double> E = energy.value();
	CodeGenerator<double> generator;
	E.addToGeneratorAsResult(generator, energyName);
	generator.sortNodes();
	return generator.generateCode();
}

// TODO: duplicate code as function above --> remove
template<typename F, typename... A>
std::string generateEnergyCodeADDR(F &&f, A&&... a)
{
	ADDR energy = std::forward<F>(f)(std::forward<A>(a)...);
	RecType<double> E = energy.value().value();
	CodeGenerator<double> generator;
	E.addToGeneratorAsResult(generator, energyName);
	generator.sortNodes();
	return generator.generateCode();
}

template<typename F, typename... A>
std::string generateGradientCode(const VarListX<ADR> &variables, F &&f, A&&... a)
{
	CodeGenerator<double> generator;
	for (int i = 0; i < variables.size(); i++)
	{
		(*variables[i]).deriv() = 1;
		ADR energy = std::forward<F>(f)(std::forward<A>(a)...);
		RecType<double> grad = energy.deriv();
		grad.addToGeneratorAsResult(generator, "grad[" + std::to_string(i) + "]");
		(*variables[i]).deriv() = 0;
	}

	generator.sortNodes();
	return generator.generateCode();
}

template<typename F, typename... A>
std::string generateGradientCode(const VarListX<ADDR> &variables, F &&f, A&&... a)
{
	CodeGenerator<double> generator;
	for (int i = 0; i < variables.size(); i++)
	{
		(*variables[i]).deriv() = 1;
		ADDR energy = std::forward<F>(f)(std::forward<A>(a)...);
		RecType<double> grad = energy.deriv().value();
		grad.addToGeneratorAsResult(generator, gradName + "[" + std::to_string(i) + "]");
		(*variables[i]).deriv() = 0;
	}

	generator.sortNodes();
	return generator.generateCode();
}

template<typename F, typename... A>
std::string generateGradientAndHessianCode(const VarListX<ADDR> &variables, F &&f, A&&... a)
{
	CodeGenerator<double> generator;

	for (int i = 0; i < variables.size(); i++)
	{
		(*variables[i]).deriv() = 1;

		ADDR energy = std::forward<F>(f)(std::forward<A>(a)...);
		RecType<double> grad = energy.deriv().value();
		grad.addToGeneratorAsResult(generator, gradName + "[" + std::to_string(i) + "]");

		for (int j = 0; j < 3; j++)
		{
			(*variables[j]).value().deriv() = 1;
			ADDR energy = std::forward<F>(f)(std::forward<A>(a)...);
			RecType<double> hess = energy.deriv().deriv();
			hess.addToGeneratorAsResult(generator, hessName + "(" + std::to_string(i) + ", " + std::to_string(j) + ")");
			(*variables[j]).value().deriv() = 0;
		}
		(*variables[i]).deriv() = 0;
	}

	generator.sortNodes();
	return generator.generateCode();
}

template<typename F, typename... A>
std::string generateHessianCode(const VarListX<ADDR> &variables, F &&f, A&&... a)
{
	CodeGenerator<double> generator;

	for (int i = 0; i < variables.size(); i++)
	{
		(*variables[i]).deriv() = 1;
		for (int j = 0; j < variables.size(); j++)
		{
			(*variables[j]).value().deriv() = 1;
			ADDR energy = std::forward<F>(f)(std::forward<A>(a)...);
			RecType<double> hess = energy.deriv().deriv();
			hess.addToGeneratorAsResult(generator, hessName + "(" + std::to_string(i) + ", " + std::to_string(j) + ")");
			(*variables[j]).value().deriv() = 0;
		}
		(*variables[i]).deriv() = 0;
	}

	generator.sortNodes();
	return generator.generateCode();
}

template<typename F, typename... A>
std::string generateJacobianCode(const VarListX<ADDR> &firstVariables, const VarListX<ADDR> &secondVariables, F &&f, A&&... a)
{
	CodeGenerator<double> generator;

	for (int i = 0; i < firstVariables.size(); i++)
	{
		(*firstVariables[i]).deriv() = 1;
		for (int j = 0; j < secondVariables.size(); j++)
		{
			(*secondVariables[j]).value().deriv() = 1;
			ADDR energy = std::forward<F>(f)(std::forward<A>(a)...);
			RecType<double> J = energy.deriv().deriv();
			J.addToGeneratorAsResult(generator, jacobianName + "(" + std::to_string(i) + ", " + std::to_string(j) + ")");
			(*secondVariables[j]).value().deriv() = 0;
		}
		(*firstVariables[i]).deriv() = 0;
	}

	generator.sortNodes();
	return generator.generateCode();
}

template<typename F, typename... A>
void printEnergyCodeADR(const std::string &fileName, F &&f, A&&... a)
{
	std::string code = generateEnergyCodeADR(f, std::forward<A>(a)...);

	std::string type = std::forward<F>(f)(std::forward<A>(a)...).getGeneratedType();

	writeCodeToFile(fileName, "compute_E", type, energyName, code, std::forward<A>(a)...);
}

template<typename F, typename... A>
void printEnergyCodeADR_customName(const std::string &fileName, const std::string &customEnergyName, F &&f, A&&... a)
{
	std::string code = generateEnergyCodeADR(f, std::forward<A>(a)...);

	std::string type = std::forward<F>(f)(std::forward<A>(a)...).getGeneratedType();

	writeCodeToFile(fileName, "compute_" + customEnergyName, type, energyName, code, std::forward<A>(a)...);
}

template<typename F, typename... A>
void printGradientCode(const std::string &fileName, VarListX<ADR> &variables, F &&f, A&&... a)
{
	std::string code = generateGradientCode(variables, f, std::forward<A>(a)...);

	std::string type = variables.getInnerType();

	std::string gradType = getGradType(type, variables.size());

	writeCodeToFile(fileName, "compute_dE_dx", gradType, gradName, code, std::forward<A>(a)...);
}

template<typename F, typename... A>
void printGradientCode_customName(const std::string &fileName, const std::string &customEnergyName, VarListX<ADR> &variables, const std::string &variablesName,F &&f, A&&... a)
{
	std::string code = generateGradientCode(variables, f, std::forward<A>(a)...);

	std::string type = variables.getInnerType();

	std::string gradType = getGradType(type, variables.size());

	writeCodeToFile(fileName, "compute_d" + customEnergyName + "_d" + variablesName, gradType, gradName, code, std::forward<A>(a)...);
}

template<typename F, typename... A>
void printEnergyCodeADDR(const std::string &fileName, F &&f, A&&... a)
{
	std::string code = generateEnergyCodeADDR(f, std::forward<A>(a)...);

	std::string type = std::forward<F>(f)(std::forward<A>(a)...).getGeneratedType();

	writeCodeToFile(fileName, "compute_E", type, energyName, code, std::forward<A>(a)...);
}

template<typename F, typename... A>
void printGradientCode(const std::string &fileName, VarListX<ADDR> &variables, F &&f, A&&... a)
{
	std::string code = generateGradientCode(variables, f, std::forward<A>(a)...);

	std::string type = variables.getInnerType();

	std::string gradType = getGradType(type, variables.size());

	writeCodeToFile(fileName, "compute_dE_dx", gradType, gradName, code, std::forward<A>(a)...);
}

template<typename F, typename... A>
void printHessianCode(const std::string &fileName, VarListX<ADDR> &variables, F &&f, A&&... a)
{
	std::string code = generateHessianCode(variables, f, std::forward<A>(a)...);

	std::string type = variables.getInnerType();

	std::string hessType = getHessType(type, variables.size());

	writeCodeToFile(fileName, "compute_ddE_dxdx", hessType, hessName, code, std::forward<A>(a)...);
}

template<typename F, typename... A>
void printJacobianCode(const std::string &fileName, VarListX<ADDR> &firstVariables, VarListX<ADDR> &secondVariables, const std::string &secondVariablesName, F &&f, A&&... a)
{
	std::string code = generateJacobianCode(firstVariables, secondVariables, f, std::forward<A>(a)...);

	std::string type;
	std::string type1 = firstVariables.getInnerType();
	std::string type2 = secondVariables.getInnerType();
	assert(type1.compare(type2) == 0);
	type = type1;

	std::string jacobianType = getJacobianType(type, firstVariables.size(), secondVariables.size());

	writeCodeToFile(fileName, "compute_ddE_dxd" + secondVariablesName, jacobianType, jacobianName, code, std::forward<A>(a)...);
}

template<typename F, typename... A>
void printGradientAndHessianCode(const std::string &fileName, VarListX<ADDR> &variables, F &&f, A&&... a)
{
	std::string code = generateGradientAndHessianCode(variables, f, std::forward<A>(a)...);

	std::string type = variables.getInnerType();

	std::string gradType = getGradType(type, variables.size());
	std::string hessType = getHessType(type, variables.size());

	std::vector<std::string> outputTypes = { gradType, hessType };
	std::vector<std::string> outputNames = { gradName, hessName };

	writeCodeToFile(fileName, "compute_dE_dx_and_ddE_dxdx", outputTypes, outputNames, code, std::forward<A>(a)...);
}

}

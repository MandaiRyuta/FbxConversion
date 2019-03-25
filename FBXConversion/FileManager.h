#pragma once
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <iterator>
#include <vector>
#include <algorithm>

struct VERTEX
{
	float x;
	float y;
	float z;

	VERTEX()
	{
		x = y = z = 0.0f;
	}
};

struct COLOR
{
	float r;
	float g;
	float b;
	float a;

	COLOR()
	{
		r = g = b = a = 0.0f;
	}
};

struct UV
{
	float U;
	float V;

	UV()
	{
		U = V = 0.0f;
	}
};

struct StaticMeshModelInfomation
{
	std::vector<VERTEX> vtx;
	std::vector<int> idx;
	std::vector<COLOR> color;
	std::vector<UV> uv;
};

class OutPutModelDataFile
{
public:
	OutPutModelDataFile(std::string CreateFileName, StaticMeshModelInfomation model);
	~OutPutModelDataFile();

public:
	void CreateTextData();
private:
	void OutputVertex(std::ofstream& ofs);
	void OutputIndex(std::ofstream& ofs);
	void OutputColor(std::ofstream& ofs);
	void OutputUV(std::ofstream& ofs);
private:
	std::string filepath_;
	StaticMeshModelInfomation model_;
};

class InputModelDataFile
{
public:
	InputModelDataFile(std::string LoadFileName);
	~InputModelDataFile();

public:
	int CreateModelData();
private:
	int InputVertex(std::ifstream& ifs);
	int InputIndex(std::ifstream& ifs);
	int InputColor(std::ifstream& ifs);
	int InputUV(std::ifstream& ifs);
private:
	StaticMeshModelInfomation model_;
	std::string filepath_;
};
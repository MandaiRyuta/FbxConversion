#include "pch.h"
#include "FileManager.h"

OutPutModelDataFile::OutPutModelDataFile(std::string CreateFileName, StaticMeshModelInfomation model)
	: filepath_(CreateFileName)
	, model_(model)
{
	
}

OutPutModelDataFile::~OutPutModelDataFile()
{
}

void OutPutModelDataFile::CreateTextData()
{
	std::ofstream ofst(filepath_);

	OutputVertex(ofst);
	OutputIndex(ofst);
	OutputColor(ofst);
	OutputUV(ofst);
}

void OutPutModelDataFile::OutputVertex(std::ofstream& ofs)
{
	if (!model_.vtx.empty())
	{
		for (auto itr = model_.vtx.begin(); itr != model_.vtx.end(); itr++)
		{
			ofs << (*itr).x << std::endl;
			ofs << (*itr).y << std::endl;
			ofs << (*itr).z << std::endl;
		}
		ofs << "#" << std::endl;
	}
	else
	{
		ofs << "VertexDataが存在しません" << std::endl;
	}
}

void OutPutModelDataFile::OutputIndex(std::ofstream& ofs)
{
	if (!model_.idx.empty() > 0)
	{
		for (auto itr = model_.idx.begin(); itr != model_.idx.end(); itr++)
		{
			ofs << (*itr) << std::endl;
		}
		ofs << "##" << std::endl;
	}
	else
	{
		ofs << "IndexDataがありません" << std::endl;
	}
}

void OutPutModelDataFile::OutputColor(std::ofstream& ofs)
{
	if (!model_.color.empty())
	{
		for (auto itr = model_.color.begin(); itr != model_.color.end(); itr++)
		{
			ofs << (*itr).r << std::endl;
			ofs << (*itr).g << std::endl;
			ofs << (*itr).b << std::endl;
			ofs << (*itr).a << std::endl;
		}
		ofs << "###" << std::endl;
	}
	else
	{
		ofs << "ColorDataがありません" << std::endl;
	}
}

void OutPutModelDataFile::OutputUV(std::ofstream& ofs)
{
	if(!model_.uv.empty())
	{
		for (auto itr = model_.uv.begin(); itr != model_.uv.end(); itr++)
		{
			ofs << (*itr).U << std::endl;
			ofs << (*itr).V << std::endl;
		}
		ofs << "####" << std::endl;
	}
	else
	{
		ofs << "UVDataがありません" << std::endl;
	}
}

InputModelDataFile::InputModelDataFile(std::string LoadFileName)
	: filepath_(LoadFileName)
{
}

InputModelDataFile::~InputModelDataFile()
{
}

int InputModelDataFile::CreateModelData()
{
	std::ifstream ifst(filepath_);
	
	InputVertex(ifst);
	InputIndex(ifst);
	InputColor(ifst);
	InputUV(ifst);

	return 0;
}

int InputModelDataFile::InputVertex(std::ifstream& ifs)
{
	if (ifs.fail())
	{
		std::cerr << "データが存在しません" << std::endl;
		return -1;
	}
	VERTEX vtx;
	std::string reading_line_buffer_X;
	std::string reading_line_buffer_Y;
	std::string reading_line_buffer_Z;
	while (!ifs.eof())
	{
		std::getline(ifs, reading_line_buffer_X);
		std::getline(ifs, reading_line_buffer_Y);
		std::getline(ifs, reading_line_buffer_Z);

		if (reading_line_buffer_X == "#")
		{
			break;
		}
		else if (reading_line_buffer_Y == "#")
		{
			break;
		}
		else if (reading_line_buffer_Z == "#")
		{
			break;
		}

		if (reading_line_buffer_X != "")
		{
			vtx.x = std::stof(reading_line_buffer_X);
		}
		if (reading_line_buffer_Y != "")
		{
			vtx.y = std::stof(reading_line_buffer_Y);
		}
		if (reading_line_buffer_Z != "")
		{
			vtx.z = std::stof(reading_line_buffer_Z);
			model_.vtx.push_back(vtx);
			std::cout << "VERTEX (" << vtx.x << "," << vtx.y << "," << vtx.z << ")" << std::endl;
		}

	}
	std::cout << "VERTEX Complite" << std::endl;

	return 0;
}

int InputModelDataFile::InputIndex(std::ifstream& ifs)
{
	if (ifs.fail())
	{
		std::cerr << "データが存在しません" << std::endl;
		return -1;
	}

	int Idx;
	std::string reading_line_buffer_Idx;
	while (!ifs.eof())
	{
		std::getline(ifs, reading_line_buffer_Idx);

		if (reading_line_buffer_Idx == "##")
		{
			break;
		}

		if (reading_line_buffer_Idx != "")
		{
			Idx = std::stoi(reading_line_buffer_Idx);
			model_.idx.push_back(Idx);
			std::cout << "Index::" << Idx << std::endl;
		}

	}
	std::cout << "INDEX Complite" << std::endl;
	
	return 0;
}

int InputModelDataFile::InputColor(std::ifstream& ifs)
{
	if (ifs.fail())
	{
		std::cerr << "データが存在しません" << std::endl;
		return -1;
	}

	COLOR color;
	std::string reading_line_buffer_R, reading_line_buffer_G, reading_line_buffer_B, reading_line_buffer_A;

	while (!ifs.eof())
	{
		std::getline(ifs, reading_line_buffer_R);
		std::getline(ifs, reading_line_buffer_G);
		std::getline(ifs, reading_line_buffer_B);
		std::getline(ifs, reading_line_buffer_A);

		if (reading_line_buffer_R == "###")
		{
			break;
		}
		else if (reading_line_buffer_G == "###")
		{
			break;
		}
		else if (reading_line_buffer_B == "###")
		{
			break;
		}
		else if (reading_line_buffer_A == "###")
		{
			break;
		}

		if (reading_line_buffer_R != "")
		{
			color.r = std::stof(reading_line_buffer_R);
		}
		if (reading_line_buffer_G != "")
		{
			color.g = std::stof(reading_line_buffer_G);
		}
		if (reading_line_buffer_B != "")
		{
			color.b = std::stof(reading_line_buffer_B);
		}
		if (reading_line_buffer_A != "")
		{
			color.a = std::stof(reading_line_buffer_A);
			model_.color.push_back(color);
			std::cout << "RGBA :: (" << color.r << "," << color.g << "," << color.b << "," << color.a << std::endl;
		}
	}

	std::cout << "COLOR Complite" << std::endl;

	return 0;
}

int InputModelDataFile::InputUV(std::ifstream& ifs)
{
	if (ifs.fail())
	{
		std::cerr << "データが存在しません" << std::endl;
		return -1;
	}
	UV uv;
	std::string  reading_line_buffer_U, reading_line_buffer_V;

	while (!ifs.eof())
	{
		std::getline(ifs, reading_line_buffer_U);
		std::getline(ifs, reading_line_buffer_V);

		if (reading_line_buffer_U == "####")
		{
			break;
		}
		else if (reading_line_buffer_V == "####")
		{
			break;
		}

		if (reading_line_buffer_U != "")
		{
			uv.U = std::stof(reading_line_buffer_U);
		}
		if (reading_line_buffer_V != "")
		{
			uv.V = std::stof(reading_line_buffer_V);
			model_.uv.push_back(uv);
			std::cout << "UV:: (" << uv.U << "," << uv.V << ")" << std::endl;
		}
	}

	std::cout << "UV Complite" << std::endl;

	return 0;
}

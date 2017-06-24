#pragma once

#include <sstream>

using namespace Windows::Storage::Streams;

struct Vertex {
	float X, Y, Z;
};

struct Facet {
	Vertex normal, v1, v2, v3;
};

class StlFile sealed
{
public:
	StlFile();
	StlFile(IBuffer^ buffer, Platform::String^ p_fileName);
	std::vector<Facet> facets;

	Vertex translate;
	float scale;

	Vertex msize, mmin, mmax; 

private:
	Platform::String^ fileName;
	
	bool ReadBinary(DataReader^ datareader);
	Vertex ReadBinaryVertex(DataReader^ datareader);
	float ReadFloat(DataReader^ datareader);

	bool ReadAscii(DataReader^ datareader);
	std::string ReadAsciiLine(DataReader^ datareader);
	Vertex StlFile::ReadAsciiVertex(std::string line, int skipCount = 1);
	
	void ComputeStats();
};


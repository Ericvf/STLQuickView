#include "pch.h"
#include "StlFile.h"

#define HEADER_SIZE 84
#define JUNK_SIZE 80
#define SIZE_OF_FACET 50
#define ASCII_LINES_PER_FACET 7

#pragma region
StlFile::StlFile() {
	scale = 1;
}

StlFile::StlFile(IBuffer^ buffer, Platform::String^ p_fileName)
{
	scale = 1;
	fileName = p_fileName;

	auto datareader = DataReader::FromBuffer(buffer);
	datareader->UnicodeEncoding = UnicodeEncoding::Utf8;
	datareader->ByteOrder = ByteOrder::LittleEndian;

	auto isFileLoaded = ReadBinary(datareader);
	delete datareader;

	if (!isFileLoaded)
	{
		datareader = DataReader::FromBuffer(buffer);
		isFileLoaded = ReadAscii(datareader);
		delete datareader;
	}

	if (isFileLoaded)
		ComputeStats();
}
#pragma endregion Constructors

bool StlFile::ReadBinary(DataReader^ datareader)
{
	auto fileContentSize = datareader->UnconsumedBufferLength - HEADER_SIZE;
	auto fileSize = datareader->UnconsumedBufferLength;

	if (fileContentSize % SIZE_OF_FACET != 0)
		return false;

	auto numFacets = fileContentSize / SIZE_OF_FACET;
	for (size_t i = 0; i < JUNK_SIZE; i++)
		datareader->ReadByte();

	auto headerNumFacets = datareader->ReadUInt32();
	if (numFacets != headerNumFacets)
		return false;

	while (numFacets-- > 0) {
		Facet f;

		f.normal = ReadBinaryVertex(datareader);
		f.v1 = ReadBinaryVertex(datareader);
		f.v2 = ReadBinaryVertex(datareader);
		f.v3 = ReadBinaryVertex(datareader);
		datareader->ReadUInt16();

		facets.push_back(f);
	}

	return true;
}

Vertex StlFile::ReadBinaryVertex(DataReader^ datareader) {
	Vertex vertex;
	vertex.X = ReadFloat(datareader);
	vertex.Z = ReadFloat(datareader);
	vertex.Y = ReadFloat(datareader);
	return vertex;
}

float StlFile::ReadFloat(DataReader^ datareader) {
	union
	{
		int intValue;
		float floatValue;
	} value;

	value.intValue = datareader->ReadByte();
	value.intValue |= datareader->ReadByte() << 0x08;
	value.intValue |= datareader->ReadByte() << 0x10;
	value.intValue |= datareader->ReadByte() << 0x18;

	return value.floatValue;
}

bool StlFile::ReadAscii(DataReader^ datareader)
{
	auto line = ReadAsciiLine(datareader); // solid
	Facet f;

	std::string _;
	while (datareader->UnconsumedBufferLength > 0) {
		line = ReadAsciiLine(datareader); // facet normal || endsolid

		if (line.substr(0, 8) == "endsolid")
			break;

		std::istringstream in(line);
		in >> _; if (_ != "facet") break;
		in >> _; if (_ != "normal") break;

		f.normal = ReadAsciiVertex(line, 2);

		line = ReadAsciiLine(datareader); // outer loop
		line = ReadAsciiLine(datareader); // vertex 
		f.v1 = ReadAsciiVertex(line);

		line = ReadAsciiLine(datareader); // vertex 
		f.v2 = ReadAsciiVertex(line);

		line = ReadAsciiLine(datareader); // vertex 
		f.v3 = ReadAsciiVertex(line);

		line = ReadAsciiLine(datareader); // end loop
		line = ReadAsciiLine(datareader); // end facet

		facets.push_back(f);
	}

	return datareader->UnconsumedBufferLength == 0;
}

std::string StlFile::ReadAsciiLine(DataReader^ datareader) {
	std::string line;

	while (datareader->UnconsumedBufferLength > 0)
	{
		char c = datareader->ReadByte();
		if (c == 10)
			break;

		line += c;
	}

	return line;
}

Vertex StlFile::ReadAsciiVertex(std::string line, int skipCount) {
	std::istringstream in(line);

	for (int i = 0; i < skipCount; i++)
		in >> line;

	Vertex vertex;
	in >> vertex.X >> vertex.Z >> vertex.Y;
	return vertex;
}

void StlFile::ComputeStats()
{
	Vertex min, max;
	min.X = facets[0].v1.X;
	min.Y = facets[0].v1.Y;
	min.Z = facets[0].v1.Z;
	max.X = facets[0].v1.X;
	max.Y = facets[0].v1.Y;
	max.Z = facets[0].v1.Z;

	auto c = facets.size();
	for (int i = 0; i < c; i++)
	{
		auto facet = facets[i];
		min.X = min(min.X, facet.v1.X);
		min.X = min(min.X, facet.v2.X);
		min.X = min(min.X, facet.v3.X);
		min.Y = min(min.Y, facet.v1.Y);
		min.Y = min(min.Y, facet.v2.Y);
		min.Y = min(min.Y, facet.v3.Y);
		min.Z = min(min.Z, facet.v1.Z);
		min.Z = min(min.Z, facet.v2.Z);
		min.Z = min(min.Z, facet.v3.Z);
		max.X = max(max.X, facet.v1.X);
		max.X = max(max.X, facet.v2.X);
		max.X = max(max.X, facet.v3.X);
		max.Y = max(max.Y, facet.v1.Y);
		max.Y = max(max.Y, facet.v2.Y);
		max.Y = max(max.Y, facet.v3.Y);
		max.Z = max(max.Z, facet.v1.Z);
		max.Z = max(max.Z, facet.v2.Z);
		max.Z = max(max.Z, facet.v3.Z);
	}

	mmin = min;
	mmax = max;
	msize = Vertex{
		max.X - min.X,
		max.Y - min.Y,
		max.Z - min.Z
	};

	float globalMax = 0;
	globalMax = max(globalMax, max.X - min.X);
	globalMax = max(globalMax, max.Y - min.Y);
	globalMax = max(globalMax, max.Z - min.Z);
	scale = 1 / globalMax;

	translate.X = -(((max.X - min.X) / 2) + min.X);
	translate.Y = -(((max.Y - min.Y) / 2) + min.Y);
	translate.Z = -(((max.Z - min.Z) / 2) + min.Z);
}

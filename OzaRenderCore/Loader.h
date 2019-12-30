#pragma once
#include <string>
#include <cstdio>

const auto LOADER_INPUT_LENGTH = 1024;
class Loader
{
public:
	Loader();
	Loader(char* fName);
	~Loader();

	bool Open(char* fName);
	bool Close(void);

	bool eof(void) { return (feof(f) != 0); }
	bool ReadChar(char* c);
	bool ReadChar(char& c) { return ReadChar(&c); }
	bool PeekChar(char* c);
	bool PeekChar(char& c) { return PeekChar(&c); }
	bool ReadString(char* buffer, char* delims = NULL);
	bool ReadToken(char* token, bool trim = true, bool lcase = true);
	void PeekTokens(char* tokens, int nTokens);
	bool ReadDouble(double* d);
	bool ReadDouble(double& d) { return ReadDouble(&d); }
	bool ReadFloat(float* d);
	bool ReadFloat(float& d) { return ReadFloat(&d); }
	bool ReadInt(int* k);
	bool ReadInt(int& k) { return ReadInt(&k); }
	bool ReadCurrentLine(int& curPosInLine, char* buffer, int size = LOADER_INPUT_LENGTH);

	bool RewindChar(void);
	bool Rewind(int dist);
	bool FastForward(int dist);
	bool MoveTo(int pos);
	bool NextLine(void);
	int CurPos(void);

	bool IsWhitespace(char c);
	bool IsReturn(char c);
	bool IsNumeric(char* token);

	void EatWhitespace(void);
	void EatReturns(void);
	void EatNumbers(void);  // Ignore a series of numbers following a command

	void Error(char* token, char* message, ...);
	void Error(char* message, ...);
	void Warning(char* token, char* message, ...);
	void Warning(char* message, ...);

private:

	void Trim(char* token);
	void UCase(char* token);
	void LCase(char* token);
	bool CharInStr(char c, char* delims);

	int curLine;

	FILE* f;
	int fileLength;

	std::string fileName;
	std::string fileFullPath;
};


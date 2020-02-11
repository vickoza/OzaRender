#include <iostream>
#include <cstdarg>
#include <cstdlib>
#include <io.h>
#include "Loader.h"

Loader::Loader(): f(nullptr)
{
}

Loader::Loader(char* fName): f(nullptr)
{
	Open(fName);
}

Loader::~Loader()
{
	Close();
}

bool Loader::Close(void)
{
	if (f)
		return (fclose(f) != EOF);
	else
		return true;
}

bool Loader::Open(char* fName)
{
	Close();

	fileName = fName;
	f = fopen(fileName.c_str(), "rb");
	if (!f)
		return false;

	_fullpath((char*)fileFullPath.data(), fileName.c_str(), _MAX_PATH);
	fileLength = _filelength(_fileno(f));

	curLine = 0;
	return true;
}

bool Loader::ReadChar(char* buffer)
{
	if (!feof(f))
	{
		(*buffer) = fgetc(f);

		if (*buffer == '\n')
			curLine++;
		else if (*buffer == EOF)
			return false;
	}
	else
	{
		*buffer = EOF;
		return false;
	}
	return true;
}

bool Loader::PeekChar(char* buffer)
{
	if (!feof(f))
	{
		*buffer = fgetc(f);
		fseek(f, -1, SEEK_CUR);
		return true;
	}
	else
		return false;
}

bool Loader::RewindChar(void)
{
	if (ftell(f) > 0)
	{
		fseek(f, -1, SEEK_CUR);
		if (fgetc(f) == '\n')
			curLine--;
		fseek(f, -1, SEEK_CUR);
		return true;
	}
	else
		return false;
}

bool Loader::Rewind(int dist)
{
	bool result;
	for (int i = 0; i < dist; i++)
		result = RewindChar();
	return result;
}

bool Loader::FastForward(int dist)
{
	int i = 0;
	char c;

	while (i < dist)
	{
		ReadChar(c);
		i++;
	}
	return (c == EOF);
}

bool Loader::IsWhitespace(char c)
{
	return ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') || (c == '\f') ||
		(c == '\v'));
}

bool Loader::IsReturn(char c)
{
	return ((c == '\n') || (c == '\r') || (c == '\f'));
}

void Loader::Trim(char* token)
{
	int first, last;
	for (first = 0; IsWhitespace(token[first]); first++);
	for (last = strlen(token) - 1; IsWhitespace(token[last]); last--);
	token[last + 1] = '\0';

	for (int i = first; i <= last + 1; i++)
		token[i - first] = token[i];
}

void Loader::UCase(char* token)
{
	int len = strlen(token);
	for (int i = 0; i < len; i++)
		if (token[i] >= 'a' && token[i] <= 'z')
			token[i] = (token[i] + 'A' - 'a');
}

void Loader::LCase(char* token)
{
	int len = strlen(token);
	for (int i = 0; i < len; i++)
		if (token[i] >= 'A' && token[i] <= 'Z')
			token[i] = (token[i] - 'A' + 'a');
}

void Loader::EatWhitespace(void)
{
	char c;
	do
	{
		ReadChar(c);
	} while (IsWhitespace(c));

	if (!feof(f))
		fseek(f, -1, SEEK_CUR); // Don't have to check for \n because it is not whitespace
}

void Loader::EatReturns(void)
{
	char c;
	do
	{
		ReadChar(c);
	} while (IsReturn(c));

	if (!feof(f))
		fseek(f, -1, SEEK_CUR); // Don't have to check for \n because it is not whitespace
}

bool Loader::IsNumeric(char* token)
{
	for (int j = 0; token[j]; ++j)
		if (!(isdigit(token[j]) || (token[j] == '.') || (token[j] == '-') ||
			(token[j] == '+') || (token[j] == 'e') || (token[j] == 'E') ||
			(token[j] == 'd') || (token[j] == 'D')))
			return false;
	return true;
}

int Loader::CurPos(void)
{
	return ftell(f);
}

bool Loader::MoveTo(int pos)
{
	int curPos = ftell(f);
	if (curPos > pos)
		return Rewind(curPos - pos);
	else
		return FastForward(pos - curPos);
}

bool Loader::NextLine()
{
	char c;
	do
	{
		ReadChar(c);
	} while (c != '\n');
	EatWhitespace();

	if (feof(f))
		return false;
	else
		return true;
}

void Loader::EatNumbers(void)  // Ignore a series of numbers following a command
{
	double n;
	while (ReadDouble(n));
}

bool Loader::CharInStr(char c, char* delims)
{
	for (unsigned int i = 0; i < strlen(delims); i++)
		if (c == delims[i])
			return true;
	return false;
}

bool Loader::ReadString(char* buffer, char* delims)
{
	int i = -1;
	bool result;
	char Whitespace[8] = " \t\b\r\f\v\012";
	char* Delims;

	if (feof(f))
		return false;

	Delims = (delims == NULL ? Whitespace : delims);

	do
	{
		i++;
		result = ReadChar(buffer + i);
	} while (result && !CharInStr(buffer[i], Delims));
	buffer[i] = '\0';

	return true;
}

bool Loader::ReadToken(char* token, bool trim, bool lcase)
{
	int i = 0;

	do
	{
		strcpy(token, "");

		EatWhitespace();
		if (!ReadString(token))
			return false;

		if ((strncmp(token, "//", 2) == 0) || (strncmp(token, "#", 1) == 0))
		{
			NextLine(); // Ignore the comment line
			strcpy(token, "");
		}
	} while (!strcmp(token, ""));
	if (trim)
		Trim(token);
	if (lcase)
		LCase(token);

	return true;
}

void Loader::PeekTokens(char* tokens, int nTokens)
{
	char token[LOADER_INPUT_LENGTH];

	int OldPos = ftell(f);
	tokens[0] = '\0';
	for (int i = 0; i < nTokens; i++)
	{
		ReadToken(token, false);
		strcat(tokens, token);
		strcat(tokens, " ");
	}
	MoveTo(OldPos);
	Trim(tokens);
}

bool Loader::ReadDouble(double* d)
{
	char token[LOADER_INPUT_LENGTH];

	int OldPos = ftell(f);

	ReadToken(token);
	if (!IsNumeric(token))
	{
		MoveTo(OldPos);
		return false;
	}
	else
	{
		*d = atof(token);
		return true;
	}
}

bool Loader::ReadFloat(float* k)
{
	double d = 0;

	if (ReadDouble(d))
	{
		*k = (float)d;
		return true;
	}
	else
		return false;
}

bool Loader::ReadInt(int* k)
{
	double d = 0;

	if (ReadDouble(d))
	{
		*k = (int)d;
		return true;
	}
	else
		return false;
}

bool Loader::ReadCurrentLine(int& curPosInLine, char* buffer, int size)
{
	if (feof(f))
		return false;

	int CurPos = ftell(f);

	curPosInLine = -1;
	char c;
	do
	{
		RewindChar();
		PeekChar(c);
		if (c != '\t')
			curPosInLine++;
		else
			curPosInLine += 4;
	} while (c != '\n' && ftell(f) > 0);
	EatReturns();

	fgets(buffer, LOADER_INPUT_LENGTH, f);
	buffer[strlen(buffer) - 1] = '\0';

	fseek(f, CurPos, SEEK_SET);

	return true;
}

//#include "Windows.h"

void Loader::Error(const char* token, const char* message, ...)
{
	char curLineContents[LOADER_INPUT_LENGTH];
	char s[LOADER_INPUT_LENGTH];

	va_list list;
	va_start(list, message);

	vsprintf(s, message, list);

	int curPosInLine;
	ReadCurrentLine(curPosInLine, curLineContents);
	std::cout << "Error:\t" << s << "\n    File: \t" << fileName << "\n    Line: \t" << curLine << "\n    " << curLineContents << '\n';
	//printf("Error:\t%s\n    File: \t%s\n    Line: \t%d\n    %s\n    ", s, fileName, curLine, curLineContents);

	char* found = strstr(curLineContents, token);
	int tokenPos;

	if (found == NULL)
		tokenPos = 0;
	else
		tokenPos = int(found - curLineContents);
	for (int i = 0; i < tokenPos; i++)
		printf(" ");
	printf("^\n");

	//char tmp[1024];
	//sprintf(tmp, "Error in %s(%d):\n%s\nFor more info, see the console window", fileName, curLine, s);
	//MessageBox(NULL, tmp, "Error", MB_OK);
	exit(1);
}

void Loader::Error(const char* message, ...)
{
	char curLineContents[LOADER_INPUT_LENGTH];
	char s[LOADER_INPUT_LENGTH];

	va_list list;
	va_start(list, message);

	vsprintf(s, message, list);

	int curPosInLine;
	ReadCurrentLine(curPosInLine, curLineContents);
	std::cout << "Error:\t" << s <<"\n    File: \t" <<fileName <<"\n    Line: \t" << curLine << "\n    " << curLineContents << '\n';
	//printf("Error:\t%s\n    File: \t%s\n    Line: \t%d\n    %s\n    ", s, fileName, curLine, curLineContents);
	for (int i = 0; i < curPosInLine; i++)
		printf(" ");
	printf("^\n");

	//char tmp[1024];
	//sprintf(tmp, "Error in %s(%d):\n%s\nFor more info, see the console window", fileName, curLine, s);
	//MessageBox(NULL, tmp, "Error", MB_OK);
	exit(1);
}

void Loader::Warning(const char* token, const char* message, ...)
{
	char curLineContents[LOADER_INPUT_LENGTH];
	char s[LOADER_INPUT_LENGTH];

	va_list list;
	va_start(list, message);

	vsprintf(s, message, list);

	int curPosInLine;
	ReadCurrentLine(curPosInLine, curLineContents);
	std::cout << "Warning:\t" << s << "\n    File: \t" << fileName << "\n    Line: \t" << curLine << "\n    " << curLineContents << '\n';
	//printf("Warning:\t%s\n    File: \t%s\n    Line: \t%d\n    %s\n    ", s, fileName, curLine, curLineContents);

	char* found = strstr(curLineContents, token);
	int tokenPos;

	if (found == NULL)
		tokenPos = 0;
	else
		tokenPos = int(found - curLineContents);
	for (int i = 0; i < tokenPos; i++)
		printf(" ");
	printf("^\n");

}

void Loader::Warning(const char* message, ...)
{
	char curLineContents[LOADER_INPUT_LENGTH];
	char s[LOADER_INPUT_LENGTH];

	va_list list;
	va_start(list, message);

	vsprintf(s, message, list);

	int curPosInLine;
	ReadCurrentLine(curPosInLine, curLineContents);
	std::cout << "Warning:\t" << s << "\n    File: \t" << fileName << "\n    Line: \t" << curLine << "\n    " << curLineContents << '\n';
	//printf("Warning:\t%s\n    File: \t%s\n    Line: \t%d\n    %s\n    ", s, fileName, curLine, curLineContents);
	for (int i = 0; i < curPosInLine; i++)
		printf(" ");
	printf("^\n");
}
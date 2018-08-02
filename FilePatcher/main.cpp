#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

#define INRANGE(x, a, b) (x >= a && x <= b) 
#define getBits(x) (INRANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xA): (INRANGE(x, '0', '9') ? x - '0': 0))
#define getByte(x) (getBits(x[0]) << 4 | getBits(x[1]))

DWORD FindPattern(unsigned char* buffer, DWORD fileSize, const char* targetPattern)
{
	const char* pattern = targetPattern;

	uintptr_t firstMatch = 0;

	for (size_t position = 0; position < fileSize; position++) {
		if (!*pattern)
			return firstMatch;
		
		const uint8_t patternCurrent = *reinterpret_cast<const uint8_t*>(pattern);
		const uint8_t memoryCurrent = *reinterpret_cast<const uint8_t*>(buffer[position]);

		if (patternCurrent == '\?' || memoryCurrent == getByte(pattern)) {
			if (!firstMatch)
				firstMatch = position;

			if (!pattern[2])
				return firstMatch;

			pattern += patternCurrent != '\?' ? 3 : 2;
		}
		else {
			pattern = targetPattern;
			firstMatch = 0;
		}
	}

	return NULL;
}

void Read(unsigned char* buffer, size_t size, DWORD offset)
{
	cout << setfill('0');
	for (DWORD i = offset; i < offset + size; i++)
	{
		cout << hex << uppercase << setw(2) << int(buffer[i]) << endl;
	}
}

int main()
{
	int type = 0;
	DWORD offset = 0;
	string signaturePattern = "";
	string fileName = "";
	HANDLE hFile;
	BYTE* patchBytes;
	size_t size = 0;
	DWORD written = 0;
	DWORD fileSize = 0;
	unsigned char* buffer;

	cout << "Insert the filename (must be in this folder):" << endl;
	getline(cin, fileName);

	// file handle
	hFile = INVALID_HANDLE_VALUE;
	// open input file in write mode (use GENERIC_WRITE flag)
	hFile = CreateFile(fileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		cout << "Couldn't find the given file." << endl;
		system("PAUSE");
		return -1;
	}
	fileSize = GetFileSize(hFile, NULL);

	buffer = (unsigned char*)malloc(fileSize);
	DWORD read = 0;
	ReadFile(hFile, buffer, fileSize, &read, NULL);

	cout << "How do you want to patch?" << endl;
	cout << "1) Offset" << endl;
	cout << "2) Signature" << endl;
	cin >> type;
	if (type < 1 || type > 3)
	{
		cout << "You had one job. How could you fuck this up?" << endl;
		system("PAUSE");
		return -1;
	}

	//Get Offset/SigPattern
	cout << "Type in the " << (type == 1 ? "Offset (as hex)" : "Pattern (IDA Style)") << ":" << endl;
	switch (type)
	{
	case 1:
		cin >> hex >> offset;
		break;
	case 2:
		getline(cin, signaturePattern);
		break;
	default:
		break;
	}

	if (type == 2) //Check the signature -> get offset
	{
		offset = FindPattern(hFile, signaturePattern.c_str());

		if (offset == 0)
		{
			cout << "Couldn't find the pattern." << endl;
			system("PAUSE");
			return -1;
		}
	}

	cout << "Type in how many bytes you want to patch:" << endl;
	cin >> size;
	
	cout << "Do you want to read only? (y/n)" << endl;
	char answer;
	cin >> answer;
	if (answer == 'y')
	{
		Read(buffer, size, offset);
		system("PAUSE");
		return 0;
	}

	patchBytes = (BYTE*)malloc(size);
	for (size_t i = 0; i < size; i++)
	{
		cout << "Type in byte Number " << i << endl;
		unsigned input = 0;
		cin >> hex >> input;
		patchBytes[i] = input;
	}

	SetFilePointer(hFile, offset, 0, FILE_BEGIN);

	bool result = WriteFile(hFile, patchBytes, sizeof(patchBytes), &written, NULL);

	cout << "Result: " << result << ". Bytes written: " << written << endl;

	// close the file handle
	CloseHandle(hFile);

	system("PAUSE");
	return 0;
}
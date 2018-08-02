#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
using namespace std;

#define INRANGE(x, a, b) (x >= a && x <= b) 
#define getBits(x) (INRANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xA): (INRANGE(x, '0', '9') ? x - '0': 0))
#define getByte(x) (getBits(x[0]) << 4 | getBits(x[1]))

DWORD FindPattern(unsigned char* buffer, DWORD fileSize, string targetPattern)
{
	vector<unsigned> newPattern;
	for (size_t i = 0; i < targetPattern.size(); i++) //Remove spaces and convert bytes to char so we can compare with buffer char
	{
		if (targetPattern[i] == ' ')
			continue;
		if (targetPattern[i] == '?')
		{
			newPattern.push_back(63);
			continue;
		}

		int f = (getBits(targetPattern[i]) << 4 | getBits(targetPattern[i + 1]));
		newPattern.push_back(f);
		i++;
	}

	size_t sigSize = newPattern.size();
	unsigned char *pBasePtr = buffer;
	unsigned char *pEndPtr = buffer + fileSize;
	size_t i;

	while (pBasePtr < pEndPtr) {
		for (i = 0; i < sigSize; i++) {
			if ((newPattern[i] != 63) && (newPattern[i] != pBasePtr[i]))
				break;
		}

		// If 'i' reached the end, we know we have a match!
		if (i == sigSize)
			return pBasePtr - buffer;

		pBasePtr++;
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
	string signaturePattern;
	string fileName = "";
	HANDLE hFile;
	BYTE* patchBytes;
	size_t size = 0;
	DWORD written = 0;
	DWORD fileSize = 0;
	unsigned char* buffer;

	cout << "Insert the filename (must be relative to the .exe):" << endl;
	getline(cin, fileName);

	// file handle
	hFile = INVALID_HANDLE_VALUE;
	// open input file in write mode (use GENERIC_WRITE flag)
	hFile = CreateFile(fileName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

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
		cin.ignore();
		getline(cin, signaturePattern);
		break;
	default:
		break;
	}

	if (type == 2) //Check the signature -> get offset
	{
		offset = FindPattern(buffer, fileSize, signaturePattern);

		if (offset == 0)
		{
			cout << "Couldn't find the pattern." << endl;
			system("PAUSE");
			return -1;
		}

		cout << "Offset: " << hex << uppercase << offset << endl;

		cout << "Offset the address by how much:" << endl;
		int offoffset = 0;
		cin >> offoffset;
		offset += offoffset;
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

	bool result = WriteFile(hFile, patchBytes, size, &written, NULL);

	cout << "Result: " << result << ". Bytes written: " << written << endl;

	// close the file handle
	CloseHandle(hFile);

	system("PAUSE");
	return 0;
}
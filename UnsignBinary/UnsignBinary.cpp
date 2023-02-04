// UnsignBinary.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Simple CLI tool to remove all digital signatures from a file

// Usage
//	UnsignBinary.exe <FILE_TO_UNSIGN>
// Returns
//	0	- All signatures have been removed
//  -1	- Incorrect args
//  -2	- Invalid file handle
//  -3	- No digital signatures on file
//  -4	- Failed to remove all digital signatures

#include <Windows.h>
#include <ImageHlp.h>
#include <iostream>

int main(int argc, char** argv)
{
	int retVal = -1; 

	if (argc == 2)
	{
		HANDLE hFile;

		hFile = CreateFileA(argv[1], 
			FILE_READ_DATA | FILE_WRITE_DATA, 
			FILE_SHARE_READ, 
			NULL, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			// Check if the file is actually signed
			DWORD dwNumCerts{ 0 };
			DWORD dwIndices[256];

			ZeroMemory(dwIndices, 256);

			BOOL bCerts = ImageEnumerateCertificates(hFile,
				CERT_SECTION_TYPE_ANY,
				&dwNumCerts,
				dwIndices,
				256);

			if (bCerts)
			{
				BOOL bAllCertsRemoved{ TRUE };
				for (DWORD dwIndex = 0; dwIndex < dwNumCerts; ++dwIndex)
				{
					BOOL bRemovedCert = ImageRemoveCertificate(hFile,
						dwIndex);

					if(!bRemovedCert)
					{
						std::cout << "Failed to remove cert. Index: " << dwIndex << std::endl;
						bAllCertsRemoved = FALSE; 
					}
				}

				if (!bAllCertsRemoved)
				{
					retVal = -4; 
				}
			}
			else
			{
				std::cout << "File has no digital signatures. No work done.\n";
				retVal = -3; 
			}

			CloseHandle(hFile);

		}
		else
		{
			std::cout << "Error opening file. No work done\n";
			retVal = -2; 
		}
	}
	else
	{
		std::cout << "No file argument provided. No work done\n";
	}

	return retVal; 
}
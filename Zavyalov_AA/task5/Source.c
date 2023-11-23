#define _CRT_SECURE_NO_WARNINGS

#include <io.h>
#include <stdio.h>
#include <stdlib.h> 
#include <fcntl.h>
#include <windows.h>
#include <locale.h>
#include <wchar.h> // using wchars so that program works correctly with paths named in non-english
#include "sortings.h"

struct FileInfo {
	wchar_t name[256];
	unsigned long long size;
};

int fileQuantity(wchar_t* dirPath) {
	int size = 0;
	WIN32_FIND_DATAW findFileData;
	HANDLE hfind = FindFirstFileW(dirPath, &findFileData);
	if (hfind == INVALID_HANDLE_VALUE) {
		wprintf(L"An error occurred while trying to open the directory. Please, try entering different directory.\n");
		return -1;
	}
	while (FindNextFileW(hfind, &findFileData) != 0)
		if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0 && !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			size++;
	FindClose(hfind);
	return size;
}

int checkPath(wchar_t* dirPath) {
	if (wcslen(dirPath) == 0 || dirPath == L'\n') {// An empty path is entered
		wprintf(L"The path cannot be empty!\n");
		return 0;
	}
	if (wcslen(dirPath) == 2 && L'A' <= dirPath[0] <= L'z' && dirPath[1] == L':') { // Disk name is entered
		return 1;
	}
	if (((dirPath[0] < L'A' || dirPath[0] > L'Z') && (dirPath[0] < L'a' || dirPath[0] > L'z')) || (dirPath[1] != L':') || (dirPath[2] != L'\\')) {
		wprintf(L"Please enter the path including disk name.\n");
		return 0;
	}
	if (wcsstr(dirPath, "\\\\") != NULL) {
		wprintf(L"Please consider replacing all \"\\\\\" with \"\\\".\n");
		return 0;
	}
	wchar_t specs[4] = L"/?*";
	for (int i = 0; i < 3; i++) {
		if (wcschr(dirPath, specs[i]) != NULL) {
			wprintf(L"Please check the path for special characters. (The path cannot include %c)\n", specs[i]);
			return 0;
		} 
	}
	return 1;
}

void formPrev(wchar_t* dirPath, wchar_t* prev) { // Formation of prev
	for (int i = 0; i < 256; i++) {
		prev[i] = dirPath[i];
	}
}

void getPath(wchar_t* dirPath) {
	wchar_t prev[256];
	do {
		formPrev(dirPath, prev);
		wprintf(L"Enter the directory path in \"diskName:\\folder1\\folder2...\" format: ");
		wscanf(L"%255[^\n]", dirPath, (unsigned)_countof(dirPath));
		getwchar();
		if (wcscmp(dirPath, prev) == 0) dirPath[0] = L'\0';
	} while (!checkPath(dirPath));
}

void getFiles(wchar_t* dirPath, struct FileInfo* files) {
	WIN32_FIND_DATAW findFileData;
	HANDLE hfind = FindFirstFileW(dirPath, &findFileData);
	DWORD fileAttributes = GetFileAttributesW(dirPath);
	int i = 0;
	do {
		if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0 && !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			wcscpy(files[i].name, findFileData.cFileName);
			files[i].size = (findFileData.nFileSizeHigh * (MAXDWORD + 1) + findFileData.nFileSizeLow);
			i++;
		}
	} while (FindNextFileW(hfind, &findFileData) != 0);
	FindClose(hfind);
}

int maxlen(struct FileInfo* files, int size) {
	int max = 9; // =  wcslen(L"File name");
	for (int i = 0; i < size; i++) {
		if (wcslen(files[i].name) > max) {
			max = wcslen(files[i].name);
		}
	}
	return max;
}

int main() {

	SetConsoleCP(65001);
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);
	wchar_t dirPath[256];
	int size = -1;
	wprintf(L"Example of correct directory path: D:\\Documents\\project\n");

	while (size == -1 || size == 0) { // Getting the directory path
		getPath(dirPath);
		wcscat(dirPath, L"\\*.*");
		wprintf(L"%ls\n", dirPath);
		size = fileQuantity(dirPath);
		if (size == 0) 
			wprintf(L"The directory is empty. Please, try entering different directory.\n");
		
	}

	struct FileInfo* files = (struct FileInfo*)malloc(size * sizeof(struct FileInfo));
	getFiles(dirPath, files);
	struct FileInfo* tempfiles = (struct FileInfo*)malloc(size * sizeof(struct FileInfo));


		for (int i = 0; i < size; i++)
			tempfiles[i] = files[i];

		sort(tempfiles, size, 5, 0);

		int mxln = maxlen(files, size);
		for (int i = 0; i < (mxln - 9) / 2; i++) 
			wprintf(L" ");
		
		wprintf(L"File name");
		for (int i = 0; i < (mxln - 9) / 2; i++) 
			wprintf(L" ");
		
		if ((mxln - 9) % 2)
			wprintf(L" ");
		wprintf(L" | File size\n");
		for (int i = 0; i < size; i++) {
			// wprintf(L"File name: %ls, file size: %llu\n", tempfiles[i].name, tempfiles[i].size);
			wprintf(L"%ls", tempfiles[i].name);
			int curlen = wcslen(tempfiles[i].name);
			for (int i = curlen; i < mxln; i++) {
				wprintf(L" ");
			}
			wprintf(L" | ");
			wprintf(L"%d\n", tempfiles[i].size);
		}



	wprintf(L"%d\n", size);
	if(files != NULL) free(files);
	if (tempfiles != NULL) free(tempfiles);
	system("pause");
	return 0;
}

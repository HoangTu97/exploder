#pragma once

#include <iostream>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <string>
#include <vector>
#pragma comment(lib, "User32.lib")
using namespace std;

class Directory {
private:
	wstring name;
	vector<Directory> sub;
public:
	Directory();
	Directory(wstring name);
	wstring getName();
	wstring setName(wstring name);
	vector<wstring> getListAll();
	vector<wstring> getListDir();
	Directory & getSubByName(wstring name);
	Directory & getSubByIndex(int index);
	void treeView();
	void Access();
};

vector<wstring> getDrives();
vector<Directory> initRoot();

static vector<Directory> root = initRoot();
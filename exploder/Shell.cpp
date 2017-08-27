#include "Shell.h"

// mặc định khỏi tạo thư mục ở ổ C
Directory::Directory()
{
	this->name = L"C:\\";
	this->sub.resize(0);
}

// hàm khỏi tạo tên cho thư mục
Directory::Directory(wstring name)
{
	this->name = name;
	this->sub.resize(0);
}

// khỏi tạo root bao gồm tất cả ổ đĩa
vector<Directory>initRoot()
{
	vector<Directory> ret;
	ret.resize(0);
	vector<wstring> drives = getDrives();
	for (int i = 0; i < drives.size(); i++) {
		Directory temp;
		temp.setName(drives[i]);
		ret.push_back(temp);
	}
	return ret;
}

// lấy tên của thư mục (tên đầy đủ)
wstring Directory::getName()
{
	return this->name;
}

// gán tên cho thư mục
wstring Directory::setName(wstring name)
{
	this->name = name;
	return name;
}

// hàm lấy danh sách ổ đĩa
// trả về mảng các đường dẫn gốc ổ đĩa
vector<wstring> getDrives()
{
	vector<wstring> ret;
	ret.resize(0);

	// lấy danh sách ổ đĩa
	DWORD mydrives = 100;
	WCHAR Buffer[100];
	LPWSTR lpBuffer = Buffer;
	DWORD test = GetLogicalDriveStrings(mydrives, lpBuffer);
	wstring temp = L"";
	for (int i = 0;; i++) {
		if (!lpBuffer[i]) {
			temp.pop_back();
			ret.push_back(temp);
			if (!lpBuffer[i + 1]) {
				break;
			}
			temp = L"";
		}
		else {
			temp += lpBuffer[i];
		}
	}

	return ret;
}

// lấy toàn bộ nội dung trong thư mục (cho phần hiển thị thư mục)
vector<wstring> Directory::getListAll()
{
	vector<wstring> ret;
	ret.resize(0);

	TCHAR * argv = (wchar_t *)this->name.c_str();
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.
	StringCchLength(argv, MAX_PATH, &length_of_arg);

	if (length_of_arg > (MAX_PATH - 3))
	{
		return ret;
	}

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	StringCchCopy(szDir, MAX_PATH, argv);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		return ret;
	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			basic_string<TCHAR> fileName = ffd.cFileName;
			if (fileName != TEXT(".") && fileName != TEXT("..")) {
				ret.push_back(ffd.cFileName);
			}
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			//_tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
			ret.push_back(ffd.cFileName);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
	return ret;
}

// hàm lấy các thư mục con bên trong (cho phần tree view)
// trả về tên thư mục nếu cần
// và tự động thêm thư mục con vào phần sub
vector<wstring> Directory::getListDir()
{
	vector<wstring> ret;
	ret.resize(0);

	TCHAR * argv = (wchar_t *)this->name.c_str();
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.
	StringCchLength(argv, MAX_PATH, &length_of_arg);

	if (length_of_arg > (MAX_PATH - 3))
	{
		return ret;
	}

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	StringCchCopy(szDir, MAX_PATH, argv);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		return ret;
	}

	// List all the files in the directory with some info about them.
	this->sub.resize(0);
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			basic_string<TCHAR> fileName = ffd.cFileName;
			if (fileName != TEXT(".") && fileName != TEXT("..")) {
				ret.push_back(ffd.cFileName);
				this->sub.push_back(Directory(this->name + L"\\" + ffd.cFileName));
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

	return ret;
}

// lấy thư mục con theo tên
Directory & Directory::getSubByName(wstring name)
{
	for (int i = 0; i < this->sub.size(); i++)
		if (this->sub[i].name.substr(this->sub[i].name.find_last_of(L"\\") + 1) == name)
			return this->sub[i];
}

// lấy thư mục con tại index
Directory & Directory::getSubByIndex(int index)
{
	return this->sub[index];
}

// hàm test chức năng đọc thư mục con trong tree view
// cách sử dụng:
// 
void Directory::treeView()
{
	wcout << this->name << endl;
	this->getListDir();
	for (int i = 0; i < this->sub.size(); i++) {
		this->sub[i].treeView();
	}
}

// hàm access để truy cập thử và 1 nhánh thư mục
// và lấy hết nội dung bên trong thư mục đấy
// cách sử dụng:
//		root[1].getListDir();
//		root[1].getSubByIndex(1).Access(); hoặc root[1].getSubByName(L"FileName").Access();
void Directory::Access()
{
	wcout << this->name << endl;
	vector<wstring> item = this->getListAll();
	for (int i = 0; i < item.size(); i++) {
		wcout << (this->name + L"\\" + item[i]) << endl;
	}
}

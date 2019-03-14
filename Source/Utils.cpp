//2014-11-09 Sun.

#include "stdafx.h"

/*
================================================================================
string
================================================================================
*/
int AnsiToUnicode(const char * Ansi, wchar_t * Buffer, int BufferElemCount)
{
	int AnsiLen = strlen(Ansi);
	int Need = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, Ansi, AnsiLen, nullptr, 0);

	if (BufferElemCount < Need + 1)
	{
		printf("destination buffer overflow\n");
		return FALSE;
	}

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, Ansi, AnsiLen, Buffer, Need);
	Buffer[Need] = 0;

	return TRUE;
}

/*
================================================================================
file
================================================================================
*/

int FileSize(FILE *f)
{
	int pos = ftell(f);
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, pos, SEEK_SET);
	return size;
}

FileType GetFileType(const wchar_t * FileName)
{
	byte BOM[4]; //byte order mark
	FileType Result = FT_UNKNOWN;

	FILE * f = 0;
	_wfopen_s(&f, FileName, L"rb");
	if (!f)
	{
		printf("could not open file %s\n", FileName);
		return Result;
	}

	Result = FT_ANSI;
	int Size = FileSize(f);

	if (Size >= 3)
	{
		fread(BOM, 1, 3, f);

		if (BOM[0] == 0xFF && BOM[1] == 0xFE)
		{
			Result = FT_UNICODE_LE;
		}
		else if (BOM[0] == 0xFE && BOM[1] == 0xFF)
		{
			Result = FT_UNICODE_BE;
		}
		else if (BOM[0] == 0xEF && BOM[1] == 0xBB && BOM[2] == 0xBF)
		{
			Result = FT_UTF8;
		}
	}
	else if (Size >= 2)
	{
		fread(BOM, 1, 3, f);

		if (BOM[0] == 0xFF && BOM[1] == 0xFE)
		{
			Result = FT_UNICODE_LE;
		}
		else if (BOM[0] == 0xFE && BOM[1] == 0xFF)
		{
			Result = FT_UNICODE_BE;
		}
	}

	fclose(f);
	f = 0;

	return Result;
}

int UTF8ToAnsi(const char * utf8, char *ansi, int ansiBytes) {
	if (!utf8) {
		if (ansi && ansiBytes > 0) { //treat as null string
			ansi[0] = 0;
		}
		return 0;
	}

	int len = (int)strlen(utf8); // this is not character count, utf-8 using vary length coding, but len is sufficent for wbuf
	wchar_t * wbuf = (wchar_t*)malloc(sizeof(wchar_t) * (len + 1));
	if (!wbuf) {
		ansi[0] = 0;
		return 0;
	}

	int translen = MultiByteToWideChar(CP_UTF8, 0, utf8, len, wbuf, len + 1);
	wbuf[translen] = 0;

	int needBytes = WideCharToMultiByte(CP_ACP, 0, wbuf, translen, nullptr, 0, nullptr, nullptr); //return bytes count
	if (!ansi) {
		return needBytes;
	}

	if (ansiBytes < needBytes + 1) {
		ansi[0] = 0;
		return 0;
	}

	translen = WideCharToMultiByte(CP_ACP, 0, wbuf, translen, ansi, ansiBytes, nullptr, nullptr);
	ansi[translen] = 0;

	free(wbuf);

	return translen;
}

BOOL ReadAnsiText(const wchar_t * FileName, char * &Data, int &Len)
{
	Data = nullptr;
	Len = 0;

	FileType ft = GetFileType(FileName);

	if (FT_ANSI == ft) {
		FILE * f = 0;
		_wfopen_s(&f, FileName, L"rb");
		if (!f)
		{
			printf("could not open file %s\n", FileName);
			return FALSE;
		}

		int Size = FileSize(f);

		Len = Size + 1;
		Data = new char[Len];

		fread(Data, 1, Size, f);
		Data[Size] = 0;
		fclose(f);

		return TRUE;
	}
	else if (FT_UNICODE_LE == ft) {
		FILE * f = 0;
		_wfopen_s(&f, FileName, L"rb");
		if (!f)
		{
			printf("could not open file %s\n", FileName);
			return FALSE;
		}

		int Size = FileSize(f);
		if (0 == Size % 2) {
			wchar_t * buffer = new wchar_t[Size / 2 + 1];
			fread(buffer, 2, Size / 2, f);
			buffer[Size / 2] = 0;

			Len = Size + 1;
			Data = new char[Len];

			int translen = WideCharToMultiByte(CP_ACP, 0, buffer + 1, Size / 2 - 1, Data, Len, nullptr, nullptr);
			Data[translen] = 0;
		
			delete[] buffer;

			fclose(f);

			return TRUE;
		}
		else {
			fclose(f);
			printf("bad file size\n");
			return FALSE;
		}		
	}
	else if (FT_UTF8 == ft) { // 2017-11-01 Wed.
		FILE * f = 0;
		_wfopen_s(&f, FileName, L"rb");
		if (!f)
		{
			printf("could not open file %s\n", FileName);
			return FALSE;
		}

		int Size = FileSize(f);

		Len = Size + 1;
		char *utf8 = new char[Len];

		fread(utf8, 1, Size, f);
		utf8[Size] = 0;
		fclose(f);

		Data = new char[Len];
		UTF8ToAnsi(utf8, Data, Len);

		return TRUE;
	}
	else {
		printf("unsupported file type\n");
		return FALSE;
	}
}

/*
================================================================================
main dialog
================================================================================
*/
CCPPSrcCounterDlg * GetMainDialog()
{
	//return (CCPPSrcCounterDlg*)AfxGetMainWnd(); //return NULL
	/*
	If AfxGetMainWnd is called from the application¡¯s primary thread,
	it returns the application¡¯s main window according to the above rules.
	If the function is called from a secondary thread in the application, the function returns
	the main window associated with the thread that made the call.
	*/

	return (CCPPSrcCounterDlg*)(AfxGetApp()->m_pMainWnd);
}

/*
================================================================================
c/c++ source
================================================================================
*/

static int IsBlankChar(wchar_t c)
{
	return c == L' ' || c == L'\t' || c == L'\r';
}

void GetSrcFileStatistic(const wchar_t * Buffer, SrcFileStatistic &Statistic)
{
	int CodeLines = 0;
	int CodeCommentLines = 0;
	int CommentLines = 0;
	int BlankLines = 0;

	const wchar_t * PC = Buffer;

	if (*PC) {

		enum { S_CODE, S_LINECOMMENT, S_BLOCKCOMMENT } State;
		State = S_CODE;

		int HasCode = 0;
		int HasComment = 0;

		while (*PC)
		{
			//new line start
			const wchar_t * LineStart = PC;

			HasCode = 0;
			HasComment = 0;

			while (*PC && *PC != L'\n')
			{
				if (S_CODE == State)
				{
					if (*PC == L'/' && *(PC + 1) == L'*')
					{
						State = S_BLOCKCOMMENT;
						HasComment = 1;
						PC += 2;
					}
					else if (*PC == L'/' && *(PC + 1) == L'/')
					{
						State = S_LINECOMMENT;
						HasComment = 1;
						PC += 2;
					}
					else if (!IsBlankChar(*PC))
					{
						HasCode = 1;
						PC++;
					}
					else
					{
						PC++;
					}
				}
				else if (S_BLOCKCOMMENT == State)
				{
					if (*PC == L'*' && *(PC + 1) == L'/')
					{
						State = S_CODE;
						HasComment = 1;
						PC += 2;
					}
					else if (!IsBlankChar(*PC))
					{
						HasComment = 1;
						PC++;
					}
					else
					{
						PC++;
					}
				}
				else
				{
					PC++;
				}
			}

			//line end
			if (*PC == L'\n')
			{
				if (HasComment && HasCode)
				{
					CodeCommentLines++;
				}
				else if (HasComment)
				{
					CommentLines++;
				}
				else if (HasCode)
				{
					CodeLines++;
				}
				else
				{
					BlankLines++;
				}

				if (S_LINECOMMENT == State)
				{
					State = S_CODE;
				}

				PC++; //move to next line
			}

		}

		if (HasComment && HasCode)
		{
			CodeCommentLines++;
		}
		else if (HasComment)
		{
			CommentLines++;
		}
		else if (HasCode)
		{
			CodeLines++;
		}
		else
		{
			BlankLines++;
		}

	}

	Statistic.CodeLines = CodeLines;
	Statistic.CodeCommentLines = CodeCommentLines;
	Statistic.CommentLines = CommentLines;
	Statistic.BlankLines = BlankLines;
}

/*
================================================================================
SrcFileList
================================================================================
*/

SrcFileList::SrcFileList()
{
}

SrcFileList::~SrcFileList()
{
}

void SrcFileList::Init(const wchar_t *dir)
{
	RecursiveInit(dir);
}

static BOOL IsDots(const wchar_t *filename)
{
	return (filename[0] == L'.') && (filename[1] == 0 || ((filename[1] == L'.') && (filename[2] == 0)));
}

static const wchar_t * ExtractExt(const wchar_t *path)
{
	static wchar_t buffer[MAX_PATH];
	wcscpy_s(buffer, path);

	int l = (int)wcslen(path);
	for (int i = l - 1; i >= 0; i--)
	{
		if (buffer[i] == L'.')
		{
			return buffer + i + 1;
		}
	}

	return nullptr;
}

void SrcFileList::RecursiveInit(const wchar_t *dir)
{
	wchar_t findfilter[MAX_PATH];
	swprintf_s(findfilter, L"%s\\*.*", dir);

	WIN32_FIND_DATA fd;
	HANDLE Handle = FindFirstFile(findfilter, &fd);
	if (INVALID_HANDLE_VALUE == Handle)
	{
		//AfxOutputDebugString(_T("find faile : %s\n"), dir);
		return;
	}

	BOOL Found = TRUE;
	while (Found)
	{
		if (!IsDots(fd.cFileName))
		{
			wchar_t fullpath[MAX_PATH];
			swprintf_s(fullpath, L"%s\\%s", dir, fd.cFileName);

			if ((fd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY)) != 0) //directory
			{
				RecursiveInit(fullpath);
			}
			else
			{
				const wchar_t * ext = ExtractExt(fd.cFileName);
				if (ext && (!_wcsicmp(ext, L"h") || !_wcsicmp(ext, L"hpp") || !_wcsicmp(ext, L"c") || !_wcsicmp(ext, L"cpp") || !_wcsicmp(ext, L"inl")))
				{
					mFileList.AddString(fullpath);
				}
			}
		}

		Found = FindNextFile(Handle, &fd);
	}

	FindClose(Handle);
}

const wchar_t * SrcFileList::GetFirst()
{
	return mFileList.GetFirstString();

}

const wchar_t * SrcFileList::GetNext()
{
	return mFileList.GetNextString();
}

int SrcFileList::GetCount() const
{
	return mFileList.GetCount();
}

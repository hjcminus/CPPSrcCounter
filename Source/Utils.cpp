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

BOOL ReadAnsiText(const wchar_t * FileName, char * &Data, int &Len)
{
	Data = nullptr;
	Len = 0;

	if (GetFileType(FileName) != FT_ANSI)
	{
		printf("could not detect file type\n");
		return FALSE;
	}

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

static const wchar_t * SkipWhiteChar(const wchar_t * pc)
{
	while (*pc == L' ' || *pc == L'\t' || *pc == L'\r')
	{
		pc++;
	}
	return pc;
}

void GetSrcFileStatistic(const wchar_t * Buffer, SrcFileStatistic &Statistic)
{
	int TotalLine = 0;
	int CommentLine = 0;
	int BlankLine = 0;

	const wchar_t * PC = Buffer;

	enum { S_CODE, S_LINECOMMENT, S_BLOCKCOMMENT } State;
	State = S_CODE;

	while (*PC)
	{
		//new line start
		const wchar_t * LineStart = PC;
		PC = SkipWhiteChar(PC);

		if (S_CODE == State && (*PC == L'\n'))
		{
			BlankLine++;
		}

		while (*PC && *PC != L'\n')
		{
			if (S_CODE == State)
			{
				if (*PC == L'/' && *(PC + 1) == L'*')
				{
					State = S_BLOCKCOMMENT;
					PC += 2;
				}
				else if (*PC == L'/' && *(PC + 1) == L'/')
				{
					State = S_LINECOMMENT;
					PC += 2;
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
					State = S_CODE; //back to code mode
					PC += 2;

					PC = SkipWhiteChar(PC);
					if (*PC == L'\n')
					{
						CommentLine++; //no valid character
					}
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
			TotalLine++;

			if (S_BLOCKCOMMENT == State)
			{
				CommentLine++;
			}
			else if (S_LINECOMMENT == State)
			{
				CommentLine++;
				State = S_CODE;
			}

			PC++; //move to next line
		}

	}

	if (S_CODE == State)
	{
		BlankLine++;
	}
	else if (S_BLOCKCOMMENT == State)
	{
		CommentLine++;
	}
	else if (S_LINECOMMENT == State)
	{
		CommentLine++;
		State = S_CODE;
	}

	Statistic.CodeLine = TotalLine - CommentLine - BlankLine;
	Statistic.CommentLine = CommentLine;
	Statistic.BlankLine = BlankLine;
	Statistic.TotalLine = TotalLine;
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
				if (ext && (!wcscmp(ext, L"h") || !wcscmp(ext, L"hpp") || !wcscmp(ext, L"c") || !wcscmp(ext, L"cpp")))
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

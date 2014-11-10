//2014-11-09 Sun.

#pragma once

/*
================================================================================
string
================================================================================
*/
BOOL AnsiToUnicode(const char * Ansi, wchar_t * Buffer, int BufferElemCount);

/*
================================================================================
file
================================================================================
*/

int FileSize(FILE *f);

enum FileType
{
	FT_UNKNOWN,
	FT_ANSI,
	FT_UNICODE_LE,
	FT_UNICODE_BE,
	FT_UTF8,
};

//http://stackoverflow.com/questions/4520184/how-to-detect-the-character-encoding-of-a-text-file
FileType GetFileType(const wchar_t * FileName);
BOOL     ReadAnsiText(const wchar_t * FileName, char * &Data, int &Len);

/*
================================================================================
main dialog
================================================================================
*/
class CCPPSrcCounterDlg * GetMainDialog();

/*
================================================================================
c/c++ source
================================================================================
*/

struct SrcFileStatistic
{
	int CodeLines;
	int CodeCommentLines;
	int CommentLines;
	int BlankLines;
};

void GetSrcFileStatistic(const wchar_t * Buffer, SrcFileStatistic &Statistic);

/*
================================================================================
SrcFileList
================================================================================
*/
class SrcFileList
{
public:

	SrcFileList();
	~SrcFileList();

	void              Init(const wchar_t *dir);

	const wchar_t *   GetFirst();
	const wchar_t *   GetNext();
	int               GetCount() const;

private:

	QueuedStringPool  mFileList;

	void              RecursiveInit(const wchar_t *dir);
};

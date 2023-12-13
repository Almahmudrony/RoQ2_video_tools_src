#include <windows.h>
#include <direct.h>
#include "sb4.h"

static char current_filename[4096];
static char cwd_path[4096];
static bool firstfile = 1;

#define OFNSTYLING OFN_EXPLORER

const char *SB4SaveFile(const char *baseFile, const char *filterStrings, const char *extension, const char *title)
{
	OPENFILENAME openInfo;
	int succeeded;

	memset(&openInfo, 0, sizeof(openInfo));

	if(!baseFile)
	{
		current_filename[0] = '\0';
		_getcwd(cwd_path, sizeof(cwd_path));
		openInfo.lpstrInitialDir = cwd_path;
	}
	else
		strcpy(current_filename, baseFile);

	openInfo.lStructSize = sizeof(openInfo);

	openInfo.lpstrFilter = filterStrings;

	openInfo.lpstrFile = current_filename;
	openInfo.nMaxFile = sizeof(current_filename);
	openInfo.lpstrTitle = title;
	openInfo.Flags = 
//		OFN_DONTADDTORECENT |
		OFN_ENABLESIZING |
		OFNSTYLING |
		OFN_HIDEREADONLY |
		OFN_NOTESTFILECREATE |
		OFN_OVERWRITEPROMPT |
		OFN_EXTENSIONDIFFERENT;

	openInfo.lpstrDefExt = extension;

	succeeded = GetSaveFileName(&openInfo);
	if(!succeeded)
		exit(1);


	return current_filename;
}

const char *SB4GetFile(const char *baseFile, const char *filterStrings, const char *title)
{
	OPENFILENAME openInfo;
	int succeeded;

	memset(&openInfo, 0, sizeof(openInfo));

	if(!baseFile)
	{
		current_filename[0] = '\0';
		_getcwd(cwd_path, sizeof(cwd_path));
		openInfo.lpstrInitialDir = cwd_path;
	}
	else
		strcpy(current_filename, baseFile);

	openInfo.lStructSize = sizeof(openInfo);

	openInfo.lpstrFilter = filterStrings;

	openInfo.lpstrFile = current_filename;
	openInfo.nMaxFile = sizeof(current_filename);
	openInfo.lpstrTitle = title;
	openInfo.Flags = 
//		OFN_DONTADDTORECENT |
		OFN_ENABLESIZING |
		OFNSTYLING |
		OFN_FILEMUSTEXIST |
		OFN_HIDEREADONLY |
		OFN_NOTESTFILECREATE |
		OFN_EXTENSIONDIFFERENT;

	succeeded = GetOpenFileName(&openInfo);
	if(!succeeded)
		exit(1);

	firstfile = 0;

	return current_filename;
}


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vfw.h>

#include "sb4soundreader.h"
#include "sb4lsfreader.h"
#include "sb4avisoundreader.h"
#include "../sb4.h"

#include "../../tdp/tdp_string.hpp"

using namespace TDP;

int muxFiles4(SoundReader *sr, const char *audioName, const char *videoName, const char *outputName, bool adpcm);


int main(int argc, char **argv)
{
	terString baseFile;
	terString baseName;
	terString soundFile;
	terString outName;

	SoundReader *sr;
	
	bool soundIsAVI;
	bool adpcm;

	AVIFileInit();

	if(argc >= 2)
		baseFile = argv[1];
	else
		baseFile = SB4GetFile(NULL, "RoQ video file (*.roq)\0" "*.roq\0", "Select input RoQ file");

	if(argc >= 3)
		soundFile = argv[2];
	else
		soundFile = SB4GetFile(NULL, "AVI video (*.avi, *.wav)\0" "*.avi;*.wav\0", "Select input audio file");

	if(soundFile.SubString(soundFile.Length() - 4, 4).NCSEquals(".avi"))
		sr = new AVISoundReader();
	else
		sr = new LSFSoundReader();

	if(argc >= 4)
		outName = argv[3];
	else
		outName = SB4SaveFile(NULL, "RoQ media file (*.roq)\0" "*.roq\0", ".roq", "Save RoQ output file");

	adpcm = false;
	if(argc >= 5 && !strcmp(argv[4], "adpcm"))
		adpcm = true;

	muxFiles4(sr, soundFile.CStr(), baseFile.CStr(), outName.CStr(), adpcm);

	return 1;
}

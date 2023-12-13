Background
==========

Originally, roq.exe was a tool developed by id software for the Quake 3 engine (now known as the id Tech 3 engine) to develop videos in their proprietary ROQ format. This format essentially links together a stream of TGA images and audio, which is loaded by the renderer.
 
Raven Software developed a new form of roq.exe (roq2) which is a lot more advanced, and has some new features. It's also capable of outputting ROQs in specific dimensions, whereas the original roq.exe was constrained to a specific output dimension.
 
In JKA and JK2, you can render movies and videos from IBI scripts or ingame using a videoMap shader keyword. This can be quite handy.
 
Basic Usage
roq.exe is essentially a command-line application that takes a parameter file. In the documentation, it seems to mention that .param is required, yet it appears to work just fine with any other file type. The parameter file lists all the files which are used for the frames, the audio, and some other miscellaneous stuff.
 
How to make an ROQ
The process is quite simple, as @DT85 has outlined here.
 
First, you'll need to put your frames somewhere where roq.exe can find them. Your path cannot contain spaces. I usually put mine in something like E:\video-editing. (Please see note at bottom for supported image formats)
You'll need to create your .param file. I've detailed the .param file a bit further below.
You'll now need to create a .bat which runs roq.exe. Something like this is appropriate:
roq.exe Your_Param_Here.param
pause
 
The program will then build an ROQ based on the parameters in your .param file. This may take a while! The finished ROQ will show up in the same directory as roq.exe.
If you want the program to not output as many lines, simply add a -q at the end. Or, you can output ROQ frames once the program has finished the file by doing -d <directory to store frames>
 
Details on the .param file
The basic syntax of the .param file follows:
 
++++++++++++++++++++++
INPUT_DIR C:\Where_To_Find_The_Frames
FILENAME _your_roq_here.roq
SOUND _your_audio_here.wav
INPUT
*.tga [000-100]
END_INPUT
//++++++++++++++++++++++
 
Some notes:
-----------

INPUT_DIR cannot have any spaces, as I mentioned before. It tells the program where to find your frames.
FILENAME is the output name of the file.
SOUND is what audio file to use. It must be in the same directory as roq.exe! (alternatively, you could use an absolute path, I think, but again, no spaces) This field is entirely optional.
INPUT/END_INPUT specifies how to process input. So for instance, I used "*.tga [000-100]". This specifies that the frames will be called 000.tga, 001.tga, etc all the way up to 100.tga. You could also do something like "screenshot*.jpg [000=100]" and it'll use screenshot000.jpg all the way up to screenshot100.jpg. Please see note at the bottom for supported image formats.
Some extra fields that you could add to your param file:
SCALE_DOWN - If this line is added to the file, the input will be scaled down. Usage: SCALE_DOWN
FULLSEARCH - Appears to make the frame size bigger (don't know). Usage: FULLSEARCH
SCALEABLE - The ROQ will scale up to higher resolutions, by resampling the ROQ. Usage: SCALEABLE
NO_ALPHA - If this line is added to the file, alpha channel will be ignored entirely (?). Usage: NO_ALPHA
TIMECODE - Only use this line for certain MPEG files that aren't being displayed at the correct resolution. Usage: TIMECODE
HAS_SOUND - Has no actual effect at all.
START_PALETTE - Specifies the name of the palette file to use for start. (I'm not actually sure what these are used for.) The program will search for these in <your path>/LocalLibrary/vdxPalettes Usage: START_PALETTE <palette file name>
END_PALETTE - Specifies the name of the palette file to use for end (see above notes on START_PALETTE) Usage: END_PALETTE <palette file name>
FIXED_PALETTE - Uses this fixed palette for the whole video. (See above notes on START_PALETTE) Usage: FIXED_PALETTE <palette file name>
SCREENSHOT - If this line is added, frames are treated as screenshots instead of frames (Not sure on what the distinction is, I suppose it stores the files whole or something) Usage: SCREENSHOT
KEY_COLOR - Specifies the key color of the video. Usage: KEY_COLOR <R> <G> <B>
JUST_DELTA - Has no effect whatsoever. Usage: JUST_DELTA
CODEBOOK - Makes codebook vector tables. Usage: CODEBOOK
FIRSTFRAMESIZE - Sets the frame size of the first frame. Note that this is only one number, not two. Usage: FIRSTFRAMESIZE <size>
NORMALFRAMESIZE - Sets the frame size that the ROQ is normally displayed at. Note that this is only one number, not two. Usage: NORMALFRAMESIZE <size>
STILLFRAMEQUALITY - Has something to do with JPEG quality, I'm not certain on this yet. Usage: STILLFRAMEQUALITY <quality>
FADEDOWNSTARTFRAME - Specifies the start frame to do a fade down effect. ROQ.exe will fade the video out, starting at this frame. Usage: FADEDOWNSTARTFRAME <framenum>
FADEDOWNDURATION - Specifies how many frames it takes to do a fade down effect. Usage: FADEDOWNDURATION <frames>
FADEUPSTARTFRAME - Similar to FADEDOWNSTARTFRAME, but has the opposite (a fade in effect). This specifies which frame to start this effect on. Usage: FADEUPSTARTFRAME <framenum>
FADEUPDURATION - Specifies how many frames it takes to do a fade up effect. Usage: FADEUPDURATION <frames>
MPEGCROPWIDTH - Crops the MPEG file (if used). Usage: MPEGCROPWIDTH <width>
MPEGCROPHEIGHT - Crops the MPEG file (if used). Usage: MPEGCROPHEIGHT <height>
MPEGCROPXORIGIN - Crops the MPEG file (if used). Usage: MPEGCROPXORIGIN <x origin>
MPEGCROPYORIGIN - Crops the MPEG file (if used). Usage: MPEGCROPYORIGIN <y origin>
FPS - Frames per second. (CAUTION: I'm not sure if this means that it simply duplicates/drops frames in order to maintain FPS, or if it actually changes the rendering FPS for the ROQ...proceed with caution) Usage: FPS <fps>
 
Supported Image Formats
-----------------------
roq.exe uses an image library designed for supercomputers with a huge selection of supported formats. As far as I can tell, the following formats are probably supported:
.jpg, .bmp, .tiff, .tga, .gif, .pcx, .ico, .pic, .pict, .hdf, .pix, .cur, .eps, .epi, .epsi, .epsf, .icon, .cursor, .pr, .iff, .vff, .suniff, .tanc, .miff, .x (NOT directx), .avs, ... (tons of other obscure formats)
I haven't tested all of these.
 
.png is NOT supported however
 
Credits
-------
DT85 for help with ROQ stuff
jedis for putting code up on Github for roq2, this was a big help
San Diego Supercomputer Center, their docs helped me figure out which image formats were supported

Links
-----
Source code for RoQ2: https://github.com/ioquake/jedi-outcast/tree/master/utils/roq2
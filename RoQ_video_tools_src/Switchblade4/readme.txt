Switchblade 4.0


Copyright (c)2007 Eric Lasota/Windshear Interactive
  Portions based on the FFMPEG RoQ encoder (c)2007 Vitor
  Portions based on FFMPEG (c)2006 Michael Niedermayer
  CInputBox (c)2006 legolas558
  LibSNDFile (c)1999-2006 Erik de Castro Lopo


Before you encode, please read the IMPORTANT.txt file.  Videos that aren't
prepped may fail to encode, or fail to play back.

There are three programs in Switchblade4, each of which perform one step of
creating an RoQ video.

ONLY AVIFILE-COMPATIBLE CODECS WILL BE ACCEPTED BY THE UTILITIES.
VIDEOS ENCODED WITH DIRECTSHOW-ONLY CODECS WILL FAIL.

DivX, Indeo Video, and On2's VP series of codecs are AVIfile-compatible.
XviD is not.



--- sbcodebooks4 ---
sbcodebooks4 creates codebooks for an AVI, outputting a .cbc file.  sbvideo4
needs this file to encode.  This is a separate stage so that if you need to
change the quality settings on sbvideo4, you won't need to regenerate the
codebooks.

You can drop an AVI file directly on the sbcodebooks4 executable, or run it
and select it from the dialog.


--- sbvideo4 ---
sbvideo4 takes an AVI and .cbc file and makes an RoQ video out of it.  It
does not do sound.  When you start it, you'll be prompted for the output
bitrate in bits per second.  Higher values will take up proportionally more
disk space, but will look better.

You can drop an AVI file directly on the sbvideo4 executable, or run it and
select the inputs manually.

--- sbmux4 ---
sbmux4 takes an RoQ video and an audio source (either an AVI or WAV file) and
adds adds sound to it.

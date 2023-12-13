#ifndef __SB4_H__
#define __SB4_H__

// FFMPEG-ported defines
#define ROUNDED_DIV(a,b) (((a)>0 ? (a) + ((b)>>1) : (a) - ((b)>>1))/(b))
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))

inline int mid_pred(int a, int b, int c)
{
	if(a>b){
		if(c>b){
            if(c>a) b=a;
            else    b=c;
        }
    }else{
        if(b>c){
            if(c>a) b=c;
            else    b=a;
        }
    }
    return b;
}

template <class _D> inline void Swap(_D &a, _D &b)
{
	_D temp;
	temp = a;
	a = b;
	b = temp;
}

inline void bytestream_put_le32(unsigned char **h, int n)
{
	unsigned char *ptr;
	ptr = *h;

	ptr[0] = (unsigned char)n;
	ptr[1] = (unsigned char)(n>>8);
	ptr[2] = (unsigned char)(n>>16);
	ptr[3] = (unsigned char)(n>>24);
	*h = ptr + 4;
}

inline void bytestream_put_le16(unsigned char **h, int n)
{
	unsigned char *ptr;
	ptr = *h;

	ptr[0] = (unsigned char)n;
	ptr[1] = (unsigned char)(n>>8);
	*h = ptr + 2;
}

inline void bytestream_put_byte(unsigned char **h, int n)
{
	unsigned char *ptr;
	ptr = *h;

	*ptr = (unsigned char)n;
	*h = ptr + 1;
}


// SB4AVI
typedef struct
{
	int width, height;

	int numFrames;
} sb4aviinfo_t;

typedef struct sb4vidstream_s sb4vidstream_t;


void SB4AVIInit();
int SB4GetAVIInfo(const char *fileName, sb4aviinfo_t *aviinfo);
int SB4GetAVIVideoStream(const char *fileName, sb4vidstream_t **vsp);
class SB4Image *SB4GetAVIVideoFrame(sb4vidstream_t *vs);
int SB4CloseAVIVideoStream(sb4vidstream_t *vs);


// CCIR 601-1 YCbCr colorspace.

#define R2Y(n)  (n * 19595)					// 0.299
#define R2CB(n) (n * (-11056))				// -0.1687
#define R2CR(n) (n<<15)						// 0.5

#define G2Y(n)  (n * 38470)					// 0.587
#define G2CB(n) (n * (-21712))				// -0.3313
#define G2CR(n) (n * (-27440))				// -0.4187

#define B2Y(n)  (n * 7471)					// 0.114
#define B2CB(n) (n<<15)						// 0.5
#define B2CR(n) (n * (-5328))				// -0.0813

#define CB2G(n) ( ((int)n-128)*(-22554) )	// -0.34414
#define CB2B(n) ( ((int)n-128)*112853   )	// 1.722

#define CR2R(n) ( ((int)n-128)*91881    )	// 1.402
#define CR2G(n) ( ((int)n-128)*(-46802) )	// -0.71414


static void rgb2yuv(unsigned char r, unsigned char g, unsigned char b,
		  unsigned char *yout, unsigned char *uout, unsigned char *vout)
{

	int y;
	int cb;
	int cr;

	y = (R2Y(r) + G2Y(g) + B2Y(b)) >> 16;
	y = (y < 0) ? 0 : ((y > 255) ? 255 : y);

	cb = ((R2CB(r) + G2CB(g) + B2CB(b)) >> 16) + 128;
	cb = (cb < 0) ? 0 : ((cb > 255) ? 255 : cb);

	cr = ((R2CR(r) + G2CR(g) + B2CR(b)) >> 16) + 128;
	cr = (cr < 0) ? 0 : ((cr > 255) ? 255 : cr);

	*yout = (unsigned char)y;
	*uout = (unsigned char)cb;
	*vout = (unsigned char)cr;
}


static void yuv2rgb(unsigned char y, unsigned char u, unsigned char v,
			  unsigned char *rout, unsigned char *gout, unsigned char *bout)
{
	int r;
	int g;
	int b;

	r = y + (CR2R(v)>>16);
	r = (r < 0) ? 0 : ((r > 255) ? 255 : r);

	g = y + ((CB2G(u) + CR2G(v))>>16);
	g = (g < 0) ? 0 : ((g > 255) ? 255 : g);

	b = y + (CB2B(u)>>16);
	b = (b < 0) ? 0 : ((b > 255) ? 255 : b);

	*rout = (unsigned char)r;
	*gout = (unsigned char)g;
	*bout = (unsigned char)b;
}

const char *SB4GetFile(const char *baseFile, const char *filterStrings, const char *title);
const char *SB4SaveFile(const char *baseFile, const char *filterStrings, const char *extension, const char *title);


#endif

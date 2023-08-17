#include <monohome.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <byteswap.h>


void CGraphics::InitFrameBuffer()
{
    LOG("Initializing framebuffer...\n");
    fbDev = open("/dev/graphics/fb0", O_RDWR);
	
	fb_fix_screeninfo fInfo;
	
	ioctl(fbDev, FBIOGET_VSCREENINFO, &vInfo);
	ioctl(fbDev, FBIOGET_FSCREENINFO, &fInfo);
	
	fbDesc.width = vInfo.xres;
	fbDesc.height = vInfo.yres;
	fbDesc.pixels = (unsigned char*)mmap(0, fInfo.smem_len, PROT_WRITE, MAP_SHARED, fbDev, 0);
	fbDesc.length = fInfo.smem_len;
	fbDesc.lineLength = fInfo.line_length;

    //vInfo.bits_per_pixel = 32;
    //ioctl(fbDev, FBIOPUT_VSCREENINFO, &vInfo);

    backBuffer = (char*)malloc(fbDesc.width * fbDesc.height * 3);
    LOGF("Framebuffer settings is: %i %i %i\n", fbDesc.width, fbDesc.height, vInfo.bits_per_pixel);
}
//export LD_LIBRARY_PATH='.:/sbin:/system/vendor/lib/:/system/vendor/lib/egl/'

void CGraphics::InitContext()
{
    /*LOG("Initializing EGL context...\n");
    // Init EGL context

    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if(display == EGL_NO_DISPLAY)
    {
        LOG("eglGetDisplay failed\n");
        return;
    }

    LOGF("Display %i", display);

    int eglMajor, eglMinor;

    if(eglInitialize(display, &eglMajor, &eglMinor) == EGL_FALSE)
        LOG("EGL failed %i\n", eglGetError());

    LOGF("EGL version %i.%i", eglMajor, eglMinor);

    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    EGLint pbufAttribs[] = {
        EGL_WIDTH, 320,
        EGL_HEIGHT, 480,
        EGL_NONE
    };
LOG("init\n");
    surface = eglCreatePbufferSurface(display, config, (EGLint*)&pbufAttribs);
    LOG("pbuf\n");
    context = eglCreateContext(display, config, NULL, NULL);
    LOG("ctx\n");

    if(!surface)
        LOG("PBuferSurface not valid\n");

    if(!context)
        LOG("Context not valid\n");

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOG("eglMakeCurrent failed\n");
        return;
    }

    LOGF("GL renderer: %s\n", glGetString(GL_RENDERER));

    //eglQuerySurface(display, surface, EGL_WIDTH, &w);
    //eglQuerySurface(display, surface, EGL_HEIGHT, &h);*/
}

CGraphics::CGraphics()
{
    InitFrameBuffer();
    InitContext();

    CColor col;
    col.R = 255;
    col.G = 0;
    col.B = 255;

}

CGraphics::~CGraphics()
{
    close(fbDev);
    free(backBuffer);
}

__inline void __PutPixel565(char* backBuf, int scanlineLength, int x, int y, colorref r, colorref g, colorref b)
{
    /*short c16 = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
    c16 = c16;

    short* absPtr = (short*)&backBuf[y * scanlineLength + (x * 2)];
    *absPtr = c16;*/
}

__inline void __ClipPrimitive(CFrameBuffer* fbDesc, int* dw, int* dh)
{
    if(*dw > fbDesc->width)
        *dw = fbDesc->width - 1;

    if(*dh > fbDesc->height)
        *dh = fbDesc->height - 1;
}

void CGraphics::PutPixel(int x, int y, CColor color)
{
    if(x < 0 || y < 0)
        return;
    
    char* col = &backBuffer[(y * fbDesc.width + x) * 3];
    col[0] = color.R;
    col[1] = color.G;
    col[2] = color.B;
}

void CGraphics::PutPixelAlpha(int x, int y, CColor color, float alpha)
{
    if(x < 0 || y < 0)
        return;
    
    char* col = &backBuffer[(y * fbDesc.width + x) * 3];

    col[0] = (byte)(color.R * alpha + col[0] * (1.0f - alpha));
	col[1] = (byte)(color.G * alpha + col[1] * (1.0f - alpha));
	col[2] = (byte)(color.B * alpha + col[2] * (1.0f - alpha));
}

void CGraphics::LineTo(int x, int y)
{

}

void CGraphics::FillRect(int x, int y, int w, int h, CColor color, float alpha)
{
    w = x + w;
    h = y + h;
    __ClipPrimitive(&fbDesc, &w, &h);


    for(int i = x; i < w; i++)
    {
        for(int j = y; j < h; j++)
            PutPixel(i, j, color);
    }
}

void CGraphics::DrawImage(CImage* img, int x, int y)
{
    if(img)
    {
        if(!img->IsTransparent)
        {
            for(int i = 0; i < img->Height; i++)
            {
                for(int j = 0; j < img->Width; j++)
                {
                    if(j >= fbDesc.width)
                        break;

                    CColor col;
                    unsigned char* pixels = &img->Pixels[((img->Height - i - 1) * img->Width + j) * 3];

                    col.R = pixels[2];
                    col.G = pixels[1];
                    col.B = pixels[0];

                    PutPixel(x + j, y + i, col);
                }

                if(i >= fbDesc.height)
                    break;
            }
        }
        else
        {
            for(int i = 0; i < img->Height; i++)
            {
                for(int j = 0; j < img->Width; j++)
                {
                    if(j >= fbDesc.width)
                        break;

                    CColor col;
                    unsigned char* pixels = &img->Pixels[((img->Height - i - 1) * img->Width + j) * 4];

                    col.R = pixels[2];
                    col.G = pixels[1];
                    col.B = pixels[0];
                    float alpha = (float)pixels[3] / 255;

                    PutPixelAlpha(x + j, y + i, col, alpha);
                }

                if(i >= fbDesc.height)
                    break;
            }
        }
    }
}

void CGraphics::DrawImageEx(CImage* img, int x, int y, float rotation, float alphaBlending, CColor colorMultiply)
{
    
}

// Special variation of DrawImage to draw magenta-transparent images
void CGraphics::DrawGlyph(CImage* img, int x, int y, CColor colorMultiply)
{
    if(img)
    {
        float lR = (float)colorMultiply.R / 255;
        float lG = (float)colorMultiply.G / 255;
        float lB = (float)colorMultiply.B / 255;
        
        for(int i = 0; i < img->Height; i++)
            {
                for(int j = 0; j < img->Width; j++)
                {
                    if(j >= fbDesc.width)
                        break;

                    CColor col;
                    unsigned char* pixels = &img->Pixels[(i * img->Width + j) * 3];

                    col.R = (int)((float)pixels[2] * lR);
                    col.G = (int)((float)pixels[1] * lG);
                    col.B = (int)((float)pixels[0] * lB);

                    if(pixels[0] == 255 && pixels[1] == 0 && pixels[2] == 255)
                        continue;

                    PutPixel(x + j, y + i, col);
                }

                if(i >= fbDesc.height)
                    break;
            }
    }
}

void CGraphics::DrawString(CFont* font, char* str, int x, int y)
{
    CColor col = { 64, 64, 64 };

    DrawStringColored(font, str, x, y, col);
}

void CGraphics::DrawStringColored(CFont* font, char* str, int x, int y, CColor colorMultiply)
{
    if(font && str && strlen(str) > 0)
    {
        
        for(int i = 0; i < strlen(str); i++)
        {
            DrawGlyph(font->Glyphs[str[i]], x + (i * (font->Glyphs[str[i]]->Width - 5)), y, colorMultiply);
        }
    }
}

void CGraphics::Flip()
{
    for(int i = 0; i < fbDesc.width; i++)
    {
        for(int j = 0; j < fbDesc.height; j++)
        {
            short* absPixel = (short*)&fbDesc.pixels[(j * fbDesc.lineLength) + (i * 2)];
            char* absBackPixel = &backBuffer[(j * fbDesc.width + i) * 3];

            short c16 = ((absBackPixel[0] & 0b11111000) << 8) | ((absBackPixel[1] & 0b11111100) << 3) | (absBackPixel[2] >> 3);
            *absPixel = c16;
        }
    }

    // We should pass a bit changed VSCREENINFO structure back to FB driver, to make it update our screen
    // This seems like a bit non-standard behaviour, because Android recovery uses this too: probably, something to save power.
    flip = !flip;
    vInfo.yres_virtual = (int)flip;
    ioctl(fbDev, FBIOPUT_VSCREENINFO, &vInfo);
}

CImage* CImage::FromFile(char* fileName)
{
    FILE* f = fopen(fileName, "r");
	
    LOGF("Loading %s\n", fileName);

	if(!f)
	{
        LOGF("Unable to open %s\n", fileName);
		return 0;
	}

	CTgaHeader hdr;
	fread(&hdr, sizeof(hdr), 1, f);

	if(hdr.paletteType)
	{
		LOG("Palette images are unsupported\n");
		return 0;
	}
	
	if(hdr.bpp != 24 && hdr.bpp != 32)
	{
		LOG("Unsupported BPP\n");
		return 0;
	}

	unsigned char* buf = (unsigned char*)malloc(hdr.width * hdr.height * (hdr.bpp / 8));
	
    if(!buf)
    {
        LOG("Memory exhausted\n");
        return 0;
    }

	//fseek(f, hdr.headerLength, SEEK_SET);

	fread(buf, hdr.width * hdr.height * (hdr.bpp / 8), 1, f);
	fclose(f);
	CImage* ret = new CImage();
	ret->Width = hdr.width;
	ret->Height = hdr.height;
	ret->Pixels = buf;
    ret->IsTransparent = hdr.bpp == 32;

	LOGF("Loaded %s %ix%i\n", fileName, ret->Width, ret->Height);
	return ret;
}

CFont::CFont()
{
    
}

CFont::~CFont()
{
    for(int i = 0; i < GlyphCount; i++)
    {
        delete Glyphs[i];
    }
}

CFont* CFont::FromFile(char* fileName)
{
    FILE* f = fopen(fileName, "r");

	if(!f)
	{
        LOGF("Failed to load %s\n", fileName);
		return 0;
	}

	CFontHeader hdr;
	fread(&hdr, sizeof(hdr), 1, f);

	CFont* fnt = new CFont();

	for(int i = 0; i < 255; i++)
	{
		byte* rgb = (byte*)malloc(hdr.GlyphSize * hdr.GlyphSize * 3);
		fread(rgb, hdr.GlyphSize * hdr.GlyphSize * 3, 1, f);
		
		fnt->Glyphs[i] = new CImage();
        fnt->Glyphs[i]->Width = hdr.GlyphSize;
        fnt->Glyphs[i]->Height = hdr.GlyphSize;
        fnt->Glyphs[i]->IsTransparent = false;
        fnt->Glyphs[i]->Pixels = rgb;
	}

    fnt->GlyphCount = 255;

	fclose(f);
    LOGF("Loaded font %s\n", fileName);

	return fnt;
}
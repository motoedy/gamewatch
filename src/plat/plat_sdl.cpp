#include <ctime>
#include <cmath>
#include "plat/plat_sdl.h"
#include "gwdefs.h"
#include "gwdbg.h"
#ifdef GW_USE_ZDATA
#include <plat/SDL_rwops_zzip.h>
#endif

//////////////////////////////////////////
////
//// GW_PlatformSDL_Image
////
//////////////////////////////////////////
GW_PlatformSDL_Image::GW_PlatformSDL_Image(SDL_Renderer *renderer, const string &filename, GW_Platform_RGB *tcolor)
{
    SDL_Surface* _surface;
#ifndef GW_USE_ZDATA
    _surface = SDL_LoadBMP( filename.c_str() );
#else
    SDL_RWops *l = SDL_RWFromZZIP( filename.c_str(), "rb" );
    if (l)
        _surface = SDL_LoadBMP_RW( l, 1 );
    else
        _surface = NULL;
    //SDL_FreeRW(l);
#endif

    if (!_surface)
        throw GW_Exception(string("Unable to load image "+filename+": "+string(SDL_GetError())));
    if (tcolor)
        SDL_SetColorKey(_surface, SDL_TRUE, SDL_MapRGB(_surface->format, tcolor->r, tcolor->g, tcolor->b));

    texture_ = SDL_CreateTextureFromSurface(renderer, _surface);
    if (!texture_)
            throw GW_Exception(string("Unable to load image "+filename+": "+string(SDL_GetError())));
    SDL_QueryTexture(texture_, NULL, NULL, &w_, &h_);

    // Cleanup ...
    SDL_FreeSurface(_surface);
#ifdef GW_DEBUG
    GWDBG_FVOUTPUT("\tGW_PlatformSDL_Image(file %s, texture %p, width %i, height %i)\n", filename.c_str(), texture_, w_, h_);
#endif
}

bool GW_PlatformSDL_Image::resize_fit(int w, int h)
{
    return false;
}

GW_PlatformSDL_Image::~GW_PlatformSDL_Image()
{
    SDL_DestroyTexture(texture_);
}

//////////////////////////////////////////
////
//// GW_PlatformSDL_Sound
////
//////////////////////////////////////////
GW_PlatformSDL_Sound::GW_PlatformSDL_Sound(const string &filename)
{
#ifndef GW_NO_SDL_MIXER

#ifndef GW_USE_ZDATA
    sample_ = Mix_LoadWAV( filename.c_str() );
#else
    SDL_RWops *l = SDL_RWFromZZIP( filename.c_str(), "rb" );
    if (l)
        sample_ = Mix_LoadWAV_RW( l, 1 );
    else
        sample_=NULL;
    //SDL_FreeRW(l);
#endif

    if (!sample_)
        throw GW_Exception(string("Unable to load sound sample "+filename+": "+string(Mix_GetError())));
#endif
}

GW_PlatformSDL_Sound::~GW_PlatformSDL_Sound()
{
#ifndef GW_NO_SDL_MIXER
	Mix_FreeChunk(sample_);
#endif
}

//////////////////////////////////////////
////
//// GW_PlatformSDL
////
//////////////////////////////////////////
GW_PlatformSDL::GW_PlatformSDL(int width, int height, bool fullscreen) :
    GW_Platform(), width_(width), height_(height), 
	fullscreen_(fullscreen), initialized_(false)
{

}

GW_PlatformSDL::~GW_PlatformSDL()
{
    if (initialized_)
        finalize();
}

void GW_PlatformSDL::initialize()
{
    if (!initialized_)
    {
		GWDBG_OUTPUT("SDL: Initialize");

		// initialize SDL video
        if ( SDL_Init( sdlinit(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER) ) < 0 )
            throw GW_Exception(string("Unable to init SDL: "+string(SDL_GetError())));

#ifndef GW_NO_SDL_MIXER
        if ( Mix_OpenAudio(22050, AUDIO_S16SYS, 1, audiobufsize_get()) < 0)
            throw GW_Exception(string("Unable to init SDL_mixer: "+string(Mix_GetError())));
#endif

#ifndef GW_NO_SDL_TTF
        if ( TTF_Init() < 0 )
            throw GW_Exception(string("Unable to init SDL_ttf: "+string(TTF_GetError())));
#endif

#ifndef GW_NO_SDL_MIXER
		Mix_AllocateChannels(6);
        sound_volume(75);
#endif

        custom_initialize();

        // make sure SDL cleans up before exit
        atexit(SDL_Quit);

        // set application icon
        plat_init();

        // create a new window
        window_ = SDL_CreateWindow("Game & Watch simulator - by Hitnrun / ZsoltK & Madrigal",
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   width_, height_,
                                   fullscreen_ ?  SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_SHOWN);
        if ( !window_ )
            throw GW_Exception(string("Unable to allocate game window: " + string(SDL_GetError())));

        renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
        if ( !renderer_ )
            throw GW_Exception(string("Unable to allocate renderer: " + string(SDL_GetError())));


        // Let SDL & GPU do the scaling
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        SDL_RenderSetLogicalSize(renderer_, width_, height_);

        SDL_ShowCursor(SDL_DISABLE);

#ifndef GW_NO_SDL_TTF
        // load font
        //font_=TTF_OpenFont( bf::path( bf::path(platformdata_get() ) / "andalemo.ttf" ).string().c_str(), fontsize_get() );
		string pfont(platformdata_get() + "/" + "andalemo.ttf" );
		font_=TTF_OpenFont( pfont.c_str(), fontsize_get() );
        if (!font_)
            throw GW_Exception(string("Unable to load font: "+string(TTF_GetError())));
#endif

        initialized_=true;
    }
}

void GW_PlatformSDL::finalize()
{
    if (initialized_)
    {
        custom_finalize();

        // set application icon
        plat_finish();

#ifndef GW_NO_SDL_TTF
        TTF_CloseFont(font_);
        TTF_Quit();
#endif

#ifndef GW_NO_SDL_MIXER
        Mix_CloseAudio();
#endif

        SDL_Quit();

#ifdef GW_DEBUG
		GWDBG_OUTPUT("SDL: Finalize");
#endif

		initialized_=false;
    }
}

unsigned int GW_PlatformSDL::ticks_get()
{
    return SDL_GetTicks();
}

unsigned int GW_PlatformSDL::time_ms_get()
{
    time_t date;
    struct tm  *ts;
    time(&date);
    ts=localtime(&date);

    return (ts->tm_sec + (ts->tm_min * 60) + (ts->tm_hour * 60 * 60)) * 1000;
    //return SDL_GetTicks();
}

GW_Platform_Time GW_PlatformSDL::time_get()
{
    time_t date;
    struct tm  *ts;
    time(&date);
    ts=localtime(&date);

    GW_Platform_Time ret;
    ret.d=ts->tm_mday;
    ret.m=ts->tm_mon+1;
    ret.y=ts->tm_year;
    ret.h=ts->tm_hour;
    ret.n=ts->tm_min;
    ret.s=ts->tm_sec;
    return ret;
}

bool GW_PlatformSDL::event(GW_Platform_GameType gametype,
    GW_Platform_Event *event)
{
#ifdef GW_DEBUG
    GWDBG_OUTPUT("GW_PlatformSDL::event(GW_Platform_GameType gametype, GW_Platform_Event *event)");
#endif
    SDL_Event sdlevent;
    while (true)
    {
        if (SDL_PollEvent(&sdlevent))
        {
            if (process_event(gametype, &sdlevent, event))
                return true;
        }
        else
            return false;
    }
    return false;
}


GW_Platform_Point GW_PlatformSDL::translate_touch_coordinates(GW_Platform_FPoint* touch)
{
    int wnd_w, wnd_h;

    SDL_GetWindowSize(window_, &wnd_w, &wnd_h);
    GW_PLATFORM_POINT(ps,
                      (int)ceil(touch->x * wnd_w),
                      (int)ceil(touch->y * wnd_h))
#ifdef GW_DEBUG
    GWDBG_FVOUTPUT("GW_PlatformSDL::translate_touch_coordinates(touch->x[%f]4, touch->y[%f]) {\n"
        "\t(int)ceil(touch->x[%f] * wnd_w[%d])[%d]\n"
        "\t(int)ceil(touch->y[%f] * wnd_h[%d])[%d]\n}",
        touch->x, touch->y,
        touch->x, wnd_w, (int)ceil(touch->x * wnd_w),
        touch->y, wnd_h, (int)ceil(touch->y * wnd_h));
#endif
    return ps;
}

void GW_PlatformSDL::draw_clear()
{
#ifdef GW_DEBUG
    GWDBG_OUTPUT("GW_PlatformSDL::draw_clear()");
#endif
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
}

void GW_PlatformSDL::draw_image(GW_Platform_Image *image, int x, int y)
{
#ifdef GW_DEBUG
    GWDBG_FVOUTPUT("GW_PlatformSDL::draw_image(image %p, x %i, y %i)\n", image, x, y);
#endif
    if (!image)
        return;

    SDL_Rect tpos;
    tpos.x=x; tpos.y=y;
    tpos.w = image->width_get();
    tpos.h = image->height_get();
    SDL_Texture* texture = dynamic_cast<GW_PlatformSDL_Image*>(image)->texture_get();

#ifdef GW_DEBUG
    GWDBG_FVOUTPUT("\ttpos(x %i, y %i, w %i, h %i)\n", tpos.x, tpos.y, tpos.w, tpos.h);
#endif

    SDL_RenderCopy(renderer_, texture, NULL, &tpos);
}

void GW_PlatformSDL::draw_line(int x1, int y1, int x2, int y2, GW_Platform_RGB *color)
{
#ifdef GW_DEBUG
    GWDBG_FVOUTPUT("GW_PlatformSDL::draw_line(x1 %i, y1 %i, x2 %i, y2 %i, color %p)\n", x1, y1, x2, y2, color);
    GWDBG_RGBOUT(color);
#endif
	GW_PLATFORM_RGB(lcolor,255,255,255);
    if (color) lcolor=*color;

    SDL_SetRenderDrawColor(renderer_, lcolor.r, lcolor.g, lcolor.b, 255);
    SDL_RenderDrawLine(renderer_, x1, y1, x2, y2);
}

void GW_PlatformSDL::draw_rectangle(int x1, int y1, int x2, int y2,
    GW_Platform_RGB *forecolor, GW_Platform_RGB *backcolor, int alpha)
{
#ifdef GW_DEBUG
    GWDBG_FVOUTPUT("GW_PlatformSDL::draw_rectangle(x1 %i, y1 %i, x2 %i, y2 %i,"
                         " fclr %p, bclr %p, alpha %i)\n", x1, y1, x2, y2,
                        forecolor, backcolor, alpha);
    GWDBG_RGBOUT(forecolor);
    GWDBG_RGBOUT(backcolor);
#endif
    SDL_Rect dfrect;

    dfrect.x = x1;
    dfrect.y = y1;
    dfrect.w = x2 - x1;
    dfrect.h = y2 - y1;

    if (forecolor || (!forecolor && !backcolor))
    {
        GW_PLATFORM_RGB(lcolor,255,255,255);
        if (forecolor)
            lcolor=*forecolor;

        SDL_SetRenderDrawColor(renderer_, lcolor.r, lcolor.g, lcolor.b, (alpha > -1 ? alpha : 255));
        SDL_RenderDrawRect(renderer_, &dfrect);
    }
    if (backcolor)
    {
        SDL_SetRenderDrawColor(renderer_, backcolor->r, backcolor->g, backcolor->b, (alpha > -1 ? alpha : 255));
        SDL_RenderFillRect(renderer_, &dfrect);
    }
}


void GW_PlatformSDL::draw_flip()//int output_width, int output_height)
{
#ifdef GW_DEBUG
    GWDBG_OUTPUT("GW_PlatformSDL::draw_flip()");
#endif
    SDL_RenderPresent(renderer_);
}

void GW_PlatformSDL::text_draw(int x, int y, const string &text,
    GW_Platform_RGB *color)
{
#ifndef GW_NO_SDL_TTF
#ifdef GW_DEBUG
    GWDBG_FVOUTPUT("GW_PlatformSDL::text_draw(x %i, y %i, '%s',"
                         " clr %p)\n", x, y, text.c_str(), color);
    GWDBG_RGBOUT(color);
#endif
    SDL_Surface *tsurf;
    SDL_Color tcolor={255,255,255,255};
    if (color)
    {
        tcolor.r=color->r; tcolor.g=color->g; tcolor.b=color->b;
    }

    SDL_Rect tpos;
    tpos.x=x; tpos.y=y;
    //tsurf=TTF_RenderText_Solid(font_, text.c_str(), tcolor);
    GWDBG_RGBAOUT(tcolor);

    tsurf=TTF_RenderText_Blended(font_, text.c_str(), tcolor);
    tpos.w=tsurf->w;
    tpos.h=tsurf->h;

    if (tsurf) {
        SDL_Texture *ttext = SDL_CreateTextureFromSurface(renderer_, tsurf);
        SDL_FreeSurface(tsurf);
        SDL_RenderCopy(renderer_, ttext, NULL, &tpos);
        SDL_DestroyTexture(ttext);
    }
#endif
}

int GW_PlatformSDL::text_fontheight()
{
#ifndef GW_NO_SDL_TTF
    return TTF_FontLineSkip(font_);
#else
	return 1;
#endif
}

int GW_PlatformSDL::text_width(const string &text)
{
#ifndef GW_NO_SDL_TTF
    int s;
    if (TTF_SizeText(font_, text.c_str(), &s, NULL) == 0)
        return s;
    return -1;
#else
	return 1;
#endif
}

int GW_PlatformSDL::text_height(const string &text)
{
#ifndef GW_NO_SDL_TTF
    int s;
    if (TTF_SizeText(font_, text.c_str(), NULL, &s) == 0)
        return s;
#endif
	return -1;
}

void GW_PlatformSDL::sound_play(GW_Platform_Sound *sound)
{
#ifndef GW_NO_SDL_MIXER
	Mix_PlayChannel(-1, dynamic_cast<GW_PlatformSDL_Sound*>(sound)->sample_get(), 0);
#endif
}

void GW_PlatformSDL::sound_stop_all()
{
#ifndef GW_NO_SDL_MIXER
	Mix_HaltChannel(-1);
#endif
}

unsigned short GW_PlatformSDL::sound_volume(unsigned short volume)
{
#ifndef GW_NO_SDL_MIXER
	//if (volume<0) volume=0;
    if (volume>100) volume=100;

    Mix_Volume(-1, (MIX_MAX_VOLUME/100)*volume);
    return volume;
#else
	return 0;
#endif
}

GW_Platform_Image *GW_PlatformSDL::image_load(const string &filename, GW_Platform_RGB *tcolor)
{
    return new GW_PlatformSDL_Image(renderer_, filename, tcolor);
}

GW_Platform_Sound *GW_PlatformSDL::sound_load(const string &filename)
{
	return new GW_PlatformSDL_Sound(filename);
}

#ifdef GW_DEBUG
void GW_PlatformSDL::draw_touch(const char* log, GW_Platform_Point* touch) {
    SDL_Rect rect;
    rect.x=touch->x;
    rect.y=touch->y;
    rect.w=2;
    rect.h=2;

    unsigned char r, g, b, a;
    SDL_GetRenderDrawColor(renderer_, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);

    SDL_RenderFillRect(renderer_, &rect);
    GWDBG_FVOUTPUT(log, touch->x, touch->y);

    SDL_SetRenderDrawColor(renderer_, r, g, b, a);
}
#endif

bool GW_PlatformSDL::process_event(GW_Platform_GameType gametype,
    SDL_Event *sdlevent, GW_Platform_Event *event)
{
#ifdef GW_DEBUG
    GWDBG_LOG("gamewatch-event", "GW_PlatformSDL::process_event().type :: %u", sdlevent->type);
#endif
    switch (sdlevent->type)
    {
    // QUIT
    case SDL_QUIT:
        event->id=GPE_QUIT;
        break;
    // Keypress
    case SDL_KEYDOWN:
        {
            event->id=GPE_KEYDOWN;
#ifdef GW_DEBUG
            GWDBG_FOUTPUT("KEY: %d\n", sdlevent->key.keysym.sym);
#endif
            switch (sdlevent->key.keysym.sym)
            {
            case SDLK_LEFT:
                event->data=GPK_LEFT;
                break;
            case SDLK_RIGHT:
                event->data=GPK_RIGHT;
                break;
            case SDLK_UP:
                event->data=GPK_UP;
                break;
            case SDLK_DOWN:
                event->data=GPK_DOWN;
                break;
            case SDLK_q:
                event->data=GPK_UPLEFT;
                break;
            case SDLK_a:
                event->data=GPK_DOWNLEFT;
                break;
            case SDLK_p:
                event->data=GPK_UPRIGHT;
                break;
            case SDLK_l:
                event->data=GPK_DOWNRIGHT;
                break;
            case SDLK_RETURN:
                event->data=GPK_TURNON;
                break;
            case SDLK_BACKSPACE:
                event->data=GPK_TURNOFF;
                break;
            case SDLK_ESCAPE:
                event->data=GPK_QUIT;
                break;
            case SDLK_1:
                event->data=GPK_GAMEA;
                break;
            case SDLK_2:
                event->data=GPK_GAMEB;
                break;
            case SDLK_3:
                event->data=GPK_TIME;
                break;
            case SDLK_PLUS:
                event->data=GPK_VOLUP;
                break;
            case SDLK_MINUS:
                event->data=GPK_VOLDOWN;
                break;
			case SDLK_7:
				event->data=GPK_ZOOM_NORMAL;
				break;
			case SDLK_8:
				event->data=GPK_ZOOM_DEVICE;
				break;
			case SDLK_9:
				event->data=GPK_ZOOM_GAME;
				break;
            default:
                return false;
            }
        }
        break;
    default:
#ifdef GW_DEBUG
        GWDBG_OUTPUT("return false");
#endif
        return false;
    }
#ifdef GW_DEBUG
    GWDBG_OUTPUT("return true");
#endif
    return true;
}

void GW_PlatformSDL::plat_init()
{
#if defined(_WIN32) && !defined(GW_NO_WIN32)
    HWND hwnd;

    HINSTANCE handle = ::GetModuleHandle(NULL);
    icon_ = ::LoadIcon(handle, "GWICON");

    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version)
    if (SDL_GetWMInfo(&wminfo) != 1)
    {
    // error: wrong SDL version
    }

    hwnd = wminfo.window;

    ::SetClassLong(hwnd, GCL_HICON, (LONG) icon_);

    //oldProc = (WNDPROC) ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG) WndProc);
#endif //_WIN32
}

void GW_PlatformSDL::plat_finish()
{
#if defined(_WIN32) && !defined(GW_NO_WIN32)
    ::DestroyIcon(icon_);
#endif //_WIN32
}


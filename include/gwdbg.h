#ifndef H__GWDBG__H
#define H__GWDBG__H

#ifdef GW_DEBUG

#ifdef GW_PLAT_ANDROID
#include <android/log.h>

#define GWDBG_OUTPUT(x)		__android_log_print(ANDROID_LOG_INFO, "gamewatch", "%s\n", x)
#define GWDBG_FOUTPUT(x, y)		__android_log_print(ANDROID_LOG_INFO, "gamewatch", x, y)
#define GWDBG_FVOUTPUT(x, args...)	__android_log_print(ANDROID_LOG_INFO, "gamewatch", x, ## args)
#define GWDBG_RGBAOUT(tcolor) __android_log_print(ANDROID_LOG_INFO, "gamewatch", "tcolor(red :: %i, green :: %i, blue :: %i, alpha :: %i), ", tcolor.r, tcolor.g, tcolor.b, tcolor.a)
#define GWDBG_RGBOUT(tcolor) __android_log_print(ANDROID_LOG_INFO, "gamewatch", "tcolor(red :: %i, green :: %i, blue :: %i), ", tcolor->r, tcolor->g, tcolor->b)
#define GWDBG_LOG(x, y, args...) __android_log_print(ANDROID_LOG_INFO, x, y, ## args)

#else
#define GWDBG_OUTPUT(x)		printf("%s\n", x)
#define GWDBG_FOUTPUT(x, y)		printf(x, y)
#define GWDBG_FVOUTPUT(x, args...)		printf(x, ## args)
#define GWDBG_RGBAOUT(tcolor)		printf("tcolor(red :: %i, green :: %i, blue :: %i, alpha :: %i), ", tcolor.r, tcolor.g, tcolor.b, tcolor.a)
#define GWDBG_RGBOUT(tcolor)		printf("tcolor(red :: %i, green :: %i, blue :: %i), ", tcolor->r, tcolor->g, tcolor->b)
#define GWDBG_LOG(x, y, args...) printf(x, ## args)
#endif

#else

#define GWDBG_OUTPUT(x)
#define GWDBG_FOUTPUT(x, y)
#define GWDBG_FVOUTPUT(x, args...)
#define GWDBG_RGBAOUT(tcolor)
#define GWDBG_RGBOUT(tcolor)
#define GWDBG_LOG(x, y, args...) 

#endif

#endif // H__GWDBG__H

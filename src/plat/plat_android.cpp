#include "gwdefs.h"
#include "plat/plat_android.h"
#include "gwdbg.h"

#define GWSDLK_MENU  1073741942
#define GWSDLK_BACK  1073742094
#define GWSDLK_SEARCH  1073742092

bool GW_PlatformAndroid::process_event(GW_Platform_GameType gametype,
    SDL_Event *sdlevent, GW_Platform_Event *event)
{
#ifdef GW_DEBUG
    GWDBG_LOG("gamewatch-event", "GW_PlatformAndroid::process_event().type :: %u", sdlevent->type);
#endif
    bool proc=false;
    if (sdlevent->type == SDL_FINGERDOWN)
    {
#ifdef GW_DEBUG
        GWDBG_OUTPUT("case SDL_FINGERDOWN:");
#endif
        SDL_TouchFingerEvent* inTouch = &sdlevent->tfinger;
        if(inTouch != NULL)
        {
            proc = true;
            event->id=GPE_TOUCH;
            GW_PLATFORM_FPOINT(_ps, inTouch->x, inTouch->y);
            event->raw_point=_ps;
            event->translated_point = translate_touch_coordinates(&event->raw_point);
#ifdef GW_DEBUG
            GWDBG_FVOUTPUT("FINGER DOWN: x=%f - y=%f", event->raw_point.x, event->raw_point.y);
#endif
        }
    }

    if (!proc)
        return GW_PlatformSDL::process_event(gametype, sdlevent, event);
    return true;
}

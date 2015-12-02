#include <SDL.h>

#define GW_DEBUG 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "gwdefs.h"
#include "gwdbg.h"
#include "device.h"
#include "devices/dev_monkey.h"
#include "plat/plat_sdl.h"
#ifdef GW_PLAT_ANDROID
#include "plat/plat_android.h"
#endif
#ifdef GW_PLAT_IOS
#include "plat/plat_ios.h"
#endif
/*
#ifdef _WIN32
#include <windows.h>
#endif
*/
#include "gamelist.h"
#include "gwmenu.h"

/*
#include <sdlmain.h>
#include <sdlepocapi.h>
*/

#include "util/anyoption.h"

//class GW_Menu;

//#ifndef _WIN32
//int main(int argc, char** argv)
#ifdef GW_PLAT_ANDROID
int SDL_main(int argc, char** argv)
#else
int main(int argc, char** argv)
#endif
/*
#else
int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
#endif
*/
{
	string datapath;

	AnyOption *opt = new AnyOption();
	
    opt->addUsage( "" );
    opt->addUsage( "Usage: " );
    opt->addUsage( "" );
    opt->addUsage( " -h  --help			Prints this help " );
    opt->addUsage( " -d  --data-path /dir		Data files path " );
    opt->addUsage( "" );

	opt->setFlag(  "help", 'h' );
	opt->setOption(  "data-path", 'd' );

	opt->processCommandArgs( argc, argv );

    if( opt->getFlag( "help" ) || opt->getFlag( 'h' ) ) 
	{
    	GWDBG_OUTPUT("Usage");
        opt->printUsage();
		delete opt;
		return 0;
	}
	if( opt->getValue( 'd' ) != NULL  || opt->getValue( "data-size" ) != NULL  )
		datapath = opt->getValue('d');

	delete opt;

    GW_PlatformSDL* platform;
    try
    {
#ifdef GW_PLAT_ANDROID
        platform = new GW_PlatformAndroid();
#elif defined(GW_PLAT_IOS)
        platform = new GW_PlatformIOS(640, 480)
#else
        platform = new GW_PlatformSDL(640, 480);
#endif
		if (!datapath.empty())
			platform->datapath_set(datapath);

        platform->initialize();
        GWDBG_OUTPUT("platform->initialize();");

        GW_GameList gamelist(platform);
        GWDBG_OUTPUT("GW_GameList gamelist(platform);");

        GW_Menu menu(platform, &gamelist);
        GWDBG_OUTPUT("GW_Menu menu(platform, &gamelist);");
        menu.Run();
        GWDBG_OUTPUT("menu.Run();");

        platform->finalize();
    } catch (GW_Exception &e) {
    	GWDBG_OUTPUT(e.what().c_str());
        fprintf(stderr, "%s\n", e.what().c_str());
        return 1;
    }
    delete platform;

    return 0;
}

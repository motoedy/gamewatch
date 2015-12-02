#include <chrono>
#include <thread>
#include <time.h>

#include "gwdbg.h"
#include "gwmenu.h"

void GW_Menu::Run()
{
#ifdef GW_DEBUG
    GWDBG_OUTPUT("GW_Menu::Run()");
#endif
    if (gamelist_->count()>0)
        current_set(0);

    bool quit=false;
    while (!quit)
    {
#ifdef GW_DEBUG
        GWDBG_OUTPUT("*** *** *** W_Menu::Run() *** *** ***");
#endif
        GW_Platform_Event event;
        while (platform_->event(GPG_MENU, &event))
        {
#ifdef GW_DEBUG
            GWDBG_OUTPUT("while (platform_->event(GPG_MENU, &event))");
#endif
            if (!process_event(&event))
            {
                quit=true;
                break;
            }
        }
        if (quit)
            break;

        if (changed_)
        {
#ifdef GW_DEBUG
            GWDBG_OUTPUT("if (changed_)");
#endif
            // clear screen
            platform_->draw_clear();
#ifdef GW_DEBUG
            GWDBG_OUTPUT("platform_->draw_clear();");
#endif

            draw_bg();
#ifdef GW_DEBUG
            GWDBG_OUTPUT("draw_bg();");
#endif

            draw_title();
#ifdef GW_DEBUG
            GWDBG_OUTPUT("draw_title();");
#endif
            draw_gamelist();
#ifdef GW_DEBUG
            GWDBG_OUTPUT("draw_gamelist();");
#endif

            platform_->draw_flip();
#ifdef GW_DEBUG
            GWDBG_OUTPUT("platform_->draw_flip();");
#endif
            changed_=false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    } // end main loop
}

bool GW_Menu::process_event(GW_Platform_Event *event)
{
#ifdef GW_DEBUG
    GWDBG_OUTPUT("GW_Menu::process_event(GW_Platform_Event *event)");
#endif
    switch (event->id)
    {
    case GPE_QUIT:
        return false;
    case GPE_KEYDOWN:
        switch (event->data)
        {
        case GPK_QUIT:
            return false;
        case GPK_TURNON:
        case GPK_TURNONTOGGLE:
            rungame();
            changed_=true;
            // selected
            break;
        case GPK_UP:
            current_set(current_-1);
            break;
        case GPK_DOWN:
            current_set(current_+1);
            break;
        }
    case GPE_TOUCH:
        {
#ifdef GW_DEBUG
            GWDBG_OUTPUT("GW_Menu::process_event(GPE_TOUCH)");
            GWDBG_FVOUTPUT("GW_Menu::process_event(touch_map_.size() = %u)", touch_map_.size());
#endif

            GW_Platform_Point ps = platform_->translate_touch_coordinates(&event->raw_point);


            for (int i = 0; i<touch_map_.size(); i++) {
                GW_Platform_Rect* bound_rect_ = touch_map_[i];
#ifdef GW_DEBUG
                GWDBG_FVOUTPUT("GW_Menu::process_event(bound_rect_(%i, %i, %i, %i) / finger(%i, %i))\n",
                               bound_rect_->x, bound_rect_->y, bound_rect_->w, bound_rect_->h,
                               ps.x, ps.y);
#endif

                if ((bound_rect_->x <= ps.x && bound_rect_->x + bound_rect_->w >= ps.x) &&
                    (bound_rect_->y <= ps.y && bound_rect_->y + bound_rect_->y >= ps.y))
                {
#ifdef GW_DEBUG
                    GWDBG_OUTPUT("GW_Menu::process_event(GPE_TOUCH - Match)");
#endif
                    current_set(i);
                    rungame();
                    changed_=true;
                }
            }
        }
        break;
    default:
        break;
    }
    return true;
}

void GW_Menu::draw_text_center(int y, const string &text)
{
#ifdef GW_DEBUG
    GWDBG_FVOUTPUT("GW_Menu::draw_text_center(y %i, '%s')\n", y, text.c_str());
#endif
    platform_->text_draw(
            (platform_->width_get() / 2) - (platform_->text_width(text) / 2),
            y-(platform_->text_height(text) / 2),
            text);
}

void GW_Menu::draw_textbox_center(int y, const string &text, GW_Platform_RGB &color, GW_Platform_Rect *touch_rect, bool box)
{
#ifdef GW_DEBUG
    GWDBG_FVOUTPUT("GW_Menu::draw_textbox_center(y %i, '%s', clr %p)\n", y, text.c_str(), &color);
#endif
    int xdraw=(platform_->width_get() / 2) - (platform_->text_width(text) / 2);
    int ydraw=y-(platform_->text_height(text) / 2);

    if (box)
        platform_->draw_rectangle(xdraw, ydraw, xdraw+platform_->text_width(text), ydraw+platform_->text_fontheight(), NULL, &color);

    platform_->text_draw(xdraw, ydraw, text);

    // Set up the touch area
    if (touch_rect) {
        touch_rect->x = xdraw;
        touch_rect->y = ydraw;
        touch_rect->w = platform_->text_width(text);
        touch_rect->h = platform_->text_fontheight();
#ifdef GW_DEBUG
        GWDBG_FVOUTPUT("touch_rect[x - %i, y - %i, w - %i, h - %i]", touch_rect->x, touch_rect->y, touch_rect->w, touch_rect->h);
#endif
    }
}

void GW_Menu::draw_title()
{
#ifdef GW_DEBUG
    GWDBG_OUTPUT("GW_Menu::draw_title()");
#endif
    const int spacing=10;

    draw_title_and_line(spacing, spacing, "Game & Watch Simulator");
    draw_title_and_line(platform_->height_get()-spacing, spacing, "by Hitnrun - ZsoltK and Madrigal");

    platform_->draw_line(spacing, spacing, spacing, platform_->height_get()-spacing);
    platform_->draw_line(platform_->width_get()-spacing, spacing, platform_->width_get()-spacing, platform_->height_get()-spacing);
}

void GW_Menu::draw_title_and_line(int y, int spacing, const string &title)
{
#ifdef GW_DEBUG
    GWDBG_FVOUTPUT("GW_Menu::draw_title_and_line(y %i, spacing %i, '%s')\n", y, spacing, title.c_str());
#endif
    int titwidth=platform_->text_width(title);

    draw_text_center(y, title);
    platform_->draw_line(spacing, y, ((platform_->width_get()-titwidth)/2)-spacing, y);
    platform_->draw_line(titwidth+((platform_->width_get()-titwidth)/2)+spacing, y, platform_->width_get()-spacing, y);
}

void GW_Menu::draw_gamelist()
{
#ifdef GW_DEBUG
    GWDBG_OUTPUT("GW_Menu::draw_gamelist()");
#endif
    int dpos=5+(2*platform_->text_fontheight());

    GW_PLATFORM_RGB(boxcolor, 255, 0, 0);
    for (int i=0; i<gamelist_->count(); i++)
    {
#ifdef GW_DEBUG
        GWDBG_FVOUTPUT("touch_map_[%i] = %p", i, touch_map_[i]);
#endif
        if (i==current_)
            draw_textbox_center(dpos, gamelist_->get(i)->description(), boxcolor, touch_map_[i],true);
        else
            draw_textbox_center(dpos, gamelist_->get(i)->description(), boxcolor, touch_map_[i],false);

        dpos+=platform_->text_fontheight();
    }
}

void GW_Menu::draw_bg()
{
#ifdef GW_DEBUG
    GWDBG_OUTPUT("GW_Menu::draw_bg()");
#endif
    if (bg_.get())
    {
        int xdraw=(platform_->width_get() / 2) - (bg_->width_get() / 2);
        int ydraw=(platform_->height_get() / 2) - (bg_->height_get() / 2);

        platform_->draw_image(bg_.get(), xdraw, ydraw);
    }
}

void GW_Menu::rungame()
{
    GW_Game *game=gamelist_->get(current_)->create();
    GW_Device device(platform_);
    device.Run(game);
    delete game;
}

void GW_Menu::current_set(int cur)
{
    current_=cur;
    if (current_<0) current_=0;
    if (current_>gamelist_->count()-1) current_=gamelist_->count()-1;
    if (current_>-1)
    {
        bg_.reset(platform_->image_load(gamelist_->get(current_)->bgimg_path(),
            (gamelist_->get(current_)->istcolor()?&gamelist_->get(current_)->tcolor():NULL)));
        bg_->resize_fit(platform_->width_get(), platform_->height_get());
    }
    else
        bg_.release();
    changed_=true;
}

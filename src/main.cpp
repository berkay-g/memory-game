#include "App.h"

#include "SDL2_framerate.h"

int EventFilter(void *userdata, SDL_Event *event)
{
    if (event->type == SDL_EVENT_WINDOW_RESIZED || event->type == SDL_EVENT_WINDOW_MOVED)
    {
        // Handle window resize event here
        App *pThis = reinterpret_cast<App *>(userdata);
        pThis->SetWindowWidthHeight(event->window.data1, event->window.data2);
        pThis->Draw();

        // SDL_Log("Window resized: %dx%d\n", event->window.data1, event->window.data2);
    }

    return 1;
}

int main()
{
    App app("memory", 640, 360, SDL_WINDOW_RESIZABLE, SDL_RENDERER_ACCELERATED, SDL_INIT_VIDEO);
    app.SetWindowMinimumSize(640, 360);
    SDL_SetEventFilter(EventFilter, &app);

    ImGuiIO &io = app.ImguiInit();

    app.Setup();

    FPSmanager fps;
    SDL_initFramerate(&fps);
    SDL_setFramerate(&fps, 60);

    SDL_Event event;
    bool quit = false;
    while (!quit)
    {
        app.Update(event, quit, io.DeltaTime);
        app.Draw();
        SDL_framerateDelay(&fps);
    }

    return 0;
}

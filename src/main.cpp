#include "App.h"
#include "util.h"

#include "SDL2_framerate.h"
#include "SDL2_gfxPrimitives.h"

#include <vector>
#include <algorithm>
#include <random>

struct Pie
{
    Sint16 x, y, radius, start_angle, end_angle;

    Pie(){};
    Pie(Sint16 x, Sint16 y, Sint16 radius, Sint16 start_angle, Sint16 end_angle)
        : x(x), y(y), radius(radius), start_angle(start_angle), end_angle(end_angle)
    {
    }

    int draw(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
    {
        return filledPieRGBA(renderer, x, y, radius, start_angle, end_angle, r, g, b, a);
    }

    int draw(SDL_Renderer *renderer, const Color &color)
    {
        return filledPieRGBA(renderer, x, y, radius, start_angle, end_angle, color.r, color.g, color.b, color.a);
    }
};

constexpr bool IsMouseInsidePie(float mouse_x, float mouse_y, const Pie &pie)
{
    // Calculate distance from mouse to pie center
    float dx = mouse_x - pie.x;
    float dy = mouse_y - pie.y;
    float distance_squared = dx * dx + dy * dy;

    // Check if mouse is within the radius of the pie
    if (distance_squared <= (pie.radius * pie.radius))
    {
        // Calculate angle from the center of the pie to the mouse position
        double angle = atan2(dy, dx) * 180 / M_PI;

        // Normalize angle to be between 0 and 360 degrees
        if (angle < 0)
        {
            angle += 360;
        }

        // Check if angle is within the pie's start and end angles
        if (angle >= pie.start_angle && angle <= pie.end_angle)
        {
            return true;
        }
    }

    return false;
}

constexpr bool IsMouseInsideThickLine(float mouse_x, float mouse_y, float x1, float y1, float x2, float y2, float width)
{
    // Calculate the bounding box of the thick line
    float min_x = (x1 < x2) ? x1 : x2;
    float max_x = (x1 > x2) ? x1 : x2;
    float min_y = (y1 < y2) ? y1 : y2;
    float max_y = (y1 > y2) ? y1 : y2;

    // Check if the mouse is inside the bounding box
    if (mouse_x >= min_x - width / 2 && mouse_x <= max_x + width / 2 &&
        mouse_y >= min_y - width / 2 && mouse_y <= max_y + width / 2)
    {
        // Calculate the distance between the mouse and the line
        float dx = x2 - x1;
        float dy = y2 - y1;
        double dist = fabs(dy * mouse_x - dx * mouse_y + x2 * y1 - y2 * x1) / sqrt(dx * dx + dy * dy);

        // Check if the distance is within half the width of the line
        if (dist <= width / 2)
        {
            return true;
        }
    }

    return false;
}

bool states[4] = {0};
void setPieState(bool states[4], int index)
{
    for (int i = 0; i < 4; i++)
        states[i] = false;

    if (index >= 0 && index <= 4)
        states[index] = true;
}

Pie yellow(200, 200, 190, 180, 270);
Pie blue(200, 200, 190, 270, 360);
Pie red(200, 200, 190, 90, 180);
Pie green(200, 200, 190, 0, 90);

Pie black(200, 200, 80, 0, 359);
Pie gray(200, 200, 70, 0, 359);

constexpr bool IsMouseInsideButton(float mouse_x, float mouse_y, const Pie &pie)
{
    if (IsMouseInsidePie(mouse_x, mouse_y, pie) &&
        !IsMouseInsideThickLine(mouse_x, mouse_y, 0, 200, 400, 200, 10) &&
        !IsMouseInsideThickLine(mouse_x, mouse_y, 200, 0, 200, 400, 10) &&
        !IsMouseInsidePie(mouse_x, mouse_y, black))
    {
        return true;
    }
    return false;
}

bool pause = true;
bool turn = false;
double deltaTime = 0;

std::vector<int> numbers(10);

double count = 0;
int index = 0;

void test(double time_interval, int level)
{
    setPieState(states, numbers[index]);

    count += deltaTime;
    if (count >= time_interval)
    {
        count -= time_interval;
        index++;
        setPieState(states, -1);

        if (index > level)
        {
            count = 0;
            index = 0;
            turn = true;
        }
    }
}

inline void fillVectorRandom(std::vector<int> &vec)
{   
    vec.clear();
    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 3);

    // Generate all four numbers at least once
    for (int i = 0; i < 4; ++i) {
        int randomNumber;
        do {
            randomNumber = dis(gen);
        } while (std::find(vec.begin(), vec.end(), randomNumber) != vec.end());
        vec.push_back(randomNumber);
    }

    // Fill the rest of the vector with the logic to avoid three consecutive repetitions
    const int desiredSize = 10; // Adjust as needed
    for (int i = vec.size(); i < desiredSize; ++i) {
        int randomNumber;
        int lastNumber = vec.back();
        int secondLastNumber = vec[vec.size() - 2];
        do {
            randomNumber = dis(gen);
        } while (randomNumber == lastNumber && randomNumber == secondLastNumber);
        vec.push_back(randomNumber);
    }

    // Shuffle the numbers to make them appear more random
    std::shuffle(vec.begin(), vec.end(), gen);
}

std::vector<int> guess;
int level = 0;
double time_interval = 1.0;

bool wait = false;
double wait_counter = 0.0;

bool lit[4] = {0};
double lit_counter[4] = {0};
void cheat();

void reset()
{
    pause = true;
    count = 0;
    index = 0;
    turn = false;
    time_interval = 1.0;
    level = 0;
    fillVectorRandom(numbers);
    guess.clear();
    cheat();
}

void draw(App *app)
{
    SDL_Renderer *renderer = app->GetSDLRenderer();
    ImGuiIO &io = app->ImguiNewFrame();
    app->SetRenderDrawColor({100, 100, 100});
    SDL_RenderClear(renderer);

    if (wait)
        wait_counter += deltaTime;
    if (wait_counter >= 1.2)
    {
        wait_counter -= 1.2;
        wait = false;
        turn = false;
    }

    for (int i = 0; i < 4; i++)
    {
        if (lit[i])
            lit_counter[i] += deltaTime;
        if (lit_counter[i] >= 0.25)
        {
            lit_counter[i] -= 0.25;
            lit[i] = false;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        if (lit[i])
        {
            states[i] = true;
        }
        else
            states[i] = false;
    }

    if (!turn && !pause)
    {
        test(time_interval, level);
    }

    if (guess.size() == numbers.size())
    {
        SDL_Log("you won");
        SDL_SetWindowTitle(app->GetSDLWindow(), "You Won!");
        reset();
    }

    for (size_t i = 0; i < guess.size(); i++)
    {
        if (guess[i] != numbers[i])
        {
            SDL_Log("try again");
            SDL_SetWindowTitle(app->GetSDLWindow(), "You Lost!");
            reset();
            break;
        }

        if (i == level)
        {
            guess.clear();
            wait = true;
            setPieState(states, -1);
            level++;
            time_interval -= 0.05;
        }
    }

    ImGui::SetNextWindowPos({0, 0}, ImGuiCond_FirstUseEver);
    ImGui::Begin("menu", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    if (ImGui::Button("start"))
    {
        pause = false;
        SDL_SetWindowTitle(app->GetSDLWindow(), "App");
    }
    ImGui::End();

    app->DrawString(std::to_string(level), {360, 0}, 200, {255, 255, 255, 255}, 1);

    filledCircleRGBA(renderer, 200, 200, 200, 20, 20, 20, 255);

    yellow.draw(renderer, 255, 255, 0, states[0] ? 255 : 50);
    blue.draw(renderer, 30, 50, 255, states[1] ? 255 : 50);
    red.draw(renderer, 255, 0, 0, states[2] ? 255 : 50);
    green.draw(renderer, 0, 255, 0, states[3] ? 255 : 50);

    black.draw(renderer, 20, 20, 20, 255);
    thickLineRGBA(renderer, 0, 200, 400, 200, 10, 20, 20, 20, 255);
    thickLineRGBA(renderer, 200, 0, 200, 400, 10, 20, 20, 20, 255);
    gray.draw(renderer, 100, 100, 100, 255);

    app->ImguiRender();
    // Update the screen
    SDL_RenderPresent(renderer);
}

void cheat()
{
    for (int num : numbers)
    {
        switch (num)
        {
        case 0:
            printf("y ");
            break;
        case 1:
            printf("b ");
            break;
        case 2:
            printf("r ");
            break;
        case 3:
            printf("g ");
            break;
        }
    }
    printf("\n");
}

int EventFilter(void *userdata, SDL_Event *event)
{

    if (event->type == SDL_EVENT_WINDOW_RESIZED || event->type == SDL_EVENT_WINDOW_MOVED)
    {
        // Handle window resize event here
        App *pThis = reinterpret_cast<App *>(userdata);

        pThis->SetWindowWidthHeight(event->window.data1, event->window.data2);

        draw(pThis);

        // SDL_Log("Window resized: %dx%d\n", event->window.data1, event->window.data2);
    }

    return 1;
}

int main()
{
    App app("App", 400, 400, SDL_WINDOW_RESIZABLE, SDL_RENDERER_SOFTWARE, SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    app.SetWindowMinimumSize(400, 400);
    SDL_SetEventFilter(EventFilter, &app);

    ImGuiIO &io = app.ImguiInit();

    fillVectorRandom(numbers);
    cheat();

    FPSmanager fps;
    SDL_initFramerate(&fps);
    SDL_setFramerate(&fps, 60);

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;

    SDL_Event event;
    int quit = 0;
    while (!quit)
    {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = ((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                quit = 1;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    quit = 1;

                if (event.key.keysym.sym == SDLK_k)
                    SDL_Log("fps = %lf", 1 / deltaTime);

                if (event.key.keysym.sym == SDLK_v)
                {
                }

                break;
            // case SDL_EVENT_MOUSE_BUTTON_UP:
            //     break;
            case SDL_EVENT_MOUSE_MOTION:
                if (!turn || wait)
                    break;

                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (!turn || wait)
                    break;

                float mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                if (IsMouseInsideButton(mouseX, mouseY, yellow))
                {
                    guess.push_back(0);
                    lit[0] = true;
                }
                else if (IsMouseInsideButton(mouseX, mouseY, blue))
                {
                    guess.push_back(1);
                    lit[1] = true;
                }
                else if (IsMouseInsideButton(mouseX, mouseY, red))
                {
                    guess.push_back(2);
                    lit[2] = true;
                }
                else if (IsMouseInsideButton(mouseX, mouseY, green))
                {
                    guess.push_back(3);
                    lit[3] = true;
                }
                else
                    setPieState(states, -1);
                break;
                // case SDL_EVENT_MOUSE_WHEEL:
                //     break;
            }
        }

        draw(&app);

        SDL_framerateDelay(&fps);
    }

    return 0;
}
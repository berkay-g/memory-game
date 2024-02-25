#include "App.h"

#include "SDL2_gfxPrimitives.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <vector>
#include <algorithm>
#include <random>

ma_result result;
ma_engine engine;

struct pie
{
    Sint16 x, y, radius, start_angle, end_angle;

    pie(){};
    pie(Sint16 x, Sint16 y, Sint16 radius, Sint16 start_angle, Sint16 end_angle)
        : x(x), y(y), radius(radius), start_angle(start_angle), end_angle(end_angle)
    {
    }

    int draw(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    int draw(SDL_Renderer *renderer, const Color &color);
};
constexpr bool IsMouseInsidePie(float mouse_x, float mouse_y, const pie &p);
void setPieState(bool states[4], int index);
void start(double time_interval, int level, double deltaTime);
void fillVectorRandom(std::vector<int> &vec);
void cheat();
void reset();

bool states[4] = {0};
bool prev_states[4] = {0};

pie yellow(360, 180, 150, 180, 270);
pie blue(360, 180, 150, 270, 360);
pie red(360, 180, 150, 90, 180);
pie green(360, 180, 150, 0, 90);

bool pause = true;
bool turn = false;

std::vector<int> numbers(10);

double count = 0;
int index = 0;

bool play = false;
std::string sound;

std::vector<int> guess;
int level = 0;
double time_interval = 0.8;

bool wait = false;
double wait_counter = 0.0;

bool lit[4] = {0};
double lit_counter[4] = {0};

void App::Setup()
{
    fillVectorRandom(numbers);
    cheat();

    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS)
    {
        return;
    }

    auto& style = ImGui::GetStyle();
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    style.FrameRounding = 6;
}

void App::Update(SDL_Event &event, bool &quit, float deltaTime)
{
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

            else if (event.key.keysym.sym == SDLK_k)
                SDL_Log("fps = %f", 1 / deltaTime);

            else if (event.key.keysym.sym == SDLK_SPACE)
            {
                pause = false;
                SDL_SetWindowTitle(window, "Memory Game");
            }

            if (!turn || wait)
                break;

            if (event.key.keysym.sym == SDLK_q)
            {
                guess.push_back(0);
                lit[0] = true;
            }
            else if (event.key.keysym.sym == SDLK_w)
            {
                guess.push_back(1);
                lit[1] = true;
            }
            else if (event.key.keysym.sym == SDLK_a)
            {
                guess.push_back(2);
                lit[2] = true;
            }
            else if (event.key.keysym.sym == SDLK_s)
            {
                guess.push_back(3);
                lit[3] = true;
            }

            break;
        case SDL_EVENT_MOUSE_MOTION:
            if (!turn || wait)
                break;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (!turn || wait)
                break;

            float mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            if (IsMouseInsidePie(mouseX, mouseY, yellow))
            {
                guess.push_back(0);
                lit[0] = true;
            }
            else if (IsMouseInsidePie(mouseX, mouseY, blue))
            {
                guess.push_back(1);
                lit[1] = true;
            }
            else if (IsMouseInsidePie(mouseX, mouseY, red))
            {
                guess.push_back(2);
                lit[2] = true;
            }
            else if (IsMouseInsidePie(mouseX, mouseY, green))
            {
                guess.push_back(3);
                lit[3] = true;
            }
            else
                setPieState(states, -1);
            break;
        }
    }
    
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
        if (states[i] && states[i] != prev_states[i])
        {
            sound = std::to_string(i + 1);
            play = true;
            prev_states[i] = states[i];
        }
    }
    for (int i = 0; i < 4; i++)
        prev_states[i] = states[i];

    if (play)
    {
        ma_engine_play_sound(&engine, std::string(sound + ".wav").c_str(), NULL);
        play = false;
    }

    for (int i = 0; i < 4; i++)
    {
        if (lit[i])
            lit_counter[i] += deltaTime;
        if (lit_counter[i] >= 0.20)
        {
            lit_counter[i] -= 0.20;
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
        start(time_interval, level, deltaTime);
    }

    if (guess.size() == numbers.size())
    {
        SDL_Log("you won");
        SDL_SetWindowTitle(window, "You Won!");
        reset();
    }

    for (size_t i = 0; i < guess.size(); i++)
    {
        if (guess[i] != numbers[i])
        {
            SDL_Log("try again");
            SDL_SetWindowTitle(window, "You Lost!");
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
}

void App::Draw()
{
    ImGuiIO &io = this->ImguiNewFrame();
    this->SetRenderDrawColor({22, 22, 22});
    SDL_RenderClear(renderer);

    ImGui::SetNextWindowPos({30, 100}, ImGuiCond_FirstUseEver);
    ImGui::Begin("menu", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    if (ImGui::Button("Start", {120, 30}))
    {
        pause = false;
        SDL_SetWindowTitle(window, "Memory Game");
    }
    ImGui::End();

    ImGui::SetNextWindowPos({30, 150}, ImGuiCond_FirstUseEver);
    ImGui::Begin("menu 2", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    if (ImGui::Button("Reset", {120, 30}))
    {
        reset();
        SDL_SetWindowTitle(window, "Memory Game");
    }
    ImGui::End();

    this->DrawString(std::to_string(level), {540, 10}, 200, {turn ? 255.f : 0.f, turn ? 255.f : 0.f, turn ? 255.f : 0.f, 255}, 1);

    yellow.draw(renderer, 255, 255, 0, states[0] ? 255 : 100);
    blue.draw(renderer, 0, 0, 255, states[1] ? 255 : 100);
    red.draw(renderer, 255, 0, 0, states[2] ? 255 : 100);
    green.draw(renderer, 0, 255, 0, states[3] ? 255 : 100);

    this->ImguiRender();
    SDL_RenderPresent(renderer);
}

int pie::draw(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    return filledPieRGBA(renderer, x, y, radius, start_angle, end_angle, r, g, b, a);
}

int pie::draw(SDL_Renderer *renderer, const Color &color)
{
    return filledPieRGBA(renderer, x, y, radius, start_angle, end_angle, color.r, color.g, color.b, color.a);
}

constexpr bool IsMouseInsidePie(float mouse_x, float mouse_y, const pie &p)
{
    // Calculate distance from mouse to pie center
    float dx = mouse_x - p.x;
    float dy = mouse_y - p.y;
    float distance_squared = dx * dx + dy * dy;

    // Check if mouse is within the radius of the pie
    if (distance_squared <= (p.radius * p.radius))
    {
        // Calculate angle from the center of the pie to the mouse position
        double angle = atan2(dy, dx) * 180 / M_PI;

        // Normalize angle to be between 0 and 360 degrees
        if (angle < 0)
        {
            angle += 360;
        }

        // Check if angle is within the pie's start and end angles
        if (angle >= p.start_angle && angle <= p.end_angle)
        {
            return true;
        }
    }

    return false;
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

void reset()
{
    pause = true;
    count = 0;
    index = 0;
    turn = false;
    time_interval = 0.8;
    level = 0;
    fillVectorRandom(numbers);
    guess.clear();
    cheat();
}

void start(double time_interval, int level, double deltaTime)
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

void fillVectorRandom(std::vector<int> &vec)
{
    vec.clear();
    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 3);

    // Generate all four numbers at least once
    for (int i = 0; i < 4; ++i)
    {
        int randomNumber;
        do
        {
            randomNumber = dis(gen);
        } while (std::find(vec.begin(), vec.end(), randomNumber) != vec.end());
        vec.push_back(randomNumber);
    }

    // Fill the rest of the vector with the logic to avoid three consecutive repetitions
    const int desiredSize = 10; // Adjust as needed
    for (size_t i = vec.size(); i < desiredSize; ++i)
    {
        int randomNumber;
        int lastNumber = vec.back();
        int secondLastNumber = vec[vec.size() - 2];
        do
        {
            randomNumber = dis(gen);
        } while (randomNumber == lastNumber && randomNumber == secondLastNumber);
        vec.push_back(randomNumber);
    }

    // Shuffle the numbers to make them appear more random
    std::shuffle(vec.begin(), vec.end(), gen);
}

void setPieState(bool states[4], int index)
{
    for (int i = 0; i < 4; i++)
        states[i] = false;

    if (index >= 0 && index <= 4)
        states[index] = true;
}
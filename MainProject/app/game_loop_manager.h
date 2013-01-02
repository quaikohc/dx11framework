
#pragma once


void  (*g_userGameLoop)(float);

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


void BasicGameLoop(float elapsedTime)
{
    Globals::Get().inputManager.HandleRawInput(NULL);

    Globals::Get().physics.Update();

//    Globals::Get().visibilitySystem.Update();

    g_userGameLoop(elapsedTime);

    Globals::Get().renderer.m_camera.Update();

    Globals::Get().scene.Update();

    Globals::Get().renderer.Draw();

   Globals::Get().inputManager.Update();

   Sleep(0);
}



//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//  loop based on article "Fix your timestep"
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

float t = 0.0f;
float dt = 0.066f;

float accumulator = 0.0f;

void InterpolatedGameLoop(float elapsedTime)
{
    if (elapsedTime > 0.25f)
        elapsedTime = 0.25f;

    accumulator += elapsedTime;

    while (accumulator >= dt)
    {
        accumulator -= dt;

// previousState = currentState;
// integrate(current, t, dt);

        Globals::Get().inputManager.HandleRawInput(NULL);

        Globals::Get().physics.Update();

//        Globals::Get().visibilitySystem.Update();

        g_userGameLoop(dt);

        Globals::Get().renderer.m_camera.Update();

        Globals::Get().scene.Update();

        t += dt;
    }
    //State state = interpolate(previous, current, accumulator/dt);

    Globals::Get().renderer.Draw();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//  loop taken from http://www.idevgames.com/articles/timebasedanimation
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


#define MAXIMUM_FRAME_RATE 120
#define MINIMUM_FRAME_RATE 15
#define UPDATE_INTERVAL (1.0 / MAXIMUM_FRAME_RATE)
#define MAX_CYCLES_PER_FRAME (MAXIMUM_FRAME_RATE / MINIMUM_FRAME_RATE)

void FixedIntervalGameLoop(float elapsedTime)
{
    static double lastFrameTime = 0.0;
    static double cyclesLeftOver = 0.0;

    double updateIterations;

    updateIterations = (elapsedTime + cyclesLeftOver);

    if (updateIterations > (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL)) 
    {
        updateIterations = (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL);
    }

    while (updateIterations > UPDATE_INTERVAL) 
    {
        Globals::Get().inputManager.HandleRawInput(NULL);
        Globals::Get().physics.Update();
//        Globals::Get().visibilitySystem.Update();

        g_userGameLoop((float)updateIterations);

        Globals::Get().renderer.m_camera.Update();
        Globals::Get().scene.Update();

        updateIterations -= UPDATE_INTERVAL;

    }

    cyclesLeftOver = updateIterations;

    Globals::Get().renderer.Draw();
}


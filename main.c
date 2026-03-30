#include "raylib.h"
#include "raymath.h"    // Required for Vector3 math (dot products, lerps, etc)
#include "vehicle_system.h"
#include "rlgl.h"       // Required for 3D transformations
#include <stdio.h>      // We need this to format our text strings
#include <math.h>       // Required for sinf(), cosf(), and sqrtf()
#include <string.h>     // Required for strcmp()

int main(void) {
    // 1. Initialize the Window
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "ORBIT ENGINE v0.1 - The Dyno Room");

    // 2. Set up the 3D Camera (So we can see our creation)
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Floating up and away
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Looking at the center
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // 3. Spawn our first proprietary vehicle
    Car myCar = { 0 };
    InitCar(&myCar); // Initialize with Stock parts and baseline stats

    bool inGarage = false;

    SetTargetFPS(60);

    // 4. THE MAIN GAME LOOP (The Chaos Begins)
    while (!WindowShouldClose()) {
        
        // --- INPUT & STATE MANAGEMENT ---
        if (IsKeyPressed(KEY_TAB)) inGarage = !inGarage;

        // Directional vectors based on current rotation (Radians) - Always calculated for camera
        Vector3 forward = { sinf(myCar.rotation), 0.0f, cosf(myCar.rotation) };
        Vector3 right = { cosf(myCar.rotation), 0.0f, -sinf(myCar.rotation) };
        float currentSpeedMagnitude = Vector3Length(myCar.velocity);

        // --- GARAGE INTERACTION ---
        if (inGarage) {
            // Part 1: Intake Toggle
            if (IsKeyPressed(KEY_ONE)) {
                if (myCar.equipped_parts[PART_INTAKE].hp_modifier == 0.0f) 
                    myCar.equipped_parts[PART_INTAKE] = GetNovaLinkIntake();
                else 
                    myCar.equipped_parts[PART_INTAKE] = GetStockIntake();
                ApplyParts(&myCar);
            }
            // Part 2: Turbo Toggle
            if (IsKeyPressed(KEY_TWO)) {
                if (myCar.equipped_parts[PART_TURBO].hp_modifier == 0.0f) 
                    myCar.equipped_parts[PART_TURBO] = GetVortexTurbo();
                else 
                    myCar.equipped_parts[PART_TURBO] = GetStockTurbo();
                ApplyParts(&myCar);
            }
            // Part 3: Radiator Toggle
            if (IsKeyPressed(KEY_THREE)) {
                if (myCar.equipped_parts[PART_RADIATOR].cooling_modifier == 1.0f) 
                    myCar.equipped_parts[PART_RADIATOR] = GetAetherRadiator();
                else 
                    myCar.equipped_parts[PART_RADIATOR] = GetStockRadiator();
                ApplyParts(&myCar);
            }
        }

        // --- CORE GAMEPLAY LOGIC (Physics & Tuning) ---
        // We only update physics if the car is NOT in the garage
        if (!inGarage) {
            // 1. Engine Tuning (Chaos Mechanic)
            if (IsKeyPressed(KEY_SPACE)) {
                if (myCar.engine.heat_level < 100.0f) {
                    ApplyTuning(&myCar, 20.0f); 
                }
            }

            // 2. Physics & Movement (WASD)
            float dt = GetFrameTime();
            
            // Forward/Backward Acceleration
            if (IsKeyDown(KEY_W) && myCar.engine.heat_level < 100.0f) {
                float thrust = (myCar.engine.base_hp * 0.1f) * dt; 
                myCar.velocity.x += forward.x * thrust;
                myCar.velocity.z += forward.z * thrust;

                // Engine Heating: Scaled by HP and the part-specific Heat Penalty
                float heatingRate = (currentSpeedMagnitude * 0.15f) * (myCar.engine.base_hp / 150.0f) + myCar.engine.heat_penalty;
                myCar.engine.heat_level += heatingRate * dt;
            } else {
                // Cooling: Uses the dynamic cooling_rate from our inventory/stat system
                float coolingRate = myCar.engine.cooling_rate + (currentSpeedMagnitude * 2.0f); 
                myCar.engine.heat_level -= coolingRate * dt;
            }

            // Clamp heat
            myCar.engine.heat_level = Clamp(myCar.engine.heat_level, 0.0f, 100.0f);

            if (IsKeyDown(KEY_S)) {
                float brake = 0.05f;
                myCar.velocity.x -= forward.x * brake;
                myCar.velocity.z -= forward.z * brake;
            }

            // --- LATERAL FRICTION ---
            float forwardSpeed = Vector3DotProduct(myCar.velocity, forward);
            float lateralSpeed = Vector3DotProduct(myCar.velocity, right);

            // --- AIR DRAG ---
            float dragForce = (forwardSpeed * forwardSpeed) * 0.02f;
            if (forwardSpeed > 0) forwardSpeed = fmaxf(0, forwardSpeed - dragForce * dt);
            else if (forwardSpeed < 0) forwardSpeed = fminf(0, forwardSpeed + dragForce * dt);

            // --- ROLL RESISTANCE & ENGINE SEIZURE ---
            if (myCar.engine.heat_level >= 100.0f) {
                forwardSpeed *= 0.95f; // Seized block: Heavy mechanical resistance
            } else {
                forwardSpeed *= 0.99f; // Normal smooth rolling resistance
            }
            lateralSpeed *= 0.95f;

            myCar.velocity.x = (forward.x * forwardSpeed) + (right.x * lateralSpeed);
            myCar.velocity.z = (forward.z * forwardSpeed) + (right.z * lateralSpeed);

            // Steering logic
            if (currentSpeedMagnitude > 0.1f) {
                float turnSpeed = 0.04f;
                if (IsKeyDown(KEY_A)) myCar.rotation += turnSpeed;
                if (IsKeyDown(KEY_D)) myCar.rotation -= turnSpeed;
            }

            // Apply Velocity to Position
            myCar.position.x += myCar.velocity.x;
            myCar.position.z += myCar.velocity.z;
        }

        // 3. Smooth Camera Follow Logic
        float speedMPH = currentSpeedMagnitude * 20.0f;
        
        // Calculate the ideal follow position (Capped at 15.0 units distance)
        float maxPullback = 15.0f;
        float cameraDist = fminf(maxPullback, 12.0f + (speedMPH * 0.05f)); 
        Vector3 targetCameraPos = {
            myCar.position.x - forward.x * cameraDist,
            myCar.position.y + 5.0f,
            myCar.position.z - forward.z * cameraDist
        };

        // Smoothly interpolate the camera position to prevent snapping
        camera.position = Vector3Lerp(camera.position, targetCameraPos, 0.1f);
        camera.target = Vector3Lerp(camera.target, myCar.position, 0.2f);

        // --- DRAWING THE WORLD ---
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // Draw the 3D World
            BeginMode3D(camera);
                DrawGrid(1000, 1.0f); // The World floor (Expanded)
                
                // Rotated Car Drawing (Transformation Block)
                rlPushMatrix();
                    rlTranslatef(myCar.position.x, myCar.position.y, myCar.position.z);
                    rlRotatef(myCar.rotation * RAD2DEG, 0, 1, 0);
                    
                    Color carColor = RED;
                    if (myCar.engine.heat_level > 80.0f) carColor = MAROON;
                    if (myCar.engine.heat_level >= 100.0f) carColor = BLACK;
                    
                    DrawCube((Vector3){0, 0.5f, 0}, 2.0f, 1.0f, 4.0f, carColor);
                    DrawCubeWires((Vector3){0, 0.5f, 0}, 2.0f, 1.0f, 4.0f, DARKGRAY);
                rlPopMatrix();
            EndMode3D();

            // --- THE UI (Mechanic's Diagnostics Tablet) ---
            DrawRectangle(10, 10, 260, 155, Fade(BLACK, 0.8f));
            DrawText("ORBIT DIAGNOSTICS", 20, 20, 10, GREEN);
            
            // Format and print HP
            char hpText[64];
            sprintf(hpText, "Horsepower: %.1f", myCar.engine.base_hp);
            DrawText(hpText, 20, 40, 20, WHITE);

            // Format and print Heat
            char heatText[64];
            sprintf(heatText, "Heat Level: %.1f%%", myCar.engine.heat_level);
            DrawText(heatText, 20, 70, 20, (myCar.engine.heat_level > 80) ? RED : ORANGE);

            // Speedometer (MPH)
            char speedText[64];
            sprintf(speedText, "Speed: %.1f MPH", speedMPH);
            DrawText(speedText, 20, 100, 20, SKYBLUE);

            DrawText("SPACE: Overclock | WASD: Drive | TAB: Garage", 20, 135, 10, LIGHTGRAY);

            // The Consequence
            if (myCar.engine.heat_level >= 100.0f) {
                DrawText("ENGINE OVERHEATED! COAST TO COOL DOWN", 140, 200, 20, RED);
            }

            // --- GARAGE OVERLAY ---
            if (inGarage) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));
                DrawRectangle(screenWidth/2 - 150, screenHeight/2 - 100, 300, 200, Fade(DARKGRAY, 0.9f));
                DrawRectangleLines(screenWidth/2 - 150, screenHeight/2 - 100, 300, 200, RAYWHITE);
                
                DrawText("MECHANIC GARAGE", screenWidth/2 - 80, screenHeight/2 - 80, 20, GREEN);
                DrawText("Press 1, 2, or 3 to swap parts", screenWidth/2 - 75, screenHeight/2 - 50, 10, LIGHTGRAY);
                
                DrawText("Currently Equipped:", screenWidth/2 - 130, screenHeight/2 - 10, 10, WHITE);
                for (int i = 0; i < 3; i++) {
                    DrawText(TextFormat("[%d] %s", i + 1, myCar.equipped_parts[i].name), screenWidth/2 - 120, screenHeight/2 + 10 + (i * 20), 10, SKYBLUE);
                }
                
                DrawText("Press TAB to Return to Street", screenWidth/2 - 80, screenHeight/2 + 80, 10, GRAY);
            }

        EndDrawing();
    }

    CloseWindow(); // Turn the lights off when you leave
    return 0;
}

#ifndef VEHICLE_SYSTEM_H
#define VEHICLE_SYSTEM_H

#include "raylib.h"
#include "inventory_system.h"

// Engine subsystem: defined as a lean struct to be embedded directly into Car.
// 24 bytes total (assuming 4-byte floats and no padding).
typedef struct Engine {
    float base_hp;
    float current_boost;
    float heat_level;
    float cooling_rate;      // Base radiator efficiency per second
    float synergy_multiplier;
} Engine;

// Car system: Core entity for the Orbit Engine physics calculations.
// Roughly 64 bytes total, optimized for cache locality by embedding the Engine struct.
typedef struct Car {
    Engine engine;
    Vector3 position;
    Vector3 velocity;
    float rotation;
    float handling_coefficient;
    CarPart equipped_parts[3]; // Slots for INTAKE, TURBO, RADIATOR
} Car;

/**
 * @brief Initializes a car with baseline performance and "Stock" equipment.
 */
void InitCar(Car* car);

/**
 * @brief Applies tuning to the car's engine, increasing power at the cost of heat.
 * 
 * @param car Pointer to the Car instance to modify.
 * @param added_boost The amount of base horsepower to add.
 */
void ApplyTuning(Car* car, float added_boost);

#endif // VEHICLE_SYSTEM_H

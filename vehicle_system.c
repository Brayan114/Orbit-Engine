#include <stddef.h>
#include <string.h>   // Required for strncpy()
#include "vehicle_system.h"
#include "inventory_system.h"

// Implementation of ApplyTuning for the Orbit Engine.
// Memory-efficient and using Raylib's internal random RNG to avoid extra overhead.
void ApplyTuning(Car* car, float added_boost) {
    if (car == NULL) return;

    // Direct HP increase
    car->engine.base_hp += added_boost;

    // Calculate heat penalty: (added_boost * 1.5) + randomized noise (0-10)
    // GetRandomValue is a Raylib utility for discrete integers.
    float heat_penalty = (added_boost * 1.5f) + (float)GetRandomValue(0, 10);

    // Apply the penalty
    car->engine.heat_level += heat_penalty;
}

/**
 * @brief Initializes a car with baseline performance and "Stock" equipment.
 */
void InitCar(Car* car) {
    if (car == NULL) return;

    // Default basic physics and thermal state
    car->position = (Vector3){0.0f, 0.5f, 0.0f};
    car->velocity = (Vector3){0.0f, 0.0f, 0.0f};
    car->rotation = 0.0f;
    car->engine.heat_level = 0.0f;
    
    // Default base metrics
    car->engine.base_hp = 150.0f;
    car->engine.cooling_rate = 10.0f;

    // Populate equipped_parts with "Stock" baseline parts
    // Use strncpy to safely copy strings into fixed-size buffers
    strncpy(car->equipped_parts[PART_INTAKE].name, "Stock Intake", 31);
    car->equipped_parts[PART_INTAKE].type = PART_INTAKE;
    car->equipped_parts[PART_INTAKE].hp_modifier = 0.0f;
    car->equipped_parts[PART_INTAKE].cooling_modifier = 1.0f;

    strncpy(car->equipped_parts[PART_TURBO].name, "Stock Turbo", 31);
    car->equipped_parts[PART_TURBO].type = PART_TURBO;
    car->equipped_parts[PART_TURBO].hp_modifier = 0.0f;
    car->equipped_parts[PART_TURBO].cooling_modifier = 1.0f;

    strncpy(car->equipped_parts[PART_RADIATOR].name, "Stock Radiator", 31);
    car->equipped_parts[PART_RADIATOR].type = PART_RADIATOR;
    car->equipped_parts[PART_RADIATOR].hp_modifier = 0.0f;
    car->equipped_parts[PART_RADIATOR].cooling_modifier = 1.0f;

    // Finalize car statistics based on the baseline parts
    ApplyParts(car);
}

#include "inventory_system.h"
#include "vehicle_system.h"
#include <stddef.h>
#include <string.h>

/**
 * @brief Recalculates engine stats by aggregating modifiers from equipped parts.
 * HP and Heat Penalty are additive, while Cooling is multiplicative.
 */
void ApplyParts(Car* car) {
    if (car == NULL) return;

    // 1. Reset to factory baseline metrics (Orbit Engine 150HP Default)
    car->engine.base_hp = 150.0f;
    car->engine.cooling_rate = 10.0f;
    car->engine.heat_penalty = 0.0f;

    // 2. Iterate through equipped parts slots (INTAKE, TURBO, RADIATOR)
    for (int i = 0; i < 3; i++) {
        // HP Modifier (Additive)
        car->engine.base_hp += car->equipped_parts[i].hp_modifier;
        
        // Heat Penalty (Additive): Increases heat buildup rate
        car->engine.heat_penalty += car->equipped_parts[i].heat_penalty;
        
        // Cooling Modifier (Multiplicative)
        if (car->equipped_parts[i].cooling_modifier > 0.0f) {
            car->engine.cooling_rate *= car->equipped_parts[i].cooling_modifier;
        }
    }
}

// --- GLOBAL SHOP PARTS DEFINITIONS ---
// These are static templates that main.c can use to populate the Car.

CarPart GetStockIntake() {
    CarPart p;
    strncpy(p.name, "Stock Intake", 31);
    p.type = PART_INTAKE;
    p.hp_modifier = 0.0f;
    p.cooling_modifier = 1.0f;
    p.heat_penalty = 0.0f;
    return p;
}

CarPart GetNovaLinkIntake() {
    CarPart p;
    strncpy(p.name, "Nova-Link Intake", 31);
    p.type = PART_INTAKE;
    p.hp_modifier = 30.0f;
    p.cooling_modifier = 1.0f;
    p.heat_penalty = 0.0f;
    return p;
}

CarPart GetStockTurbo() {
    CarPart p;
    strncpy(p.name, "Stock Turbo", 31);
    p.type = PART_TURBO;
    p.hp_modifier = 0.0f;
    p.cooling_modifier = 1.0f;
    p.heat_penalty = 0.0f;
    return p;
}

CarPart GetVortexTurbo() {
    CarPart p;
    strncpy(p.name, "Vortex Twin-Turbo", 31);
    p.type = PART_TURBO;
    p.hp_modifier = 120.0f;
    p.cooling_modifier = 1.0f;
    p.heat_penalty = 50.0f; // High heat penalty for massive power
    return p;
}

CarPart GetStockRadiator() {
    CarPart p;
    strncpy(p.name, "Stock Radiator", 31);
    p.type = PART_RADIATOR;
    p.hp_modifier = 0.0f;
    p.cooling_modifier = 1.0f;
    p.heat_penalty = 0.0f;
    return p;
}

CarPart GetAetherRadiator() {
    CarPart p;
    strncpy(p.name, "Aether-Core Radiator", 31);
    p.type = PART_RADIATOR;
    p.hp_modifier = 0.0f;
    p.cooling_modifier = 3.0f; // 300% cooling efficiency
    p.heat_penalty = 0.0f;
    return p;
}

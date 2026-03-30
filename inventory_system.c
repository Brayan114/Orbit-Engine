#include "inventory_system.h"
#include "vehicle_system.h"
#include <stddef.h>

/**
 * @brief Recalculates engine stats by aggregating modifiers from equipped parts.
 * HP modifiers are additive, while Cooling modifiers are multiplicative.
 */
void ApplyParts(Car* car) {
    if (car == NULL) return;

    // 1. Reset to factory baseline metrics
    car->engine.base_hp = 150.0f;
    car->engine.cooling_rate = 10.0f;

    // 2. Iterate through equipped parts slots (INTAKE, TURBO, RADIATOR)
    for (int i = 0; i < 3; i++) {
        // HP Modifier (Additive): 
        // e.g., +10.0 HP for a street intake.
        car->engine.base_hp += car->equipped_parts[i].hp_modifier;
        
        // Cooling Modifier (Multiplicative): 
        // e.g., 1.25x cooling efficiency for a racing radiator.
        // We ensure a neutral default of 1.0f if no modifier is set.
        if (car->equipped_parts[i].cooling_modifier > 0.0f) {
            car->engine.cooling_rate *= car->equipped_parts[i].cooling_modifier;
        }
    }
}

#ifndef INVENTORY_SYSTEM_H
#define INVENTORY_SYSTEM_H

// Enum to define the distinct types of car parts that can be swapped.
typedef enum PartType {
    PART_INTAKE,
    PART_TURBO,
    PART_RADIATOR,
    PART_LENGTH // Used for array sizing constraints if needed
} PartType;

// Data structure to define an individual car part.
// Fixed-size name for memory efficiency and zero fragmentation on older hardware.
typedef struct CarPart {
    char name[32];
    PartType type;
    float hp_modifier;       // Additive modifier for engine base horsepower
    float cooling_modifier;  // Multiplicative modifier for the engine radiator efficiency
    float heat_penalty;      // Additive modifier for engine heat buildup rate
} CarPart;

// Forward declaration of Car is needed since it's defined in vehicle_system.h
struct Car;

/**
 * @brief Recalculates engine stats (HP, cooling, heat penalty) by aggregating modifiers from equipped parts.
 */
void ApplyParts(struct Car* car);

// Part Factory Functions (for equipping in the garage)
CarPart GetStockIntake();
CarPart GetNovaLinkIntake();
CarPart GetStockTurbo();
CarPart GetVortexTurbo();
CarPart GetStockRadiator();
CarPart GetAetherRadiator();

#endif // INVENTORY_SYSTEM_H

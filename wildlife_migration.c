/**
 *TANAKA CHANDAMARA C24158553K
 * file: wildlife_migration.c
 * description: Open MPI simulation of elephant herd migration across Zimbabwe's savannah.
 * compilation: mpicc -o wildlife_migration wildlife_migration.c -lm
 * execution:   mpirun -np [number_of_herds] ./wildlife_migration
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WATERING_HOLE 200.0
#define MAX_DAYS 1000

// Helper function for safe MPI error handling
void check_mpi(int err, const char *msg) {
    if (err != MPI_SUCCESS) {
        fprintf(stderr, "MPI Error in %s: Code %d\n", msg, err);
        MPI_Abort(MPI_COMM_WORLD, err);
    }
}

int main(int argc, char *argv[]) {
    int rank, size, err;

    // 1. Initialize MPI environment 
    err = MPI_Init(&argc, &argv);
    check_mpi(err, "MPI_Init");

    err = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    check_mpi(err, "MPI_Comm_rank");

    err = MPI_Comm_size(MPI_COMM_WORLD, &size);
    check_mpi(err, "MPI_Comm_size");

    // Seeded random number generator using rank and current time for uniqueness
    srand((unsigned int)(time(NULL) + rank * 1337));

    // Simulation variables
    float position = 0.0f;
    int day = 0;
    int winner_found = 0;
    int winning_rank = -1;

    // Buffer to gather positions of all herds
    float *all_positions = (float *)malloc(size * sizeof(float));
    if (all_positions == NULL) {
        fprintf(stderr, "Rank %d: Memory allocation failed for all_positions\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Main Simulation Loop (Day by Day)
    while (!winner_found && day < MAX_DAYS) {
        day++;

        // 2. Movement Logic 
        // Only move if the herd hasn't reached the watering hole yet
        if (position < WATERING_HOLE) {
            float base_move = (float)(5 + rand() % 21);
            float terrain_modifier = 0.0f;

            // 3. Zimbabwe-Specific Terrain Factors 
            if (position < 80.0f) {
                // Hwange Region: Fast terrain
                terrain_modifier = (float)(rand() % 6); // +0 to +5 km bonus
            } else if (position >= 80.0f && position < 150.0f) {
                // Deti Vlei: Slow marshy terrain
                terrain_modifier = -(float)(rand() % 9); // -0 to -8 km penalty
            } else {
                // Victoria Falls Region: Dense bush
                terrain_modifier = 0.0f; // Normal movement
            }

            float actual_move = base_move + terrain_modifier;
            
            // Enforce minimum movement rule for harsh terrain (Deti Vlei marsh)
            if (actual_move < 2.0f) {
                actual_move = 2.0f;
            }

            position += actual_move;
            if (position > WATERING_HOLE) {
                position = WATERING_HOLE;
            }
        }

        // 4. Communication: Gather all positions to rank 0 
        err = MPI_Allgather(&position, 1, MPI_FLOAT, all_positions, 1, MPI_FLOAT, MPI_COMM_WORLD);
        check_mpi(err, "MPI_Allgather");

        // 5. Output Formatting: Print daily status from rank 0 
        if (rank == 0) {
            for (int i = 0; i < size; i++) {
                const char *terrain_desc = "";
                if (all_positions[i] < 80.0f) {
                    terrain_desc = "(Hwange Plains - Fast terrain)";
                } else if (all_positions[i] >= 80.0f && all_positions[i] < 150.0f) {
                    terrain_desc = "(Deti Vlei - Slow terrain)";
                } else {
                    terrain_desc = "(Victoria Falls Region - Dense bush)";
                }
                
                printf("Herd %d: Day %d - Position: %.1f km %s\n", i, day, all_positions[i], terrain_desc);
            }
            printf("----------------------------------------\n");
            
            // Rank 0 checks if any herd reached the watering hole
            for (int i = 0; i < size; i++) {
                if (all_positions[i] >= WATERING_HOLE) {
                    winner_found = 1;
                    winning_rank = i;
                    break; // First herd evaluated to hit 200km wins
                }
            }
        }

        // 6. Termination Logic: Broadcast winning condition to all processes 
        err = MPI_Bcast(&winner_found, 1, MPI_INT, 0, MPI_COMM_WORLD);
        check_mpi(err, "MPI_Bcast winner_found");

        err = MPI_Bcast(&winning_rank, 1, MPI_INT, 0, MPI_COMM_WORLD);
        check_mpi(err, "MPI_Bcast winning_rank");

        // Synchronize processes briefly per day iteration
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Output Winner Announcement and Bonus Report
    if (rank == 0) {
        printf("\n🏆 WINNER! Herd %d reached the watering hole at Victoria Falls in %d days!\n\n", winning_rank, day);
        printf("--- FINAL SUMMARY ---\n");
        for (int i = 0; i < size; i++) {
            if (i == winning_rank) {
                printf("Herd %d: Position %.1f km (WINNER - %d days)\n", i, all_positions[i], day);
            } else if (all_positions[i] >= WATERING_HOLE) {
                printf("Herd %d: Position %.1f km (Also reached but came second)\n", i, all_positions[i]);
            } else if (all_positions[i] >= 190.0f) {
                printf("Herd %d: Position %.1f km (Just short!)\n", i, all_positions[i]);
            } else {
                printf("Herd %d: Position %.1f km\n", i, all_positions[i]);
            }
        }
        printf("---------------------\n");

        // --- BONUS: Conservation Status Report  ---
        printf("\n=== CONSERVATION STATUS REPORT (ZIMPARKS) ===\n");
        printf("Simulation completed in %d days across %d active elephant herds.\n", day, size);
        
        printf("\n1. Average Speeds:\n");
        for (int i = 0; i < size; i++) {
            printf("   -> Herd %d: %.2f km/day\n", i, all_positions[i] / (float)day);
        }
        
        printf("\n2. Terrain Analysis:\n");
        printf("   -> Deti Vlei marshy areas consistently caused the highest delays due to negative movement penalties.\n");
        
        printf("\n3. Route Recommendation:\n");
        printf("   -> Maintain protected corridors through Hwange Open Plains to maximize movement efficiency before navigating the Deti Vlei bottleneck.\n");
        printf("==============================================\n");
    }

    // 7. Cleanup and Finalize
    free(all_positions);
    MPI_Finalize();

    return 0;
}
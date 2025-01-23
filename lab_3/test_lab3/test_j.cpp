#include "test.h"

#include "simulation/naive_cuda_simulation.cuh"
#include "simulation/naive_sequential_simulation.h"
#include "simulation/constants.h"

#include "input_generator/input_generator.h"

#include <exception>
#include <iostream>

#include "test_j.cuh"

#include "utilities.h"

class Lab3Test : public LabTest {};

TEST_F(Lab3Test, test_j){
    // check positions correctly calculated
    Universe reference_uni;
    InputGenerator::create_random_universe(1337, reference_uni);

    Universe uni;
    InputGenerator::create_random_universe(1337, uni);
    for(std::int32_t i = 0; i < uni.num_bodies; i++){
        uni.weights[i] = reference_uni.weights[i];
        uni.forces[i].set(reference_uni.forces[i][0], reference_uni.forces[i][1]);
        uni.positions[i].set(reference_uni.positions[i][0], reference_uni.positions[i][1]);
        uni.velocities[i].set(reference_uni.velocities[i][0], reference_uni.velocities[i][1]);
    }

    NaiveSequentialSimulation::calculate_forces(uni);
    NaiveSequentialSimulation::calculate_velocities(uni);
    NaiveSequentialSimulation::calculate_forces(reference_uni);
    NaiveSequentialSimulation::calculate_velocities(reference_uni);

    void* d_weights;
    void* d_forces;
    void* d_velocities;
    void* d_positions;

    NaiveCudaSimulation::allocate_device_memory(uni, &d_weights, &d_forces, &d_velocities, &d_positions);
    NaiveCudaSimulation::copy_data_to_device(uni, d_weights, d_forces, d_velocities, d_positions);

    NaiveCudaSimulation::calculate_positions(uni, d_velocities, d_positions);

    NaiveCudaSimulation::copy_data_from_device(uni, d_weights, d_forces, d_velocities, d_positions);

    NaiveCudaSimulation::free_device_memory(&d_weights, &d_forces, &d_velocities, &d_positions);

    // calculate reference
    NaiveSequentialSimulation::calculate_positions(reference_uni);

    ASSERT_EQ(uni.num_bodies, reference_uni.num_bodies);
    ASSERT_EQ(uni.weights.size(), reference_uni.weights.size());
    ASSERT_EQ(uni.forces.size(), reference_uni.forces.size());
    ASSERT_EQ(uni.velocities.size(), reference_uni.velocities.size());
    ASSERT_EQ(uni.positions.size(), reference_uni.positions.size());

    for(int i = 0; i < uni.num_bodies; i++){
        ASSERT_FLOAT_EQ(round_to(uni.weights[i], 0.000001), round_to(reference_uni.weights[i], 0.000001));
        ASSERT_FLOAT_EQ(round_to(uni.forces[i][0], 0.000001), round_to(reference_uni.forces[i][0], 0.000001));
        ASSERT_FLOAT_EQ(round_to(uni.forces[i][1], 0.000001), round_to(reference_uni.forces[i][1], 0.000001));
        ASSERT_FLOAT_EQ(round_to(uni.velocities[i][0], 0.000001), round_to(reference_uni.velocities[i][0], 0.000001));
        ASSERT_FLOAT_EQ(round_to(uni.velocities[i][1], 0.000001), round_to(reference_uni.velocities[i][1], 0.000001));
        ASSERT_FLOAT_EQ(round_to(uni.positions[i][0], 0.000001), round_to(reference_uni.positions[i][0], 0.000001));
        ASSERT_FLOAT_EQ(round_to(uni.positions[i][1], 0.000001), round_to(reference_uni.positions[i][1], 0.000001));
    }
}

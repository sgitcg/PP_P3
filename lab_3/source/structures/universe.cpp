#include "structures/universe.h"
#include "image/bitmap_image.h"
#include "io/image_parser.h"
#include "image/pixel.h"
#include <ctime>

#include <cstdint>
#include <random>
#include <tuple>
#include <set>
#include <omp.h>
#include <cmath>

void Universe::print_bodies_to_console(){
    // print bodies
    for(int i = 0; i < num_bodies; i++){
        std::cout << "Body:" << std::endl;
        std::cout << "\tweight: " << weights[i] << std::endl;
        std::cout << "\tvelocity: (" << velocities[i][0] << " , " << velocities[i][1] << ")" << std::endl;
        std::cout << "\tposition: " << positions[i][0] << " , " << positions[i][1] << ")" << std::endl;
    }
}

BoundingBox Universe::get_bounding_box(){
    double x_min = std::numeric_limits<double>::max();
    double x_max = std::numeric_limits<double>::min();
    double y_min = std::numeric_limits<double>::max();;
    double y_max = std::numeric_limits<double>::min();;

    for(auto position: positions){
        double pos_x, pos_y;
        pos_x = position[0];
        pos_y = position[1];

        if(pos_x > x_max){
            x_max = pos_x;
        }
        if(pos_x < x_min){
            x_min = pos_x;
        }
        if(pos_y > y_max){
            y_max = pos_y;
        }
        if(pos_y < y_min){
            y_min = pos_y;
        }
    }

    return BoundingBox(x_min, x_max, y_min, y_max);
}


BoundingBox Universe::parallel_cpu_get_bounding_box(){
    double x_min = std::numeric_limits<double>::max();
    double x_max = std::numeric_limits<double>::min();
    double y_min = std::numeric_limits<double>::max();;
    double y_max = std::numeric_limits<double>::min();;

    double pos_x, pos_y;
    # pragma omp parallel for private(pos_x, pos_y) reduction(max: x_max) reduction(min: x_min) reduction(max: y_max) reduction(min: y_min)
    for(std::int32_t idx = 0; idx < positions.size(); idx++){
        pos_x = positions[idx][0];
        pos_y = positions[idx][1];

        x_max = std::max(x_max, pos_x);
        x_min = std::min(x_min, pos_x);
        y_max = std::max(y_max, pos_y);
        y_min = std::min(y_min, pos_y);
    }

    return BoundingBox(x_min, x_max, y_min, y_max);
}

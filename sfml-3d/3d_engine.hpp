#pragma once
/*
Code for drawing stuff in 3D
*/

#include <SFML/Graphics.hpp>
#include <algorithm>
#include "../sfml-util/sfml_util.hpp"
#include <utility>
#include <vector>
#include <random>


#include "3d_camera.hpp"
#include "math4.hpp"

static sf::Vector2f normalize_point(sf::RenderWindow& window,
                                    sf::Vector2f raw) {
    float width = static_cast<float>(window.getSize().x);
    float height = static_cast<float>(window.getSize().y);
    return {raw.x + width / 2.0f, height / 2.0f - raw.y};
}

float NEAR_PLANE = 0.01f;

#include "Object3D.hpp"
#include "Shape2D.hpp"

// Helpers to make compatible with an older version of this library
// NOTE: Not 100% compatible. Before it was sufficient only to pass a mat4. Now
// you must pass the whole Camera object

void drawSphere(sf::RenderWindow& window, vec4 a, float radius, Camera& camera,
                sf::Color color = sf::Color::White) {
    Sphere3D sphere(a, radius);
    sphere.draw(window, camera, color);
}

void draw3DLine(sf::RenderWindow& window, vec4 a, vec4 b, Camera& camera,
                float thickness = 1.0f) {
    Line3D line(a, b, thickness);
    line.draw(window, camera);
}


//Weird draw line from a fixed screen point to a point in 3D space:
void draw3DLineTo2DPoint(sf::RenderWindow& window, vec4 a, sf::Vector2f b, Camera& camera, float thickness = 1.0f, sf::Color color = sf::Color::White) {
    mat4 camera_inverse = camera.cf.inverse_rigid();
    vec4 a_t = camera_inverse * a;

    if (a_t.z <= NEAR_PLANE)
        a_t.z = NEAR_PLANE*2; //basically still near

    sf::Vector2f a_ = normalize_point(window, Object3D::convert_3d_to_2d(a_t, camera));

    drawLine(window, a_, b, thickness, color);

}


// CUBE (basically a collection of 12 Line3D objects)
std::array<vec4, 8> cubeVertices(vec4 center, float edge) {
    float h = edge / 2.f;

    return {
        vec4{center.x - h, center.y - h, center.z - h, 1},  // 0
        vec4{center.x + h, center.y - h, center.z - h, 1},  // 1
        vec4{center.x + h, center.y + h, center.z - h, 1},  // 2
        vec4{center.x - h, center.y + h, center.z - h, 1},  // 3
        vec4{center.x - h, center.y - h, center.z + h, 1},  // 4
        vec4{center.x + h, center.y - h, center.z + h, 1},  // 5
        vec4{center.x + h, center.y + h, center.z + h, 1},  // 6
        vec4{center.x - h, center.y + h, center.z + h, 1}   // 7
    };
}

const int cubeEdges[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},  // bottom face
    {4, 5}, {5, 6}, {6, 7}, {7, 4},  // top face
    {0, 4}, {1, 5}, {2, 6}, {3, 7}   // vertical edges
};


std::random_device rand_vec4_rd;
std::mt19937 rand_vec4_gen(rand_vec4_rd());
vec4 rand_vec4(float r0, float r1) {
    std::uniform_real_distribution<float> dist(r0, r1);
    return vec4(dist(rand_vec4_gen), dist(rand_vec4_gen), dist(rand_vec4_gen));
}
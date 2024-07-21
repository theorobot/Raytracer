#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <string>
#include "ray.h"

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 480;

const int IMAGE_WIDTH = 360;
const int IMAGE_HEIGHT = 240;

const int RAYS_PER_FRAME = 100;

std::random_device rd;
std::mt19937 genPos(rd());
std::uniform_real_distribution<> disPos(-0.05f, 0.05f);

std::mt19937 genDir(rd());
std::uniform_real_distribution<> disDir(-0.001f, 0.001f);

sf::Image GenerateImage(int num_rays, int frameCount) {
    float aspectRatio = (float)IMAGE_WIDTH / IMAGE_HEIGHT;

    sf::Image new_image;
    new_image.create(IMAGE_WIDTH, IMAGE_HEIGHT, sf::Color::Black);

    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        for (int x = 0; x < IMAGE_WIDTH; ++x) {
            float px = (2.0f * (float)x / IMAGE_WIDTH - 1.0f) * aspectRatio;
            float py = 1.0f - 2.0f * (float)y / IMAGE_HEIGHT;

            Color averageColor(0.0f, 0.0f, 0.0f);
            for (int ray_num = 0; ray_num < num_rays; ray_num++) {
                Ray r(Vec3f(disPos(genPos), disPos(genPos), 0.0f), Vec3f(px + disDir(genDir), py + disDir(genDir), 1.0f), Color(1.0f, 1.0f, 1.0f), 0);
                Color ray_color = calculateRayColor(r);
                averageColor.r += ray_color.r;
                averageColor.g += ray_color.g;
                averageColor.b += ray_color.b;
            }

            averageColor.r /= num_rays;
            averageColor.g /= num_rays;
            averageColor.b /= num_rays;

            averageColor.r = pow(std::min(1.0f, std::max(0.0f, averageColor.r)), 1.0f / 1.0f);
            averageColor.g = pow(std::min(1.0f, std::max(0.0f, averageColor.g)), 1.0f / 1.0f);
            averageColor.b = pow(std::min(1.0f, std::max(0.0f, averageColor.b)), 1.0f / 1.0f);

            sf::Color newColor = averageColor.ToSFColor();

            new_image.setPixel(x, y, newColor);
        }
    }

    return new_image;
}

sf::Image averageImages(const std::vector<sf::Image>& images) {
    if (images.empty()) {
        // Handle the case where the vector is empty
        return sf::Image();
    }

    sf::Vector2u imageSize = images[0].getSize();
    unsigned int width = imageSize.x;
    unsigned int height = imageSize.y;

    std::vector<unsigned int> sumR(width * height, 0);
    std::vector<unsigned int> sumG(width * height, 0);
    std::vector<unsigned int> sumB(width * height, 0);
    std::vector<unsigned int> sumA(width * height, 0);

    for (const auto& image : images) {
        for (unsigned int y = 0; y < height; ++y) {
            for (unsigned int x = 0; x < width; ++x) {
                sf::Color pixel = image.getPixel(x, y);
                sumR[y * width + x] += pixel.r;
                sumG[y * width + x] += pixel.g;
                sumB[y * width + x] += pixel.b;
                sumA[y * width + x] += pixel.a;
            }
        }
    }

    sf::Image averagedImage;
    averagedImage.create(width, height);
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            sf::Color averagedPixel;
            averagedPixel.r = static_cast<sf::Uint8>(sumR[y * width + x] / images.size());
            averagedPixel.g = static_cast<sf::Uint8>(sumG[y * width + x] / images.size());
            averagedPixel.b = static_cast<sf::Uint8>(sumB[y * width + x] / images.size());
            averagedPixel.a = static_cast<sf::Uint8>(sumA[y * width + x] / images.size());
            averagedImage.setPixel(x, y, averagedPixel);
        }
    }

    return averagedImage;
}

void GenerateImage() {
    std::vector<sf::Image> images;

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Raytracing");

    int frameCount = 1;

    auto start = std::chrono::high_resolution_clock::now();

    while (window.isOpen()) {
        std::string title = "Frame " + std::to_string(frameCount);
        window.setTitle(title);
        images.push_back(GenerateImage(RAYS_PER_FRAME, frameCount));
        std::cout << frameCount << std::endl;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Time taken to render image: " << elapsed.count() << " seconds\n";

        sf::Texture texture;
        texture.loadFromImage(averageImages(images));

        sf::Sprite sprite(texture);
        sprite.setScale(SCREEN_WIDTH / sprite.getLocalBounds().width,
                        SCREEN_HEIGHT / sprite.getLocalBounds().height);

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();

        frameCount++;

        sf::Event event;
        bool isClosed = false;
        while (!isClosed) {
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    isClosed = true;
                }
            }
        }
        break;
    }
}

int main() {
    GenerateImage();

    return 0;
}

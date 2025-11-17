#pragma once
#include "vec.h"

class texture {
  public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point& p) const = 0;
};

class solid_color : public texture {
  public:
    solid_color(const color& albedo) : albedo(albedo) {}

    solid_color(double red, double green, double blue) : solid_color(color(red,green,blue)) {}

    color value(double u, double v, const point& p) const override {
        return albedo;
    }

  private:
    color albedo;
};

class striped_texture : public texture {
public:
    striped_texture(double scale, std::shared_ptr<texture> even, std::shared_ptr<texture> odd)
        : inv_scale(1.0 / scale), even(even), odd(odd) {}

    striped_texture(double scale, const color& c1, const color& c2)
        : striped_texture(scale, std::make_shared<solid_color>(c1), std::make_shared<solid_color>(c2)) {}

    color value(double u, double v, const point& p) const override {
        auto yInteger = int(std::floor(inv_scale * p.y()));
        bool isEven = yInteger % 2 == 0;
        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    std::shared_ptr<texture> even;
    std::shared_ptr<texture> odd;
};


#include "stb_image.h" 

class image_texture : public texture {
public:
    // load image
    image_texture(const std::string& filename) {
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 3);
        if (!data) {
            std::cerr << "Failed to load texture: " << filename << "\n";
            width = height = 0;
            return;
        }

        pixels.assign(data, data + width * height * 3);
        stbi_image_free(data);
    }

    bool is_valid() const { return width > 0 && height > 0; }

    color value(double u, double v, const point& p) const override {
        if (!is_valid()) return color(0,1,1); // if image fails to load, default to cyan

        u = clamp(u, 0.0, 1.0);
        v = clamp(v, 0.0, 1.0);
        v = 1.0 - v;

        int i = std::min(int(u * width), width - 1);
        int j = std::min(int(v * height), height - 1);

        int idx = 3 * (j * width + i);
        double scale = 1.0 / 255.0;
        return color(
            scale * pixels[idx],
            scale * pixels[idx + 1],
            scale * pixels[idx + 2]
        );
    }

private:
    int width = 0, height = 0, channels = 0;
    std::vector<unsigned char> pixels;

    static double clamp(double x, double min, double max) {
        return x < min ? min : (x > max ? max : x);
    }
};

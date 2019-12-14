#pragma once
#include<string>
#include<SFML/Graphics.hpp>

extern sf::Font game_font;

class ResourceLoadException {
  public:
    ResourceLoadException(const std::string &reason): what_(reason) {}
    ResourceLoadException(std::string &&reason): what_(reason) {}
    std::string what() const noexcept {
        return what_;
    }
  private:
    std::string what_;
};

template <typename T, typename U, typename V>
bool in_range(T a, U lower_bound, V upper_bound) {
    return lower_bound <= a && a < upper_bound;
}

#pragma once
#include<vector>
#include<SFML/Graphics.hpp>

#include "common.hpp"

class Board {
  public:
    // constants
    static constexpr int UNOPENED = 0;
    static constexpr int BOMB = 1;
    static constexpr int FLAGGED = 2;
    static constexpr int FLAGGED_BOMB = 3;
    static constexpr int EXPLODED_BOMB = 4;
    static constexpr int OPENED = 5;
    static constexpr int TILE_SIZE_PIXELS = 24;
    // methods
    Board(int x, int y, int num_cols=30, int num_rows=16, int num_bombs=99);
    int get_value_at_pos(int x, int y) const noexcept;
    void draw(sf::RenderWindow& window) const noexcept;
    void handle_mouse_click(sf::Event::MouseButtonEvent event) noexcept;
    void expand_square(int tx, int ty) noexcept;
    void expand_square_neighbours(int tx, int ty) noexcept;
    void flag_square(int tx, int ty) noexcept;
    // public members
    int num_cols_;
    int num_rows_;
    int num_bombs_;
    bool game_ended_;
  private:
    // methods
    int count_neighbour_bombs(int tx, int ty) const noexcept;
    int count_neighbour_of_type(int tx, int ty, int type) const noexcept;
    void generate() noexcept;
    int draw_x_;
    int draw_y_;
    // private members
    std::vector<std::vector<int>> board_;
    sf::Texture board_texture_;
};

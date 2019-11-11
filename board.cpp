#include "board.hpp"
#include<cstdlib>
#include<iostream>

sf::Color colors[8] {
    {0,0,255,255},
    {0,127,0,255},
    {255,0,0,255},
    {0,0,127,255},
    {127,0,0,255},
    {0,191,191,255},
    {0,0,0,255},
    {215,215,215,255},
};

Board::Board(int x, int y, int num_cols, int num_rows, int num_bombs) :
        num_cols_{num_cols},
        num_rows_{num_rows},
        num_bombs_{num_bombs},
        game_ended_{false},
        draw_x_{x},
        draw_y_{y},
        board_(num_cols)
    {
    std::cout << "Initialized board:\n";
    std::cout << "  Columns: " << num_cols_ << '\n';
    std::cout << "  Rows: " << num_rows_ << '\n';
    std::cout << "  Bombs: " << num_bombs_ << '\n';
    for (auto &vec: board_) {
        vec.resize(num_rows);
    }
    if (!board_texture_.loadFromFile("res/board.png")) {
        throw ResourceLoadException("Failed to load board.png spritesheet");
    }
    generate();
}

int Board::get_value_at_pos(int x, int y) const noexcept {
    return board_[x][y];
}

void Board::generate() noexcept {
    for (int i = 0; i < num_bombs_;) {
        int x = random() % num_cols_;
        int y = random() % num_rows_;
        if (board_[x][y] == UNOPENED) {
            board_[x][y] = BOMB;
            ++i;
        }
    }
}

void Board::draw(sf::RenderWindow& window) const noexcept {
    //std::cout << "Rendering Board\n";
    //std::cout << board_.size() << ' ' << board_[0].size() << '\n';
    sf::Sprite unopened_tile(board_texture_, {
        0, 0, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS
    });
    sf::Sprite flagged_tile(board_texture_, {
        TILE_SIZE_PIXELS, 0, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS
    });
    sf::Sprite opened_tile(board_texture_, {
        2*TILE_SIZE_PIXELS, 0, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS
    });
    sf::Sprite bomb_tile(board_texture_, {
        0, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS
    });
    sf::Sprite exploded_bomb_tile(board_texture_, {
        TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS
    });
    for(int tx = 0; tx < num_cols_; ++tx) {
        for(int ty = 0; ty < num_rows_; ++ty) {
            sf::Sprite *sprite_to_draw;
            switch (board_[tx][ty]) {
                case UNOPENED: sprite_to_draw = &unopened_tile; break;
                case BOMB: sprite_to_draw = game_ended_ ? &bomb_tile : &unopened_tile; break;
                case EXPLODED_BOMB: sprite_to_draw = &exploded_bomb_tile; break;
                case FLAGGED: case FLAGGED_BOMB:
                    sprite_to_draw = &flagged_tile;
                    break;
                default: sprite_to_draw = &opened_tile; break;
            }
            sprite_to_draw->setPosition(
                draw_x_+tx*TILE_SIZE_PIXELS,
                draw_y_+ty*TILE_SIZE_PIXELS
            );
            window.draw(*sprite_to_draw);
        }
    }
    //std::cout << "Rendered Board\n";
}

void Board::expand_square(int tx, int ty) noexcept {
    switch (board_[tx][ty]) {
        case UNOPENED: board_[tx][ty] = OPENED; break;
        case BOMB: board_[tx][ty] = EXPLODED_BOMB; game_ended_ = true; break;
    }
}

void Board::expand_square_neighbours(int tx, int ty) noexcept {
}

void Board::flag_square(int tx, int ty) noexcept {
    switch (board_[tx][ty]) {
        case UNOPENED: board_[tx][ty] = FLAGGED; break;
        case BOMB: board_[tx][ty] = FLAGGED_BOMB; break;
        case FLAGGED: board_[tx][ty] = UNOPENED; break;
        case FLAGGED_BOMB: board_[tx][ty] = BOMB; break;
    }
}

void Board::handle_mouse_click(sf::Event::MouseButtonEvent event) noexcept {
    if (game_ended_) return;
    int mouse_x = event.x - draw_x_;
    int mouse_y = event.y - draw_y_;
    sf::Mouse::Button mouse_button = event.button;
    const int board_width = TILE_SIZE_PIXELS * num_cols_;
    const int board_height = TILE_SIZE_PIXELS * num_rows_;
    if (in_range(mouse_x, 0, board_width) && in_range(mouse_y, 0, board_height)) {
        int tx = mouse_x / TILE_SIZE_PIXELS;
        int ty = mouse_y / TILE_SIZE_PIXELS;
        switch (mouse_button) {
        case sf::Mouse::Button::Left: expand_square(tx, ty); break;
        case sf::Mouse::Button::Right: flag_square(tx, ty); break;
        case sf::Mouse::Button::Middle: expand_square_neighbours(tx, ty); break;
        }
    }
}

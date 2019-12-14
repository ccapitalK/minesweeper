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

int Board::get_num_remaining() const noexcept {
    return num_bombs_ - num_flagged_;
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
    sf::Sprite misflagged_tile(board_texture_, {
        2*TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS, TILE_SIZE_PIXELS
    });
    char square_text_string[2] = {'0', '\0'};
    sf::Text square_text("0", game_font, 16);
    for(int tx = 0; tx < num_cols_; ++tx) {
        for(int ty = 0; ty < num_rows_; ++ty) {
            sf::Sprite *sprite_to_draw;
            // determine tile to draw
            switch (board_[tx][ty]) {
                case UNOPENED: sprite_to_draw = &unopened_tile; break;
                case BOMB: sprite_to_draw = game_ended_ ? &bomb_tile : &unopened_tile; break;
                case EXPLODED_BOMB: sprite_to_draw = &exploded_bomb_tile; break;
                case FLAGGED_BOMB: sprite_to_draw = &flagged_tile; break;
                case FLAGGED: sprite_to_draw = game_ended_ ? &misflagged_tile : &flagged_tile; break;
                default:
                    sprite_to_draw = &opened_tile;
                    break;
            }
            // draw tile
            sprite_to_draw->setPosition(
                draw_x_+tx*TILE_SIZE_PIXELS,
                draw_y_+ty*TILE_SIZE_PIXELS
            );
            window.draw(*sprite_to_draw);
            // draw text
            if (board_[tx][ty] > OPENED) {
                int num_surrounding = board_[tx][ty] - OPENED;
                square_text_string[0] = num_surrounding - 1 + '1';
                square_text.setString(square_text_string);
                square_text.setFillColor(colors[num_surrounding-1]);
                square_text.setPosition(
                    draw_x_+tx*TILE_SIZE_PIXELS+4,
                    draw_y_+ty*TILE_SIZE_PIXELS+2
                );
                window.draw(square_text);
            }
        }
    }
    //std::cout << "Rendered Board\n";
}

int Board::count_neighbour_bombs(int tx, int ty) const noexcept {
    return count_neighbour_of_type(tx, ty, BOMB)
            + count_neighbour_of_type(tx, ty, FLAGGED_BOMB)
            + count_neighbour_of_type(tx, ty, EXPLODED_BOMB);
}

int Board::count_neighbour_of_type(int tx, int ty, int type) const noexcept {
    int ans = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            int x = tx+dx;
            int y = ty+dy;
            if ((dx || dy) && in_range(x, 0, num_cols_) && in_range(y, 0, num_rows_)) {
                if(board_[x][y] == type) ++ans;
            }
        }
    }
    return ans;
}

void Board::expand_square(int tx, int ty) noexcept {
    switch (board_[tx][ty]) {
        case UNOPENED:
            board_[tx][ty] = OPENED + count_neighbour_bombs(tx, ty);
            if (board_[tx][ty] == OPENED) {
                expand_square_neighbours(tx, ty);
            }
            break;
        case BOMB: board_[tx][ty] = EXPLODED_BOMB; game_ended_ = true; break;
    }
}

void Board::expand_square_neighbours(int tx, int ty) noexcept {
    if (board_[tx][ty] < OPENED) return;
    int num_surrounding_bombs = board_[tx][ty] - OPENED;
    int num_surrounding_flags = count_neighbour_of_type(tx, ty, FLAGGED)
            + count_neighbour_of_type(tx, ty, FLAGGED_BOMB);
    if (num_surrounding_bombs == num_surrounding_flags) {
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                int x = tx+dx;
                int y = ty+dy;
                if ((dx || dy) && in_range(x, 0, num_cols_) && in_range(y, 0, num_rows_)) {
                    expand_square(x, y);
                }
            }
        }
    }
}

void Board::flag_square(int tx, int ty) noexcept {
    switch (board_[tx][ty]) {
        case UNOPENED: board_[tx][ty] = FLAGGED; ++num_flagged_; break;
        case BOMB: board_[tx][ty] = FLAGGED_BOMB; ++num_flagged_; break;
        case FLAGGED: board_[tx][ty] = UNOPENED; --num_flagged_; break;
        case FLAGGED_BOMB: board_[tx][ty] = BOMB; --num_flagged_; break;
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

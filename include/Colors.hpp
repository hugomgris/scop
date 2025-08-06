#pragma once

#ifndef COLORS_HPP
# define COLORS_HPP

# include "glm/glm.hpp"
# include "glm/gtc/matrix_transform.hpp"
# include "glm/gtc/type_ptr.hpp"

struct Color {
    float r, g, b, a;
    
    constexpr Color(float r, float g, float b, float a = 1.0f) 
        : r(r), g(g), b(b), a(a) {}
};

namespace Colors {
    constexpr Color BLACK_CHARCOAL_1{0.12f, 0.12f, 0.13f, 1.0f};
    constexpr Color BLACK_CHARCOAL_2{0.15f, 0.15f, 0.15f, 1.0f};
    constexpr Color BLACK_CHARCOAL_3{0.1f, 0.1f, 0.12f, 1.0f};
    constexpr Color BLACK_CHARCOAL_4{0.08f, 0.08f, 0.08f, 1.0f};
    constexpr Color WHITE{1.0f, 1.0f, 1.0f, 1.0f};
    constexpr Color OFF_WHITE{0.9f, 0.9f, 0.85f, 1.0f};
    constexpr Color RED{1.0f, 0.0f, 0.0f, 1.0f};
    constexpr Color GREEN{0.0f, 1.0f, 0.0f, 1.0f};
    constexpr Color BLUE{0.0f, 0.0f, 1.0f, 1.0f};
    constexpr Color GRAY{0.5f, 0.5f, 0.5f, 1.0f};
    constexpr Color DARK_GRAY{0.3f, 0.3f, 0.3f, 1.0f};
}

//inline is used so that the function can be defined in the .hpp file (avoiding multiple definitions error)
inline void setClearColor(const Color &color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

inline void setLineColor(int lineColorLoc, const Color &color) {
    glUniform3f(lineColorLoc, color.r, color.g, color.b);
}

#endif
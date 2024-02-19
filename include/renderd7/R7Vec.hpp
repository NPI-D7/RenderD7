#pragma once

struct R7Vec2 {
  // Init Funcs
  R7Vec2() : x(0), y(0) {}
  R7Vec2(float i0, float i1) : x(i0), y(i1) {}
  R7Vec2(const R7Vec2 &i) {
    x = i.x;
    y = i.y;
  }

  // Operators
  // Add
  R7Vec2 &operator+=(const R7Vec2 &i) {
    x += i.x;
    y += i.y;
    return *this;
  }

  R7Vec2 operator+(const R7Vec2 &i) const { return R7Vec2(x + i.x, y + i.y); }

  // Sub
  R7Vec2 &operator-=(const R7Vec2 &i) {
    x -= i.x;
    y -= i.y;
    return *this;
  }

  R7Vec2 operator-(const R7Vec2 &i) const { return R7Vec2(x - i.x, y - i.y); }

  // Compare
  bool operator==(const R7Vec2 &in) const { return x == in.x && y == in.y; }

  bool operator!=(const R7Vec2 &in) const {
    // use the first comparefuncs result
    // and swap it lol
    return !(*this == in);
  }
  // Internal Values
  float x;
  float y;
};
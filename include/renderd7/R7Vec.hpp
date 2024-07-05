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

struct R7Vec4 {
  // Init Funcs
  R7Vec4() : x(0), y(0), z(0), w(0) {}
  R7Vec4(float i0, float i1, float i2, float i3) : x(i0), y(i1), z(i2), w(i3) {}
  R7Vec4(const R7Vec4 &i) {
    x = i.x;
    y = i.y;
    z = i.z;
    w = i.w;
  }

  R7Vec4(const R7Vec2 &i0, const R7Vec2 &i1) {
    x = i0.x;
    y = i0.y;
    z = i1.x;
    w = i1.y;
  }

  // Operators
  // Add
  R7Vec4 &operator+=(const R7Vec4 &i) {
    x += i.x;
    y += i.y;
    z += i.z;
    w += i.w;
    return *this;
  }

  R7Vec4 operator+(const R7Vec4 &i) const {
    return R7Vec4(x + i.x, y + i.y, z + i.z, w + i.w);
  }

  // Sub
  R7Vec4 &operator-=(const R7Vec4 &i) {
    x -= i.x;
    y -= i.y;
    z -= i.z;
    w -= i.w;
    return *this;
  }

  R7Vec4 operator-(const R7Vec4 &i) const {
    return R7Vec4(x - i.x, y - i.y, z - i.z, w - i.w);
  }

  // Compare
  bool operator==(const R7Vec4 &in) const {
    return x == in.x && y == in.y && z == in.z && w == in.w;
  }

  bool operator!=(const R7Vec4 &in) const {
    // use the first comparefuncs result
    // and swap it lol
    return !(*this == in);
  }
  // Internal Values
  float x;
  float y;
  float z;
  float w;
};
#pragma once

#include <rd7.hpp>

class Sample : public RD7::Scene {
 public:
  enum State {
    State_Menu,
  };
  Sample();
  ~Sample();

  void Draw() const override;
  void Logic() override;

 private:
  // Just as the RD7 Settings Shows here as example
  // by using a mutale map you're able to process
  // requests from the const Draw function in the
  // Logic Function! you could also create lots of
  // mutable bool or make all Variables mutable but
  // i think seperating draw and logic is better
  // editable map  request_id    data/operation
  mutable std::map<unsigned int, unsigned int> shared_requests;
  // For Checkbox its best to use mutable
  mutable bool debug_background = false;
  mutable std::string search__ = "";
  mutable std::vector<std::string> names;
  mutable std::vector<std::string> files;
  mutable int sel;
  mutable float txt_size = 0.5f;
  State state = State_Menu;
};
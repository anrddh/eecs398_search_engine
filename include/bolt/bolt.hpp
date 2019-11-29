#pragma once

#include "fb/memory.hpp"
#include "html_page.hpp"

class BoltImpl;

class Bolt {
 public:
  Bolt();
  Bolt(Bolt &&other);
  Bolt operator=(Bolt &&other);
  ~Bolt();

  void run();

  void registerHandler(fb::String path, HtmlPage (*page)());

  void setDefaultPath(HtmlPage (*page)());

  Bolt(Bolt const &other) = delete;
  Bolt operator=(Bolt const &other) = delete;

 private:
  fb::UniquePtr<BoltImpl> pImpl;
};

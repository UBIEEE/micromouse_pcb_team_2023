#pragma once

class Subsystem {

protected:
  Subsystem() = default;

public:
  virtual ~Subsystem() = default;

  virtual void process() {}
};

// device.h
#ifndef DEVICE_H
#define DEVICE_H

#define TEMP 1
#define FAN 0
#include <string>
#include <utility>
#include <vector>

class Device {
  std::string set_name(std::string p);
  int set_sensor_count(std::string p, bool type);
  // std::vector<std::pair> sensor_readings;

  public:
  const std::string file_path;
  const std::string name;
  const int temp_sensor_count;
  const int fan_sensor_count;

  Device(std::string p);
};

#endif

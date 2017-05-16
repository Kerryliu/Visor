// device.h
#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <utility>
#include <vector>

#define TEMP 1
#define FAN 0

class Device {
  public:
  const std::string file_path;
  const std::string name;
  const int temp_sensor_count;
  const int fan_sensor_count;

  Device(std::string p);

  const std::vector<std::pair<std::string, int>> &get_sensor_readings(bool type) const;

  private:
  std::vector<std::pair<std::string, int>> temp_sensor_readings;
  std::vector<std::pair<std::string, int>> fan_sensor_readings;

  std::string set_name(std::string p);
  int set_sensor_count(std::string p, bool type);
};

#endif

// device.h
#ifndef DEVICE_H
#define DEVICE_H

#include <fstream>
#include <string>
#include <utility>
#include <vector>

// Only doing 4 for now, for simplicity
// https://www.kernel.org/doc/Documentation/hwmon/sysfs-interface
#define VOLTAGE 0
#define FAN 1
#define PWM 2
#define TEMPERATURE 3

class Device {
public:
  const std::string file_path;
  const std::string name;
  const std::vector<std::string> sensor_types = {"/in", "/fan", "/pwm",
                                                 "/temp"};
  const std::vector<unsigned int> sensor_type_counts =
      std::vector<unsigned int>(sensor_types.size());

  Device(std::string file_path);

  const std::vector<std::vector<std::pair<std::string, int>>> &
  get_sensor_readings();

private:
  std::vector<std::vector<std::pair<std::string, int>>> sensor_readings =
      std::vector<std::vector<std::pair<std::string, int>>>(
          sensor_types.size());

  std::string set_name(std::string file_path);
  const std::vector<unsigned int> set_sensor_count(std::string file_path);
  void refresh_sensors();
};

#endif

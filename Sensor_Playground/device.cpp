#include "device.h"
#include <fstream>

Device::Device(std::string p)
    : file_path(p), name(this->set_name(p)),
      temp_sensor_count(this->set_sensor_count(p, TEMP)),
      fan_sensor_count(this->set_sensor_count(p, FAN)) {}

const std::vector<std::pair<std::string, int>> &Device::get_sensor_readings(bool type) const {
  return type ? temp_sensor_readings : fan_sensor_readings;
}

std::string Device::set_name(std::string p) {
  std::ifstream file;
  std::string name;
  file.open(p + '/' + "name");
  getline(file, name);
  return name;
}

int Device::set_sensor_count(std::string p, bool type) {
  std::ifstream file;
  std::string sensor_type = type ? "/temp" : "/fan";
  for(unsigned int sensor_count = 0; sensor_count < 100; sensor_count++) {
    std::string filename = p + sensor_type + std::to_string(sensor_count + 1) + "_input";
    file.open(filename);
    if (file) {
      file.close();
    } else {
      return sensor_count;
    }
  }
  return 0;
}

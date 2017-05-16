#include <string>
#include <iostream>
#include <vector>
#include "device.h"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

int main() {
  std::vector<Device> devices;
  std::string path = "/sys/class/hwmon/";

  for (auto & p : fs::directory_iterator(path)) {
    devices.push_back(Device(p.path().string()));
  }

  for (auto & d : devices) {
    std::cout << "Device Name: " << d.name << std::endl;
    //std::cout << "Temperature Sensor Count: " << d.sensor_type_counts[TEMPERATURE] << std::endl;
    //std::cout << "Fan Sensor Count: " << d.sensor_type_counts[FAN] << std::endl;

    std::vector<std::vector<std::pair<std::string, int>>> readings = d.get_sensor_readings();
    std::vector<std::pair<std::string, int>> temp_readings = readings[TEMPERATURE];
    std::vector<std::pair<std::string, int>> fan_readings = readings[FAN];
  
    for(auto &name_value_pair : temp_readings) {
      std::cout << name_value_pair.first << ":  " << name_value_pair.second/1000 << " degrees" << std::endl;
    }

    for(auto &name_value_pair : fan_readings) {
      std::cout << name_value_pair.first << ":  " << name_value_pair.second << " rpm" << std::endl;
    }
    std::cout << std::endl;
  }
}

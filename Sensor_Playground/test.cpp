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
    std::cout << "  Temperature Sensor Count: " << d.temp_sensor_count << std::endl;
    std::cout << "  Fan Sensor Count: " << d.fan_sensor_count << std::endl;
  }
}

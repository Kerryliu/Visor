// device.h
#ifndef DEVICE_H
#define DEVICE_H

#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <iomanip>

// Only doing 4 for now, for simplicity
// https://www.kernel.org/doc/Documentation/hwmon/sysfs-interface
#define VOLTAGE 0
#define FAN 1
#define PWM 2
#define TEMPERATURE 3

using std::vector;
using std::string;

class Device {
public:
  const string file_path;
  const string name;
  // Make sure follow two vectors have same size or shit will hit the fan
  const vector<string> sensor_types = {"Voltage", "Fan", "PWM", "Temperature"};
  const vector<string> sensor_types_paths = {"/in", "/fan", "/pwm", "/temp"};

  const vector<unsigned int> sensor_type_counts =
      vector<unsigned int>(sensor_types.size());

  Device(string file_path);

  void refresh_sensors();
  static string formatValue(int value, int sensor_type);
  const vector<vector<std::pair<string, int>>> &get_sensor_readings();

private:
  vector<vector<std::pair<string, int>>> sensor_readings =
      vector<vector<std::pair<string, int>>>(sensor_types.size());

  std::string set_name(std::string file_path);
  const vector<unsigned int> set_sensor_count(std::string file_path);
};

#endif

// device.h
#ifndef DEVICE_H
#define DEVICE_H

#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <iomanip>
#include <climits>

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
  struct sensor_reading {
    string name;
    int current_value = 0;
    int min_value = INT_MAX;
    int max_value = INT_MIN;
    int refresh_count = 0;
    int average_value = 0;
  };

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
  const vector<vector<sensor_reading>> &get_sensor_readings();

private:
  vector<vector<sensor_reading>> sensor_readings =
      vector<vector<sensor_reading>>(sensor_types.size());

  std::string set_name(std::string file_path);
  const vector<unsigned int> set_sensor_count(std::string file_path);
};

#endif

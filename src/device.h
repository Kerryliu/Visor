#ifndef DEVICE_H
#define DEVICE_H

#include <climits>
#include <string>
#include <vector>

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
    int cur_val = 0;
    int min_val = INT_MAX;
    int max_val = INT_MIN;
    int tick = 0;
    long int avg_val = 0;
  };

  const string file_path;
  const string name;
  // Make sure follow vectors have same size or shit will hit the fan
  static const vector<string> sensor_types;
  static const vector<string> sensor_types_paths;
  static const vector<string> sensor_types_icons;
  static const vector<int> sensor_max_vals;
  static const vector<string> sensor_units;

  const vector<unsigned int> sensor_type_counts =
      vector<unsigned int>(sensor_types.size());

  Device(string file_path);

  void refresh_sensors();
  static string formatValue(int val, int sensor_type);
  const vector<vector<sensor_reading>> &get_sensor_readings() const;

private:
  vector<vector<sensor_reading>> sensor_readings =
      vector<vector<sensor_reading>>(sensor_types.size());

  std::string set_name(std::string file_path);
  const vector<unsigned int> set_sensor_count(std::string file_path);
};

#endif

#include "device.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

using std::vector;
using std::string;

const vector<string> Device::sensor_types({"Voltage", "Fan", "PWM",
                                           "Temperature"});
const vector<string> Device::sensor_types_paths({"/in", "/fan", "/pwm",
                                                 "/temp"});
const vector<string> Device::sensor_types_icons(
    {"assets/in.svg", "assets/fan.svg", "assets/pwm.svg", "assets/temp.svg"});
const vector<unsigned int> Device::sensor_max_vals({10000, 3000, 10, 100000});
const vector<string> Device::sensor_units({" V", " RPM", " V", " \u2103"});

Device::Device(string file_path)
    : file_path(file_path), name(set_name(file_path)),
      sensor_type_counts(set_sensor_count(file_path)) {
  // Resize vector that stores all the stuffs for the detected sensor count~
  for (unsigned int type = 0; type < sensor_types.size(); type++) {
    sensor_readings[type].resize(sensor_type_counts[type]);
  }
}

const vector<vector<Device::sensor_reading>> &
Device::get_sensor_readings() const {
  return sensor_readings;
}

void Device::refresh_sensors() {
  vector<vector<Device::sensor_reading>> prev_sensor_readings =
      get_sensor_readings();
  for (unsigned int type = 0; type < sensor_types.size(); type++) {
    vector<sensor_reading> cur_sensor_type_readings(sensor_type_counts[type]);
    vector<sensor_reading> &prev_sensor_type_readings =
        prev_sensor_readings[type];

    for (unsigned int sensor_index = 0; sensor_index < sensor_type_counts[type];
         sensor_index++) {
      string cur_sensor_name;

      // Previous values to compare against:
      sensor_reading prev_sensor_type_reading =
          prev_sensor_type_readings[sensor_index];
      int prev_cur_val = prev_sensor_type_reading.cur_val;
      int prev_min_val = prev_sensor_type_reading.min_val;
      int prev_max_val = prev_sensor_type_reading.max_val;
      int prev_avg_val = prev_sensor_type_reading.avg_val;
      int prev_tick = prev_sensor_type_reading.tick;

      // New values to be:
      int cur_cur_val;
      int cur_min_val;
      int cur_max_val;
      int cur_avg_val;
      int cur_tick;
      std::ifstream file;
      string temp_string;

      // Get sensor value:
      file.open(file_path + sensor_types_paths[type] +
                std::to_string(sensor_index + 1) + "_input");
      getline(file, temp_string);
      file.close();

      // Sleep sometimes breaks stoi for some reason
      try {
        cur_cur_val = std::stoi(temp_string);
      } catch (...) {
        cur_cur_val = prev_cur_val;
      }
      // Do calculations for min, max, and avg
      cur_min_val = (cur_cur_val < prev_min_val) ? cur_cur_val : prev_min_val;
      cur_max_val = (cur_cur_val > prev_max_val) ? cur_cur_val : prev_max_val;
      cur_tick = prev_tick + 1;
      // Probably a better way of calculating avg.  This will do for now.
      cur_avg_val = ((prev_avg_val * prev_tick) + cur_cur_val) / cur_tick;

      // see if sensor value has name:
      file.open(file_path + sensor_types_paths[type] +
                std::to_string(sensor_index + 1) + "_label");
      if (file) {
        getline(file, cur_sensor_name);
        file.close();
      } else {
        cur_sensor_name = "Sensor " + std::to_string(sensor_index + 1);
      }

      // Create new sensor_reading struct for cur vals
      sensor_reading cur_sensor_reading;
      cur_sensor_reading.name = cur_sensor_name;
      cur_sensor_reading.cur_val = cur_cur_val;
      cur_sensor_reading.min_val = cur_min_val;
      cur_sensor_reading.max_val = cur_max_val;
      cur_sensor_reading.tick = cur_tick;
      cur_sensor_reading.avg_val = cur_avg_val;
      cur_sensor_type_readings[sensor_index] = cur_sensor_reading;
    }
    sensor_readings[type] = cur_sensor_type_readings;
  }
}

string Device::formatValue(int val, int sensor_type) {
  std::ostringstream temp;
  string val_string;
  switch (sensor_type) {
  case VOLTAGE:
    temp << std::setprecision(2) << std::fixed << ((double)val / 1000);
    val_string = temp.str();
    break;
  case TEMPERATURE:
    temp << std::setprecision(1) << std::fixed << ((double)val / 1000);
    val_string = temp.str();
    break;
  default:
    val_string = std::to_string(val);
    break;
  }
  val_string += sensor_units[sensor_type];
  return val_string;
}

string Device::set_name(string file_path) {
  std::ifstream file;
  string file_name;
  file.open(file_path + '/' + "name");
  if (file) {
    getline(file, file_name);
    file.close();
  } else {
    file_name = "unknown";
  }
  return file_name;
}

const vector<unsigned int> Device::set_sensor_count(string file_path) {
  vector<unsigned int> sensor_type_counts(sensor_types.size());
  // Try opening sensor file; if it succeeds, the sensor exists
  for (unsigned int type = 0; type < sensor_types.size(); type++) {
    std::ifstream file;
    for (unsigned int sensor_count = 0; sensor_count < 100; sensor_count++) {
      file.open(file_path + sensor_types_paths[type] +
                std::to_string(sensor_count + 1) + "_input");
      if (file) {
        file.close();
      } else {
        sensor_type_counts[type] = sensor_count;
        break;
      }
    }
  }
  return sensor_type_counts;
}

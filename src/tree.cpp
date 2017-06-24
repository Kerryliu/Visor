#include "tree.h"
#include <experimental/filesystem>
#include <gtkmm.h>
#include <iostream>
#include <vector>

namespace fs = std::experimental::filesystem;

Tree::Tree(vector<vector<vector<Device::sensor_reading>>> &all_readings,
           vector<string> &device_names) {
  this->all_readings = all_readings;
  this->device_names = device_names;

  make_tree_view();
}

void Tree::make_tree_view() {
  m_ref_tree_model = Gtk::TreeStore::create(m_columns);
  m_treeview.set_model(m_ref_tree_model);
  Gtk::TreeModel::Row device_row;
  Gtk::TreeModel::Row sensor_types_row;
  Gtk::TreeModel::Row readings_row;

  for (unsigned int i = 0; i < all_readings.size(); i++) {
    vector<vector<Device::sensor_reading>> &device_readings = all_readings[i];

    device_row = *(m_ref_tree_model->append());
    device_row[m_columns.m_col_name] = device_names[i] + ": ";
    device_row[m_columns.m_pixbuf] = Gdk::Pixbuf::create_from_file(
        "assets/chip.svg", icon_size, icon_size, true);

    for (unsigned int sensor_type = 0; sensor_type < device_readings.size();
         sensor_type++) {
      if (!device_readings[sensor_type].empty()) {
        sensor_types_row = *(m_ref_tree_model->append(device_row.children()));
        sensor_types_row[m_columns.m_col_name] =
            Device::sensor_types[sensor_type] + ": ";
        sensor_types_row[m_columns.m_pixbuf] = Gdk::Pixbuf::create_from_file(
            Device::sensor_types_icons[sensor_type], icon_size, icon_size,
            true);

        for (unsigned int j = 0; j < device_readings[sensor_type].size(); j++) {
          readings_row =
              *(m_ref_tree_model->append(sensor_types_row.children()));
          readings_row[m_columns.m_col_name] =
              device_readings[sensor_type][j].name + ": ";
          readings_row[m_columns.m_pixbuf] = Gdk::Pixbuf::create_from_file(
              "assets/sensor.svg", icon_size, icon_size, true);
          readings_row[m_columns.m_col_cur_val] = Device::formatValue(
              device_readings[sensor_type][j].cur_val, sensor_type);
          readings_row[m_columns.m_col_min_val] = Device::formatValue(
              device_readings[sensor_type][j].min_val, sensor_type);
          readings_row[m_columns.m_col_max_val] = Device::formatValue(
              device_readings[sensor_type][j].max_val, sensor_type);
          readings_row[m_columns.m_col_avg_val] = Device::formatValue(
              device_readings[sensor_type][j].avg_val, sensor_type);
        }
      }
    }
  }

  // Add the TreeView's view columns:
  m_columns.sensor_column->pack_start(m_columns.m_pixbuf, false);
  m_columns.sensor_column->pack_end(m_columns.m_col_name, true);
  m_treeview.append_column(*m_columns.sensor_column);
  m_treeview.append_column("Current", m_columns.m_col_cur_val);
  m_treeview.append_column("Min", m_columns.m_col_min_val);
  m_treeview.append_column("Max", m_columns.m_col_max_val);
  m_treeview.append_column("Average", m_columns.m_col_avg_val);

  m_treeview.expand_all();
  m_treeview.set_enable_tree_lines(true);
  m_treeview.set_rules_hint(true);
  m_treeview.get_column(0)->set_expand(true);

  for (unsigned int i = 0; i < m_treeview.get_n_columns(); i++) {
    m_treeview.get_column(i)->set_resizable(true);
  }
}

void Tree::update_tree_view(
    vector<vector<vector<Device::sensor_reading>>> &all_readings) {
  this->all_readings = all_readings;
  Gtk::TreeModel::Children rows = m_ref_tree_model->children();
  unsigned int device_index = 0;
  for (auto &device_row : rows) {
    // Get Readings
    vector<vector<Device::sensor_reading>> &device_readings =
        all_readings[device_index];

    // Get Fan & Temp children of device
    Gtk::TreeModel::Children sensor_types_row = device_row->children();
    Gtk::TreeModel::Children::iterator iter_sensor_type =
        sensor_types_row.begin();

    for (unsigned int sensor_type = 0; sensor_type < device_readings.size();
         sensor_type++) {
      if (!device_readings[sensor_type].empty()) {
        Gtk::TreeModel::Children sensors = iter_sensor_type->children();
        Gtk::TreeModel::Children::iterator iter_sensors = sensors.begin();
        for (unsigned int sensor_val = 0;
             sensor_val < device_readings[sensor_type].size();
             sensor_val++, iter_sensors++) {
          Gtk::TreeModel::Row vals = *iter_sensors;
          vals[m_columns.m_col_cur_val] = Device::formatValue(
              device_readings[sensor_type][sensor_val].cur_val, sensor_type);
          vals[m_columns.m_col_min_val] = Device::formatValue(
              device_readings[sensor_type][sensor_val].min_val, sensor_type);
          vals[m_columns.m_col_max_val] = Device::formatValue(
              device_readings[sensor_type][sensor_val].max_val, sensor_type);
          vals[m_columns.m_col_avg_val] = Device::formatValue(
              device_readings[sensor_type][sensor_val].avg_val, sensor_type);
        }
        iter_sensor_type++;
      }
    }
    device_index++;
  }
}

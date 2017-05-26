#ifndef GTKMM_LEGEND_H
#define GTKMM_LEGEND_H

#include "device.h"
#include <gtkmm.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

class Legend {

public:
  Gtk::FlowBox m_legend;
  Legend(const vector<Device::sensor_reading> &sensor_readings,
         unsigned int device_index, unsigned int type);
  const unsigned int get_type() const;
  const unsigned int get_device_index() const;

private:
  vector<std::shared_ptr<Gdk::RGBA>> m_colors;
  vector<std::shared_ptr<Gtk::ColorButton>> m_color_buttons;
  vector<std::shared_ptr<Gtk::Label>> m_labels;
  vector<std::shared_ptr<Gtk::Box>> m_key_boxes;
  unsigned int device_index;
  unsigned int type;
};

#endif

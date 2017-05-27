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
         unsigned int device_index, unsigned int type,
         vector<Gdk::RGBA> &m_colors);

  const unsigned int get_type() const;
  const unsigned int get_device_index() const;

private:
  unsigned int device_index;
  unsigned int type;
  vector<std::shared_ptr<Gtk::ColorButton>> m_color_buttons;
  vector<std::shared_ptr<Gtk::Label>> m_labels;
  vector<std::shared_ptr<Gtk::Box>> m_key_boxes;
  vector<Gdk::RGBA> &m_colors;

  void on_color_button_color_set(unsigned int color_button_index);
};

#endif

#include "device.h"
#include "legend.h"
#include <gtkmm.h>
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;

Legend::Legend(const vector<Device::sensor_reading> &sensor_readings,
               unsigned int device_index, unsigned int type, unsigned int width)
    : device_index(device_index), type(type) {
  m_legend.set_max_children_per_line(30);
  m_legend.set_selection_mode(Gtk::SelectionMode::SELECTION_NONE);
  m_legend.set_homogeneous();
  // Create all the color key thingies
  for (unsigned int i = 0; i < sensor_readings.size(); i++) {
    // Make label:
    std::shared_ptr<Gtk::Label> cur_label(
        new Gtk::Label(sensor_readings[i].name));
    m_labels.push_back(cur_label);

    // Make color:
    std::shared_ptr<Gdk::RGBA> cur_color(new Gdk::RGBA);
    cur_color->set_red(0.5);
    cur_color->set_green(0.5);
    cur_color->set_blue(0.5);
    cur_color->set_alpha(1.0); // opaque
    m_colors.push_back(cur_color);

    // Make color button:
    std::shared_ptr<Gtk::ColorButton> cur_color_button(new Gtk::ColorButton);
    cur_color_button->set_rgba(*cur_color);
    cur_color_button->set_relief(Gtk::RELIEF_NONE);
    m_color_buttons.push_back(cur_color_button);

    // Pack them together:
    std::shared_ptr<Gtk::Box> cur_key_box(new Gtk::Box);
    cur_key_box->pack_start(*cur_color_button, Gtk::PACK_SHRINK);
    cur_key_box->pack_start(*cur_label, Gtk::PACK_SHRINK);
    m_key_boxes.push_back(cur_key_box);
    m_legend.add(*cur_key_box);
  }
}

const unsigned int Legend::get_type() const { return type; }
const unsigned int Legend::get_device_index() const { return device_index; }

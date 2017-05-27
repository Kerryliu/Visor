#include "device.h"
#include "legend.h"
#include <gtkmm.h>
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;

Legend::Legend(const vector<Device::sensor_reading> &sensor_readings,
               unsigned int device_index, unsigned int type,
               vector<Gdk::RGBA> &m_colors)
    : device_index(device_index), type(type), m_colors(m_colors) {
  m_legend.set_max_children_per_line(30);
  m_legend.set_selection_mode(Gtk::SelectionMode::SELECTION_NONE);
  m_legend.set_homogeneous();
  // Create all the color key thingies
  for (unsigned int i = 0; i < sensor_readings.size(); i++) {
    // Make label:
    std::shared_ptr<Gtk::Label> cur_label(
        new Gtk::Label(sensor_readings[i].name));
    m_labels.push_back(cur_label);

    // Make color button:
    std::shared_ptr<Gtk::ColorButton> cur_color_button(new Gtk::ColorButton);
    cur_color_button->set_rgba(m_colors[i]);
    cur_color_button->set_relief(Gtk::RELIEF_NONE);
    m_color_buttons.push_back(cur_color_button);

    // Link em':
    cur_color_button->signal_color_set().connect(sigc::bind(
        sigc::mem_fun(*this, &Legend::on_color_button_color_set), i));

    // Pack them together:
    std::shared_ptr<Gtk::Box> cur_key_box(new Gtk::Box);
    cur_key_box->pack_start(*cur_color_button, Gtk::PACK_SHRINK);
    cur_key_box->pack_start(*cur_label, Gtk::PACK_SHRINK);
    cur_key_box->set_can_focus(false);
    m_key_boxes.push_back(cur_key_box);
    m_legend.add(*cur_key_box);
  }
}

void Legend::on_color_button_color_set(unsigned int color_button_index) {
  m_colors[color_button_index] =
      m_color_buttons[color_button_index]->get_rgba();
}

const unsigned int Legend::get_type() const { return type; }
const unsigned int Legend::get_device_index() const { return device_index; }

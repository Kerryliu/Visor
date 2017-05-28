#ifndef GRAPH_H
#define GRAPH_H

#include "device.h"
#include <gtkmm/drawingarea.h>
#include <memory>
#include <vector>

using std::vector;

class Graph : public Gtk::DrawingArea {
public:
  Graph(const vector<Device::sensor_reading> &sensor_readings, int device_index,
        int type, vector<Gdk::RGBA> &m_colors);
  void update_vals(vector<Device::sensor_reading> &sensor_readings);
  const unsigned int get_type() const;
  const unsigned int get_device_index() const;

private:
  const unsigned int device_index;
  const unsigned int type;

  unsigned int width;
  unsigned int height;

  const unsigned int line_width = 1;
  const unsigned int graph_x_start = 25;
  const unsigned int graph_y_start = 40;
  const unsigned int right_padding = 55;
  const unsigned int scale_offset = 15;

  unsigned int graph_width;
  unsigned int graph_height;

  const unsigned int over_shoot = 5;
  unsigned int prev_height = 0;

  vector<std::list<unsigned int>> raw_vals;
  vector<std::list<unsigned int>> scaled_vals;
  vector<Device::sensor_reading> sensor_readings;
  vector<Gdk::RGBA> &m_colors;
  bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
  bool update();
  void check_resize();
  void draw_title(const Cairo::RefPtr<Cairo::Context> &cr);
  void draw_graph_grid(const Cairo::RefPtr<Cairo::Context> &cr);
  void make_plot(const Cairo::RefPtr<Cairo::Context> &cr);
};

#endif
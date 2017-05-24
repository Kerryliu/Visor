#ifndef GRAPH_H
#define GRAPH_H

#include "device.h"
#include <gtkmm/drawingarea.h>
#include <memory>
#include <vector>

using std::vector;

class Graph : public Gtk::DrawingArea {
public:
  Graph(const vector<Device::sensor_reading> &device_readings, int device_index,
        int type);
  virtual ~Graph();
  void update_values(vector<Device::sensor_reading> &device_readings);
  const unsigned int get_type() const;
  const unsigned int get_device_index() const;

private:
  unsigned int width;
  unsigned int height;
  const unsigned int line_width = 1;
  const unsigned int x_start = 25;
  const unsigned int y_start = 40;
  unsigned int right_padding = 55;
  unsigned int legend_offset;
  unsigned int rectangle_width = width - x_start - right_padding;
  unsigned int rectangle_height = height - y_start - legend_offset;
  const unsigned int max_line_count = 10;
  const unsigned int min_line_spacing = 40;
  const unsigned int over_shoot = 5;
  vector<vector<double>> colors;

  unsigned int prev_y_axis_pixels = 0;

  vector<std::list<unsigned int>> original_y;
  vector<std::list<unsigned int>> normalized_y;

  vector<Device::sensor_reading> device_readings;
  const unsigned int device_index;
  const unsigned int type;
  const vector<unsigned int> max_type_values = {10, 3000, 10, 100};
  const vector<string> type_units = {" V", " RPM", " V", " \u2103"};

  bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
  bool update();
  void check_resize();
  void gen_colors();
  void draw_title(const Cairo::RefPtr<Cairo::Context> &cr);
  // Returns bottom spacing needed for the legend
  void draw_legend(const Cairo::RefPtr<Cairo::Context> &cr);
  // Returns top left and bottom right corners of the rectangle
  void draw_graph_grid(const Cairo::RefPtr<Cairo::Context> &cr);
  void make_plot(const Cairo::RefPtr<Cairo::Context> &cr,
                 unsigned int sensor_index);
};

#endif

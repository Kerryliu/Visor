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
        int type);
  void update_values(vector<Device::sensor_reading> &sensor_readings);
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

  unsigned int right_padding = 55;
  unsigned int legend_offset;
  unsigned int graph_width = width - graph_x_start - right_padding;
  unsigned int graph_height = height - graph_y_start - legend_offset;

  const unsigned int max_line_count = 10;
  const unsigned int min_line_spacing = 40;
  const unsigned int over_shoot = 5;
  unsigned int prev_height = 0;

  vector<vector<double>> colors;
  vector<std::list<unsigned int>> raw_vals;
  vector<std::list<unsigned int>> scaled_vals;
  vector<Device::sensor_reading> sensor_readings;

  bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
  bool update();
  void check_resize();
  void gen_colors();
  void draw_title(const Cairo::RefPtr<Cairo::Context> &cr);
  void draw_legend(const Cairo::RefPtr<Cairo::Context> &cr);
  void draw_graph_grid(const Cairo::RefPtr<Cairo::Context> &cr);
  void make_plot(const Cairo::RefPtr<Cairo::Context> &cr);
};

#endif

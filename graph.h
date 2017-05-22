#ifndef GRAPH_H
#define GRAPH_H

#include "device.h"
#include <gtkmm/drawingarea.h>
#include <memory>
#include <vector>

using std::vector;

class Graph : public Gtk::DrawingArea {
public:
  Graph(const vector<Device::sensor_reading> &device_readings, int type);
  virtual ~Graph();

protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;

private:
  int width;
  int height;

  vector<vector<double>> colors;

  const unsigned int type;
  const vector<unsigned int> max_type_values = {10, 3000, 10, 100};
  vector<Device::sensor_reading> device_readings;

  void gen_colors();
  void draw_title(const Cairo::RefPtr<Cairo::Context> &cr);
  //Returns bottom spacing needed for the legend
  int draw_legend(const Cairo::RefPtr<Cairo::Context> &cr);
  // Returns top left and bottom right corners of the rectangle
  const vector<unsigned int>
  draw_graph_grid(const Cairo::RefPtr<Cairo::Context> &cr, int legend_offset);
};

#endif

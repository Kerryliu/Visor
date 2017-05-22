#ifndef GRAPH_H
#define GRAPH_H

#include "device.h"
#include <gtkmm/drawingarea.h>
#include <memory>
#include <vector>

class Graph : public Gtk::DrawingArea {
public:
  Graph(const vector<Device::sensor_reading> &device_readings,
        int type);
  virtual ~Graph();

protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;

private:
  int width;
  int height;

  void draw_title(const Cairo::RefPtr<Cairo::Context> &cr);
  void draw_graph_grid(const Cairo::RefPtr<Cairo::Context> &cr, int legend_offset);
  int draw_legend(const Cairo::RefPtr<Cairo::Context> &cr);
  int type;
  vector<Device::sensor_reading> device_readings;
};

#endif

#ifndef GRAPH_H
#define GRAPH_H

#include "device.h"
#include "memory"
#include <gtkmm/drawingarea.h>

class Graph : public Gtk::DrawingArea {
public:
  Graph(const Device &device, int type);
  virtual ~Graph();

protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;

private:
  int width;
  int height;
  // const int width = allocation.get_width();
  // const int height = allocation.get_height();

  void draw_title(const Cairo::RefPtr<Cairo::Context> &cr);
  void draw_graph_grid(const Cairo::RefPtr<Cairo::Context> &cr);
  int type;
  Device device;
  // Device device;
};

#endif

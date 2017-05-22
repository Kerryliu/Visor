#include "graph.h"
#include <gtkmm/drawingarea.h>
#include <iostream>

Graph::Graph(const Device &device, int type) : type(type), device(device) {}

Graph::~Graph() {}

// https://developer.gnome.org/gtkmm-tutorial/stable/chapter-drawingarea.html.en
bool Graph::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
  Gtk::Allocation allocation = get_allocation();
  width = allocation.get_width();
  height = allocation.get_height();
  draw_title(cr);
  draw_graph_grid(cr);
  return true;
}

void Graph::draw_title(const Cairo::RefPtr<Cairo::Context> &cr) {
  const int left_offset = 10;
  const int top_offset = 10;
  const double fifty_shades_of_grey = 0.4;
  cr->move_to(left_offset, top_offset);
  cr->set_source_rgb(fifty_shades_of_grey, fifty_shades_of_grey,
                     fifty_shades_of_grey);

  Pango::FontDescription font;
  font.set_weight(Pango::WEIGHT_BOLD);
  auto layout = create_pango_layout(Device::sensor_types[type]);

  layout->set_font_description(font);

  int text_width;
  int text_height;

  // get the text dimensions (it updates the variables -- by reference)
  layout->get_pixel_size(text_width, text_height);
  layout->show_in_cairo_context(cr);
}

void Graph::draw_graph_grid(const Cairo::RefPtr<Cairo::Context> &cr) {
  // Draw outer rectangle:
  const int bottom_offset = -25;
  const int left_offset = 0;
  unsigned int rectangle_height = height - 150;
  unsigned int rectangle_width = width - 100;
  unsigned int x_coord = (width - rectangle_width) / 2 + left_offset;
  unsigned int y_coord = (height - rectangle_height) / 2 + bottom_offset;
  const double fifty_shades_of_grey = 0.9;
  cr->set_source_rgb(fifty_shades_of_grey, fifty_shades_of_grey,
                     fifty_shades_of_grey);
  cr->rectangle(x_coord, y_coord, rectangle_width, rectangle_height);
  cr->fill();

  // Draw inner rectangle:
  const int inner_offset = 2;
  rectangle_height -= inner_offset;
  rectangle_width -= inner_offset;
  x_coord = (width - rectangle_width) / 2 + left_offset;
  y_coord = (height - rectangle_height) / 2 + bottom_offset;
  cr->set_source_rgb(1, 1, 1);
  cr->rectangle(x_coord, y_coord, rectangle_width, rectangle_height);
  cr->fill();

  // Draw lines:
  cr->set_line_width(1);
  cr->set_source_rgb(fifty_shades_of_grey, fifty_shades_of_grey,
                     fifty_shades_of_grey);
  // Vertical:
  const unsigned int max_line_count = 5;
  const unsigned int min_line_spacing = 25;
  unsigned int line_count = (rectangle_height / min_line_spacing > max_line_count)
                                ? max_line_count
                                : rectangle_height / min_line_spacing;
  unsigned int line_spacing = rectangle_height / line_count;
  for (unsigned int line_index = 1; line_index < line_count; line_index++) {
    cr->move_to(x_coord, y_coord + line_spacing * line_index);
    cr->line_to(x_coord + rectangle_width, y_coord + line_spacing * line_index);
  }
  // Horizontal:
  line_spacing = rectangle_width / max_line_count;
  for (unsigned int line_index = 1; line_index < max_line_count; line_index++) {
    cr->move_to(x_coord + line_spacing * line_index, y_coord);
    cr->line_to(x_coord + line_spacing * line_index,
                y_coord + rectangle_height);
  }
  cr->stroke();
}

#include "graph.h"
#include <glibmm/main.h>
#include <iostream>
#include <list>
#include <vector>

using std::vector;

Graph::Graph(const vector<Device::sensor_reading> &sensor_readings,
             int device_index, int type, vector<Gdk::RGBA> &m_colors)
    : device_index(device_index), type(type), sensor_readings(sensor_readings),
      m_colors(m_colors) {
  set_size_request(0, 25);
  raw_vals.resize(sensor_readings.size());
  scaled_vals.resize(sensor_readings.size());
}

const unsigned int Graph::get_type() const { return type; }
const unsigned int Graph::get_device_index() const { return device_index; }

void Graph::update_vals(vector<Device::sensor_reading> &sensor_readings) {
  this->sensor_readings = sensor_readings;
  update();
}

// https://developer.gnome.org/gtkmm-tutorial/stable/chapter-drawingarea.html.en
bool Graph::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
  Gtk::Allocation allocation = get_allocation();
  width = allocation.get_width();
  height = allocation.get_height();
  draw_title(cr);
  graph_width = width - graph_x_start - right_padding;
  graph_height = height - graph_y_start - scale_offset;
  check_resize();
  draw_graph_grid(cr);
  make_plot(cr);
  return true;
}

const unsigned int Graph::scale_val(unsigned int raw_val) const {
  const unsigned int scaled_val =
      graph_y_start + graph_height -
      ((double)raw_val / Device::sensor_max_vals[type]) * graph_height;
  return scaled_val;
}

const bool Graph::update() {
  // update values
  for (unsigned int i = 0; i < sensor_readings.size(); i++) {
    const unsigned int raw_val =
        (sensor_readings[i].cur_val >= 0) ? sensor_readings[i].cur_val : 0;
    const unsigned int scaled_val = scale_val(raw_val);
    raw_vals[i].push_front(raw_val);
    scaled_vals[i].push_front(scaled_val);
    if (scaled_vals[i].size() > ticks + 1) {
      raw_vals[i].pop_back();
      scaled_vals[i].pop_back();
    }
  }

  // Refresh window
  auto win = get_window();
  if (win) {
    Gdk::Rectangle r(graph_x_start, graph_y_start, graph_x_start + width,
                     graph_y_start + height);
    win->invalidate_rect(r, false);
  }
  return true;
}

void Graph::check_resize() {
  // On resize, need to scale all previous values
  if (prev_height != height) {
    prev_height = height;
    for (unsigned int i = 0; i < sensor_readings.size(); i++) {
      for (auto orig_y_it = raw_vals[i].begin(),
                norm_y_it = scaled_vals[i].begin();
           orig_y_it != raw_vals[i].end(); orig_y_it++, norm_y_it++) {
        *norm_y_it = scale_val(*orig_y_it);
      }
    }
  }
}

void Graph::draw_title(const Cairo::RefPtr<Cairo::Context> &cr) {
  const int left_offset = 10;
  const int top_offset = 10;
  const double title_color = 0.4;
  cr->move_to(left_offset, top_offset);
  cr->set_source_rgb(title_color, title_color, title_color);

  Pango::FontDescription font;
  font.set_weight(Pango::WEIGHT_BOLD);
  auto layout = create_pango_layout(Device::sensor_types[type]);

  layout->set_font_description(font);
  layout->show_in_cairo_context(cr);
}

void Graph::draw_graph_grid(const Cairo::RefPtr<Cairo::Context> &cr) {
  // See https://www.cairographics.org/FAQ/#sharp_lines for the 0.5's here
  cr->set_line_width(line_width);
  const unsigned int overshoot = 5;
  const double scale_line_color = 0.8;

  // Vertical scale lines:
  const unsigned int vect_min_line_spacing = 20;
  const unsigned int max_line_count = 20;
  cr->set_source_rgb(scale_line_color, scale_line_color, scale_line_color);
  unsigned int vert_line_count = graph_height / vect_min_line_spacing;
  if (vert_line_count >= max_line_count) {
    vert_line_count = max_line_count;
  } else if (vert_line_count >= max_line_count / 2) {
    vert_line_count = max_line_count / 2;
  } else if (vert_line_count >= max_line_count / 4) {
    vert_line_count = max_line_count / 4;
  } else if (vert_line_count == 3) {
    vert_line_count = 2;
  }

  const double vert_line_spacing = (double)graph_height / vert_line_count;
  for (unsigned int i = 1; i < vert_line_count; i++) {
    cr->move_to(graph_x_start,
                0.5 + graph_y_start + round(vert_line_spacing * i));
    cr->line_to(graph_x_start + graph_width + overshoot,
                0.5 + graph_y_start + round(vert_line_spacing * i));
  }

  // Horizontal scale lines:
  const unsigned int hor_line_count = ticks / 10;
  const double hor_line_spacing = (double)graph_width / hor_line_count;
  for (unsigned int i = 1; i < hor_line_count; i++) {
    cr->move_to(0.5 + graph_x_start + round(hor_line_spacing * i),
                graph_y_start);
    cr->line_to(0.5 + graph_x_start + round(hor_line_spacing * i),
                graph_y_start + graph_height + overshoot);
  }
  cr->stroke();

  // Draw outer box:
  const double box_line_color = 0.6;
  cr->set_source_rgb(box_line_color, box_line_color, box_line_color);
  cr->move_to(graph_x_start, 0.5 + graph_y_start);
  cr->line_to(graph_x_start + graph_width + overshoot, 0.5 + graph_y_start);
  cr->move_to(0.5 + graph_x_start, graph_y_start);
  cr->line_to(0.5 + graph_x_start, graph_y_start + graph_height + overshoot);
  cr->move_to(0.5 + graph_x_start + graph_width, graph_y_start);
  cr->line_to(0.5 + graph_x_start + graph_width,
              graph_y_start + graph_height + overshoot);
  cr->move_to(graph_x_start, 0.5 + graph_y_start + graph_height + line_width);
  cr->line_to(graph_x_start + graph_width + overshoot,
              0.5 + graph_y_start + graph_height + line_width);
  cr->stroke();

  // Make that scale:
  const double scale_color = 0.4;
  cr->set_source_rgb(scale_color, scale_color, scale_color);
  Pango::FontDescription font;
  font.set_absolute_size(10000); // Not sure why this is so big
  // Vertical scale:
  if (vert_line_count) {
    const unsigned int vert_stepping =
        (vert_line_count != 0)
            ? (Device::sensor_max_vals[type] / vert_line_count)
            : Device::sensor_max_vals[type];
    for (unsigned int i = 0; i <= vert_line_count; i++) {
      cr->move_to(graph_width + graph_x_start + overshoot,
                  graph_y_start + round(vert_line_spacing * i));
      auto layout = create_pango_layout(
          Device::formatValue(vert_stepping * (vert_line_count - i), type));
      layout->set_font_description(font);
      layout->show_in_cairo_context(cr);
    }
  }
  // Horizontal scale:
  const unsigned int hor_stepping = ticks / hor_line_count;
  for (unsigned int i = 0; i <= hor_line_count; i++) {
    cr->move_to(graph_x_start + round(hor_line_spacing * i) - overshoot,
                graph_y_start + graph_height + overshoot);
    auto layout = create_pango_layout(
        std::to_string(hor_stepping * (hor_line_count - i)));
    layout->set_font_description(font);
    layout->show_in_cairo_context(cr);
  }
}

void Graph::make_plot(const Cairo::RefPtr<Cairo::Context> &cr) {
  for (unsigned int i = 0; i < sensor_readings.size(); i++) {
    const unsigned int starting_x_val = graph_width + graph_x_start;
    const double delta_x = (double)graph_width / ticks;

    // Attempt to plot
    cr->set_line_width(line_width);
    cr->set_source_rgb(m_colors[i].get_red(), m_colors[i].get_green(),
                       m_colors[i].get_blue());
    cr->move_to(starting_x_val, scaled_vals[i].front());
    unsigned int prev_y = 0;
    unsigned int loop_index = 0;
    for (unsigned int y_val : scaled_vals[i]) {
      if (!prev_y) {
        prev_y = y_val;
        cr->line_to(starting_x_val - delta_x * loop_index, prev_y);
      } else {
        // This mathy stuff is from gnome-system-monitor. I consider it magic.
        cr->curve_to(starting_x_val - (delta_x * (loop_index - 0.5)), prev_y,
                     starting_x_val - (delta_x * (loop_index - 0.5)), y_val,
                     starting_x_val - delta_x * loop_index, y_val);
        prev_y = y_val;
      }
      loop_index++;
    }
    cr->stroke();
  }
}

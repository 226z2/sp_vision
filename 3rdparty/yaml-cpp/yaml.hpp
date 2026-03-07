#pragma once
// Minimal header-only YAML subset parser for Holon GraphSpec.
// Supports: mappings, sequences, scalars, and flow maps like { a: 1 }.
// This is NOT a full YAML implementation.

#include <cctype>
#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace yaml_lite {

struct Node {
  enum class Kind { Null, Scalar, Map, Seq } kind{Kind::Null};
  std::string scalar{};
  std::map<std::string, Node> map{};
  std::vector<Node> seq{};

  bool is_null() const { return kind == Kind::Null; }
  bool is_scalar() const { return kind == Kind::Scalar; }
  bool is_map() const { return kind == Kind::Map; }
  bool is_seq() const { return kind == Kind::Seq; }

  const Node* get(const std::string& key) const {
    auto it = map.find(key);
    if (it == map.end()) return nullptr;
    return &it->second;
  }
};

struct ParseResult {
  Node root{};
  std::string error{};
  bool ok{false};
};

namespace detail {

inline std::string trim(std::string_view s) {
  std::size_t b = 0;
  while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
  std::size_t e = s.size();
  while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
  return std::string(s.substr(b, e - b));
}

inline std::string unquote(std::string_view s) {
  if (s.size() >= 2) {
    char q = s.front();
    if ((q == '"' || q == '\'') && s.back() == q) {
      std::string out;
      out.reserve(s.size() - 2);
      for (std::size_t i = 1; i + 1 < s.size(); ++i) {
        char c = s[i];
        if (c == '\\' && i + 1 < s.size() - 1) {
          char esc = s[++i];
          out.push_back(esc);
        } else {
          out.push_back(c);
        }
      }
      return out;
    }
  }
  return std::string(s);
}

inline bool is_flow_map(std::string_view s) {
  auto t = trim(s);
  return t.size() >= 2 && t.front() == '{' && t.back() == '}';
}

inline bool is_flow_seq(std::string_view s) {
  auto t = trim(s);
  return t.size() >= 2 && t.front() == '[' && t.back() == ']';
}

inline std::vector<std::string> split_top_level(std::string_view s, char delim) {
  std::vector<std::string> out;
  std::string cur;
  int depth_brace = 0;
  int depth_bracket = 0;
  bool in_quote = false;
  char quote = '\0';
  for (std::size_t i = 0; i < s.size(); ++i) {
    char c = s[i];
    if (in_quote) {
      cur.push_back(c);
      if (c == quote) in_quote = false;
      continue;
    }
    if (c == '"' || c == '\'') {
      in_quote = true;
      quote = c;
      cur.push_back(c);
      continue;
    }
    if (c == '{') ++depth_brace;
    if (c == '}') --depth_brace;
    if (c == '[') ++depth_bracket;
    if (c == ']') --depth_bracket;
    if (c == delim && depth_brace == 0 && depth_bracket == 0) {
      out.push_back(trim(cur));
      cur.clear();
      continue;
    }
    cur.push_back(c);
  }
  if (!cur.empty()) out.push_back(trim(cur));
  return out;
}

inline Node parse_value(std::string_view v);

inline Node parse_flow_map(std::string_view v) {
  Node n{};
  n.kind = Node::Kind::Map;
  auto t = trim(v);
  if (t.size() <= 2) return n;
  std::string inner = std::string(t.substr(1, t.size() - 2));
  auto parts = split_top_level(inner, ',');
  for (const auto& part : parts) {
    auto pos = part.find(':');
    if (pos == std::string::npos) continue;
    std::string key = trim(std::string_view(part).substr(0, pos));
    std::string val = trim(std::string_view(part).substr(pos + 1));
    Node child = parse_value(val);
    n.map.emplace(unquote(key), std::move(child));
  }
  return n;
}

inline Node parse_flow_seq(std::string_view v) {
  Node n{};
  n.kind = Node::Kind::Seq;
  auto t = trim(v);
  if (t.size() <= 2) return n;
  std::string inner = std::string(t.substr(1, t.size() - 2));
  auto parts = split_top_level(inner, ',');
  for (const auto& part : parts) {
    n.seq.push_back(parse_value(part));
  }
  return n;
}

inline Node parse_value(std::string_view v) {
  auto t = trim(v);
  if (t.empty()) {
    return Node{};
  }
  if (is_flow_map(t)) return parse_flow_map(t);
  if (is_flow_seq(t)) return parse_flow_seq(t);
  Node n{};
  n.kind = Node::Kind::Scalar;
  n.scalar = unquote(t);
  return n;
}

struct Line {
  std::size_t indent{0};
  std::string text{};
};

inline std::string strip_comment(const std::string& line) {
  bool in_quote = false;
  char quote = '\0';
  for (std::size_t i = 0; i < line.size(); ++i) {
    char c = line[i];
    if (in_quote) {
      if (c == quote) in_quote = false;
      continue;
    }
    if (c == '"' || c == '\'') {
      in_quote = true;
      quote = c;
      continue;
    }
    if (c == '#') {
      return line.substr(0, i);
    }
  }
  return line;
}

inline bool starts_with_dash(const std::string& s) {
  return s.size() >= 2 && s[0] == '-' && std::isspace(static_cast<unsigned char>(s[1]));
}

inline Node parse_block(const std::vector<Line>& lines, std::size_t& idx, std::size_t indent, std::string& err);

inline Node parse_seq(const std::vector<Line>& lines, std::size_t& idx, std::size_t indent, std::string& err) {
  Node out{};
  out.kind = Node::Kind::Seq;
  while (idx < lines.size()) {
    const Line& ln = lines[idx];
    if (ln.indent < indent) break;
    if (ln.indent != indent || !starts_with_dash(ln.text)) break;

    std::string rest = trim(std::string_view(ln.text).substr(2));
    ++idx;

    if (rest.empty()) {
      Node child = parse_block(lines, idx, indent + 2, err);
      if (!err.empty()) return out;
      out.seq.push_back(std::move(child));
      continue;
    }

    // Inline map entry like: - key: value
    auto pos = rest.find(':');
    if (pos != std::string::npos) {
      std::string key = trim(std::string_view(rest).substr(0, pos));
      std::string val = trim(std::string_view(rest).substr(pos + 1));
      Node map{};
      map.kind = Node::Kind::Map;
      if (!val.empty()) {
        map.map.emplace(unquote(key), parse_value(val));
      } else {
        Node child = parse_block(lines, idx, indent + 2, err);
        if (!err.empty()) return out;
        map.map.emplace(unquote(key), std::move(child));
      }

      // Merge subsequent indented map entries into this item.
      while (idx < lines.size() && lines[idx].indent == indent + 2 && !starts_with_dash(lines[idx].text)) {
        const Line& ln2 = lines[idx];
        auto pos2 = ln2.text.find(':');
        if (pos2 == std::string::npos) {
          err = "expected key: value in map";
          return out;
        }
        std::string k2 = trim(std::string_view(ln2.text).substr(0, pos2));
        std::string v2 = trim(std::string_view(ln2.text).substr(pos2 + 1));
        if (v2.empty()) {
          ++idx;
          Node child = parse_block(lines, idx, indent + 4, err);
          if (!err.empty()) return out;
          map.map.emplace(unquote(k2), std::move(child));
        } else {
          map.map.emplace(unquote(k2), parse_value(v2));
          ++idx;
        }
      }

      out.seq.push_back(std::move(map));
      continue;
    }

    out.seq.push_back(parse_value(rest));
  }
  return out;
}

inline Node parse_map(const std::vector<Line>& lines, std::size_t& idx, std::size_t indent, std::string& err) {
  Node out{};
  out.kind = Node::Kind::Map;
  while (idx < lines.size()) {
    const Line& ln = lines[idx];
    if (ln.indent < indent) break;
    if (ln.indent != indent) break;
    if (starts_with_dash(ln.text)) break;

    auto pos = ln.text.find(':');
    if (pos == std::string::npos) {
      err = "expected key: value";
      return out;
    }
    std::string key = trim(std::string_view(ln.text).substr(0, pos));
    std::string val = trim(std::string_view(ln.text).substr(pos + 1));
    ++idx;

    if (val.empty()) {
      Node child = parse_block(lines, idx, indent + 2, err);
      if (!err.empty()) return out;
      out.map.emplace(unquote(key), std::move(child));
    } else {
      out.map.emplace(unquote(key), parse_value(val));
    }
  }
  return out;
}

inline Node parse_block(const std::vector<Line>& lines, std::size_t& idx, std::size_t indent, std::string& err) {
  if (idx >= lines.size()) return Node{};
  const Line& ln = lines[idx];
  if (ln.indent < indent) return Node{};
  if (starts_with_dash(ln.text)) {
    return parse_seq(lines, idx, indent, err);
  }
  return parse_map(lines, idx, indent, err);
}

}  // namespace detail

inline ParseResult parse(std::string_view text) {
  ParseResult res{};
  std::vector<detail::Line> lines;

  std::size_t line_start = 0;
  while (line_start < text.size()) {
    std::size_t line_end = text.find('\n', line_start);
    if (line_end == std::string_view::npos) line_end = text.size();
    std::string line(text.substr(line_start, line_end - line_start));
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (!line.empty() && line[0] == '\xEF') { // strip UTF-8 BOM if present
      if (line.size() >= 3 && static_cast<unsigned char>(line[0]) == 0xEF &&
          static_cast<unsigned char>(line[1]) == 0xBB &&
          static_cast<unsigned char>(line[2]) == 0xBF) {
        line = line.substr(3);
      }
    }
    line = detail::strip_comment(line);
    // trim right
    while (!line.empty() && std::isspace(static_cast<unsigned char>(line.back()))) line.pop_back();
    std::size_t indent = 0;
    while (indent < line.size() && line[indent] == ' ') ++indent;
    if (indent < line.size() && line[indent] == '\t') {
      res.error = "tabs not supported";
      return res;
    }
    std::string trimmed = line.substr(indent);
    if (!trimmed.empty()) {
      lines.push_back(detail::Line{indent, trimmed});
    }

    line_start = line_end + 1;
  }

  std::size_t idx = 0;
  std::string err;
  res.root = detail::parse_block(lines, idx, 0, err);
  if (!err.empty()) {
    res.error = err;
    return res;
  }
  res.ok = true;
  return res;
}

}  // namespace yaml_lite

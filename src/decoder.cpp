#include "decoder.hpp"
#include <cstddef>
#include <string>

json Decoder::decode(const std::string &encoded_value) {
  size_t index = 0;
  return decodeValue(encoded_value, index);
}

json Decoder::decodeValue(const std::string &encoded_value,
                          std::size_t &index) {
  if (index >= encoded_value.length()) {
    throw std::runtime_error("Unexpected end of input");
  }

  json parsed_json;
  char first_char = encoded_value[index];

  if (std::isdigit(first_char)) {
    parsed_json = Decoder::decodeStrings(encoded_value, index);
  } else if (first_char == 'i') {
    parsed_json = Decoder::decodeIntegers(encoded_value, index);
  } else if (first_char == 'l') {
    parsed_json = Decoder::decodeLists(encoded_value, index);
  } else if (first_char == 'd') {
    parsed_json = Decoder::decodeDictionary(encoded_value, index);
  } else {
    throw std::runtime_error("Unhandled encoded value: " + encoded_value);
  }

  return parsed_json;
}

json Decoder::decodeStrings(const std::string &encoded_value,
                            std::size_t &index) {
  size_t colon_index = encoded_value.find(':', index);
  if (colon_index == std::string::npos) {
    throw std::runtime_error("Invalid string: no colon found");
  }

  std::string length_str = encoded_value.substr(index, colon_index - index);
  int64_t length = std::atoll(length_str.c_str());

  if (length < 0) {
    throw std::runtime_error("Invalid string length: negative");
  }

  size_t start_pos = colon_index + 1;
  if (start_pos + length > encoded_value.length()) {
    throw std::runtime_error("Invalid string: length exceeds remaining data");
  }

  std::string str = encoded_value.substr(start_pos, length);
  index = start_pos + length;

  return json(str);
}

json Decoder::decodeIntegers(const std::string &encoded_value, size_t &index) {
  if (encoded_value.size() <= 2) {
    throw std::runtime_error("Invalid encoded integer: too short > " +
                             encoded_value);
  }

  size_t end_index = encoded_value.find('e', index + 1);
  if (end_index == std::string::npos) {
    throw std::runtime_error("Invalid integer: no ending 'e' found");
  }

  std::string value = encoded_value.substr(index + 1, end_index - index - 1);

  if (value.empty()) {
    throw std::runtime_error("Invalid encoded integer: empty value > " +
                             encoded_value);
  }

  if (value == "-" || value == "+") {
    throw std::runtime_error("Invalid encoded integer: invalid sign > " +
                             encoded_value);
  }

  if (value == "-0") {
    throw std::runtime_error("Invalid encoded integer: negative zero > " +
                             encoded_value);
  }

  if (value.length() > 1 && value[0] == '0') {
    throw std::runtime_error("Invalid encoded integer: leading zero > " +
                             encoded_value);
  }

  if (value.length() > 2 && value[0] == '-' && value[1] == '0') {
    throw std::runtime_error("Invalid encoded integer: leading zero > " +
                             encoded_value);
  }

  index = end_index + 1;

  int64_t num = std::stoll(value);
  return json(num);
}

json Decoder::decodeLists(const std::string &encoded_value,
                          std::size_t &index) {
  json json_list = json::array();
  ++index; // skip l

  while (index < encoded_value.length() && encoded_value[index] != 'e') {
    json list_element = Decoder::decodeValue(encoded_value, index);
    json_list.push_back(list_element);
  }

  if (index >= encoded_value.length() || encoded_value[index] != 'e') {
    throw std::runtime_error("Invalid list: no ending 'e' found");
  }

  ++index; // skip e

  return json_list;
}

json Decoder::decodeDictionary(const std::string &encoded_value,
                               std::size_t &index) {
  json json_dict = json::object();
  ++index; // skip d

  while (index < encoded_value.length() && encoded_value[index] != 'e') {
    std::string dict_el_key = Decoder::decodeValue(encoded_value, index);
    json dict_element = Decoder::decodeValue(encoded_value, index);
    json_dict[dict_el_key] = dict_element;
  }

  ++index; // skip e

  return json_dict;
}

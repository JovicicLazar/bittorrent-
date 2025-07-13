#include "lib/nlohmann/json.hpp"
#include <cstddef>
#include <string>

using json = nlohmann::json;

class Decoder {
public:
  static json decode(const std::string &encoded_value);

private:
  static json decodeValue(const std::string &encoded_value, std::size_t &index);
  static json decodeStrings(const std::string &encoded_value,
                            std::size_t &index);
  static json decodeIntegers(const std::string &encoded_value,
                             std::size_t &index);
  static json decodeLists(const std::string &encoded_value, std::size_t &index);
  static json decodeDictionary(const std::string &encoded_value,
                               std::size_t &index);
};

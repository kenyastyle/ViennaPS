#pragma once

#include <fstream>
#include <initializer_list>
#include <sstream>
#include <string>
#include <vector>

#include <vcLogger.hpp>

namespace viennaps {

using namespace viennacore;

template <class Iterator>
std::string join(Iterator begin, Iterator end,
                 const std::string &separator = ",") {
  std::ostringstream ostr;
  if (begin != end)
    ostr << *begin++;
  while (begin != end)
    ostr << separator << *begin++;
  return ostr.str();
}

// A simple CSV writer class
template <class NumericType> class CSVWriter {

  std::string filename;
  std::ofstream file;
  std::string header;
  bool initialized = false;
  int numCols = 0;

public:
  CSVWriter() {}
  CSVWriter(std::string passedFilename, std::string passedHeader = "")
      : filename(passedFilename), header(passedHeader) {}

  void setFilename(std::string passedFilename) { filename = passedFilename; }
  void setHeader(std::string passedHeader) { header = passedHeader; }

  bool initialize() {
    if (filename.empty()) {
      Logger::getInstance()
          .addWarning("CSVWriter: No filename provided!")
          .print();
      return false;
    }
    if (file.is_open())
      return false;
    file.open(filename);
    if (file.is_open()) {
      if (!header.empty()) {
        std::string line;
        std::istringstream iss(header);
        while (std::getline(iss, line)) {
          // If a line starts with an exclamation point, it's a parameter line,
          // thus we don't want to have an extra space after the hashtag.
          if (line.rfind('!', 1) == 0) {
            file << "#" << line << '\n';
          } else {
            file << "# " << line << '\n';
          }
        }
      }
    } else {
      Logger::getInstance()
          .addWarning("CSVWriter: Error opening file '" + filename + "'")
          .print();
      return false;
    }
    initialized = true;
    return true;
  }

  bool writeRow(const std::vector<NumericType> &data) {
    if (!initialized)
      if (!initialize())
        return false;

    // The first row determins the number of columns
    if (numCols == 0)
      numCols = data.size();

    if (data.size() != numCols) {
      Logger::getInstance()
          .addWarning("Unexpected number of items in the provided row!")
          .print();
      return false;
    }

    if (!file.is_open()) {
      Logger::getInstance()
          .addWarning("Couldn't open file `" + filename + "`")
          .print();
      return false;
    }

    file << join(data.cbegin(), data.cend()) << '\n';
    return true;
  }

  bool writeRow(std::initializer_list<NumericType> data) {
    if (!initialized)
      if (!initialize())
        return false;

    // The first row determins the number of columns
    if (numCols == 0)
      numCols = data.size();

    if (data.size() != numCols) {
      Logger::getInstance()
          .addWarning("Unexpected number of items in the provided row! (" +
                      std::to_string(data.size()) + " instead of " +
                      std::to_string(numCols) + ")")
          .print();
      return false;
    }
    if (!file.is_open()) {
      Logger::getInstance()
          .addWarning("Couldn't open file `" + filename + "`")
          .print();
      return false;
    }
    file << join(data.begin(), data.end()) << '\n';
    return true;
  }

  void flush() {
    if (file.is_open())
      file.flush();
  }

  ~CSVWriter() {
    if (file.is_open())
      file.close();
  }
};

} // namespace viennaps

/*
 * Copyright (C) 2008-today The SG++ project
 * This file is part of the SG++ project. For conditions of distribution and
 * use, please see the copyright notice provided with SG++ or at
 * sgpp.sparsegrids.org
 *
 * GridTypeParser.hpp
 *
 * Created on: Jan 25, 2017
 *     Author: Michael Lettrich
 */

#pragma once

#include <sgpp/base/grid/Grid.hpp>

#include <map>
#include <string>

namespace sgpp {
namespace datadriven {
using sgpp::base::GridType;

class GridTypeParser {
 public:
  /**
   * Convert strings to values #sgpp::datadriven::DataSourceFileType. Throws if there is no valid
   * representation
   * @param input case insensitive string representation of a
   * #sgpp::datadriven::DataSourceFileType.
   * @return the corresponding #sgpp::datadriven::DataSourceFileType.
   */
  static GridType parse(const std::string& input);

  /**
   * generate string representations for values of #sgpp::datadriven::DataSourceFileType.
   * @param type enum value.
   * @return string representation of a #sgpp::datadriven::DataSourceFileType.
   */
  static const std::string& toString(GridType type);

 private:
  using GridTypeMap_t = std::map<GridType, std::string>;

  /**
   * Map containing all values of  #sgpp::datadriven::DataSourceFileType and the corresponding
   * string representation.
   */
  static const GridTypeMap_t gridTypeMap;
};

} /* namespace datadriven */
} /* namespace sgpp */

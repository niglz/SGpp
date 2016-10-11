/* Copyright (C) 2008-today The SG++ project
 * This file is part of the SG++ project. For conditions of distribution and
 * use, please see the copyright notice provided with SG++ or at
 * sgpp.sparsegrids.org
 *
 * FileBasedDataSourceBuilder.hpp
 *
 *  Created on: 01.06.2016
 *      Author: Michael Lettrich
 */

#pragma once

#include <memory.h>
#include <sgpp/datadriven/datamining/modules/dataSource/DataSource.hpp>
#include <sgpp/datadriven/datamining/modules/dataSource/DataSourceConfig.hpp>
#include <string>
#include <vector>

namespace sgpp {
namespace datadriven {

class DataSourceBuilder {
 public:
  DataSourceBuilder();
  virtual ~DataSourceBuilder();
  DataSourceBuilder& withPath(const std::string& filePath);
  DataSourceBuilder& withCompression(bool isCompressed);
  DataSourceBuilder& withFileType(const std::string& fileType);
  DataSourceBuilder& inBatches(size_t howMany);
  DataSourceBuilder& withBatchSize(size_t batchSize);
  DataSource* assemble();
  DataSource* fromConfig(const DataSourceConfig& config);

 private:
  void grabTypeInfoFromFilePath();
  const std::string gz = "gz";

  DataSourceConfig config;
};

} /* namespace datadriven */
} /* namespace sgpp */

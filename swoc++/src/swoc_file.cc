/** @file

    Minimalist version of std::filesystem.

    @section license License

    Licensed to the Apache Software Foundation (ASF) under one or more contributor license
    agreements.  See the NOTICE file distributed with this work for additional information regarding
    copyright ownership.  The ASF licenses this file to you under the Apache License, Version 2.0
    (the "License"); you may not use this file except in compliance with the License.  You may
    obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
 */

#include <fcntl.h>
#include <unistd.h>
#include "swoc/swoc_file.h"
#include "swoc/bwf_base.h"

using namespace swoc::literals;

namespace swoc
{
namespace file
{
  path
  path::parent_path() const
  {
    TextView parent{_path};
    parent.split_suffix_at(SEPARATOR);
    return parent ? parent : "/"_tv;
  };

  path &
  path::operator/=(std::string_view that)
  {
    if (!that.empty()) { // don't waste time appending nothing.
      if (that.front() == SEPARATOR || _path.empty()) {
        _path.assign(that);
      } else {
        if (_path.back() == SEPARATOR) {
          _path.reserve(_path.size() + that.size());
        } else {
          _path.reserve(_path.size() + that.size() + 1);
          _path.push_back(SEPARATOR);
        }
        _path.append(that);
      }
    }
    return *this;
  }

  file_status
  status(path const &file, std::error_code &ec) noexcept
  {
    file_status zret;
    if (::stat(file.c_str(), &zret._stat) >= 0) {
      ec.clear();
    } else {
      ec = std::error_code(errno, std::system_category());
    }
    return zret;
  }

  int
  file_type(const file_status &fs)
  {
    return fs._stat.st_mode & S_IFMT;
  }

  off_t
  file_size(const file_status &fs)
  {
    return fs._stat.st_size;
  }

  bool
  is_char_device(const file_status &fs)
  {
    return file_type(fs) == S_IFCHR;
  }

  bool
  is_block_device(const file_status &fs)
  {
    return file_type(fs) == S_IFBLK;
  }

  bool
  is_regular_file(const file_status &fs)
  {
    return file_type(fs) == S_IFREG;
  }

  bool
  is_dir(const file_status &fs)
  {
    return file_type(fs) == S_IFDIR;
  }

  bool
  is_readable(const path &p)
  {
    return 0 == access(p.c_str(), R_OK);
  }

  std::string
  load(const path &p, std::error_code &ec)
  {
    std::string zret;
    int fd(::open(p.c_str(), O_RDONLY));
    ec.clear();
    if (fd < 0) {
      ec = std::error_code(errno, std::system_category());
    } else {
      struct stat info;
      if (0 != ::fstat(fd, &info)) {
        ec = std::error_code(errno, std::system_category());
      } else {
        int n = info.st_size;
        zret.resize(n);
        auto read_len = ::read(fd, const_cast<char *>(zret.data()), n);
        if (read_len < n) {
          ec = std::error_code(errno, std::system_category());
        }
      }
      ::close(fd);
    }
    return zret;
  }

} // namespace file

BufferWriter &
bwformat(BufferWriter &w, bwf::Spec const &spec, file::path const &p)
{
  return bwformat(w, spec, p.string());
}

} // namespace swoc

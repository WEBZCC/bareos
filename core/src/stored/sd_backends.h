/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2014-2014 Planets Communications B.V.
   Copyright (C) 2014-2022 Bareos GmbH & Co. KG

   This program is Free Software; you can redistribute it and/or
   modify it under the terms of version three of the GNU Affero General Public
   License as published by the Free Software Foundation and included
   in the file LICENSE.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/
// Marco van Wieringen, June 2014
/**
 * @file
 * Dynamic loading of SD backend plugins.
 */

#ifndef BAREOS_STORED_SD_BACKENDS_H_
#define BAREOS_STORED_SD_BACKENDS_H_

#include <lib/plugin_registry.h>

namespace storagedaemon {

class BackendInterface {
 public:
  virtual ~BackendInterface() = default;
  virtual Device* GetDevice() = 0;
};

template <typename T> class Backend : public BackendInterface {
 public:
  Device* GetDevice(void) override { return new T(); }
};

template <typename T> BackendInterface* BackendFactory(void) { return new T(); }

extern "C" {
typedef BackendInterface* (*t_backend_base)(void);
BackendInterface* GetBackend(void);
}

#if defined(HAVE_WIN32)
#  define DYN_LIB_EXTENSION ".dll"
#elif defined(HAVE_DARWIN_OS)
/* cmake MODULE creates a .so files; cmake SHARED creates .dylib */
// #define DYN_LIB_EXTENSION ".dylib"
#  define DYN_LIB_EXTENSION ".so"
#else
#  define DYN_LIB_EXTENSION ".so"
#endif

#if defined(HAVE_DYNAMIC_SD_BACKENDS)
#  include <map>
void SetBackendDeviceDirectories(std::vector<std::string>&& new_backend_dirs);
Device* InitBackendDevice(JobControlRecord* jcr,
                          const std::string& device_type);
void FlushAndCloseBackendDevices();

bool LoadStorageBackend(const std::string& dev_type,
                        const std::vector<std::string>& backend_directories);
#endif

#define REGISTER_SD_BACKEND(backend_name, backend_class) \
  [[maybe_unused]] static bool backend_name##_backend_   \
      = PluginRegistry<BackendInterface>::Add(           \
          #backend_name, BackendFactory<Backend<backend_class>>);

} /* namespace storagedaemon */

#endif  // BAREOS_STORED_SD_BACKENDS_H_

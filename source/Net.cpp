/**
 *   This file is part of RenderD7
 *   Copyright (C) 2021-2024 NPI-D7, tobid7
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// TODO: Make Download2File faster on large files

#include <3ds.h>
#include <curl/curl.h>
#include <malloc.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <regex>
#include <renderd7/Net.hpp>
#include <renderd7/internal_db.hpp>

static RenderD7::Net::Error rd7i_check_wifi() {
  // if (rd7i_is_citra) return 0;
  int s = osGetWifiStrength();
  return (s == 0);
}

static size_t rd7i_handle_data(char* ptr, size_t size, size_t nmemb,
                               void* userdata) {
  size_t ms = size * nmemb;
  ((std::string*)userdata)->append(ptr, ms);
  return ms;
}

static size_t rd7i_handle_file(char* ptr, size_t size, size_t nmemb,
                               void* out) {
  size_t ms = size * nmemb;
  ((std::ofstream*)out)->write(reinterpret_cast<const char*>(ptr), ms);
  return ms;
}

struct rd7i_net_dl {
  unsigned long long current = 0;
  unsigned long long total = 1;
  void Reset() {
    current = 0;
    total = 1;
  }
};

static rd7i_net_dl rd7i_net_dl_spec;

static int rd7i_handle_curl_progress(CURL* hnd, curl_off_t dltotal,
                                     curl_off_t dlnow, curl_off_t ultotal,
                                     curl_off_t ulnow) {
  rd7i_net_dl_spec.total = dltotal;
  rd7i_net_dl_spec.current = dlnow;
  return 0;
}

static void rd7i_setup_curl_context(CURL* hnd, const std::string& url,
                                    void* userptr, bool mem) {
  std::string user_agent =
      rd7i_app_name + "/RenderD7 (Version: " + std::string(RENDERD7VSTRING) +
      ")";

  if (!mem) {
    curl_easy_setopt(hnd, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(hnd, CURLOPT_ACCEPT_ENCODING, "gzip");
    curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, rd7i_handle_curl_progress);
  }

  curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, user_agent.c_str());
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, userptr);
  curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION,
                   mem ? rd7i_handle_data : rd7i_handle_file);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(hnd, CURLOPT_STDERR, stdout);
}

static bool rd7i_curl_is_busy = false;
static bool rd7i_apir_is_busy = false;

namespace RenderD7 {
namespace Net {
Error Download(const std::string& url, std::string& data) {
  if (rd7i_curl_is_busy) return Error_Busy;
  Error ret = rd7i_check_wifi();
  if (ret != 0) {
    if (ret == 1) {
      return Error_NoWifi;
    } else {
      return ret;
    }
  }
  rd7i_curl_is_busy = true;
  rd7i_net_dl_spec.Reset();
  ret = rd7i_soc_init();
  if (ret) {
    rd7i_curl_is_busy = false;
    return ret;
  }

  auto hnd = curl_easy_init();
  rd7i_setup_curl_context(hnd, url, &data, true);

  CURLcode curl_res = curl_easy_perform(hnd);
  curl_easy_cleanup(hnd);

  if (curl_res != CURLE_OK) {
    data.clear();
    rd7i_soc_deinit();
    rd7i_curl_is_busy = false;
    return ((static_cast<Error>(curl_res) << 32) |
            static_cast<Error>(Error_Curl));
  }

  rd7i_soc_deinit();
  rd7i_curl_is_busy = false;
  return 0;
}

Error Download2File(const std::string& url, const std::string& path) {
  if (rd7i_curl_is_busy) return Error_Busy;
  Error ret = rd7i_check_wifi();
  if (ret != 0) {
    if (ret == 1) {
      return Error_NoWifi;
    } else {
      return ret;
    }
  }
  rd7i_curl_is_busy = true;
  rd7i_net_dl_spec.Reset();
  ret = rd7i_soc_init();
  if (ret) {
    rd7i_curl_is_busy = false;
    return ret;
  }

  // std::filesystem::create_directories(
  // std::filesystem::path(path).remove_filename());
  std::ofstream file(path, std::ios::binary);

  if (!file.is_open()) {
    rd7i_curl_is_busy = false;
    return Error_Write;
  }

  auto hnd = curl_easy_init();
  rd7i_setup_curl_context(hnd, url, &file, false);

  CURLcode curl_res = curl_easy_perform(hnd);
  curl_easy_cleanup(hnd);

  file.close();
  if (curl_res != CURLE_OK) {
    rd7i_soc_deinit();
    if (RenderD7::FS::FileExist(path)) {
      std::filesystem::remove(path);
    }
    rd7i_curl_is_busy = false;
    return ((static_cast<Error>(curl_res) << 32) |
            static_cast<Error>(Error_Curl));
  }

  rd7i_soc_deinit();
  rd7i_curl_is_busy = false;
  return 0;
}

Error GitDownloadRelease(const std::string& url, const std::string& asset_name,
                         const std::string& path, bool prerelease) {
  if (rd7i_apir_is_busy) return Error_Busy;
  Error ret = rd7i_check_wifi();
  if (ret != 0) {
    if (ret == 1) {
      return Error_NoWifi;
    } else {
      return ret;
    }
  }
  rd7i_apir_is_busy = true;
  std::regex parse("github\\.com\\/(.+)\\/(.+)");
  std::smatch res;
  std::regex_search(url, res, parse);

  std::string user = res[1].str();
  std::string repo = res[2].str();

  std::stringstream req;
  req << "https://api.github.com/repos/" << user << "/" << repo
      << (prerelease ? "/releases" : "/releases/latest");

  std::string buf;
  ret = Download(req.str(), buf);
  if (ret) {
    rd7i_apir_is_busy = false;
    return ret;
  }

  int pret = 0;
  std::string freq;
  if (nlohmann::json::accept(buf)) {
    nlohmann::json api = nlohmann::json::parse(buf);
    if (!api.size()) pret = -1;
    if (pret != -1) {
      if (prerelease) api = api[0];
      if (api["assets"].is_array()) {
        for (const auto& asset : api["assets"]) {
          if (asset.is_object() && asset["name"].is_string() &&
              asset["browser_download_url"].is_string()) {
            if (std::regex_match(std::string(asset["name"]),
                                 std::regex(asset_name))) {
              freq = asset["browser_download_url"];
              break;
            }
          }
        }
      }
    }
  } else {
    pret = -1;
  }

  if (pret != 0 || freq.empty()) {
    rd7i_apir_is_busy = false;
    return Error_Git;
  }

  ret = Download2File(freq, path);
  rd7i_apir_is_busy = false;
  return ret;
}

Error JsonApiRequest(const std::string& api_url, nlohmann::json& res) {
  if (rd7i_apir_is_busy) return Error_Busy;
  Error ret = rd7i_check_wifi();
  if (ret != 0) {
    if (ret == 1) {
      return Error_NoWifi;
    } else {
      return ret;
    }
  }
  rd7i_apir_is_busy = true;

  std::string buf;
  ret = Download(api_url, buf);
  if (ret) {
    rd7i_apir_is_busy = false;
    return ret;
  }

  if (nlohmann::json::accept(buf)) {
    res = nlohmann::json::parse(buf);
    if (!res.size()) {
      rd7i_apir_is_busy = false;
      return Error_Invalid;
    }
  } else {
    rd7i_apir_is_busy = false;
    return Error_Invalid;
  }

  rd7i_apir_is_busy = false;
  return 0;
}

unsigned long long GetProgressCurrent() { return rd7i_net_dl_spec.current; }
unsigned long long GetProgressTotal() {
  // As curl sets total to 0 we need
  // to return a 1 as devide by zeroi will crash
  if (rd7i_net_dl_spec.total <= 0) return 1;
  return rd7i_net_dl_spec.total;
}
}  // namespace Net
}  // namespace RenderD7
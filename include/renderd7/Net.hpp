#pragma once

#include <renderd7/external/json.hpp>
#include <string>

namespace RenderD7 {
namespace Net {
// Define List of Errors
enum Error_ {
  Error_None,        // Function Executed Successfully
  Error_Memory,      // Memory  Allocation Error
  Error_Write,       // Unable to Write File
  Error_StatusCode,  // Error with Status Code
  Error_Git,         // Git Error
  Error_CtrStatus,   // 3ds Result Code
  Error_Curl,        // Curl Error
  Error_Busy,        // Another Download Taskl is already running
  Error_Invalid,     // Invalid Json struct
  Error_NoWifi,      // Console not connected to wifi
};
// Set an typedefine for Error code
using Error = unsigned long long;
// Extract Error_ from Error code
inline Error_ ErrorCode(Error err) {
  return static_cast<Error_>(static_cast<unsigned int>(err & 0xffffffff));
}
// Extract Http Status code, Curl Error Code or Ctr Result Code
inline int StatusCode(Error err) {
  Error_ c = ErrorCode(err);
  if (c != Error_StatusCode && c != Error_CtrStatus && c != Error_Curl)
    return 0;
  return static_cast<unsigned int>(err >> 32);
}
Error Download(const std::string& url, std::string& data);
Error Download2File(const std::string& url, const std::string& path);
Error GitDownloadRelease(const std::string& url, const std::string& asset_name,
                         const std::string& path, bool prerelease = false);
Error JsonApiRequest(const std::string& api_url, nlohmann::json& res);
unsigned long long GetProgressCurrent();
unsigned long long GetProgressTotal();
}  // namespace Net
}  // namespace RenderD7
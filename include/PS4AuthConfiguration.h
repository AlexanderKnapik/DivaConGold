#ifndef PS4AUTHCONFIGURATION_H_
#define PS4AUTHCONFIGURATION_H_

#include "utils/PS4AuthProvider.h"

namespace Divacon::Config::PS4Auth {

const Utils::PS4AuthProvider::Config config = {false,        // Enabled
                                               {},           // Serial
                                               {},           // Signature
                                               R"pem()pem"}; // Pem

} // namespace Divacon::Config::PS4Auth

#endif // PS4AUTHCONFIGURATION_H_
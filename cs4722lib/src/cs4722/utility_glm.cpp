
#include "cs4722/utility_glm.h"
#include <ostream>



std::ostream& operator<<(std::ostream& output, const glm::vec4& v) {
    output << "glm::vec4(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
    return output;
}



std::ostream& operator<<(std::ostream& output, const glm::vec3& v) {
    output << "glm::vec3(" << v.x << "," << v.y << "," << v.z << ")";
    return output;
}

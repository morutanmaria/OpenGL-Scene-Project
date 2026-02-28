#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace gps {
    
    enum MOVE_DIRECTION {MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_UP, MOVE_DOWN};
    
    class Camera {

    public:
        //Camera constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        glm::mat4 getViewMatrix();
        void move(MOVE_DIRECTION direction, float speed);

        void rotate(float pitch, float yaw);
        glm::vec3 getCameraPosition() const {
            return cameraPosition;
        }
        glm::vec3 getCameraFrontDirection() { 
            return cameraFrontDirection; 
        }
        void setCameraPosition(const glm::vec3& newPos) {
            cameraPosition = newPos;
            cameraTarget = cameraPosition + cameraFrontDirection;
        }
        
    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;
    };    
}

#endif /* Camera_hpp */

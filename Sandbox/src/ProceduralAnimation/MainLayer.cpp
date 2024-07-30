#include "MainLayer.h"
#include "Deimos/Events/MouseEvent.h"
#include "Deimos/Core/Core.h"

using namespace Deimos;

MainLayer::MainLayer() : m_cameraController(1280 / 720.f) {

}

void MainLayer::onAttach() {
    DM_PROFILE_FUNCTION();

    Deimos::Renderer2D::init();
    init();
}

void MainLayer::onDetach() {
    DM_PROFILE_FUNCTION();
}

void MainLayer::onUpdate(Timestep timestep) {
    DM_PROFILE_FUNCTION()

    {
        DM_PROFILE_SCOPE("CameraController::onUpdate");
        m_cameraController.onUpdate(timestep);
    }

    {
        DM_PROFILE_SCOPE("Renderer Prep");
        RenderCommand::setClearColor({0.7f, 0.7f, 0.8f, 1});
        RenderCommand::clear();
    }

    {
        DM_PROFILE_SCOPE("Renderer Draw");
        Renderer2D::beginScene(m_cameraController.getCamera());

        // starting positions for the upper and lower points
        glm::vec3 headArbit1 = { m_spineJoints[0].pos.x + (m_spineJoints[0].radius * glm::cos((m_spineJoints[0].rotation) - M_PI / 6)), 
                                 m_spineJoints[0].pos.y + (m_spineJoints[0].radius * glm::sin((m_spineJoints[0].rotation) - M_PI / 6)), 0.8f }; // head's 2nd arbitrary point (lower point)

        glm::vec3 headArbit2 = { m_spineJoints[0].pos.x + (m_spineJoints[0].radius * glm::cos((m_spineJoints[0].rotation) + M_PI / 6)), 
                                 m_spineJoints[0].pos.y + (m_spineJoints[0].radius * glm::sin((m_spineJoints[0].rotation) + M_PI / 6)), 0.8f }; // head's 1st arbitrary point (upper point)

        m_connections[0] = headArbit1;
        m_connections[1] = headArbit2;
        
        glm::vec3 upperPoint = {};
        glm::vec3 lowerPoint = {};

        for (size_t i = 0; i < m_numJoints; ++i) {
            // calculate the new positions for the upper and lower points
            upperPoint = { m_spineJoints[i].pos.x + (m_spineJoints[i].radius * glm::cos((m_spineJoints[i].rotation) + M_PI / 2)), 
                                   m_spineJoints[i].pos.y + (m_spineJoints[i].radius * glm::sin((m_spineJoints[i].rotation) + M_PI / 2)), 0.8f };
            
            lowerPoint = { m_spineJoints[i].pos.x + (m_spineJoints[i].radius * glm::cos((m_spineJoints[i].rotation) - M_PI / 2)), 
                                    m_spineJoints[i].pos.y + (m_spineJoints[i].radius * glm::sin((m_spineJoints[i].rotation) - M_PI / 2)), 0.8f };

            m_connections[i + 2] = upperPoint;
            m_connections[m_numConnections - i - 1] = lowerPoint;
        }
        
        // draw lines between the points
        for (size_t i = 0; i < m_numConnections - 1 ; ++i) {
           Renderer2D::drawLine(m_connections[i], m_connections[i + 1], 8.0f, {1.f, 1.f, 1.f, 1.f});
        }
        Renderer2D::drawLine(m_connections[m_numConnections - 1], m_connections[0], 8.0f, {1.f, 1.f, 1.f, 1.f}); // connect finishing points (which close the tail)

        Renderer2D::drawPolygon(m_connections, m_numConnections, m_bodyColor);

        Renderer2D::drawBezier(m_pectoralFins[0], m_pectoralFins[1],m_pectoralFins[2], {1.f, 1.f, 1.f, 1.f});
        Renderer2D::endScene();
    }

}

void MainLayer::onEvent(Event &event) {
    m_cameraController.onEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.dispatch<MouseMovedEvent>(DM_BIND_EVENT_FN(MainLayer::onMouseMovedEvent));
}

void MainLayer::onImGuiRender() {

}

void MainLayer::init() {
    // init the size of body parts
    m_spineJoints[0].radius = 0.1;
    m_spineJoints[1].radius = 0.12;
    m_spineJoints[2].radius = 0.13;
    m_spineJoints[3].radius = 0.122;
    m_spineJoints[4].radius = 0.113;
    m_spineJoints[5].radius = 0.09;
    m_spineJoints[6].radius = 0.075;
    m_spineJoints[7].radius = 0.055;
    m_spineJoints[8].radius = 0.047;
    m_spineJoints[9].radius = 0.027;
}

void MainLayer::updateJointsPosAndRotation() {
    m_spineJoints[0].rotation = calculateRotationAngle(glm::vec3(1, 0.f, 0.f), m_dirPos - m_spineJoints[0].pos);
    m_spineJoints[0].pos = attachToBody(m_dirPos, m_spineJoints[0].pos, m_startDistance);
    for (size_t i = 1; i < m_numJoints; ++i) {
        glm::vec3 prevPos = m_spineJoints[i].pos;
        m_spineJoints[i].pos = attachToBody(m_spineJoints[i - 1].pos, m_spineJoints[i].pos, m_startDistance + (i * 0.002));

        glm::vec3 vec = m_spineJoints[i].pos - prevPos;
        float rotationAngle = calculateRotationAngle(glm::vec3(1, 0.f, 0.f), vec); // angle between the positive direction of the Ox axis and the deviation vector
        m_spineJoints[i].rotation = rotationAngle;
    }
    m_pectoralFins[0] = { m_spineJoints[2].pos.x + (m_spineJoints[2].radius * glm::cos((m_spineJoints[2].rotation) + M_PI / 2)), 
                          m_spineJoints[2].pos.y + (m_spineJoints[2].radius * glm::sin((m_spineJoints[2].rotation) + M_PI / 2)), 0.6f };

    m_pectoralFins[2] = { m_spineJoints[4].pos.x + (m_spineJoints[4].radius * glm::cos((m_spineJoints[4].rotation) + M_PI / 3)), 
                          m_spineJoints[4].pos.y + (m_spineJoints[4].radius * glm::sin((m_spineJoints[4].rotation) + M_PI / 3)), 0.6f };
    m_pectoralFins[1] = { m_pectoralFins[2].x + 0.1f * (glm::cos((m_spineJoints[4].rotation) + M_PI / 2)), 
                          m_pectoralFins[2].y + 0.1f * (glm::sin((m_spineJoints[4].rotation)) + M_PI / 2), 0.6f };
}

float MainLayer::calculateRotationAngle(const glm::vec3 &startVec, const glm::vec3 &endVec) {
    glm::vec3 normalizedStartVec = glm::normalize(startVec);
    glm::vec3 normalizedEndVec = glm::normalize(endVec);
    float dotProduct = glm::dot(normalizedStartVec, normalizedEndVec);
    float angle = glm::acos(dotProduct); // computes the smallest angle between 0 and pi

    glm::vec3 crossProduct = glm::cross(normalizedStartVec, normalizedEndVec);
    if (crossProduct.z < 0) { // determine the direction of rotation (if positive - counterclockwise, negative - clockwise)
        return -angle;
    }
    return angle;
}

glm::vec3 MainLayer::attachToBody(const glm::vec3 &achnor, const glm::vec3 &point, float distance) {
    return glm::normalize(point - achnor) * distance + achnor; // obtain normalized vector * distance and translate it to anchor position
}

bool MainLayer::onMouseMovedEvent(MouseMovedEvent &e) {
    auto& window = Application::get().getWindow();
    // convert to camera coordinates: x from -window.getWidth() / window.getHeight() to window.getWidth() / window.getHeight(), y from -1 to 1
    m_dirPos = { ((2.f * e.getX() / window.getWidth()) - 1.f) * window.getWidth() / window.getHeight(), 1.f  - (2.f * e.getY() / window.getHeight()), 0.f};        
    updateJointsPosAndRotation(); // recalculate position and rotation 

    return false;
}

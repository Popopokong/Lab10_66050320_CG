#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Libs/Mesh.h"
#include "Window.h"
#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// เพิ่มเพื่อใช้คีย์บอร์ด/บริบท GLFW ได้ตรง ๆ
#include <GLFW/glfw3.h>

Window mainWindow(1280, 720, 3, 3);
Mesh* sharedMesh = nullptr;
std::vector<glm::mat4> modelMats;
GLuint texID = 0;
Shader shader;

/* ---------- Orbit camera params ---------- */
float g_radius    = 14.0f;   // ระยะกล้องจากจุดศูนย์กลางฉาก
float g_azimuth   = 0.0f;    // หมุนรอบแกน Y (radian)
float g_elevation = 0.0f;    // มุมเงย/กด (radian)
double g_prevTime = 0.0;

static void updateOrbitCamera(GLFWwindow* win) {
    double now = glfwGetTime();
    float dt = float(now - g_prevTime);
    g_prevTime = now;

    const float rotSpeed  = 1.5f;      // rad/sec
    const float zoomSpeed = 6.0f;      // units/sec
    const float minR = 4.0f, maxR = 30.0f;
    const float minEl = -1.2f, maxEl = 1.2f;

    if (glfwGetKey(win, GLFW_KEY_LEFT)  == GLFW_PRESS) g_azimuth   -= rotSpeed * dt;
    if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS) g_azimuth   += rotSpeed * dt;
    if (glfwGetKey(win, GLFW_KEY_UP)    == GLFW_PRESS) g_elevation += rotSpeed * dt;
    if (glfwGetKey(win, GLFW_KEY_DOWN)  == GLFW_PRESS) g_elevation -= rotSpeed * dt;

    if (glfwGetKey(win, GLFW_KEY_LEFT_BRACKET)  == GLFW_PRESS) g_radius -= zoomSpeed * dt;
    if (glfwGetKey(win, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) g_radius += zoomSpeed * dt;

    // clamp
    if (g_radius    < minR) g_radius    = minR;
    if (g_radius    > maxR) g_radius    = maxR;
    if (g_elevation < minEl) g_elevation = minEl;
    if (g_elevation > maxEl) g_elevation = maxEl;
}

/* ---------- Texture loader ---------- */
static GLuint LoadTexture2D(const char* path, bool flipY = true) {
    stbi_set_flip_vertically_on_load(flipY);
    int w,h,comp;
    unsigned char* data = stbi_load(path, &w,&h,&comp, 4);
    if (!data) { std::cerr << "Failed to load texture: " << path << "\n"; return 0; }
    GLuint id; glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    return id;
}

/* ---------- Create layout like the example (no model rotation) ---------- */
void CreateLayoutNoRotate() {
    sharedMesh = new Mesh();
    if (!sharedMesh->CreateMeshFromOBJ("Models/suzanne.obj", /*flipV=*/false)) {
        std::cerr << "Failed to load Models/suzanne.obj\n";
        return;
    }

    struct Item { float x, y, s; };
    const Item items[] = {
        {-4.0f,  2.8f, 0.95f},  // top-left
        { 0.0f,  3.2f, 0.55f},  // top-center (small)
        { 4.0f,  3.0f, 0.95f},  // top-right

        {-5.0f, -0.2f, 0.70f},  // mid-left small
        {-1.5f, -0.6f, 1.25f},  // mid-left big
        { 2.0f, -0.4f, 1.15f},  // mid-right big

        {-3.0f, -3.2f, 0.55f},  // bottom-left small
        { 1.0f, -3.4f, 0.95f},  // bottom-right mid
    };

    modelMats.reserve(std::size(items));
    for (auto &it : items) {
        glm::mat4 M(1.0f);
        M = glm::translate(M, glm::vec3(it.x, it.y, 0.0f));
        M = glm::scale(M, glm::vec3(it.s));   // ไม่ใส่ rotation -> โมเดลไม่หมุน
        modelMats.push_back(M);
    }
}

void CreateTexture() { texID = LoadTexture2D("Textures/uvmap.png", /*flipY=*/true); }
void CreateShaders() { shader.CreateFromFiles("Shaders/vertex.glsl", "Shaders/fragment.glsl"); }

int main() {
    mainWindow.initialise();
    glEnable(GL_DEPTH_TEST);

    CreateLayoutNoRotate();
    CreateTexture();
    CreateShaders();

    // Projection คงที่
    glm::mat4 proj = glm::perspective(glm::radians(60.0f),
        (float)mainWindow.getBufferWidth()/ (float)mainWindow.getBufferHeight(),
        0.1f, 100.0f);

    g_prevTime = glfwGetTime();

    // ดึง GLFWwindow* เพื่ออ่านคีย์บอร์ด (รองรับทั้งกรณีมี/ไม่มีเมธอดใน Window)
    GLFWwindow* gw = nullptr;
    if constexpr (true) { // ใช้ current context หลัง initialise()
        gw = glfwGetCurrentContext();
    }
    // ถ้าโปรเจกต์คุณมีเมธอด: gw = mainWindow.getWindow();

    while (!mainWindow.getShouldClose()) {
        glfwPollEvents();
        updateOrbitCamera(gw);

        glViewport(0,0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // พื้นหลังดำ
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.UseShader();

        // คำนวณตำแหน่งกล้องจากวงโคจร (ไม่หมุนโมเดล)
        float cx = g_radius * cosf(g_elevation) * sinf(g_azimuth);
        float cy = g_radius * sinf(g_elevation);
        float cz = g_radius * cosf(g_elevation) * cosf(g_azimuth);
        glm::mat4 view = glm::lookAt(glm::vec3(cx, cy, cz), glm::vec3(0,0,0), glm::vec3(0,1,0));
        glm::mat4 vp   = proj * view;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);
        glUniform1i(glGetUniformLocation(shader.GetShaderID(), "uTex"), 0);

        GLint locMVP = glGetUniformLocation(shader.GetShaderID(), "uMVP");
        for (const auto& M : modelMats) {
            glm::mat4 mvp = vp * M;
            glUniformMatrix4fv(locMVP, 1, GL_FALSE, &mvp[0][0]);
            if (sharedMesh) sharedMesh->RenderMesh();
        }

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    if (sharedMesh) { sharedMesh->ClearMesh(); delete sharedMesh; }
    if (texID) glDeleteTextures(1, &texID);
    glfwTerminate();
    return 0;
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr); 
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void updateZoom(float zoomDelta, const glm::vec2& mousePosition, float& zoom, glm::vec2& offset, float aspectRatio) {
    float zoomFactor = zoomDelta;

    zoom *= zoomFactor;

    glm::vec2 viewSize = glm::vec2(4.0f * zoom * aspectRatio, 4.0f * zoom);
    glm::vec2 viewCenter = offset + viewSize * 0.5f;

    offset = viewCenter - viewSize * 0.5f;
}

int main() {
    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(1900, 1200, "Testing Window", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, 1900, 1200);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error initializing GLEW!" << std::endl;
    }

    unsigned int VAO, VBO, EBO;
    float vertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::string vertexShader = R"(
    #version 330 core
    layout(location = 0) in vec3 vertexPosition;
    void main()
    {
        gl_Position = vec4(vertexPosition, 1.0);
    }
    )";

    std::string fragmentShader = R"(
    #version 330 core
    uniform float u_zoom;
    uniform vec2 u_offset;
    uniform int u_maxIterations;
    out vec4 fragColor;

    void main() {
        vec2 c = (gl_FragCoord.xy / vec2(1900, 1200) - vec2(0.5, 0.5) + u_offset) * u_zoom * vec2(4.0 * 1900.0 / 1200.0, 4.0) - vec2(2.0, 2.0);
        vec2 z = vec2(0.0, 0.0);
        int i;
        for (i = 0; i < u_maxIterations; i++) {
            if (dot(z, z) > 4.0) {
                break;
            }
            z = vec2(z.x * z.x - z.y * z.y + c.x, 2.0 * z.x * z.y + c.y);
        }
        float color = float(i) / float(u_maxIterations);
        fragColor = vec4(vec3(color), 1.0);
    }
    )";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    int zoomLocation = glGetUniformLocation(shader, "u_zoom");
    int offsetLocation = glGetUniformLocation(shader, "u_offset");
    int maxIterationsLocation = glGetUniformLocation(shader, "u_maxIterations");

    float zoom = 1.0f;
    glm::vec2 offset(0.0f, 0.0f);
    int maxIterations = 100;

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
	//std::cout<<width << ", "<< height << std::endl;
        float aspectRatio = (float)width / (float)height;

        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
            updateZoom(1.05f, glm::vec2(width / 2.0f, height / 2.0f), zoom, offset, aspectRatio);
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
            updateZoom(0.95f, glm::vec2(width / 2.0f, height / 2.0f), zoom, offset, aspectRatio);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            offset.x += 0.05f * zoom;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            offset.x -= 0.05f * zoom;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            offset.y += 0.05f * zoom;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            offset.y -= 0.05f * zoom;
        }

        glUniform1f(zoomLocation, zoom);
        glUniform2f(offsetLocation, offset.x, offset.y);
        glUniform1i(maxIterationsLocation, maxIterations);

        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}

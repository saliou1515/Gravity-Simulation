#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <cmath>
#include <vector>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"uniform vec2 uOffset;\n"
"uniform vec2 uResolution;\n"
"uniform float uRadius;\n"
"void main()\n"
"{\n"
"	vec2 pixelPos = aPos * uRadius + uOffset;\n"
"	vec2 ndc = (pixelPos / uResolution) * 2.0 - 1.0;\n"
"	gl_Position = vec4(ndc, 0.0, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\0";


int main() {

	// initialize OpenGL
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const int screenWidth = 800;
	const int screenHeight = 600;
	// create window
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Saliou's Gravity Simulation", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, 800, 600);


	// create shader program from c source code string
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	const float pPM = 100.0f;
	float dT = 0.0f;
	float lastFrame = 0.0f;

	// create circle vertices

	struct Circle {
		const float PI = 3.1415926535;
		int numSegments = 40;
		int totalVertices = numSegments + 2;
		std::vector<float> vertices;

		const float pPM = 100.0f;
		float centerX = 400.0f;
		float centerY = 500.0f;
		float circleRadius = 25.0f;

		float velocityX = 5.0f * pPM;
		float velocityY = 10.0f * pPM;
		float accelerationX = 0.0f * pPM;
		float accelerationY = 0.0f * pPM;

		Circle() {
			vertices.assign(totalVertices * 2, 0.0f);

			// adds x and y coordinates to vertices array by going to each segments angles,
			// then skipping the first two vertices (center), and skipping a number each time to
			// add the x and y coordinate to the array
			for (int i = 0; i <= numSegments; i++) {
				float angle = (i * 2 * PI) / numSegments;
				vertices[(i + 1) * 2] = cos(angle);
				vertices[(i + 1) * 2 + 1] = sin(angle);
			}
		}
	};
	
	// create circle objects

	float distanceX;
	float distanceY;
	float distanceTotal;

	Circle circle;
	Circle circle1;

	circle1.circleRadius = 75.0f;
	circle1.centerY = 300.0f;
	circle1.velocityX = 15.0f * pPM;
	circle1.velocityY = 5.0f * pPM;
	circle1.accelerationX = 0.0f * pPM;
	circle1.accelerationY = 0.0f * pPM;
		

	GLuint VBO, VAO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, circle.vertices.size() * sizeof(float), circle.vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	

	// render loop
	while (!glfwWindowShouldClose(window)) {

		// get change in time between frames;
		float currentFrame = static_cast<float>(glfwGetTime());
		dT = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// draw circle
		glUseProgram(shaderProgram);
		glUniform1f(glGetUniformLocation(shaderProgram, "uRadius"), circle.circleRadius);
		glUniform2f(glGetUniformLocation(shaderProgram, "uResolution"), screenWidth, screenHeight);
		glUniform2f(glGetUniformLocation(shaderProgram, "uOffset"), circle.centerX, circle.centerY);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, circle.totalVertices);
		glBindVertexArray(0);

		// draw circle1
		glUseProgram(shaderProgram);
		glUniform1f(glGetUniformLocation(shaderProgram, "uRadius"), circle1.circleRadius);
		glUniform2f(glGetUniformLocation(shaderProgram, "uResolution"), screenWidth, screenHeight);
		glUniform2f(glGetUniformLocation(shaderProgram, "uOffset"), circle1.centerX, circle1.centerY);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, circle1.totalVertices);
		glBindVertexArray(0);


		
		circle.velocityY += circle.accelerationY * dT;
		circle.centerY += circle.velocityY * dT;

		circle.velocityX += circle.accelerationX * dT;
		circle.centerX += circle.velocityX * dT;

		// collision in the y
		if (circle.centerY <= 0 + circle.circleRadius) {
			circle.centerY = circle.circleRadius;
			circle.velocityY *= -0.8;
		}
		if (circle.centerY >= 600 - circle.circleRadius) {
			circle.centerY = 600 - circle.circleRadius;
			circle.velocityY *= -1;
		}

		// collision in the x
		if (circle.centerX <= 0 + circle.circleRadius) {
			circle.centerX = circle.circleRadius;
			circle.velocityX *= -1;
		}
		if (circle.centerX >= 800 - circle.circleRadius) {
			circle.centerX = 800 - circle.circleRadius;
			circle.velocityX *= -1;
		}


		circle1.velocityY += circle1.accelerationY * dT;
		circle1.centerY += circle1.velocityY * dT;

		circle1.velocityX += circle1.accelerationX * dT;
		circle1.centerX += circle1.velocityX * dT;

		// collision in the y
		if (circle1.centerY <= 0 + circle1.circleRadius) {
			circle1.centerY = circle1.circleRadius;
			circle1.velocityY *= -1;
		}
		if (circle1.centerY >= 600 - circle1.circleRadius) {
			circle1.centerY = 600 - circle1.circleRadius;
			circle1.velocityY *= -1;
		}

		// collision in the x
		if (circle1.centerX <= 0 + circle1.circleRadius) {
			circle1.centerX = circle1.circleRadius;
			circle1.velocityX *= -1;
		}
		if (circle1.centerX >= 800 - circle1.circleRadius) {
			circle1.centerX = 800 - circle1.circleRadius;
			circle1.velocityX *= -1;
		}
		

		// collision between 2 circles
		distanceX = circle.centerX - circle1.centerX;
		distanceY = circle.centerY - circle1.centerY;
		distanceTotal = sqrt(pow(distanceX, 2) + pow(distanceY, 2));

		if (distanceTotal < circle.circleRadius + circle1.circleRadius) {
			float normalX = distanceX / distanceTotal;
			float normalY = distanceY / distanceTotal;
			float overlap = (circle.circleRadius + circle1.circleRadius) - distanceTotal;

			circle.centerX += normalX * overlap * 0.5f;
			circle.centerY += normalY * overlap * 0.5f;
			circle1.centerX -= normalX * overlap * 0.5f;
			circle1.centerY -= normalY * overlap * 0.5f;

			float velocityNormalA = circle.velocityX * normalX + circle.velocityY * normalY;
			float velocityNormalB = circle1.velocityX * normalX + circle1.velocityY * normalY;

			if (velocityNormalA < velocityNormalB) {
				float change = velocityNormalB - velocityNormalA;

				circle.velocityX += change * normalX;
				circle.velocityY += change * normalY;
				circle1.velocityX -= change * normalX;
				circle1.velocityY -= change * normalY;
			}
		
		}




		glfwSwapBuffers(window);
		glfwPollEvents();
	} 




	// terminate window
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

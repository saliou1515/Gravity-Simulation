#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <cmath>
#include <vector>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"uniform vec2 uOffset;\n"
"uniform vec2 uResolution;\n"
"void main()\n"
"{\n"
"	vec2 pixelPos = aPos + uOffset;\n"
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

		float velocityX = 10.0f * pPM;
		float velocityY = 20.0f * pPM;
		float accelerationX = 0.0f * pPM;
		float accelerationY = -9.8f * pPM;

		Circle() {
			vertices.assign(totalVertices * 2, 0.0f);

			// adds x and y coordinates to vertices array by going to each segments angles,
			// then skipping the first two vertices (center), and skipping a number each time to
			// add the x and y coordinate to the array
			for (int i = 0; i <= numSegments; i++) {
				float angle = (i * 2 * PI) / numSegments;
				vertices[(i + 1) * 2] = (circleRadius * cos(angle));
				vertices[(i + 1) * 2 + 1] = (circleRadius * sin(angle));
			}
		}
	};
	
	/*	float distanceX;
	 *	float distanceY;
	 *	float unitDistanceX;
	 *	float unitDistanceY;
	 *	float circleXSquared;
	 *	float circleYSquared;
	 */

	Circle circle;
	Circle circle1;

	circle1.velocityX = 30.0f * pPM;
	circle1.velocityY = 15.0f * pPM;
	circle1.accelerationX = 3.0f * pPM;
		

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


		float currentFrame = static_cast<float>(glfwGetTime());
		dT = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shaderProgram);
		glUniform2f(glGetUniformLocation(shaderProgram, "uResolution"), screenWidth, screenHeight);
		glUniform2f(glGetUniformLocation(shaderProgram, "uOffset"), circle.centerX, circle.centerY);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, circle.totalVertices);
		glBindVertexArray(0);

		glUseProgram(shaderProgram);
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
			circle1.velocityY *= -0.8;
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
		
		// broken collision between circles

		/* distanceX = abs(circle.centerX - circle1.centerX);
		distanceY = abs(circle.centerY - circle1.centerY);
		circleXSquared = pow(circle.centerX, 2) + pow(circle1.centerX, 2);
		circleYSquared = pow(circle.centerY, 2) + pow(circle1.centerY, 2);
		unitDistanceX = pow(circleXSquared, 0.5);
		unitDistanceY = pow(circleYSquared, 0.5);

		if (unitDistanceX < circle.circleRadius + circle1.circleRadius) {
			circle.velocityX *= -1;
			circle1.velocityX *= -1;
		}
		if (unitDistanceY < circle.circleRadius + circle1.circleRadius) {
			circle.velocityY *= -1;
			circle1.velocityY *= -1;
		} */

		glfwSwapBuffers(window);
		glfwPollEvents();
	} 


	// terminate window
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

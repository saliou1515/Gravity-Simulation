#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <cmath>
#include <vector>

// pixels per Meter conversion to have accurate physics equations
constexpr double pPM = 1.0e-5;
// time conversion to make simulated time faster
constexpr double timeScale = 3.0e3;

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

// create circle vertices and object
struct Circle {
	const float PI = 3.1415926535f;
	int numSegments = 40;
	int totalVertices = numSegments + 2;
	std::vector<float> vertices;

	double mass = 7.342e22;

	double centerX = 0.0;
	double centerY = 0.0;
	double circleRadius = 25 / 3.67;

	double velocityX = 0.0 * pPM;
	double velocityY = 0.0 * pPM;
	double accelerationX = 0.0 * pPM;
	double accelerationY = 0.0 * pPM;

	Circle() {
		vertices.assign(totalVertices * 2, 0.0f);

		// adds x and y coordinates to vertices array by going to each segments angles,
		// then skipping the first two vertices (center), and skipping a number each time to
		// add the x and y coordinate to the array
		for (int i = 0; i <= numSegments; i++) {
			double angle = (i * 2 * PI) / numSegments;
			vertices[(i + 1) * 2] = cos(angle);
			vertices[(i + 1) * 2 + 1] = sin(angle);
		}
	}
};

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

	double dT = 0.00;
	double lastFrame = glfwGetTime();
	
	// create circle objects
	const double G = 6.6743e-11;
	/*
	double distanceX;
	double distanceY;
	double distanceTotal;
	*/

	std::vector<Circle> bodies(3);
	Circle& mars = bodies[0];
	Circle& phobos = bodies[1];
	Circle& deimos = bodies[2];

	mars.mass = 6.417e23;
	mars.circleRadius = 3.3895e6 * pPM;
	mars.centerX = screenWidth / 2.0;
	mars.centerY = screenHeight / 2.0;
		
	phobos.mass = 1.0659e16;
	phobos.circleRadius = 4.0; // visibility - not to scale
	phobos.centerX = mars.centerX + 9.376e6 * pPM;
	phobos.centerY = mars.centerY;
	phobos.velocityY = 2137.0 * pPM;

	deimos.mass = 1.476e15;
	deimos.circleRadius = 3.0;
	deimos.centerX = mars.centerX - 2.3463e7 * pPM;
	deimos.centerY = mars.centerY;
	deimos.velocityY = -1351.0 * pPM;



	GLuint VBO, VAO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bodies[0].vertices.size() * sizeof(float), bodies[0].vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	

	// render loop
	while (!glfwWindowShouldClose(window)) {

		// get change in time between frames;
		double currentFrame = glfwGetTime();
		dT = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// prevent moving the screen from messing up motion
		if (dT > 0.05) {
			dT = 0.05;
		}

		double scaledDT = dT * timeScale;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// draw circle
		glUseProgram(shaderProgram);
		glUniform2f(glGetUniformLocation(shaderProgram, "uResolution"), screenWidth, screenHeight);
		glBindVertexArray(VAO);

		for (const Circle& body : bodies) {
			glUniform1f(glGetUniformLocation(shaderProgram, "uRadius"), static_cast<float>(body.circleRadius));
			glUniform2f(glGetUniformLocation(shaderProgram, "uOffset"), static_cast<float>(body.centerX), static_cast<float>(body.centerY));
			glDrawArrays(GL_TRIANGLE_FAN, 0, body.totalVertices);
		}
		glBindVertexArray(0);

		for (Circle& body : bodies) {
			body.velocityY += body.accelerationY * scaledDT;
			body.centerY += body.velocityY * scaledDT;

			body.velocityX += body.accelerationX * scaledDT;
			body.centerX += body.velocityX * scaledDT;
		}
		/*
		circle.velocityY += circle.accelerationY *scaledDT;
		circle.centerY += circle.velocityY *scaledDT;

		circle.velocityX += circle.accelerationX *scaledDT;
		circle.centerX += circle.velocityX *scaledDT;

		circle1.velocityY += circle1.accelerationY *scaledDT;
		circle1.centerY += circle1.velocityY *scaledDT;

		circle1.velocityX += circle1.accelerationX *scaledDT;
		circle1.centerX += circle1.velocityX *scaledDT;

		*/
		/*
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
		*/

		/*
		// collision between 2 circles
		distanceX = circle.centerX - circle1.centerX;
		distanceY = circle.centerY - circle1.centerY;
		distanceTotal = sqrt(pow(distanceX, 2) + pow(distanceY, 2));

		if (distanceTotal < circle.circleRadius + circle1.circleRadius) {
			double normalX = distanceX / distanceTotal;
			double normalY = distanceY / distanceTotal;
			double overlap = (circle.circleRadius + circle1.circleRadius) - distanceTotal;

			circle.centerX += normalX * overlap * 0.5f;
			circle.centerY += normalY * overlap * 0.5f;
			circle1.centerX -= normalX * overlap * 0.5f;
			circle1.centerY -= normalY * overlap * 0.5f;

			double velocityNormalA = circle.velocityX * normalX + circle.velocityY * normalY;
			double velocityNormalB = circle1.velocityX * normalX + circle1.velocityY * normalY;

			if (velocityNormalA < velocityNormalB) {
				double change = velocityNormalB - velocityNormalA;

				circle.velocityX += change * normalX;
				circle.velocityY += change * normalY;
				circle1.velocityX -= change * normalX;
				circle1.velocityY -= change * normalY;
			}
		
		}
		*/

		for (Circle& body : bodies) {
			body.accelerationX = 0.0;
			body.accelerationY = 0.0;
		}

		for (size_t i = 0; i < bodies.size(); i++) {
			for (size_t j = i + 1; j < bodies.size(); j++) {
				Circle& a = bodies[i];
				Circle& b = bodies[j];

				// r for gravitational force equation
				double rDistanceX = b.centerX - a.centerX;
				double rDistanceY = b.centerY - a.centerY;
				double rDistance = sqrt(pow(rDistanceX, 2) + pow(rDistanceY, 2));

				// unit vector calculation
				double unitX = rDistanceX / rDistance;
				double unitY = rDistanceY / rDistance;

				// calculating gravitational force
				double gravityForce = G * (a.mass * b.mass / pow(rDistance / pPM, 2));

				// updating acceleration
				a.accelerationX += (gravityForce / a.mass) * unitX * pPM;
				a.accelerationY += (gravityForce / a.mass) * unitY * pPM;
				b.accelerationX -= (gravityForce / b.mass) * unitX * pPM;
				b.accelerationY -= (gravityForce / b.mass) * unitY * pPM;
			}
		}


	/*
		// r for gravitational force equation
		double rDistanceX = circle1.centerX - circle.centerX;
		double rDistanceY = circle1.centerY - circle.centerY;
		double rDistance = sqrt(pow(rDistanceX, 2) + pow(rDistanceY, 2));

		// unit vector calculation
		double unitX = rDistanceX / rDistance;
		double unitY = rDistanceY / rDistance;

		// calculating gravitational force
		double gravityForce = G * (circle.mass * circle1.mass / pow(rDistance / pPM, 2));
		
		circle.accelerationX = (gravityForce / circle.mass) * unitX * pPM;
		circle.accelerationY = (gravityForce / circle.mass) * unitY * pPM;

		circle1.accelerationX = (gravityForce / circle1.mass) * -unitX * pPM;
		circle1.accelerationY = (gravityForce / circle1.mass) * -unitY * pPM;

	*/
		glfwSwapBuffers(window);
		glfwPollEvents();
	} 




	// terminate window
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

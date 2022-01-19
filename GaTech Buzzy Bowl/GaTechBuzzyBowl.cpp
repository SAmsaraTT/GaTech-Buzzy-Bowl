/*
Author: Haotian Liu
Last Date Modified: Dec.6
*/

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;
using namespace std;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include"ECE_UAV.h"
// define the collision function
bool collision(vec3 A, vec3 B);
bool collision(vec3 A, vec3 B)
{
	double Distance = glm::distance(A, B);
	if (Distance < 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//define the separate function
bool separate(vec3 A, vec3 B);
bool separate(vec3 A, vec3 B)
{
	double Distance = glm::distance(A, B);
	if (Distance > 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}



int main( void )
{
	int Lock[15][15] = {};
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1920, 1080, "Tutorial 09 - Rendering several models", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
    
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1920/2, 1080/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	GLuint intensityScale = glGetUniformLocation(programID, "Intensity");
	float fIntensity = 0.5f;
	//

	// Load the texture
	GLuint Texture = loadDDS("uvmap.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	GLuint Texture1 = loadBMP_custom("ff.bmp");

	GLuint TextureID1 = glGetUniformLocation(programID, "myTextureSampler1");

	GLuint Texture2 = loadBMP_custom("gg1.bmp");

	GLuint TextureID2 = glGetUniformLocation(programID, "myTextureSampler2");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("suzanne.obj", vertices, uvs, normals);

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);


	std::vector<glm::vec3> vertices1;
	std::vector<glm::vec2> uvs1;
	std::vector<glm::vec3> normals1;
	bool res1 = loadOBJ("footballfield.obj", vertices1, uvs1, normals1);

	std::vector<unsigned short> indices1;
	std::vector<glm::vec3> indexed_vertices1;
	std::vector<glm::vec2> indexed_uvs1;
	std::vector<glm::vec3> indexed_normals1;
	indexVBO(vertices1, uvs1, normals1, indices1, indexed_vertices1, indexed_uvs1, indexed_normals1);

	std::vector<glm::vec3> vertices2;
	std::vector<glm::vec2> uvs2;
	std::vector<glm::vec3> normals2;
	bool res2 = loadOBJ("sphere.obj", vertices2, uvs2, normals2);

	std::vector<unsigned short> indices2;
	std::vector<glm::vec3> indexed_vertices2;
	std::vector<glm::vec2> indexed_uvs2;
	std::vector<glm::vec3> indexed_normals2;
	indexVBO(vertices2, uvs2, normals2, indices2, indexed_vertices2, indexed_uvs2, indexed_normals2);

	// Load it into a VBO1

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

	// Load it into a VBO2
	GLuint vertexbuffer1;
	glGenBuffers(1, &vertexbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices1.size() * sizeof(glm::vec3), &indexed_vertices1[0], GL_STATIC_DRAW);

	GLuint uvbuffer1;
	glGenBuffers(1, &uvbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer1);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs1.size() * sizeof(glm::vec2), &indexed_uvs1[0], GL_STATIC_DRAW);

	GLuint normalbuffer1;
	glGenBuffers(1, &normalbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer1);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals1.size() * sizeof(glm::vec3), &indexed_normals1[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer1;
	glGenBuffers(1, &elementbuffer1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices1.size() * sizeof(unsigned short), &indices1[0], GL_STATIC_DRAW);

	// Load it into a VBO3
	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices2.size() * sizeof(glm::vec3), &indexed_vertices2[0], GL_STATIC_DRAW);

	GLuint uvbuffer2;
	glGenBuffers(1, &uvbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs2.size() * sizeof(glm::vec2), &indexed_uvs2[0], GL_STATIC_DRAW);

	GLuint normalbuffer2;
	glGenBuffers(1, &normalbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals2.size() * sizeof(glm::vec3), &indexed_normals2[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer2;
	glGenBuffers(1, &elementbuffer2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(unsigned short), &indices2[0], GL_STATIC_DRAW);
	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// For speed computation
	int nbFrames = 0;
	double lastTime = glfwGetTime();
	float rotAngle = (-90.0f/180.0f)*3.1415926535f;
	float rotAngle1 = (-90.0f / 180.0f) * 3.1415926535f;
	
	// enter 15 UAVs, then initialize their positions
	ECE_UAV* Uav[15];
	std::vector<ECE_UAV*>V1;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			Uav[i*5+j]= new ECE_UAV(-50.0f+j*25.0f, 0.0f, -25.0f+i*25.0f);
		}
	}
	for (int i = 0; i < 15; i++)
	{
		V1.push_back(Uav[i]);

	}
	for (int i = 0; i < 15; i++)
	{
		V1[i]->start();
	}
	
	


	double LTime = glfwGetTime();
	double deltaTime = 0, nowTime = 0;


	do {

		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1sec ago
			// printf and reset
			//printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();



		////// Start of the rendering of the first object //////

		// Use our shader
		glUseProgram(programID);

		glm::vec3 lightPos = glm::vec3(0, -250, 200);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]); // This one doesn't change between objects, so this can be done once for all objects that use "programID"


		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);
		glUniform1i(intensityScale, 1.0f);



		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		nowTime = glfwGetTime();
		deltaTime +=nowTime - LTime;
		LTime = nowTime;

		//change the intensity of light 
		fIntensity += 0.01f;
		if (fIntensity > 1.0f)
		{
			fIntensity = 0.01f;
		}
		// update the position of 15 UAVs every 10 msec
		while (deltaTime >= 0.01)
		{
			for (int i = 0; i < 15; i++)
			{
				glm::mat4 ModelMatrix1 = glm::mat4(1.0);
				ModelMatrix1 = glm::translate(ModelMatrix1, V1[i]->m_Position);
				glm::mat4 MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1;
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix1[0][0]);
				glUniform1f(intensityScale, fIntensity);
				// Draw the triangles !
				glDrawElements(
					GL_TRIANGLES,      // mode
					indices.size(),    // count
					GL_UNSIGNED_SHORT,   // type
					(void*)0           // element array buffer offset
				);
			}
			deltaTime -= 0.01;
		}
		
		
		
//detect collisions 
		for (int i = 0; i < 14; i++)
		{
			for (int j = i + 1; j <15 ; j++)
			{
				if (collision(V1[i]->m_Position, V1[j]->m_Position) && V1[i]->St == 1 && V1[j]->St == 1 && Lock[i][j]==1)
				{
					vec3 exchange = V1[j]->m_Velocity;
					V1[j]->m_Velocity = V1[i]->m_Velocity;
					V1[i]->m_Velocity = exchange;
					Lock[i][j] = 0;
				}
				if (separate(V1[i]->m_Position, V1[j]->m_Position))
				{
					Lock[i][j] = 1;
				}

			}

	    }
	
	
		
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);
		// set the positons of sphere
		glm::mat4 ModelMatrix_S = glm::mat4(1.0);
		ModelMatrix_S = glm::translate(ModelMatrix_S, glm::vec3(0.0f, 50.0f, 0.0f));
		ModelMatrix_S = glm::rotate(ModelMatrix_S, rotAngle1, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix_S = glm::scale(ModelMatrix_S, glm::vec3(2.5f, 2.5f, 2.5f));
		glm::mat4 MVP_S = ProjectionMatrix * ViewMatrix * ModelMatrix_S;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_S[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix_S[0][0]);
		glUniform1f(intensityScale, 1.0f);

		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glDrawElements(
			GL_TRIANGLES,      // mode
			indices2.size(),    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);



		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer1);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer1);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture1);

		//set the position of footfield
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix , rotAngle, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.375f, 0.375f, 0.375f));
        
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		

		glDrawElements(
			GL_TRIANGLES,      // mode
			indices1.size(),    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

		


		
		
		
		// BUT the Model matrix is different (and the MVP too)



		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	for (int i = 0; i < 15; i++)
	{
		V1[i]->stop();
	}

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);

	glDeleteBuffers(1, &vertexbuffer1);
	glDeleteBuffers(1, &uvbuffer1);
	glDeleteBuffers(1, &normalbuffer1);
	glDeleteBuffers(1, &elementbuffer1);

	glDeleteBuffers(1, &vertexbuffer2);
	glDeleteBuffers(1, &uvbuffer2);
	glDeleteBuffers(1, &normalbuffer2);
	glDeleteBuffers(1, &elementbuffer2);

	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteTextures(1, &Texture1);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	V1.clear();
	return 0;
}


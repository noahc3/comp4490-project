#include "common.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"

#include <string>
#include <format>

#include <chrono>
#include "ObjUtils.h"
#include "../opengl/blinnphong.h"

const char* WINDOW_TITLE = "Grass Simulation";
const double FRAME_RATE_MS = 1000.0 / 60.0;

typedef glm::vec4 color4;
typedef glm::vec4 point4;
typedef glm::vec3 point3;

glm::mat4 Projection = glm::mat4(0);

struct GrassProps {
	bool EnableGrass = true;
	int ShellCount = 32;
	float GrassHeight = 1.0f;
	float GrassThickness = 1.0f;
	int PlaneResolution = 64;
	float PlaneSize = 32.0f;
	glm::vec3 Skew = glm::vec3(0, 0, 0);
	glm::vec3 BaseColor = glm::vec3(0, 0.25f, 0);
	glm::vec3 TopColor = glm::vec3(0.0f, 1.0f, 0.0f);
};

struct WindProps {
	bool EnableWind = true;
	glm::vec3 WindDirection = glm::vec3(0, 0, 0);
	float WindStrength = 0.5f;
	float WindCurveSpeed = 0.5f;
};

struct SkyProps {
	glm::vec3 SkyColor = glm::vec3(87 / 255.0, 247 / 255.0, 255 / 255.0);

	bool EnableClouds = true;
	int CloudShellCount = 8;
	float CloudThickness = 10.0f;
	int CloudPlaneResolution = 1;
	float CloudPlaneSize = 32.0f;
	float CloudHeight = 5.0f;
	float CloudNoiseDroop = 0.2f;
	glm::vec3 BaseColor = glm::vec3(0, 0.25f, 0);
	glm::vec3 TopColor = glm::vec3(0.0f, 1.0f, 0.0f);
};

struct SceneProps {
	bool drawBear = true;
	bool drawChicken = true;
	SkyProps skyProps;
	GrassProps grassProps;
	WindProps windProps;
};

SceneProps activeScene;
SceneProps scene1;
SceneProps scene2;
SceneProps scene3;
SceneProps scene4;
SceneProps scene5;


void GenScenes() {
	scene1 = {
		/* drawBear */ false,
		/* drawChicken */ false,
		/* skyProps */ {
			/* SkyColor */ glm::vec3(87 / 255.0, 247 / 255.0, 255 / 255.0),
			/* EnableClouds */ true,
			/* CloudShellCount */ 36,
			/* CloudThickness */ 50.5f,
			/* CloudPlaneResolution */ 7,
			/* CloudPlaneSize */ 2048.0f,
			/* CloudHeight */ 2000.0f,
			/* CloudNoiseDroop */ -0.1f,
			/* BaseColor */ glm::vec3(215/255.0f, 229/255.0f, 229/255.0f),
			/* TopColor */ glm::vec3(1.0f)
		},
		/* grassProps */ {
			/* EnableGrass */ true,
			/* ShellCount */ 55,
			/* GrassHeight */ 0.9f,
			/* GrassThickness */ 3.579f,
			/* PlaneResolution */ 215,
			/* PlaneSize */ 200.0f,
			/* Skew */ glm::vec3(0, 0, 0),
			/* BaseColor */ glm::vec3(5 / 255.0f, 24 / 255.0f, 4 / 255.0f),
			/* TopColor */ glm::vec3(145 / 255.0f, 224 / 255.0f, 103 / 255.0f)
		},
		/* windProps */ {
			/* EnableWind */ true,
			/* WindDirection */ glm::normalize(glm::vec3(-1, 0, 0.25f)),
			/* WindStrength */ 0.1f,            
			/* WindCurveSpeed */ 0.5f
		}
	};

	scene2 = {
		/* drawBear */ true,
		/* drawChicken */ false,
		/* skyProps */ {
			/* SkyColor */ glm::vec3(224 / 255.0, 248 / 255.0, 250 / 255.0),
			/* EnableClouds */ true,
			/* CloudShellCount */ 71,
			/* CloudThickness */ 61.5f,
			/* CloudPlaneResolution */ 2,
			/* CloudPlaneSize */ 2048.0f,
			/* CloudHeight */ 2000.0f,
			/* CloudNoiseDroop */ -0.39f,
			/* BaseColor */ glm::vec3(1.0f),
			/* TopColor */ glm::vec3(160/255.0f)
		},
		/* grassProps */ {
			/* EnableGrass */ true,
			/* ShellCount */ 12,
			/* GrassHeight */ 1.053f,
			/* GrassThickness */ 10.0f,
			/* PlaneResolution */ 1024,
			/* PlaneSize */ 64.0f,
			/* Skew */ glm::vec3(0, 0, 0),
			/* BaseColor */ glm::vec3(1),
			/* TopColor */ glm::vec3(223 / 255.0f)
		},
		/* windProps */ {
			/* EnableWind */ false,
			/* WindDirection */ glm::vec3(0, 0, 0),
			/* WindStrength */ 0.3f,
			/* WindCurveSpeed */ 0.5f
		}
	};

	scene3 = {
		/* drawBear */ false ,
		/* drawChicken */ false,
		/* skyProps */ {
			/* SkyColor */ glm::vec3(255 / 255.0, 65 / 255.0, 0 / 255.0),
			/* EnableClouds */ false,
			/* CloudShellCount */ 8,
			/* CloudThickness */ 10.0f,
			/* CloudPlaneResolution */ 3,
			/* CloudPlaneSize */ 512.0f,
			/* CloudHeight */ 130.0f,
			/* CloudNoiseDroop */ 0.2f,
			/* BaseColor */ glm::vec3(0.9f),
			/* TopColor */ glm::vec3(1.0f)
		},
		/* grassProps */ {
			/* EnableGrass */ true,
			/* ShellCount */ 18,
			/* GrassHeight */ 0.791f,
			/* GrassThickness */ 10.0f,
			/* PlaneResolution */ 8,
			/* PlaneSize */ 64.0f,
			/* Skew */ glm::vec3(0, 0, 0),
			/* BaseColor */ glm::vec3(38 / 255.0f, 0, 0),
			/* TopColor */ glm::vec3(255 / 255.0f, 0, 0)
		},
		/* windProps */ {
			/* EnableWind */ true,
			/* WindDirection */ glm::normalize(glm::vec3(-1, 0, 0.25f)),
			/* WindStrength */ 0.3f,
			/* WindCurveSpeed */ 0.5f
		}
	};

	scene4 = {
		/* drawBear */ false ,
		/* drawChicken */ true,
		/* skyProps */ {
			/* SkyColor */ glm::vec3(87 / 255.0, 247 / 255.0, 255 / 255.0),
			/* EnableClouds */ true,
			/* CloudShellCount */ 8,
			/* CloudThickness */ 10.0f,
			/* CloudPlaneResolution */ 3,
			/* CloudPlaneSize */ 512.0f,
			/* CloudHeight */ 130.0f,
			/* CloudNoiseDroop */ 0.2f,
			/* BaseColor */ glm::vec3(0.9f),
			/* TopColor */ glm::vec3(1.0f)
		},
		/* grassProps */ {
			/* EnableGrass */ true,
			/* ShellCount */ 9,
			/* GrassHeight */ 1.041f,
			/* GrassThickness */ 4.234f,
			/* PlaneResolution */ 290,
			/* PlaneSize */ 64.0f,
			/* Skew */ glm::vec3(0, 0, 0),
			/* BaseColor */ glm::vec3(64 /255.0f, 82 / 255.0f, 0),
			/* TopColor */ glm::vec3(1, 1, 0)
		},
		/* windProps */ {
			/* EnableWind */ true,
			/* WindDirection */ glm::normalize(glm::vec3(-1, 0, 0.25f)),
			/* WindStrength */ 0.3f,
			/* WindCurveSpeed */ 0.5f
		}
	};

	scene5 = {
		/* drawBear */ false ,
		/* drawChicken */ false,
		/* skyProps */ {
			/* SkyColor */ glm::vec3(87 / 255.0, 247 / 255.0, 255 / 255.0),
			/* EnableClouds */ true,
			/* CloudShellCount */ 20,
			/* CloudThickness */ 30.0f,
			/* CloudPlaneResolution */ 3,
			/* CloudPlaneSize */ 2048.0f,
			/* CloudHeight */ 1397.0f,
			/* CloudNoiseDroop */ -0.15f,
			/* BaseColor */ glm::vec3(1),
			/* TopColor */ glm::vec3(113 / 255.0f, 181 / 255.0f, 198 / 255.0f)
		},
		/* grassProps */ {
			/* EnableGrass */ true,
			/* ShellCount */ 97,
			/* GrassHeight */ 5.1f,
			/* GrassThickness */ 2.433f,
			/* PlaneResolution */ 18,
			/* PlaneSize */ 85.0f,
			/* Skew */ glm::vec3(0, 0, 0),
			/* BaseColor */ glm::vec3(0, 0, 0),
			/* TopColor */ glm::vec3(0, 1, 0)
		},
		/* windProps */ {
			/* EnableWind */ true,
			/* WindDirection */ glm::normalize(glm::vec3(-1, 0, 0.25f)),
			/* WindStrength */ 0.3f,
			/* WindCurveSpeed */ 0.5f
		}
	};

	activeScene = scene1;
}

void LoadScene(SceneProps scene) {
	activeScene = scene;
}


// Table geometry (just a 2x2 plane)
point4 grass_vertices[4] = {
	point4(-0.5, 0, -0.5, 1.0),
	point4(-0.5, 0, 0.5, 1.0),
	point4(0.5, 0, 0.5, 1.0),
	point4(0.5, 0, -0.5, 1.0) };

GLuint grass_indices[] = {
	0, 1, 2,
	0, 2, 3 };

// GL Handles
GLuint ProgramGrass, ModelViewGrass, ProjectionGrass, Height, MaxHeight, Thickness, PlaneResolution, BaseColour, TopColour, WindDirection;
GLuint ProgramClouds, ModelViewClouds, ProjectionClouds, CloudHeight, CloudThickness, CloudPlaneResolution, CloudBaseColour, CloudTopColour, CloudNoiseDroop;
GLuint VAOs[2];

// Camera rotation
glm::vec3 camRot = glm::vec3(glm::radians(45.0f), 0, 0);
glm::vec3 camPos = glm::vec3(0, 4, 7);

Obj* bear;
Obj* chicken;

uint64_t currentTimeMs()
{
	return GetTickCount64();
}

//----------------------------------------------------------------------------

void setupVAO(GLuint program, GLuint vao, point4 vertices[], GLuint numVertices, GLuint indices[], GLuint numIndices, GLuint* ModelView, GLuint* Projection)
{
	glUseProgram(program);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(point4), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	*ModelView = glGetUniformLocation(program, "ModelView");
	*Projection = glGetUniformLocation(program, "Projection");
}



// OpenGL initialization
void init()
{
	// Gen Scenes
	GenScenes();

	// Init VAOs
	glGenVertexArrays(2, VAOs);

	// Load Grass
	ProgramGrass = InitShader("grass.vert", "grass.frag");
	setupVAO(ProgramGrass, VAOs[0], grass_vertices, sizeof(grass_vertices), grass_indices, sizeof(grass_indices), &ModelViewGrass, &ProjectionGrass);
	Height = glGetUniformLocation(ProgramGrass, "Height");
	MaxHeight = glGetUniformLocation(ProgramGrass, "MaxHeight");
	Thickness = glGetUniformLocation(ProgramGrass, "Thickness");
	PlaneResolution = glGetUniformLocation(ProgramGrass, "PlaneResolution");
	BaseColour = glGetUniformLocation(ProgramGrass, "BaseColour");
	TopColour = glGetUniformLocation(ProgramGrass, "TopColour");
	WindDirection = glGetUniformLocation(ProgramGrass, "WindDirection");

	// Load Clouds
	ProgramClouds = InitShader("shaders/cloud.vert", "shaders/cloud.frag");
	setupVAO(ProgramClouds, VAOs[1], grass_vertices, sizeof(grass_vertices), grass_indices, sizeof(grass_indices), &ModelViewClouds, &ProjectionClouds);
	CloudHeight = glGetUniformLocation(ProgramClouds, "Height");
	CloudThickness = glGetUniformLocation(ProgramClouds, "Thickness");
	CloudPlaneResolution = glGetUniformLocation(ProgramClouds, "PlaneResolution");
	CloudBaseColour = glGetUniformLocation(ProgramClouds, "BaseColour");
	CloudTopColour = glGetUniformLocation(ProgramClouds, "TopColour");
	CloudNoiseDroop = glGetUniformLocation(ProgramClouds, "NoiseDroop");

	//  Load Objects
	bear = LoadObj("models/bear-vnincl.obj", "models/bear-vnincl.mtl");
	ApplyBlinnPhong(bear);

	chicken = LoadObj("models/chicken.obj", "models/chicken.mtl");
	ApplyBlinnPhong(chicken);
	chicken->rotation.y = 90.0f; // I accidentally exported the model facing the wrong way, oops!


	// Set up the view matrix
	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_FLAT);
	glClearColor(87 / 255.0, 247 / 255.0, 255 / 255.0, 1.0);
}

//----------------------------------------------------------------------------

float curve(float x, float a, float b, float c) {
	return a * exp(-b * pow(x - c, 2));
}

float windCurve(float x) {
	float a = curve(x, 0.8, 1.8, 4.1) + 0.1f;
	float b = curve(x, 1.8, 1.1, 1.7);
	float c = curve(x, 1.3, 0.8, 2.8);

	return glm::max(a, glm::max(b, c)) * 0.55f;
}

//----------------------------------------------------------------------------

bool showDemoWindow = false;
bool showParametersWindow = true;
bool showDebugWindow = true;
ImGuiIO* io = nullptr;

// General Props
bool noClip = true;
bool lockMouse = false;
float mouseSensitivity = 0.5f;
float worldScale = 1.0f;

float FPSUpdateInterval = 0.5f;

int GetFrameRate()
{
	static float framesPerSecond = 0.0f;
	static int fps;
	static float lastTime = 0.0f;
	float currentTime = GetTickCount() * 0.001f;
	++framesPerSecond;

	int res = fps;

	if (currentTime - lastTime > FPSUpdateInterval)
	{
		lastTime = currentTime;
		fps = (int)(framesPerSecond / FPSUpdateInterval);
		framesPerSecond = 0;
	}

	return fps;
}

void ImGuiMaterialPanel(Material* material) {
	if (ImGui::TreeNode(("Material '" + material->name + "' Properties").c_str())) {
		ImGui::ColorEdit3("Ambient", glm::value_ptr(material->ambient));
		ImGui::ColorEdit3("Diffuse", glm::value_ptr(material->diffuse));
		ImGui::ColorEdit3("Specular", glm::value_ptr(material->specular));
		ImGui::SliderFloat("Shininess", &material->shininess, 0.0f, 100.0f);
		ImGui::Separator();
		ImGui::Checkbox("Use Fur", &material->furProps.useFur);
		ImGui::SliderInt("Fur Resolution", &material->furProps.resolution, 1, 128);
		ImGui::SliderInt("Shell Count", &material->furProps.shellCount, 1, 128);
		ImGui::SliderFloat("Fur Length", &material->furProps.shellMaxHeight, 0.01f, 5.0f);
		ImGui::SliderFloat("Fur Thickness", &material->furProps.thickness, 0.01f, 10.0f);
		ImGui::TreePop();
	}
}

void ImGuiObjectPanel(std::string name, Obj* object, bool* enable) {
	if (ImGui::CollapsingHeader(("'" + name + "' Object Properties").c_str())) {
		ImGui::Checkbox("Visible", enable);
		ImGui::SliderFloat("Scale", &object->scale, 0.01f, 10.0f);
		ImGui::DragFloat3("Position", glm::value_ptr(object->position), 0.1f, -500.0f, 500.0f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(object->rotation), 0.5f, -360, 360);
		ImGui::Text("Meshes: %d", object->numMeshes);
		ImGui::Separator();
		for (int i = 0; i < object->numMeshes; i++) {
			ImGuiMaterialPanel(object->meshes[i]->material);
		}
	}
}

void drawImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGLUT_NewFrame();
	ImGui::NewFrame();
	io = &ImGui::GetIO();

	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	if (showParametersWindow) {
		ImGui::Begin("Parameters", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Once);
		ImGui::SetWindowSize(ImVec2(450, (*io).DisplaySize.y));

		ImGui::Text("You can toggle visiblity of this window with F11");

		ImGui::SeparatorText("Load Scene");
		if (ImGui::Button("Grassy Plains")) {
			LoadScene(scene1);
		}
		ImGui::SameLine();
		if (ImGui::Button("Snowy Polar Bear")) {
			LoadScene(scene2);
		}
		ImGui::SameLine();
		if (ImGui::Button("Nether Realm")) {
			LoadScene(scene3);
		}
		if (ImGui::Button("Endless Chicken Pen")) {
			LoadScene(scene4);
		}
		ImGui::SameLine();
		if (ImGui::Button("Plains of Wind Waker")) {
			LoadScene(scene5);
		}

		ImGui::SeparatorText("Debug");
		ImGui::Text("FPS: %d", GetFrameRate());
		ImGui::Text("Camera Pos %f, %f, %f", camPos.x, camPos.y, camPos.z);
		ImGui::Text("Camera Rotation: %f, %f, %f", camRot.x, camRot.y, camRot.z);

		ImGui::SeparatorText("Controls");
		ImGui::Checkbox("Lock Mouse (Toggle with F1)", &lockMouse);
		ImGui::Checkbox("No Clip (Toggle with F2)", &noClip);
		ImGui::SliderFloat("Mouse Look Sensitivity", &mouseSensitivity, 0.01f, 2.0f);
		

		ImGui::SeparatorText("World Settings");

		if (ImGui::CollapsingHeader("Cloud Settings")) {
			ImGui::Checkbox("Render Clouds", &activeScene.skyProps.EnableClouds);
			ImGui::DragFloat("Plane Size", &activeScene.skyProps.CloudPlaneSize, 1.0f, 1.0f, 2048.0f);
			ImGui::DragInt("Fragmentation", &activeScene.skyProps.CloudPlaneResolution, 1, 1, 1024);
			ImGui::SliderInt("Cloud Shell Count", &activeScene.skyProps.CloudShellCount, 1, 128);
			ImGui::DragFloat("Cloud Height", &activeScene.skyProps.CloudHeight, 5.0f, 0.0f, 2000.0f);
			ImGui::DragFloat("Cloud Thickness", &activeScene.skyProps.CloudThickness, 0.5f, 0.5f, 256.0f);
			ImGui::DragFloat("Cloud Noise Floor", &activeScene.skyProps.CloudNoiseDroop, 0.01f, -1.0f, 1.0f);
			ImGui::ColorPicker3("Cloud Top Color", glm::value_ptr(activeScene.skyProps.TopColor));
			ImGui::ColorPicker3("Cloud Base Color", glm::value_ptr(activeScene.skyProps.BaseColor));
		}

		if (ImGui::CollapsingHeader("Sky Settings")) {
			//ImGui::SliderFloat("World Scale", &worldScale, 0.1f, 10.0f);
			ImGui::ColorPicker3("Sky Color", glm::value_ptr(activeScene.skyProps.SkyColor));
		}
		
		if (ImGui::CollapsingHeader("Grass Settings")) {
			ImGui::Checkbox("Render Grass", &activeScene.grassProps.EnableGrass);
			ImGui::SliderFloat("Plane Size", &activeScene.grassProps.PlaneSize, 1.0f, 200.0f);
			ImGui::SliderInt("Plane Resolution", &activeScene.grassProps.PlaneResolution, 1, 1024);
			ImGui::SliderInt("Shell Count", &activeScene.grassProps.ShellCount, 1, 128);
			ImGui::DragFloat("Grass Height", &activeScene.grassProps.GrassHeight, 0.1f, 0.1f, 50.0f);
			ImGui::SliderFloat("Grass Thickness", &activeScene.grassProps.GrassThickness, 0.01f, 10.0f);
			ImGui::SliderFloat3("Skew", glm::value_ptr(activeScene.grassProps.Skew), -1.0f, 1.0f);
			ImGui::ColorPicker3("Grass Top Color", glm::value_ptr(activeScene.grassProps.TopColor));
			ImGui::ColorPicker3("Grass Base Color", glm::value_ptr(activeScene.grassProps.BaseColor));
		}

		ImGui::SeparatorText("Object Settings");

		ImGuiObjectPanel("Bear", bear, &activeScene.drawBear);
		ImGuiObjectPanel("Chicken", chicken, &activeScene.drawChicken);

		ImGui::End();
	}
}

void display(void)
{
	/* Gen ImGui State */
	drawImGui();
	ImGui::Render();


	/* Clear Screen */
	glViewport(0, 0, (GLsizei)io->DisplaySize.x, (GLsizei)io->DisplaySize.y);
	glClearColor(activeScene.skyProps.SkyColor.r, activeScene.skyProps.SkyColor.g, activeScene.skyProps.SkyColor.b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/* Setup View Matrix*/
	glm::mat4 view, model_view = glm::mat4();

	view = glm::rotate(view, camRot.x, glm::vec3(1, 0, 0));
	view = glm::rotate(view, camRot.y, glm::vec3(0, 1, 0));
	view = glm::rotate(view, camRot.z, glm::vec3(0, 0, 1));
	view = glm::translate(view, -camPos);
	view = glm::scale(view, glm::vec3(worldScale * 0.1f));


	/* Draw Grass */
	if (activeScene.grassProps.EnableGrass) {
		glUseProgram(ProgramGrass);
		model_view = view;
		model_view = model_view * glm::scale(glm::mat4(), glm::vec3(activeScene.grassProps.PlaneSize * 10.0f));

		glUniformMatrix4fv(ModelViewGrass, 1, GL_FALSE, glm::value_ptr(model_view));
		glBindVertexArray(VAOs[0]);

		glUniform1f(MaxHeight, activeScene.grassProps.GrassHeight * 0.1f);
		glUniform1f(Thickness, activeScene.grassProps.GrassThickness);
		glUniform1i(PlaneResolution, activeScene.grassProps.PlaneResolution * activeScene.grassProps.PlaneSize);
		glUniform3fv(BaseColour, 1, glm::value_ptr(activeScene.grassProps.BaseColor));
		glUniform3fv(TopColour, 1, glm::value_ptr(activeScene.grassProps.TopColor));		

		if (activeScene.windProps.EnableWind) {
			float t = ((int) (currentTimeMs() * activeScene.windProps.WindCurveSpeed) % 6000) / 1000.0f;
			glm::vec3 windDirection = windCurve(t) * activeScene.windProps.WindStrength * 0.02f * activeScene.windProps.WindDirection;
			glUniform3fv(WindDirection, 1, glm::value_ptr(windDirection));
		}
		else {
			glUniform3fv(WindDirection, 1, glm::value_ptr(glm::vec3(0)));
		}
		

		for (int i = 0; i < activeScene.grassProps.ShellCount; i++)
		{
			glUniform1f(Height, ((float)i) * (1.0f / ((float)activeScene.grassProps.ShellCount)));
			glDrawElements(GL_TRIANGLES, sizeof(grass_indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		}
	}

	/* Draw Clouds */
	if (activeScene.skyProps.EnableClouds) {
		glUseProgram(ProgramClouds);
		model_view = view;
		model_view = model_view * glm::translate(glm::mat4(), glm::vec3(0, activeScene.skyProps.CloudHeight, 0));
		model_view = model_view * glm::scale(glm::mat4(), glm::vec3(activeScene.skyProps.CloudPlaneSize * 10.0f, 0, activeScene.skyProps.CloudPlaneSize * 10.0f));

		glUniformMatrix4fv(ModelViewClouds, 1, GL_FALSE, glm::value_ptr(model_view));
		glBindVertexArray(VAOs[1]);

		glUniform1f(CloudThickness, activeScene.skyProps.CloudThickness);
		glUniform1i(CloudPlaneResolution, activeScene.skyProps.CloudPlaneResolution * activeScene.skyProps.CloudPlaneSize);
		glUniform3fv(CloudBaseColour, 1, glm::value_ptr(activeScene.skyProps.BaseColor));
		glUniform3fv(CloudTopColour, 1, glm::value_ptr(activeScene.skyProps.TopColor));
		glUniform1f(CloudNoiseDroop, activeScene.skyProps.CloudNoiseDroop);

		for (int i = 0; i < activeScene.skyProps.CloudShellCount; i++)
		{
			float planeHeight = ((float)i) * (1.0f / ((float)activeScene.skyProps.CloudShellCount)) * 2.0f - 1.0f;
			glUniform1f(CloudHeight, planeHeight);
			glDrawElements(GL_TRIANGLES, sizeof(grass_indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		}
	}

	/* Draw Objects */
	if (activeScene.drawBear) DrawBlinnPhong(bear, view, Projection);
	if (activeScene.drawChicken) DrawBlinnPhong(chicken, view, Projection);


	/* Draw ImGui */
	glUseProgram(0);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	/* Swap Buffers */
	glutSwapBuffers();
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

bool shiftPressed = false;
bool keyStates[256] = { false };
bool mouseStates[3] = { false };

void keyboard(unsigned char key, int x, int y)
{
	ImGui_ImplGLUT_KeyboardFunc(key, x, y);
	
	if (key == 27) {
		if (lockMouse) {
			lockMouse = false;
		}
		else {
			glutLeaveMainLoop();
		}
	}

	keyStates[key] = true;
}

void keyboardUp(unsigned char key, int x, int y)
{
	ImGui_ImplGLUT_KeyboardUpFunc(key, x, y);

	keyStates[key] = false;
}

void special(int key, int x, int y)
{
	ImGui_ImplGLUT_SpecialFunc(key, x, y);

	if (key == GLUT_KEY_F1) {
		lockMouse = !lockMouse;
	}
	else if (key == GLUT_KEY_F2) {
		noClip = !noClip;
	}
	else if (key == GLUT_KEY_F11) {
		showParametersWindow = !showParametersWindow;
	}
	else if (key == GLUT_KEY_F9) {
		showDemoWindow = !showDemoWindow;
	}
}

void specialUp(int key, int x, int y)
{
	ImGui_ImplGLUT_SpecialUpFunc(key, x, y);
}

//----------------------------------------------------------------------------

int lastMouseX = 0;
int lastMouseY = 0;

void mouse(int button, int state, int x, int y)
{
	ImGui_ImplGLUT_MouseFunc(button, state, x, y);

	if (button < 3)
	{
		mouseStates[button] = state == GLUT_DOWN;
		lastMouseX = x;
		lastMouseY = y;
	}

}

void mouseWheel(int button, int dir, int x, int y) {
	ImGui_ImplGLUT_MouseWheelFunc(button, dir, x, y);
}

void motion(int x, int y)
{
	ImGui_ImplGLUT_MotionFunc(x, y);
}

bool lastMouseLock = false;

float wwidth, wheight = 0;

void passiveMotion(int x, int y) {
	if (io != nullptr) {
		if (lockMouse) {
			int centerX = wwidth / 2;
			int centerY = wheight / 2;

			if (lastMouseLock) {
				camRot.y += (x - centerX) * 0.01f * mouseSensitivity;
				camRot.x += (y - centerY) * 0.01f * mouseSensitivity;

				camRot.x = glm::clamp(camRot.x, -glm::radians(89.0f), glm::radians(89.0f));
			}

			glutWarpPointer(centerX, centerY);
		}		
	}

	lastMouseLock = lockMouse;
}

//----------------------------------------------------------------------------

void update(void)
{
	// Minecraft controls. WASD to move around, these should be strafes based on camRot and not affect y value.
	// Space should go up, shift should go down

	float speed = 0.1f;
	if (keyStates['w']) {
		camPos.x += sin(camRot.y) * speed;
		camPos.z -= cos(camRot.y) * speed;
	}

	if (keyStates['s']) {
		camPos.x -= sin(camRot.y) * speed;
		camPos.z += cos(camRot.y) * speed;
	}

	if (keyStates['a']) {
		camPos.x -= sin(camRot.y + 3.14159f / 2.0f) * speed;
		camPos.z += cos(camRot.y + 3.14159f / 2.0f) * speed;
	}

	if (keyStates['d']) {
		camPos.x += sin(camRot.y + 3.14159f / 2.0f) * speed;
		camPos.z -= cos(camRot.y + 3.14159f / 2.0f) * speed;
	}

	if (lockMouse) {
		glutSetCursor(GLUT_CURSOR_NONE);
	}
	else {
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}

	if (!noClip) {
		camPos.y = 0.9f;
	}
	else {
		if (keyStates[' ']) {
			camPos.y += speed;
		}

		if (keyStates['f']) {
			camPos.y -= speed;
		}
	}
}

//----------------------------------------------------------------------------

void reshape(int width, int height)
{
	ImGui_ImplGLUT_ReshapeFunc(width, height);
	glViewport(0, 0, width, height);

	wwidth = width;
	wheight = height;

	GLfloat aspect = GLfloat(width) / height;
	Projection = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 2048.0f);

	glUseProgram(ProgramGrass);
	glUniformMatrix4fv(ProjectionGrass, 1, GL_FALSE, glm::value_ptr(Projection));

	glUseProgram(ProgramClouds);
	glUniformMatrix4fv(ProjectionClouds, 1, GL_FALSE, glm::value_ptr(Projection));

	/*for (int i = 0; i < house->numMeshes; i++) {
		glUseProgram(house->meshes[i]->shaderProgram->program);
		glUniformMatrix4fv(house->meshes[i]->shaderProgram->Projection, 1, GL_FALSE, glm::value_ptr(Projection));
	}*/
}

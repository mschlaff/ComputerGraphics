//#####################################################################
// Main
// CS3451 Computer Graphics Starter Code
// Contact: Bo Zhu (bo.zhu@gatech.edu)
//#####################################################################
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <unordered_set>

#include "OpenGLMesh.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLCommon.h"
#include "OpenGLWindow.h"
#include "OpenGLViewer.h"
#include "TinyObjLoader.h"

#ifndef __Main_cpp__
#define __Main_cpp__

#ifdef __APPLE__
#define CLOCKS_PER_SEC 100000
#endif

const float M_PI = 3.1415926535;
#define DegreesToRadians(degrees) (degrees * M_PI / 180)

class MyDriver : public OpenGLViewer
{
	std::vector<OpenGLTriangleMesh*> mesh_object_array;						////mesh objects, every object you put in this array will be rendered.
	OpenGLSegmentMesh* floor = nullptr;
	std::vector<OpenGLSegmentMesh*> coords, trajectory;
	clock_t startTime;
public:
	virtual void Initialize()
	{
		draw_axes = false;
		startTime = clock();
		OpenGLViewer::Initialize();
		opengl_window->camera_distance = 25.f;
		opengl_window->camera_target = Vector3f(0, 3.5, 0);
		opengl_window->Update_Clip_Planes();
	}

	void Create_Background(const OpenGLColor& color1, const OpenGLColor& color2)
	{
		auto bg = Add_Interactive_Object<OpenGLBackground>();
		bg->Set_Color(color1, color2);
		bg->Initialize();
	}

	virtual void Initialize_Data()
	{
		Create_Background(OpenGLColor(0.71f, 0.6f, 0.17f, 1.f), OpenGLColor(0.71f, 0.87f, 0.17f, 1.f));
		OpenGLShaderLibrary::Instance()->Add_Shader_From_File("a3_vert.vert", "a3_frag.frag", "a3_shading");	////bind shader for this assignment

		Create_Angry_Bird_Palace();					////TODO: Comment this line when you start to implement your customized scene
		Create_Angry_Bird_Garden();			////TODO: Uncomment this line when you start to implement your customized scene

	}

	void Create_Angry_Bird_Palace()
	{
		//// draw the three axes
		Add_Coord({ Vector3(0, 0.01, 0), Vector3(5, 0.01, 0) }, OpenGLColor(1, 0, 0, 1));	//// X axis
		Add_Coord({ Vector3(0, 0, 0), Vector3(0, 5, 0) }, OpenGLColor(0, 1, 0, 1));	//// Y axis
		Add_Coord({ Vector3(0, 0.01, 0), Vector3(0, 0.01, 5) }, OpenGLColor(0, 0, 1, 1));	//// Z zxis

		//// draw the ground
		Add_Ground();

		//// Step 1: add the castle by reading the model from "castle.obj" 
		//// The model needs to undergo the following transform operations in sequence: 
		//// (1) rotate *counterclockwisely* around the y-axis by 90 degrees, 
		//// (2) uniformly scale by a factor of 5,
		//// (3) translate upwards by 1.3 units in the y direction.
		//// Your task is to specify the values of the 4x4 transform matrix and send it to the mesh model via Set_Model_Matrix().
		//// You are allowed to use a chain of matrix multiplications to calculate the matrix.

		/* Your implementation starts. You may add/remove/edit any part of the code in the following. */
		auto castle = Add_Obj_Mesh_Object_From_File("castle.obj", OpenGLColor(.6f, .6f, .6f, 1.f));
		{
			// 1. Rotate 90 degrees counterclockwise around the y-axis
			Matrix4f rotation_matrix = Matrix4f::Identity();
			float angle_in_radians = DegreesToRadians(90); // Convert 90 degrees to radians
			rotation_matrix.block<3, 3>(0, 0) = Eigen::AngleAxisf(angle_in_radians, Vector3f::UnitY()).toRotationMatrix();

			// 2. Uniformly scale by a factor of 5
			Matrix4f scale_matrix = Matrix4f::Identity();
			scale_matrix.block<3, 3>(0, 0) *= 5.0f; // Uniform scaling by a factor of 5

			// 3. Translate upwards by 1.3 units in the y direction
			Matrix4f translation_matrix = Matrix4f::Identity();
			translation_matrix.col(3).head(3) << 0, 1.3, 0; // Set translation vector

			// Combine the transformations using matrix multiplication
			Matrix4f combined_matrix = translation_matrix * scale_matrix * rotation_matrix;

			// Apply the transformation to the castle mesh
			castle->Set_Model_Matrix(combined_matrix);
		}
		/* Your implementation ends. */

		//// Step 2: add the axes statue by reading the model from "axes.obj" 
		//// The model needs to undergo the following transform operations in sequence: 
		//// (1) rotate *counterclockwisely* around the y-axis by 90 degrees, 
		//// (2) uniformly scale by a factor of 2,
		//// (3) translate by 6 units in the positive x direction and 1 unit in the y direction.

		/* Your implementation starts. You may add/remove/edit any part of the code in the following. */
		auto axes = Add_Obj_Mesh_Object_From_File("axes.obj", OpenGLColor(.9f, .5f, .0f, 1.f));
		{
			// Create transformation matrices
			// 1. Rotate 90 degrees counterclockwise around the y-axis
			Matrix4f rotation_matrix;
			float angle = 90.0f * M_PI / 180.0f; // Convert degrees to radians
			rotation_matrix << cos(angle), 0, sin(angle), 0,
				0, 1, 0, 0,
				-sin(angle), 0, cos(angle), 0,
				0, 0, 0, 1;

			// 2. Scale uniformly by a factor of 2
			Matrix4f scaling_matrix;
			scaling_matrix << 2, 0, 0, 0,
				0, 2, 0, 0,
				0, 0, 2, 0,
				0, 0, 0, 1;

			// 3. Translate 6 units in the positive x-direction and 1 unit in the y-direction
			Matrix4f translation_matrix;
			translation_matrix << 1, 0, 0, 6,
				0, 1, 0, 1,
				0, 0, 1, 0,
				0, 0, 0, 1;

			// Combine the transformation matrices
			Matrix4f model_matrix = translation_matrix * scaling_matrix * rotation_matrix;

			// Apply the transformation to the model
			axes->Set_Model_Matrix(model_matrix);
		}
		/* Your implementation ends. */

		//// Step 3: add the magic tower by reading the model from "tower.obj" 
		//// The model needs to undergo the following transform operations in sequence: 
		//// (1) rotate *clockwisely* around the y-axis by 45 degrees, 
		//// (2) non-uniformly scale by factors of 2, 6, 2 in the x, y, z directions
		//// (3) translate by 6 units in the negative x direction and 3 units in the y direction.

		/* Your implementation starts. You may add/remove/edit any part of the code in the following. */
		auto tower = Add_Obj_Mesh_Object_From_File("tower.obj", OpenGLColor(.0f, .5f, .5f, 1.f));
		{
			// Create transformation matrices
			// 1. Rotate 45 degrees clockwise around the y-axis
			Matrix4f rotation_matrix;
			float angle = -45.0f * M_PI / 180.0f; // Convert degrees to radians
			rotation_matrix << cos(angle), 0, -sin(angle), 0,
				0, 1, 0, 0,
				sin(angle), 0, cos(angle), 0,
				0, 0, 0, 1;

			// 2. Apply non-uniform scaling (2, 6, 2) in the x, y, z directions
			Matrix4f scaling_matrix;
			scaling_matrix << 2, 0, 0, 0,
				0, 6, 0, 0,
				0, 0, 2, 0,
				0, 0, 0, 1;

			// 3. Translate by 6 units in the negative x direction and 3 units in the y direction
			Matrix4f translation_matrix;
			translation_matrix << 1, 0, 0, -6,
				0, 1, 0, 3,
				0, 0, 1, 0,
				0, 0, 0, 1;

			// Combine the transformation matrices
			Matrix4f model_matrix = translation_matrix * scaling_matrix * rotation_matrix;

			// Apply the transformation to the model
			tower->Set_Model_Matrix(model_matrix);
		}
		/* Your implementation ends. */

		//// Step 4: add 24 trees by reading the model from "tree1.obj" 
		//// The 24 trees need to be distributed evenly and at equal distances along the circumference of the inner circle. 
		//// The circle has its center at the origin and a radius of 8.
		//// Each tree needs to be translated in the positive y axis by 0.5 unit to ensure its base is above the ground.
		//// Calculate the transform matrix for each tree in the following for-loop.

		/* Your implementation starts. You may add/remove/edit any part of the code in the following. */
		int num_trees = 24;
		float inner_circle_radius = 8.0f;
		float y_translation = 0.5f;

		for (int i = 0; i < num_trees; ++i) {
			// Calculate the angle for each tree
			float angle = (2 * M_PI * i) / num_trees;

			// Calculate the position of the tree using polar coordinates
			float x = inner_circle_radius * cos(angle);
			float z = inner_circle_radius * sin(angle);

			// Load the tree model
			auto tree = Add_Obj_Mesh_Object_From_File("tree1.obj", OpenGLColor(0.0f, 0.6f, 0.0f, 1.f));

			// Create the translation matrix
			Matrix4f translation_matrix;
			translation_matrix << 1, 0, 0, x,
				0, 1, 0, y_translation,
				0, 0, 1, z,
				0, 0, 0, 1;

			// Apply the transformation to the tree model
			tree->Set_Model_Matrix(translation_matrix);
		}
		/* Your implementation ends. */

		//// Step 5: add 36 trees by reading the model from "tree2.obj" 
		//// The 36 trees need to be distributed evenly and at equal distances along the circumference of the outer circle. 
		//// The circle has its center at the origin and a radius of 10.
		//// Each tree needs to be translated in the positive y axis by 0.5 unit to ensure its base is above the ground.
		//// Calculate the transform matrix for each tree in the following for-loop.

		/* Your implementation starts. You may add/remove/edit any part of the code in the following. */
		int num_outer_trees = 36;
		float outer_circle_radius = 10.0f;

		for (int i = 0; i < num_outer_trees; ++i) {
			// Calculate the angle for each tree
			float angle = (2 * M_PI * i) / num_outer_trees;

			// Calculate the position of the tree using polar coordinates
			float x = outer_circle_radius * cos(angle);
			float z = outer_circle_radius * sin(angle);

			// Load the tree model
			auto tree = Add_Obj_Mesh_Object_From_File("tree2.obj", OpenGLColor(0.0f, 0.6f, 0.0f, 1.f));

			// Create the translation matrix
			Matrix4f translation_matrix;
			translation_matrix << 1, 0, 0, x,
				0, 1, 0, 0.5f,
				0, 0, 1, z,
				0, 0, 0, 1;

			// Apply the transformation to the tree model
			tree->Set_Model_Matrix(translation_matrix);
		}
		/* Your implementation ends. */

		//// Step 6: add 5 stone steps by using the function `Add_Cube()`
		//// `Add_Cube()` adds a cube mesh to the scene with its center at the origin and each side measuring 1 unit.
		//// Each stone step has its size as 1, 0.1, and 0.5 along x, y, z axes.
		//// The center of the first stone step is in (0, 0, 3), incremented by 1 in positive z direction for the following stone steps.
		//// Calculate the transform matrix for each stone step in the following for-loop.

		/* Your implementation starts. You may add/remove/edit any part of the code in the following. */
		int num_steps = 5;
		float step_width = 1.0f;
		float step_height = 0.1f;
		float step_depth = 0.5f;

		// Define the position of the first step
		Vector3f step_position(0.0f, 0.0f, 3.0f);

		for (int i = 0; i < num_steps; ++i) {
			// Add a cube for each step
			auto step_cube = Add_Cube();

			// Calculate the translation matrix for each step
			Matrix4f translation_matrix;
			translation_matrix << 1, 0, 0, step_position.x(),
				0, 1, 0, step_position.y(),
				0, 0, 1, step_position.z(),
				0, 0, 0, 1;

			// Apply the translation to the step cube
			step_cube->Set_Model_Matrix(translation_matrix);

			// Update the position for the next step
			step_position.z() += 1.0f;
		}
		/* Your implementation ends. */

		//// Step 7: add 6 keyframes of a throwing angry bird following a parabola.
		//// The angry bird is thrown from (-5, 0, 0) with initial velocity (5, 9.8, 0) (unit/sec)
		//// Its angular velocity omega = 150 (deg/sec) *clockwise* 		 
		//// The gravity is g = 9.8 (unit/sec) in negative y direction
		//// The x coordinate of the bird can be calculated as x = x0 + ux * t
		//// The y coordinate of the bird can be calculated as y = uy * t - 0.5 * g * t * t
		//// Your task is to draw keyframes at time t = 0.2, 0.5, 0.8, 1.1, 1.4, 1.7 respectively.
		//// To this end, you need to calculate the transform matrix for each keyframe of the angry bird in the following for-loop.
		//// You can temporarily uncomment the following line to visualize the parabola trajectory as a reference during implementation. 
		//// Comment it out again once you complete the task.
		
		//// Add_Arc_Trajectory();

		/* Your implementation starts. You may add/remove/edit any part of the code in the following. */
		#include <cmath>

		std::vector<float> time = { 0.2f, 0.5f, 0.8f, 1.1f, 1.4f, 1.7f };
		int bird_num = 6;
		for (int i = 0; i < bird_num; i++) {
			float t = time[i];

			// Calculate position along the parabolic trajectory
			float x = -5.0f + 5.0f * t; // x0 + ux * t
			float y = 9.8f * t - 0.5f * 9.8f * t * t; // uy * t - 0.5 * g * t^2
			float z = 0.0f; // Assume no movement in the z direction

			// Calculate orientation angle
			float omega = 150.0f; // Angular velocity in degrees/sec
			float theta = omega * t * M_PI / 180.0f; // Convert angular velocity to radians

			// Create transformation matrix
			Matrix4f rotation_matrix;
			rotation_matrix <<
				cos(theta), -sin(theta), 0, 0,
				sin(theta), cos(theta), 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1;

			Matrix4f translation_matrix;
			translation_matrix <<
				1, 0, 0, x,
				0, 1, 0, y,
				0, 0, 1, z,
				0, 0, 0, 1;

			// Combine rotation and translation matrices
			Matrix4f transformation_matrix = translation_matrix * rotation_matrix;

			// Add the angry bird object and apply the transformation
			auto bird = Add_Obj_Mesh_Object_From_File("bird.obj", OpenGLColor(1.f, 0.2f, 0.f, 1.f));
			bird->Set_Model_Matrix(transformation_matrix);
		}
		/* Your implementation ends. */
	}

	//// Step 8: Create a new garden scene by using the mesh objects we provided, or download your own from online resources. 
	//// Practise matrix transformation by mimicking the way we setup matrices in the `Create_Angry_Bird_Palace()` function.

	void Create_Angry_Bird_Garden()
	{
		/* Your implementation starts. You may add/remove/edit any part of the code in the following. */

		//// draw the three axes, comment them out if you don't need them
		Add_Coord({ Vector3(0, 0.01, 0), Vector3(5, 0.01, 0) }, OpenGLColor(1, 0, 0, 1));	//// X axis
		Add_Coord({ Vector3(0, 0, 0), Vector3(0, 5, 0) }, OpenGLColor(0, 1, 0, 1));	//// Y axis
		Add_Coord({ Vector3(0, 0.01, 0), Vector3(0, 0.01, 5) }, OpenGLColor(0, 0, 1, 1));	//// Z zxis

		//// draw the ground, comment them out if you don't need them
		Add_Ground();

		//// Load Mesh Objects from the "garden" folder

		// black bird
		auto bird = Add_Obj_Mesh_Object_From_File("bird.obj", OpenGLColor(0.2f, 0.2f, 0.2f, 1.0f));
		Vector3f bird_position(5.5f, 1.5f, 0.5f);
		Matrix4f transformation_matrix_bird = Matrix4f::Identity();
		transformation_matrix_bird.block<3, 1>(0, 3) = bird_position;
		bird->Set_Model_Matrix(transformation_matrix_bird);

		// fountain
		auto fountain = Add_Obj_Mesh_Object_From_File("fountain1.obj", OpenGLColor(0.6f, 0.6f, 0.6f, 1.0f));
		Vector3f fountain_position(-5.0f, 0.5f, 3.0f);
		Matrix4f transformation_matrix_fountain1 = Matrix4f::Identity();
		transformation_matrix_fountain1.block<3, 1>(0, 3) = fountain_position;
		fountain->Set_Model_Matrix(transformation_matrix_fountain1);

		// chair1
		auto chair1 = Add_Obj_Mesh_Object_From_File("chair1.obj", OpenGLColor(0.47f, 0.36f, 0.19f, 1.0f));
		Vector3f chair1_position(-3.0f, 0.5f, -6.0f);
		Matrix4f transformation_matrix_chair1 = Matrix4f::Identity();
		transformation_matrix_chair1.block<3, 1>(0, 3) = chair1_position;
		chair1->Set_Model_Matrix(transformation_matrix_chair1);

		// chair2
		auto chair2 = Add_Obj_Mesh_Object_From_File("chair1.obj", OpenGLColor(0.47f, 0.36f, 0.19f, 1.0f));
		Vector3f chair2_position(-1.0f, 0.5f, -6.0f);
		Matrix4f transformation_matrix_chair2 = Matrix4f::Identity();
		transformation_matrix_chair2.block<3, 1>(0, 3) = chair2_position;
		chair2->Set_Model_Matrix(transformation_matrix_chair2);

		// light1
		auto light1 = Add_Obj_Mesh_Object_From_File("light1.obj", OpenGLColor(0.9f, 0.9f, 0.6f, 1.0f));
		Vector3f light1_position(-3.0f, 0.5f, -3.0f);
		Matrix4f transformation_matrix_light1 = Matrix4f::Identity();
		transformation_matrix_light1.block<3, 1>(0, 3) = light1_position;
		light1->Set_Model_Matrix(transformation_matrix_light1);

		// light2
		auto light2 = Add_Obj_Mesh_Object_From_File("light1.obj", OpenGLColor(0.9f, 0.9f, 0.6f, 1.0f));
		Vector3f light2_position(3.0f, 0.5f, -3.0f);
		Matrix4f transformation_matrix_light2 = Matrix4f::Identity();
		transformation_matrix_light2.block<3, 1>(0, 3) = light2_position;
		light2->Set_Model_Matrix(transformation_matrix_light2);

		// light3
		auto light3 = Add_Obj_Mesh_Object_From_File("light1.obj", OpenGLColor(0.9f, 0.9f, 0.6f, 1.0f));
		Vector3f light3_position(3.0f, 0.5f, 2.0f);
		Matrix4f transformation_matrix_light3 = Matrix4f::Identity();
		transformation_matrix_light3.block<3, 1>(0, 3) = light3_position;
		light3->Set_Model_Matrix(transformation_matrix_light3);

		// light4
		auto light4 = Add_Obj_Mesh_Object_From_File("light1.obj", OpenGLColor(0.9f, 0.9f, 0.6f, 1.0f));
		Vector3f light4_position(-3.0f, 0.5f, 2.0f);
		Matrix4f transformation_matrix_light4 = Matrix4f::Identity();
		transformation_matrix_light4.block<3, 1>(0, 3) = light4_position;
		light4->Set_Model_Matrix(transformation_matrix_light4);

		// vase1
		auto vase1 = Add_Obj_Mesh_Object_From_File("vase1.obj", OpenGLColor(0.3f, 0.6f, 0.2f, 1.0f));
		Vector3f vase1_position(-2.0f, 0.5f, -6.0f);
		Matrix4f transformation_matrix_vase1 = Matrix4f::Identity();
		transformation_matrix_vase1.block<3, 1>(0, 3) = vase1_position;
		vase1->Set_Model_Matrix(transformation_matrix_vase1);
		/* Your implementation ends. */
	}

	//////////////////////////////////////////////////////////////
	//// The following functions are auxiliary functions to add mesh objects into the scene
	//////////////////////////////////////////////////////////////

	//// This function adds a mesh object from an obj file
	int Add_Obj_Mesh_Object(std::string obj_file_name)
	{
		auto mesh_obj = Add_Interactive_Object<OpenGLTriangleMesh>();

		Array<std::shared_ptr<TriangleMesh<3> > > meshes;
		Obj::Read_From_Obj_File(obj_file_name, meshes);
		mesh_obj->mesh = *meshes[0];
		std::cout << "load tri_mesh from obj file, #vtx: " << mesh_obj->mesh.Vertices().size() << ", #ele: " << mesh_obj->mesh.Elements().size() << std::endl;

		mesh_object_array.push_back(mesh_obj);
		return (int)mesh_object_array.size() - 1;
	}

	//// This function adds a mesh object from an .obj file and allows the user to specify its model matrix
	OpenGLTriangleMesh* Add_Obj_Mesh_Object_From_File(std::string file_name, OpenGLColor color = OpenGLColor::White())
	{
		int obj_idx = Add_Obj_Mesh_Object(file_name);
		auto obj = mesh_object_array[obj_idx];
		obj->color = color; // set color

		obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("a3_shading"));
		Set_Polygon_Mode(obj, PolygonMode::Fill);
		Set_Shading_Mode(obj, ShadingMode::A2);
		obj->Set_Data_Refreshed();
		obj->Initialize();

		return obj;
	}
	//// This function adds a cube with a specified length and color
	OpenGLTriangleMesh* Add_Cube(float length = 1, OpenGLColor color = OpenGLColor::White()) {
		auto obj = Add_Interactive_Object<OpenGLTriangleMesh>();
		mesh_object_array.push_back(obj);
		obj->color = color; // set color
		// set up vertices and elements
		std::vector<Vector3> vertices{ Vector3(0,0,0),Vector3(1,0,0),Vector3(0,1,0),Vector3(1,1,0), Vector3(0,0,1),Vector3(1,0,1),Vector3(0,1,1),Vector3(1,1,1) };
		std::vector<Vector3i> elements{ Vector3i(4,5,7),Vector3i(4,7,6),
										Vector3i(5,1,7),Vector3i(7,1,3),
										Vector3i(2,3,1),Vector3i(0,2,1),
										Vector3i(6,2,4),Vector3i(2,0,4),
										Vector3i(2,6,3),Vector3i(6,7,3),
										Vector3i(0,1,4),Vector3i(1,5,4) };
		for (auto& v3 : vertices) { v3 -= Vector3(0.5, 0.5, 0.5); v3 *= length; };
		obj->mesh.Vertices() = vertices;
		obj->mesh.Elements() = elements;

		obj->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("a3_shading"));
		Set_Polygon_Mode(obj, PolygonMode::Fill);
		Set_Shading_Mode(obj, ShadingMode::A2);
		obj->Set_Data_Refreshed();
		obj->Initialize();

		return obj;
	}

	//// This function adds an axis with a specified color
	void Add_Coord(std::vector<Vector3> pts, OpenGLColor color)
	{
		auto axis = Add_Interactive_Object<OpenGLSegmentMesh>();
		coords.push_back(axis);
		Set_Polygon_Mode(axis, PolygonMode::Fill);
		Set_Shading_Mode(axis, ShadingMode::None);
		auto line_mesh = &axis->mesh;
		line_mesh->Vertices() = pts;
		line_mesh->Elements() = { Vector2i(0,1) };
		Set_Line_Width(axis, 2.5f);
		Set_Color(axis, color);
		axis->Set_Data_Refreshed();
		axis->Initialize();
	}

	//// This function adds a wireframe ground
	void Add_Ground()
	{
		floor = Add_Interactive_Object<OpenGLSegmentMesh>();
		Set_Polygon_Mode(floor, PolygonMode::Fill);
		Set_Shading_Mode(floor, ShadingMode::None);
		auto line_mesh = &floor->mesh;
		std::vector<Vector3> pts;
		std::vector<Vector2i> eles;
		for (int i = -10; i <= 10; i++) {
			pts.push_back(Vector3(i, 0, -10));
			pts.push_back(Vector3(i, 0, 10));
			pts.push_back(Vector3(-10, 0, i));
			pts.push_back(Vector3(10, 0, i));
		}
		for (int i = 0; i < pts.size(); i += 2) {
			eles.push_back(Vector2i(i, i + 1));
		}
		line_mesh->Vertices() = pts;
		line_mesh->Elements() = eles;
		Set_Line_Width(floor, 3.f);
		Set_Color(floor, OpenGLColor(0, 0, 0, 1));
		floor->Set_Data_Refreshed();
		floor->Initialize();
	}

	//// This function adds a parabola trajectory
	void Add_Arc_Trajectory()
	{
		float ux = 5, uy = 9.8, g = 9.8;
		std::vector<Vector3> pts;
		std::vector<Vector2i> eles;
		for (float t = 0; t <= 2; t += 0.02) {
			float x = ux * t - 5;
			float y = uy * t - 0.5 * g * t * t;
			pts.push_back(Vector3(x, y, 0));
		}
		for (int i = 0; i < pts.size() - 1; i++) {
			eles.push_back(Vector2i(i, i + 1));
		}
		auto traj = Add_Interactive_Object<OpenGLSegmentMesh>();
		trajectory.push_back(traj);
		Set_Polygon_Mode(traj, PolygonMode::Fill);
		Set_Shading_Mode(traj, ShadingMode::None);
		auto line_mesh = &traj->mesh;
		line_mesh->Vertices() = pts;
		line_mesh->Elements() = eles;
		Set_Line_Width(traj, 2.f);
		Set_Color(traj, OpenGLColor(0, 0, 1, 1));
		traj->Set_Data_Refreshed();
		traj->Initialize();
	}

	void Init_Bird_Vertices(OpenGLTriangleMesh* obj)
	{
		std::vector<Vector3>& vertices = obj->mesh.Vertices();
		for (auto& v : vertices) {
			v += Vector3(-8.35, 0, 0);
			auto x = v.x(); v.x() = v.z(); v.z() = -x;
			v /= 10.f;
		}
	}

	//// Go to next frame 
	virtual void Toggle_Next_Frame()
	{
		for (auto& mesh_obj : mesh_object_array) {
			mesh_obj->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);
		}
		OpenGLViewer::Toggle_Next_Frame();
	}

	virtual void Run()
	{
		OpenGLViewer::Run();
	}
};

int main(int argc, char* argv[])
{
	MyDriver driver;
	driver.Initialize();
	driver.Run();
}

#endif

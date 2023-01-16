#include "stdafx.h"
#include "wood_test.h" // test

int main(int argc, char **argv)
{

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GoogleTest
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// wood_test::run_all_tests();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Display
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode = 2;
	opengl_globals::shaders_folder = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\src\\viewer\\shaders\\";
	opengl_render::render(wood_test::type_library_name_ss_e_ip_2);

	return 0;
}

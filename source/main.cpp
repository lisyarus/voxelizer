#include <voxelizer/scene.hpp>
#include <voxelizer/obj.hpp>
#include <voxelizer/bbox.hpp>
#include <voxelizer/sampler.hpp>
#include <voxelizer/flood_fill.hpp>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <charconv>
#include <cstring>
#include <iomanip>

namespace
{

	template <typename T>
	T try_read(char const * str)
	{
		T value;
		auto result = std::from_chars(str, str + std::strlen(str), value);
		auto ec = std::make_error_code(result.ec);
		if (ec)
			throw std::system_error(ec);
		return value;
	}

	voxelizer::scene test_scene()
	{
		voxelizer::scene s;
		s.objects.emplace_back();
		s.objects.back().material.color = {0.f, 1.f, 0.f, 1.f};

		s.objects.back().mesh.vertices.push_back({0.f, 0.f, 0.f});
		s.objects.back().mesh.vertices.push_back({1.f, 0.f, 0.f});
		s.objects.back().mesh.vertices.push_back({0.f, 1.f, 0.f});
		s.objects.back().mesh.vertices.push_back({0.f, 0.f, 1.f});

		s.objects.back().mesh.triangles.push_back({0, 2, 1});
		s.objects.back().mesh.triangles.push_back({0, 2, 3});
		s.objects.back().mesh.triangles.push_back({0, 1, 3});
		s.objects.back().mesh.triangles.push_back({1, 2, 3});

		return s;
	}

}

int main(int argc, char ** argv) try {
	if (argc != 7) {
		std::cout << "Usage: voxelizer <input-obj> <x-dim> <y-dim> <z-dim> <nsamples> <out-file>\n";
		return EXIT_FAILURE;
	}

	std::string input_filename(argv[1]);
	glm::ivec3 dimensions;
	dimensions.x = try_read<int>(argv[2]);
	dimensions.y = try_read<int>(argv[3]);
	dimensions.z = try_read<int>(argv[4]);
	int nsamples = try_read<int>(argv[5]);
	std::string output_filename(argv[6]);

	voxelizer::scene scene;
	{
		std::cout << "Reading input file \"" << input_filename << "\" ..." << std::endl;

		scene = voxelizer::parse_obj(input_filename);

		int total_triangles = 0;
		for (auto const & o : scene.objects)
			total_triangles += o.mesh.triangles.size();

		std::cout << "Read scene: " << scene.objects.size() << " objects, " << total_triangles << " triangles" << std::endl;
	}

	voxelizer::box scene_bbox = voxelizer::bbox(scene);

	std::cout << "Scene bbox: [" << scene_bbox.min.x << ".." << scene_bbox.max.x << "]x[" << scene_bbox.min.y << ".." << scene_bbox.max.y << "]x[" << scene_bbox.min.z << ".." << scene_bbox.max.z << "]" << std::endl;

	auto progress_callback = [](float work_done, float time_left){
		std::cout << std::fixed << std::right << std::setw(7) << std::setprecision(3) << (work_done * 100.f) << "%, "
			<< time_left << "s left" << std::endl;
	};

	std::cout << "Sampling to " << dimensions.x << "x" << dimensions.y << "x" << dimensions.z << " texture using " << nsamples << " samples per voxel" << std::endl;
	voxelizer::texture_3d texture(dimensions);
	float sample_duration = voxelizer::sample(scene, texture, nsamples, scene_bbox, progress_callback);
	std::cout << "Took " << sample_duration << "s" << std::endl;

	std::cout << "Flood-filling empty pixels" << std::endl;
	voxelizer::flood_fill(texture, progress_callback);

	{
		std::cout << "Writing texture to \"" << output_filename << "\", 4 channels, 8 bit per channel (0xAABBGGRR)" << std::endl;
		std::ofstream out(output_filename);
		out.write(reinterpret_cast<char const *>(texture.data()), 4 * dimensions.x * dimensions.y * dimensions.z);
	}

	std::cout << "Finished!" << std::endl;
}
catch (std::exception const &e) {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}

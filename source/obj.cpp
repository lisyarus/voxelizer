#include <voxelizer/obj.hpp>

#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace voxelizer
{

	static std::unordered_map<std::string, material> parse_mtl(std::filesystem::path const & path)
	{
		std::ifstream input(path);

		std::unordered_map<std::string, material> result;
		std::string current;

		for (std::string line; std::getline(input, line);)
		{
			std::istringstream line_stream(line);

			std::string token;
			line_stream >> token;

			if (token == "newmtl")
			{
				line_stream >> current;
				result[current].color = {1.f, 1.f, 1.f, 1.f};
				continue;
			}

			if (token == "Kd")
			{
				line_stream >> result[current].color.r >> result[current].color.g >> result[current].color.b;
				continue;
			}

			if (token == "d")
			{
				line_stream >> result[current].color.a;
				continue;
			}
		}

		return result;
	}

	scene parse_obj(std::filesystem::path const & path)
	{
		std::ifstream input(path);

		scene result;

		auto add_object = [&]{
			result.objects.emplace_back();
			result.objects.back().material.color = {1.f, 1.f, 1.f, 1.f};
		};

		int index_shift = 0;

		std::unordered_map<std::string, material> material_library;

		for (std::string line; std::getline(input, line);)
		{
			std::istringstream line_stream(line);

			std::string token;
			line_stream >> token;

			if (token == "#")
				continue;

			if (token == "o")
			{
				if (!result.objects.empty())
					index_shift += result.objects.back().mesh.vertices.size();

				add_object();
				continue;
			}

			if (token == "usemtl")
			{
				line_stream >> token;
				result.objects.back().material = material_library.at(token);
				continue;
			}

			if (token == "v")
			{
				glm::vec3 v;
				line_stream >> v.x >> v.y >> v.z;
				if (result.objects.empty()) add_object();
				result.objects.back().mesh.vertices.push_back(v);
				continue;
			}

			if (token == "f")
			{
				std::uint32_t i0, i1, i2;
				line_stream >> i0 >> i1 >> i2;
				--i0;
				--i1;
				--i2;
				i0 -= index_shift;
				i1 -= index_shift;
				i2 -= index_shift;
				if (result.objects.empty()) add_object();
				result.objects.back().mesh.triangles.push_back({i0, i1, i2});
				continue;
			}

			if (token == "mtllib")
			{
				std::string filename;
				line_stream >> filename;
				material_library = parse_mtl(path.parent_path() / filename);
				continue;
			}
		}

		return result;
	}

}

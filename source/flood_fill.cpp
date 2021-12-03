#include <voxelizer/flood_fill.hpp>
#include <voxelizer/color.hpp>

#include <glm/vec4.hpp>

#include <chrono>
#include <deque>

namespace voxelizer
{

	void flood_fill(texture_3d & texture, progress_callback callback)
	{
		static constexpr std::uint32_t alpha_mask = 0xff000000u;

		std::deque<glm::ivec3> queue;

		glm::ivec3 dims = texture.dimensions();

		texture_3d visited(dims);
		visited.fill(0);

		int work_total = 0;

		for (int z = 0; z < dims.z; ++z)
		{
			for (int y = 0; y < dims.y; ++y)
			{
				for (int x = 0; x < dims.x; ++x)
				{
					if ((texture.at(x, y, z) & alpha_mask) != 0)
					{
						visited.at(x, y, z) = 1;
						continue;
					}
					++work_total;

					if (false
						|| (x > 0 && ((texture.at(x - 1, y, z) & alpha_mask) != 0))
						|| (y > 0 && ((texture.at(x, y - 1, z) & alpha_mask) != 0))
						|| (z > 0 && ((texture.at(x, y, z - 1) & alpha_mask) != 0))
						|| (x + 1 < dims.x && ((texture.at(x + 1, y, z) & alpha_mask) != 0))
						|| (y + 1 < dims.y && ((texture.at(x, y + 1, z) & alpha_mask) != 0))
						|| (z + 1 < dims.z && ((texture.at(x, y, z + 1) & alpha_mask) != 0))
						)
						queue.push_back(glm::ivec3{x, y, z});
				}
			}
		}

		using clock = std::chrono::high_resolution_clock;

		auto start = clock::now();
		auto last_report = start;

		int work_done = 0;

		while (!queue.empty())
		{
			auto p = queue.front();
			queue.pop_front();

			int x = p.x;
			int y = p.y;
			int z = p.z;

			if (visited.at(x, y, z) != 0) continue;
			visited.at(x, y, z) = 1;

			glm::vec4 color = glm::vec4(0.0);
			float count = 0.f;

			if (x > 0 && visited.at(x - 1, y, z) == 1) { color += to_vec4(texture.at(x - 1, y, z)); count += 1.f; }
			if (y > 0 && visited.at(x, y - 1, z) == 1) { color += to_vec4(texture.at(x, y - 1, z)); count += 1.f; }
			if (z > 0 && visited.at(x, y, z - 1) == 1) { color += to_vec4(texture.at(x, y, z - 1)); count += 1.f; }
			if (x + 1 < dims.x && visited.at(x + 1, y, z) == 1) { color += to_vec4(texture.at(x + 1, y, z)); count += 1.f; }
			if (y + 1 < dims.y && visited.at(x, y + 1, z) == 1) { color += to_vec4(texture.at(x, y + 1, z)); count += 1.f; }
			if (z + 1 < dims.z && visited.at(x, y, z + 1) == 1) { color += to_vec4(texture.at(x, y, z + 1)); count += 1.f; }

			color /= count;
			color.a = 0.f;

			texture.at(x, y, z) = to_uint(color);

			if (x > 0 && (visited.at(x - 1, y, z) == 0)) queue.push_back({x - 1, y, z});
			if (y > 0 && (visited.at(x, y - 1, z) == 0)) queue.push_back({x, y - 1, z});
			if (z > 0 && (visited.at(x, y, z - 1) == 0)) queue.push_back({x, y, z - 1});
			if (x + 1 < dims.x && (visited.at(x + 1, y, z) == 0)) queue.push_back({x + 1, y, z});
			if (y + 1 < dims.y && (visited.at(x, y + 1, z) == 0)) queue.push_back({x, y + 1, z});
			if (z + 1 < dims.z && (visited.at(x, y, z + 1) == 0)) queue.push_back({x, y, z + 1});

			++work_done;

			auto now = clock::now();

			if (now - last_report >= std::chrono::seconds{1})
			{
				last_report = now;

				float done = work_done * 1.f / work_total;

				float time_spent = std::chrono::duration_cast<std::chrono::duration<float>>(now - start).count();

				float speed = done / time_spent;

				float left = 1.f - done;

				float time_left = left / speed;

				if (callback)
					callback(done, time_left);
			}
		}
	}

}

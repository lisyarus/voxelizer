#include <voxelizer/sampler.hpp>
#include <voxelizer/sequence.hpp>
#include <voxelizer/index.hpp>
#include <voxelizer/color.hpp>

#include <chrono>
#include <thread>
#include <atomic>

namespace voxelizer
{

	static bool inside(glm::vec3 const & p, index const & i)
	{
		int intersections = 0;

		i.intersects_ray(p, {1.f, 0.f, 0.f}, [&](auto){ ++intersections; });

		return (intersections % 2) == 1;
	}

	float sample(scene const & input_scene, texture_3d & output_texture, int nsamples,
		box bbox, progress_callback callback)
	{
		std::vector<index> indices;
		for (auto const & o : input_scene.objects)
			indices.emplace_back(o.mesh);

		auto dim = output_texture.dimensions();

		using clock = std::chrono::high_resolution_clock;

		auto start = clock::now();
		auto last_report_time = start;

		int total_work = dim.x * dim.y * dim.z;
		std::atomic<int> work_done(0);

		int thread_count = std::thread::hardware_concurrency();
		if (thread_count == 0) thread_count = 1;

		int work_per_thread = total_work / thread_count;

		std::vector<std::thread> threads;

		for (int th = 0; th < thread_count; ++th)
		{
			threads.emplace_back([&, th]{
				int work_start = th * work_per_thread;
				int work_end = (th + 1 == thread_count) ? total_work : (th + 1) * work_per_thread;

				for (int work_id = work_start; work_id < work_end; ++work_id) {

					int x = work_id % dim.x;
					int y = (work_id / dim.x) % dim.y;
					int z = work_id / dim.x / dim.y;

					glm::vec4 color(0.f);

					sequence seq({0.5f, 0.5f, 0.5f});

					for (int s = 0; s < nsamples; ++s) {
						glm::vec3 p = (glm::vec3(x, y, z) + seq()) / glm::vec3(dim);
						p = p * (bbox.max - bbox.min) + bbox.min;

						for (int o = 0; o < input_scene.objects.size(); ++o)
						{
							if (inside(p, indices[o]))
								color += premult(input_scene.objects[o].material.color) / float(nsamples);
						}
					}

					color = unpremult(color);

					output_texture.at(x, y, z) = to_uint(color);

					++work_done;
				}
			});
		}

		std::this_thread::sleep_for(std::chrono::milliseconds{100});

		while (true)
		{
			if (work_done >= total_work) break;

			auto now = clock::now();

			float done = work_done * 1.f / (dim.x * dim.y * dim.z);

			float time_spent = std::chrono::duration_cast<std::chrono::duration<float>>(now - start).count();

			float speed = done / time_spent;

			float left = 1.f - done;

			float time_left = left / speed;

			if (callback)
				callback(done, time_left);

			std::this_thread::sleep_for(std::chrono::seconds{1});
		}

		for (auto & th : threads)
			th.join();

		return std::chrono::duration_cast<std::chrono::duration<float>>(clock::now() - start).count();
	}

}

#pragma once

#include <voxelizer/scene.hpp>
#include <voxelizer/bbox.hpp>

namespace voxelizer
{

	bool intersects(glm::vec3 const & origin, glm::vec3 const & direction, box const & b);
	bool intersects(glm::vec3 const & origin, glm::vec3 const & direction, glm::vec3 const & p0, glm::vec3 const & p1, glm::vec3 const & p2);

	struct index
	{
		index(mesh const & s);

		template <typename TriangleVisitor>
		void intersects_ray(glm::vec3 origin, glm::vec3 direction, TriangleVisitor && visitor) const;

	private:
		using node_id = std::uint32_t;

		static constexpr node_id null = -1;

		mesh const & mesh_;

		struct triangle_data
		{
			std::uint32_t id;
			box bbox;
		};

		struct node
		{
			box bbox;
			std::vector<triangle_data> triangles;
			std::uint32_t left = null;
			std::uint32_t right = null;
		};

		std::vector<node> nodes_;

		node_id create_node(box bbox)
		{
			nodes_.emplace_back().bbox = bbox;
			return nodes_.size() - 1;
		}

		node_id build_tree(std::vector<triangle_data> triangles, box bbox, int type);

		template <typename TriangleVisitor>
		void intersects_ray_impl(glm::vec3 origin, glm::vec3 direction, TriangleVisitor && visitor, std::uint32_t node, int type) const;
	};

	template <typename TriangleVisitor>
	void index::intersects_ray(glm::vec3 origin, glm::vec3 direction, TriangleVisitor && visitor) const
	{
		return intersects_ray_impl(origin, direction, std::forward<TriangleVisitor>(visitor), 0, 0);
	}

	template <typename TriangleVisitor>
	void index::intersects_ray_impl(glm::vec3 origin, glm::vec3 direction, TriangleVisitor && visitor, std::uint32_t node, int type) const
	{
		auto const & n = nodes_[node];
		if (!intersects(origin, direction, n.bbox)) return;

		for (auto const & t : nodes_[node].triangles)
		{
			auto tri = mesh_.triangles[t.id];
			if (intersects(origin, direction, mesh_.vertices[tri[0]], mesh_.vertices[tri[1]], mesh_.vertices[tri[2]]))
				visitor(t.id);
		}

		if (nodes_[node].left != null)
			intersects_ray_impl(origin, direction, visitor, nodes_[node].left, (type + 1) % 3);
		if (nodes_[node].right != null)
			intersects_ray_impl(origin, direction, visitor, nodes_[node].right, (type + 1) % 3);
	}

}

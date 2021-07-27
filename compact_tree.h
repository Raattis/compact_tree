#pragma once

#include <vector>
#include <assert.h>

typedef uint8_t depth_t; // Supports 255 deep trees.
//typedef uint16_t depth_t; // Supports 16k deep trees.
//typedef uint32_t depth_t;

template<typename item_t>
struct compact_tree
{
	enum { max_depth = ((depth_t)(~0) - 1) };

	std::vector<depth_t> depths;
	std::vector<item_t> items;

	inline size_t size() const { return depths.size(); }

	compact_tree(size_t reserveSize = 0)
	{
		depths.reserve(reserveSize);
		items.reserve(reserveSize);
	}
	
	item_t& get_item(size_t index)
	{
		return items[index];
	}

	depth_t& get_depth(size_t index)
	{
		return depths[index];
	}

	size_t create_root_node(item_t&& item)
	{
		size_t new_index = size();
		items.emplace_back(item);
		depths.push_back(0);
		return new_index;
	}

	// Result includes the root of the subtree (node at the index) and is hence always at least 1
	size_t count_subtree_size(size_t parent_index)
	{
		assert(parent_index < size());

		size_t result = parent_index + 1;
		depth_t parent_depth = depths[parent_index];
		while (result < size() && depths[result] > parent_depth)
		{
			++result;
		}
		return result - parent_index;
	}

	size_t create_child(size_t parent_index, item_t&& item)
	{
		assert(parent_index < size() && "Given parent index is out of bounds.");

		size_t new_index = parent_index + count_subtree_size(parent_index); // Add as last child
		depth_t new_depth = depths[parent_index] + 1;
		assert(new_depth < max_depth && "Depth overflow or underflow detected.");

		items.emplace(items.begin() + new_index, item);
		depths.insert(depths.begin() + new_index, new_depth);
		return new_index;
	}

	size_t make_child_of(size_t child, size_t parent)
	{
		assert(child != parent);
		assert(!is_child_of(parent, child) && "Child parent loops are not good.");

		size_t dest = parent + count_subtree_size(parent); // Make last child
		size_t count = count_subtree_size(child);


		depth_t depth_diff = depths[parent] + 1 - depths[child]; // Unsigned integer wrapping here is intentional!

		// Do the move
		size_t source = child;
		if (source != dest && source + count != dest)
		{
			size_t low = source < dest ? source : dest;
			size_t pivot = source < dest ? source + count : source;
			size_t high = source < dest ? dest : source + count;
			std::rotate(depths.begin() + low, depths.begin() + pivot, depths.begin() + high);
			std::rotate(items.begin() + low, items.begin() + pivot, items.begin() + high);
		}

		if (source < dest)
		{
			assert(source + count <= dest);
			dest -= count;
		}

		// Correct the depth values
		for (size_t i = 0; i < count; i++)
		{
			depths[dest + i] += depth_diff; // Unsigned integer wrapping here is intentional!
			assert(depths[dest + i] < max_depth && "Depth overflow or underflow detected.");
		}
		return dest;
	}

	void erase(size_t child)
	{
		size_t count = count_subtree_size(child);
		depths.erase(depths.begin() + child, depths.begin() + child + count);
		items.erase(items.begin() + child, items.begin() + child + count);
	}

	inline bool is_child_of(size_t child, size_t parent)
	{
		if (child <= parent || depths[child] <= depths[parent])
			return false;

		if (parent + 1 == child)
		{
			assert(depths[parent] + 1 == depths[child]);
			return true;
		}

		for (size_t i = parent + 1; i <= child - 1; i++)
		{
			if (depths[parent] >= depths[i])
				return false;
		}

		return true;
	}

	size_t find_item(const item_t& item_t, size_t starting_from = 0)
	{
		// Linear search
		for (size_t i = starting_from, end = size(); i < end; ++i)
		{
			if (items[i] == item_t)
				return i;
		}
		return ~size_t(0);
	}

	size_t find_parent(size_t child_index)
	{
		// Linear fetch
		depth_t child_depth = depths[child_index];
		size_t parent_index = child_index;
		while (parent_index-- > 0)
		{
			if (depths[parent_index] < child_depth)
				return parent_index;
		}

		return ~size_t(0);
	}

	size_t find_in_descendants(const item_t& item_t, size_t parent)
	{
		// Linear search below parents depth starting from parent index
		depth_t parent_depth = depths[parent];
		for (size_t i = parent + 1, end = size(); i < end && parent_depth < depths[i]; ++i)
		{
			if (items[i] == item_t)
				return i;
		}
		return ~size_t(0);
	}
};

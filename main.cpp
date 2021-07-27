#include <string>
#include <stdio.h>

#include "compact_tree.h"

struct foo
{
	foo() {}
	foo(std::string str) : str(str) {}

	std::string str;
	char dummy[64] = {};

	bool operator==(const foo& o) const { return str == o.str; }
};

void print_tree(compact_tree<foo>& t)
{
	for (size_t i = 0; i < t.size(); i++)
	{
		if (t.get_depth(i) > 0)
		{
			auto spacer = "| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | ";
			printf(" %.*s+-%s\n", (t.get_depth(i) - 1) * 2, spacer, t.get_item(i).str.c_str());
		}
		else
		{
			if (i != 0)
				printf("\n");

			printf(">%s\n", t.get_item(i).str.c_str());
		}
	}

	size_t totalSize32 = t.size() * (sizeof(t.items[0]) + sizeof(uint32_t)) + sizeof(t);
	size_t totalSize16 = t.size() * (sizeof(t.items[0]) + sizeof(uint16_t)) + sizeof(t);
	size_t totalSize8 = t.size() * (sizeof(t.items[0]) + sizeof(uint8_t)) + sizeof(t);
	size_t overhead32 = totalSize32 - t.size() * sizeof(t.items[0]);
	size_t overhead16 = totalSize16 - t.size() * sizeof(t.items[0]);
	size_t overhead8 = totalSize8 - t.size() * sizeof(t.items[0]);

	printf("\nWith maximum tree depth set to 2^32\n");
	printf("Total memory used by %zu items (%zu bytes each) is %zu bytes.\n", t.size(), sizeof(t.items[0]), totalSize32);
	printf("The overhead is %zu bytes or %.0f %%.\n", overhead32, double(overhead32) / double(totalSize32) * 100);

	printf("\nWith maximum tree depth set to 2^16\n");
	printf("Total memory used by %zu items (%zu bytes each) is %zu bytes.\n", t.size(), sizeof(t.items[0]), totalSize16);
	printf("The overhead is %zu bytes or %.0f %%.\n", overhead16, double(overhead16) / double(totalSize16) * 100);

	printf("\nWith maximum tree depth set to 2^8\n");
	printf("Total memory used by %zu items (%zu bytes each) is %zu bytes.\n", t.size(), sizeof(t.items[0]), totalSize8);
	printf("The overhead is %zu bytes or %.0f %%.\n", overhead8, double(overhead8) / double(totalSize8) * 100);
}

static void populate_tree(compact_tree<foo>& t, size_t parentIndex = 0, size_t childCount = 9, const size_t targetNodeCount = 200)
{
	size_t index = t.create_child(parentIndex, { std::to_string(t.size()) });
	for (size_t i = 0; i + 1 < childCount; ++i)
	{
		if (t.size() >= targetNodeCount)
			return;
		populate_tree(t, index, childCount - 1 - i, targetNodeCount);
	}
}

int main()
{
	compact_tree<foo> t;
	t.create_root_node({ "root" });
	populate_tree(t);

	print_tree(t);

	size_t new_parent = t.find_parent(t.find_parent(t.find_item({ "7" })));
	size_t child_to_move = t.find_item({ "7" });
	t.make_child_of(child_to_move, new_parent);

	print_tree(t);

	size_t child_to_erase = t.find_item({ "67" });
	printf("\nErasing \"%s\" and its %zu children\n", t.get_item(child_to_erase).str.c_str(), t.count_subtree_size(child_to_erase) - 1);
	size_t old_tree_size = t.size();
	t.erase(child_to_erase);
	printf("Tree size was %zu, now it's %zu.\n\n", old_tree_size, t.size());

	child_to_erase = t.find_in_descendants({ "3" }, t.find_item({ "1" }));
	printf("\nErasing \"%s\" and its %zu children\n", t.get_item(child_to_erase).str.c_str(), t.count_subtree_size(child_to_erase) - 1);
	old_tree_size = t.size();
	t.erase(child_to_erase);
	printf("Tree size was %zu, now it's %zu.\n\n", old_tree_size, t.size());

	print_tree(t);
}

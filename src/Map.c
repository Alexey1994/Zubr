#include "Map.h"
#include "extends.h"


static int str_map_node_comparision(String *key, MapNode *map_node)
{
    return str_comparision(key, map_node->key);
}


static int map_node_comparision(MapNode *map_node1, MapNode *map_node2)
{
    return str_comparision(map_node1->key, map_node2->key);
}


static void map_node_free(MapNode *map_node)
{
    free(map_node);
}


Map* map_init()
{
    Map *map=new(Map);

    map->tree=tree_init(str_map_node_comparision, map_node_comparision, map_node_free);

    return map;
}


void map_free(Map *map)
{
    tree_free(map->tree);
    free(map);
}


void map_add(Map *map, String *key, Variable *value)
{
    MapNode *map_node=new(MapNode);

    map_node->key=key;
    map_node->value=value;

    tree_add(map->tree, map_node);
}


Variable* map_get(Map *map, String *key)
{
    MapNode *map_node=tree_find(map->tree, key);

    if(!map_node)
        return 0;

    return map_node->value;
}

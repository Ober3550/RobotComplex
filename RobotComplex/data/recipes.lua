recipes = 
{
	iron_ingot = {
		components = {{ "iron_ingot"  ,1 }, { "iron_ore",  -1 }, { "coal_ore",-1 }},
		width = 1,
		craft_time = 5,
		animation = "furnace_animation",
	},
	copper_ingot = {
		components = {{ "copper_ingot"  ,1 }, { "copper_ore",  -1 }, { "coal_ore",-1 }},
		width = 1,
		craft_time = 5,
		animation = "furnace_animation",
	},
	steel_ingot = {
		components = 
		{{ "iron_ore",  -1 },	{ "coal_ore",  -1 }, { "iron_ore", -1 }, 
		{ "steel_ingot",1  },	{ "iron_ore"  ,-1 }, { "",  0 }},
		width = 3,
		craft_time = 2,
		animation = "",
	}
}
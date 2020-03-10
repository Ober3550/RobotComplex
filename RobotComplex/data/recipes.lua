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
		{ "steel_ingot",1  },	{ "iron_ore"  ,-1 }, { "steel_ingot",1}},
		width = 3,
		craft_time = 2,
		animation = "",
	},
	clay_brick = {
		components = {{ "clay_brick"  ,1 }, { "clay",-1 }, { "coal_ore",-1 }},
		width = 1,
		craft_time = 5,
		animation = "furnace_animation",
	},
	iron_gear = {
		components ={{ ""},				 { "iron_ingot",-1 }, { ""},
					{ "iron_ingot",-1 }, { "iron_gear",1 },	  { "iron_ingot",-1 },
					{ ""},				 { "iron_ingot",-1 }, { ""}},
		width = 3,
		craft_time = 1,
		animation = ""
	},
	copper_wire = {
		components ={{ ""},					{ "copper_ingot",-1 },	{ ""},
					{ "copper_ingot",-1 },	{ "copper_wire",4 },	{ "copper_ingot",-1 },
					{ ""},					{ "copper_ingot",-1 },	{ ""}},
		width = 3,
		craft_time = 1,
		animation = ""
	},
	iron_plate = {
		components = {{ "" },			 { "iron_plate",1 },
					{ "iron_ingot",-1 }, { "iron_ingot",-1 },
					{ "iron_ingot",-1 }, { "iron_ingot",-1 }},
		width = 2, 
		craft_time = 1,
		animation = ""
	},
	copper_plate = {
		components = {{ "" },				 { "copper_plate",1 },
					  { "copper_ingot",-1 }, { "copper_ingot",-1 },
					  { "copper_ingot",-1 }, { "copper_ingot",-1 }}, 
		width = 2, 
		craft_time = 1, 
		animation = ""
	},
	robot = {
		components ={{ "steel_ingot",-1},	{ "robot",1 },		{ "steel_ingot",-1},
					{ "steel_ingot",-1 },	{ "iron_gear",-1 },	{ "steel_ingot",-1 }},
		width = 3,
		craft_time = 1,
		animation = ""
	},
}
recipes = 
{
	{
		components = {{ "iron_ingot"  ,1 }, { "iron_ore",  -1 }, { "coal_ore",-1 }},
		width = 1,
		craft_time = 5,
		animation = "furnace_animation",
	},
	{
		components = {{ "copper_ingot"  ,1 }, { "copper_ore",  -1 }, { "coal_ore",-1 }},
		width = 1,
		craft_time = 5,
		animation = "furnace_animation",
	},
    {
		components ={{ ""},					{ "copper_ingot",-1 },	{ ""},
					{ "copper_ingot",-1 },	{ "copper_wire",4 },	{ "copper_ingot",-1 },
					{ ""},					{ "copper_ingot",-1 },	{ ""}},
		width = 3,
		craft_time = 1,
		animation = "",
	},
    {
        components = {{"copper_ingot",-1},{"redirector",1},
                      {"copper_ingot",-1},{"copper_ingot",-1}},
        width = 2,
        craft_time = 5,
        animation = "",
    },
    {
        components = {{"iron_ingot",-1},{"shover",1},
                      {"iron_ingot",-1},{"iron_ingot",-1}},
        width = 2,
        craft_time = 5,
        animation = "",
    },
    {
        components = {{"copper_ingot",-1},{"plusone",1},
                      {"iron_ingot",-1},{"iron_ingot",-1}},
        width = 2,
        craft_time = 5,
        animation = "",
    },
    {
        components = {{"iron_ingot",-1},{"iron_rod",1},
                      {"iron_ingot",-1},{"iron_ingot",-1},
                      {"iron_ingot",-1},{"iron_rod",1}},
        width = 2,
        craft_time = 5,
        animation = "",
    },
    {
        components = {{"copper_wire",-1},{"wire",3},
                      {"copper_wire",-1},{"copper_wire",-1}},
        width = 2,
        craft_time = 5,
        animation = "",
    },
    {
        components = {{"iron_ingot",-1},{"inverter",1},
                      {"copper_wire",-1},{"copper_wire",-1}},
        width = 2,
        craft_time = 5,
        animation = "",
    },
    {
        components = {{"copper_ingot",-1},{"booster",1},
                      {"copper_wire",-1},{"copper_wire",-1}},
        width = 2,
        craft_time = 5,
        animation = "",
    },
    {
        components = {{"copper_wire",-1},{"toggle",1},
                      {"inverter",-1},{"inverter",-1}},
        width = 2,
        craft_time = 5,
        animation = "",
    },
	{
		components = 
		{{ "iron_ore",  -1 },	{ "coal_ore",  -1 }, { "iron_ore", -1 }, 
		{ "steel_ingot",1  },	{ "iron_ore"  ,-1 }, { "steel_ingot",1}},
		width = 3,
		craft_time = 2,
		animation = "",
	},
	{
		components ={{ ""},				 { "iron_ingot",-1 }, { ""},
					{ "iron_ingot",-1 }, { "iron_gear",1 },	  { "iron_ingot",-1 },
					{ ""},				 { "iron_ingot",-1 }, { ""}},
		width = 3,
		craft_time = 5,
		animation = "",
	},
	{
		components = {{ "" },			 { "iron_plate",1 },
					{ "iron_ingot",-1 }, { "iron_ingot",-1 },
					{ "iron_ingot",-1 }, { "iron_ingot",-1 }},
		width = 2, 
		craft_time = 5,
		animation = "",
	},
	{
		components = {{ "" },				 { "copper_plate",1 },
					  { "copper_ingot",-1 }, { "copper_ingot",-1 },
					  { "copper_ingot",-1 }, { "copper_ingot",-1 }}, 
		width = 2, 
		craft_time = 5, 
		animation = "",
	},
    {
		components = {{ "" },			 { "steel_plate",1 },
					{ "steel_ingot",-1 }, { "steel_ingot",-1 },
					{ "steel_ingot",-1 }, { "steel_ingot",-1 }},
		width = 2, 
		craft_time = 5,
		animation = "",
	},
    {
		components = 
		{{ "" },	        { "iron_plate",  -1 },  { "motor", 3},
        {"iron_rod", -1 },	{ "copper_wire", -1},   { "copper_plate", -1 },
		{ "" },	            { "iron_plate",  -1 },  { ""}},
		width = 3,
		craft_time = 5,
		animation = "",
	},
	robot = {
		components ={{ "steel_plate",-1},	{ "robot",1 },		{ "steel_plate",-1},
					{ "steel_plate",-1 },	{ "motor",-1 },	    { "steel_plate",-1 }},
		width = 3,
		craft_time = 5,
		animation = "",
	},
    belt = {
		components = {{ "motor",-1 },		 { "belt",1 },
					  { "iron_plate",-1 }, { "iron_gear",-1 }},
		width = 2, 
		craft_time = 5, 
		animation = "",
	},
}
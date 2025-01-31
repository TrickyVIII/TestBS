class KeyCardLocations
{
    float x = 0.0, y = 0.0, z = 0.0;
	float building_x = 0.0, building_y = 0.0, building_z = 0.0;

   	int door_index = 0;
		
	bool use_announcement = false;
	string announcement_text = "";
	
 	string Location_Name = "";
	string BuildingClassName = "";
    string keycard_class_name = "";
	//string alarm_sound = "";

	bool use_alarm= false;
	int min_unlock_time = 0;
	int max_unlock_time = 0;
	int min_restock_time = 0;
	int max_restock_time = 0;
	//bool single_use= true;
	bool containers_only= false;

	ref array<ref KeyCardDoorContainer> containers;

	void KeyCardLocations(float x = 0.0, float y = 0.0, float z = 0.0, float building_x = 0.0, float building_y = 0.0, float building_z = 0.0, int door_index = 0, bool use_announcement = false, string announcement_text = "", string Location_Name = "", string BuildingClassName = "", string keycard_class_name = "", /*string alarm_sound = "",*/ bool use_alarm = false, int min_unlock_time = 0, int max_unlock_time = 0, int min_restock_time = 0, int max_restock_time = 0, /*bool single_use = false,*/ bool containers_only = false)
	{
		this.x = x;
		this.y = y;
		this.z = z;
		this.building_x = building_x;
		this.building_y = building_y;
		this.building_z = building_z;
		this.door_index = door_index;
		this.use_announcement = use_announcement;
		this.announcement_text = announcement_text;
		this.Location_Name = Location_Name;
		this.BuildingClassName = BuildingClassName;
		this.keycard_class_name = keycard_class_name;
		//this.alarm_sound = alarm_sound;
		this.use_alarm = use_alarm;
		this.min_unlock_time = min_unlock_time;
		this.max_unlock_time = max_unlock_time;
		this.min_restock_time = min_restock_time;
		this.max_restock_time = max_restock_time;
		//this.single_use = single_use;
		this.containers_only = containers_only;

		containers = new array<ref KeyCardDoorContainer>();
	}

	vector GetDoorsPosition()
	{
		return Vector(x, z, y);
	}

	vector GetBuildingPosition()
	{
		return Vector(building_x, building_z, building_y);
	}
};
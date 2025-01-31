class KeyCardHelper
{
    static ItemBase SetQuantity(ItemBase item, int quantity)
	{
		Magazine mag = Magazine.Cast(item);
		if (mag)
				mag.ServerSetAmmoCount(quantity);
		else
			item.SetQuantity(quantity);
		return item;
	}

	static vector snapToGround(vector pos)
	{
		float pos_x = pos[0];
		float pos_z = pos[2];
		float pos_y = GetGame().SurfaceY( pos_x, pos_z );
		vector tmp_pos = Vector( pos_x, pos_y, pos_z );

		return tmp_pos;
	}
    
    static void TeleportAnyPlayerCloseToDoor(vector position)
    {
        array<Object> objects = new array<Object>;
        array<CargoBase> proxy = new array<CargoBase>;

        GetGame().GetObjectsAtPosition(position, 5, objects, proxy);
        for (int v = 0; v < objects.Count(); v++) 
        { 
            PlayerBase player;
            if (Class.CastTo(player, objects.Get(v))) // Tylko gracze
            {
                Print("[Keycard] Teletransportando al jugador cerca de una puerta restringida.");
                vector playerPosition = GetRandomPos(position, 7);
                player.SetPosition(playerPosition);
            }
        }
    }

    static vector GetRandomPos(vector pos, int radius)
	{
		int count = 20;
		while(count>0)
		{
			string w_Surface="";

			array<Object> excluded_objects = new array<Object>;
			array<Object> nearby_objects = new array<Object>;

			int randomAngle = Math.RandomFloatInclusive(0.01,6.28);

			int RandomX = (pos[0] + Math.RandomFloatInclusive(30,radius) * Math.Cos(randomAngle));
			int RandomZ = (pos[2] + Math.RandomFloatInclusive(30,radius) * Math.Sin(randomAngle));
			vector randomPos = Vector(RandomX, GetGame().SurfaceY( RandomX, RandomZ ) , RandomZ);

				GetGame().SurfaceGetType3D (randomPos[0], randomPos[1], randomPos[2], w_Surface);
				if (!GetGame().IsBoxColliding(Vector(randomPos[0], GetGame().SurfaceY(randomPos[0],randomPos[2]),randomPos[2]), "0 0 0", "3 5 3", excluded_objects, nearby_objects) && w_Surface != "FreshWater" && w_Surface != "sea" && !GetGame().SurfaceIsSea(randomPos[0], randomPos[2]))
						return randomPos;

			count--;
		}

		pos = snapToGround(pos);
		return pos;
	}
}
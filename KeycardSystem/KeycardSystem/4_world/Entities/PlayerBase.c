/* modded class PlayerBase
{
	override void OnConnect()
	{
		super.OnConnect();
		
		Print("[Keycard]  (PlayerBase) - Sending Keycard Data");
	}
} */

/* modded class PlayerBase {
	autoptr KeyCardSettings m_KeyCardSettings;

	override void OnPlayerLoaded() {
		super.OnPlayerLoaded();
		m_KeyCardSettings = KeyCardSettings.Load();
		CheckIfPlayerInLockedRoom();
	}

	void CheckIfPlayerInLockedRoom() {
		Print("sprawdza");
		int radiusCheck = 10; // Radius to check for locked doors
		autoptr array<Object> objects = new array<Object>;
		GetGame().GetObjectsAtPosition(this.GetPosition(), radiusCheck, objects, null);

		for (int i = 0; i < m_KeyCardSettings.KeyCard_Locations.Count(); i++) {
			vector doorLocation = m_KeyCardSettings.KeyCard_Locations.Get(i).GetDoorsPosition();

			for (int o = 0; o < objects.Count(); o++) {
				Object obj = objects.Get(o);
				if (obj.IsTree() || obj.IsRock() || obj.IsBush() || obj.IsInventoryItem()) {
					continue;
				}

				// Use vector.Distance to compare positions
				if (vector.Distance(obj.GetPosition(), doorLocation) < 1.0) {
					TeleportPlayerCloseToDoor(doorLocation);
					LogPlayerTeleport(this, doorLocation);
					return;
				}
			}
		}
	}

	void TeleportPlayerCloseToDoor(vector position) {
		Print("[Keycard] Teleporting player near restricted door.");
		vector playerPosition = KeyCardHelper.GetRandomPos(position, 7);
		SetPosition(playerPosition);
	}

	void LogPlayerTeleport(PlayerBase player, vector location) {
		string message = "[Keycard] Player " + player.GetIdentity().GetPlainName() + " (" + player.GetIdentity().GetPlainId() + ") teleported out of locked room at location: " + location;
		Print(message);
	}
} */


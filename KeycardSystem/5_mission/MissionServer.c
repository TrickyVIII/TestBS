modded class MissionServer
{
    private ref KeyCardSettings m_KeyCardSettings; // Przechowywanie konfiguracji lokalizacji drzwi

    void MissionServer()
    {	    
        if (GetGame().IsServer() && !GetGame().IsClient())
        {
            g_Keycard = null;
            GetKeyCard();

            // Załaduj konfigurację KeyCardSettings
            m_KeyCardSettings = KeyCardSettings.Load();
            if (!m_KeyCardSettings) {
                Print("[Keycard] Failed to load KeyCard settings!");
            } else {
                Print("[Keycard] KeyCard settings loaded successfully.");
            }
        }
    }

    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity) {
        super.InvokeOnConnect(player, identity);

        if (!m_KeyCardSettings || !m_KeyCardSettings.KeyCard_Locations) {
            Print("[Keycard] No keycard settings or locations found.");
            return;
        }

        array<ref KeyCardLocations> locations = m_KeyCardSettings.KeyCard_Locations;
        foreach (KeyCardLocations location : locations) {
            vector doorPosition = location.GetDoorsPosition();

            // Sprawdź, czy gracz jest w pobliżu drzwi zdefiniowanych w konfiguracji
            if (vector.Distance(player.GetPosition(), doorPosition) < 10) {
                Print("[Keycard] Player is near a configured door. Teleporting...");
                TeleportPlayerIfCloseToDoor(player, doorPosition);
                return; // Zakończ operację po teleportacji
            }
        }
    }

    void TeleportPlayerIfCloseToDoor(PlayerBase player, vector position) {
        vector newPosition = KeyCardHelper.GetRandomPos(position, 7);
        Print("[Keycard] Teleporting player to: " + newPosition);
        player.SetPosition(newPosition);
    }
}

modded class MissionServer
{
    private ref KeyCardSettings m_KeyCardSettings; 

    void MissionServer()
    {	    
        if (GetGame().IsServer() && !GetGame().IsClient())
        {
            g_Keycard = null;
            GetKeyCard();

            // Załaduj konfigurację KeyCardSettings
            m_KeyCardSettings = KeyCardSettings.Load();
            if (!m_KeyCardSettings) {
                Print("[Keycard] ¡No se pudieron cargar las configuraciones de KeyCard!");
            } else {
                Print("[Keycard] Configuraciones de KeyCard cargadas con éxito.");
            }
        }
    }

    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity) {
        super.InvokeOnConnect(player, identity);

        if (!m_KeyCardSettings || !m_KeyCardSettings.KeyCard_Locations) {
            Print("[Keycard] No se encontraron configuraciones o ubicaciones de KeyCard.");
            return;
        }

        array<ref KeyCardLocations> locations = m_KeyCardSettings.KeyCard_Locations;
        foreach (KeyCardLocations location : locations) {
            vector doorPosition = location.GetDoorsPosition();

            if (vector.Distance(player.GetPosition(), doorPosition) < 10) {
                Print("[Keycard] El jugador está cerca de una puerta configurada. Teletransportando...");
                TeleportPlayerIfCloseToDoor(player, doorPosition);
                return; // Zakończ operację po teleportacji
            }
        }
    }

    void TeleportPlayerIfCloseToDoor(PlayerBase player, vector position) {
        vector newPosition = KeyCardHelper.GetRandomPos(position, 7);
        Print("[Keycard] Teletransportando al jugador a: " + newPosition);
        player.SetPosition(newPosition);
    }
}

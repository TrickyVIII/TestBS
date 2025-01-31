class KeyCardLootPool
{
    string lootPoolName;
    ref array<ref KeyCardItem> rewards;

    void KeyCardLootPool(string _name)
    {
        lootPoolName = _name;
        rewards = new array<ref KeyCardItem>();
    }
}

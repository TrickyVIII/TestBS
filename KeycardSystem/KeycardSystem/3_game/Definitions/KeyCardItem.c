class KeyCardItem
{
    string name;
    float chanceToSpawn;
    int quantityMin;
    int quantityMax;
    ref array<ref KeyCardAttachment> attachments;

    void KeyCardItem(string _name, float _chanceToSpawn, int _quantityMin, int _quantityMax)
    {
        name = _name;
        chanceToSpawn = _chanceToSpawn;
        quantityMin = _quantityMin;
        quantityMax = _quantityMax;
        attachments = new array<ref KeyCardAttachment>();
    }
}
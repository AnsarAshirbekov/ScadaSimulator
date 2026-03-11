#include "equipment/conveyor.h"

Conveyor::Conveyor(int plowCount)
{
    for(int i = 0; i < plowCount; i++)
    {
        m_plows.append(new Plow(this));
    }
}

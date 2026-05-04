/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "ArmsWarriorStrategy.h"

#include "Playerbots.h"

class ArmsWarriorStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    ArmsWarriorStrategyActionNodeFactory()
    {
        creators["charge"] = &charge;
        creators["intercept"] = &intercept;
        creators["hamstring"] = &hamstring;
        creators["mocking blow"] = &mocking_blow;
        creators["overpower"] = &overpower;
        creators["mortal strike"] = &mortal_strike;
        creators["rend"] = &rend;
        creators["rend on attacker"] = &rend_on_attacker;
        creators["bloodrage"] = &bloodrage;
        creators["death wish"] = &death_wish;
        creators["sweeping strikes"] = &sweeping_strikes;
        creators["bladestorm"] = &bladestorm;
        creators["heroic throw"] = &heroic_throw;
        creators["shattering throw"] = &shattering_throw;
    }

private:
    ACTION_NODE_A(charge, "charge", "intercept");
    ACTION_NODE_A(intercept, "intercept", "reach melee");
    ACTION_NODE_A(hamstring, "hamstring", "piercing howl");
    ACTION_NODE_A(mocking_blow, "mocking blow", "overpower");
    ACTION_NODE_A(overpower, "overpower", "melee");
    ACTION_NODE_A(mortal_strike, "mortal strike", "melee");
    ACTION_NODE_A(rend, "rend", "melee");
    ACTION_NODE_A(rend_on_attacker, "rend on attacker", "rend");
    ACTION_NODE_A(bloodrage, "bloodrage", "berserker rage");
    ACTION_NODE_A(death_wish, "death wish", "berserker rage");
    ACTION_NODE_A(sweeping_strikes, "sweeping strikes", "cleave");
    ACTION_NODE_A(bladestorm, "bladestorm", "cleave");
    ACTION_NODE_A(heroic_throw, "heroic throw", "melee");
    ACTION_NODE_A(shattering_throw, "shattering throw", "melee");
};

ArmsWarriorStrategy::ArmsWarriorStrategy(PlayerbotAI* botAI) : GenericWarriorStrategy(botAI)
{
    actionNodeFactories.Add(new ArmsWarriorStrategyActionNodeFactory());
}

NextAction** ArmsWarriorStrategy::getDefaultActions()
{
    return NextAction::array(0, 
                            new NextAction("death wish", ACTION_DEFAULT + 0.7f),
                            new NextAction("bladestorm", ACTION_DEFAULT + 0.6f),
                            new NextAction("mortal strike", ACTION_DEFAULT + 0.5f),
                            new NextAction("overpower", ACTION_DEFAULT + 0.4f),
                            new NextAction("slam", ACTION_DEFAULT + 0.3f),
                            new NextAction("heroic strike", ACTION_DEFAULT + 0.2f),
                            new NextAction("melee", ACTION_DEFAULT), nullptr);
}

void ArmsWarriorStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarriorStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode("enemy out of melee",
        NextAction::array(0, new NextAction("charge", ACTION_MOVE + 10), nullptr)));
    
    triggers.push_back(new TriggerNode("battle stance",
        NextAction::array(0, new NextAction("battle stance", ACTION_HIGH + 10), nullptr)));
    
    triggers.push_back(new TriggerNode("battle shout",
        NextAction::array(0, new NextAction("battle shout", ACTION_HIGH + 9), nullptr)));
    
    triggers.push_back(new TriggerNode("rend",
        NextAction::array(0, new NextAction("rend", ACTION_HIGH + 8), nullptr)));
    
    triggers.push_back(new TriggerNode("rend on attacker",
        NextAction::array(0, new NextAction("rend on attacker", ACTION_HIGH + 8), nullptr)));
    
    triggers.push_back(new TriggerNode("mortal strike",
        NextAction::array(0, new NextAction("mortal strike", ACTION_HIGH + 3), nullptr)));
    
    triggers.push_back(new TriggerNode("target critical health",
        NextAction::array(0, new NextAction("execute", ACTION_HIGH + 5), nullptr)));
    
    triggers.push_back(new TriggerNode("sudden death",
        NextAction::array(0, new NextAction("execute", ACTION_HIGH + 5), nullptr)));
    
    triggers.push_back(new TriggerNode("hamstring",
        NextAction::array(0, new NextAction("piercing howl", ACTION_HIGH), nullptr)));
    
    triggers.push_back(new TriggerNode("overpower",
        NextAction::array(0, new NextAction("overpower", ACTION_HIGH + 4), nullptr)));
    
    triggers.push_back(new TriggerNode("taste for blood",
        NextAction::array(0, new NextAction("overpower", ACTION_HIGH + 4), nullptr)));
    
    triggers.push_back(new TriggerNode("victory rush",
        NextAction::array(0, new NextAction("victory rush", ACTION_INTERRUPT), nullptr)));
    
    triggers.push_back(new TriggerNode("high rage available",
        NextAction::array(0, new NextAction("heroic strike", ACTION_HIGH),
                             new NextAction("slam", ACTION_HIGH + 1),
                                nullptr)));
    triggers.push_back(new TriggerNode("medium rage available",
        NextAction::array(0, new NextAction("heroic strike", ACTION_HIGH),
                             new NextAction("slam", ACTION_HIGH + 1),
                                nullptr)));
    triggers.push_back(
        new TriggerNode("bloodrage", NextAction::array(0, new NextAction("bloodrage", ACTION_HIGH + 2), nullptr)));
    
    triggers.push_back(
        new TriggerNode("death wish", NextAction::array(0, new NextAction("death wish", ACTION_HIGH + 2), nullptr)));
    
    triggers.push_back(new TriggerNode("critical health",
        NextAction::array(0, new NextAction("intimidating shout", ACTION_EMERGENCY), nullptr)));
}
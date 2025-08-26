/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "ListQuestsActions.h"

#include "Event.h"
#include "Playerbots.h"

bool ListQuestsAction::Execute(Event event)
{
    if (event.getParam() == "completed" || event.getParam() == "co")
    {
        ListQuests(QUEST_LIST_FILTER_COMPLETED);
    }
    else if (event.getParam() == "incompleted" || event.getParam() == "in")
    {
        ListQuests(QUEST_LIST_FILTER_INCOMPLETED);
    }
    else if (event.getParam() == "all")
    {
        ListQuests(QUEST_LIST_FILTER_ALL);
    }
    else if (event.getParam() == "travel")
    {
        ListQuests(QUEST_LIST_FILTER_ALL, QUEST_TRAVEL_DETAIL_SUMMARY);
    }
    else if (event.getParam() == "travel detail")
    {
        ListQuests(QUEST_LIST_FILTER_ALL, QUEST_TRAVEL_DETAIL_FULL);
    }
    else
    {
        ListQuests(QUEST_LIST_FILTER_SUMMARY);
    }

    return true;
}

void ListQuestsAction::ListQuests(QuestListFilter filter, QuestTravelDetail travelDetail)
{
    bool showIncompleted = filter & QUEST_LIST_FILTER_INCOMPLETED;
    bool showCompleted = filter & QUEST_LIST_FILTER_COMPLETED;

    if (showIncompleted)
        botAI->TellMaster("--- 未完成任务 ---");

    uint32 incompleteCount = ListQuests(false, !showIncompleted, travelDetail);

    if (showCompleted)
        botAI->TellMaster("--- 已完成任务 ---");

    uint32 completeCount = ListQuests(true, !showCompleted, travelDetail);

    botAI->TellMaster("--- 摘要 ---");

    std::ostringstream out;
    out << "总计: " << (completeCount + incompleteCount) << " / 25 (未完成: " << incompleteCount
        << ", 已完成: " << completeCount << ")";
    botAI->TellMaster(out);
}

uint32 ListQuestsAction::ListQuests(bool completed, bool silent, QuestTravelDetail travelDetail)
{
    TravelTarget* target;
    WorldPosition botPos(bot);
    if (travelDetail != QUEST_TRAVEL_DETAIL_NONE)
        target = context->GetValue<TravelTarget*>("travel target")->Get();

    uint32 count = 0;
    for (uint16 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 questId = bot->GetQuestSlotQuestId(slot);
        if (!questId)
            continue;

        Quest const* pQuest = sObjectMgr->GetQuestTemplate(questId);
        bool isCompletedQuest = bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE;
        if (completed != isCompletedQuest)
            continue;

        ++count;

        if (silent)
            continue;

        botAI->TellMaster(chat->FormatQuest(pQuest));

        if (travelDetail != QUEST_TRAVEL_DETAIL_NONE && target->getDestination())
        {
            if (target->getDestination()->getName() == "QuestRelationTravelDestination" ||
                target->getDestination()->getName() == "QuestObjectiveTravelDestination")
            {
                QuestTravelDestination* QuestDestination = (QuestTravelDestination*)target->getDestination();

                if (QuestDestination->GetQuestTemplate()->GetQuestId() == questId)
                {
                    std::ostringstream out;
                    out << "[进行中] 前往 " << target->getPosition()->distance(botPos);
                    out << " 到 " << QuestDestination->getTitle();
                    botAI->TellMaster(out);
                }
            }
        }

        if (travelDetail == QUEST_TRAVEL_DETAIL_SUMMARY)
        {
            std::vector<TravelDestination*> allDestinations =
                sTravelMgr->getQuestTravelDestinations(bot, questId, true, true, -1);
            std::vector<TravelDestination*> availDestinations =
                sTravelMgr->getQuestTravelDestinations(bot, questId, botAI->GetMaster(), false, -1);

            uint32 desTot = allDestinations.size();
            uint32 desAvail = availDestinations.size();
            uint32 desFull = desAvail - sTravelMgr->getQuestTravelDestinations(bot, questId, false, false, -1).size();
            uint32 desRange = desAvail - sTravelMgr->getQuestTravelDestinations(bot, questId, false, false).size();

            uint32 tpoints = 0;
            uint32 apoints = 0;

            for (auto dest : allDestinations)
                tpoints += dest->getPoints(true).size();

            for (auto dest : availDestinations)
                apoints += dest->getPoints().size();

            std::ostringstream out;
            out << desAvail << "/" << desTot << " 个目的地 " << apoints << "/" << tpoints << " 点数. ";

            if (desFull > 0)
                out << desFull << " 个拥挤.";

            if (desRange > 0)
                out << desRange << " 个超出范围.";

            botAI->TellMaster(out);
        }
        else if (travelDetail == QUEST_TRAVEL_DETAIL_FULL)
        {
            uint32 limit = 0;
            std::vector<TravelDestination*> allDestinations =
                sTravelMgr->getQuestTravelDestinations(bot, questId, true, true, -1);

            std::sort(allDestinations.begin(), allDestinations.end(),
                      [botPos](TravelDestination* i, TravelDestination* j) {
                          return i->distanceTo(const_cast<WorldPosition*>(&botPos)) <
                                 j->distanceTo(const_cast<WorldPosition*>(&botPos));
                      });

            for (auto dest : allDestinations)
            {
                if (limit > 5)
                    continue;

                std::ostringstream out;

                uint32 tpoints = dest->getPoints(true).size();
                uint32 apoints = dest->getPoints().size();

                out << round(dest->distanceTo(const_cast<WorldPosition*>(&botPos)));
                out << " 到 " << dest->getTitle();
                out << " " << apoints;

                if (apoints < tpoints)
                    out << "/" << tpoints;

                out << " 点数.";

                if (!dest->isActive(bot))
                    out << " 未激活";

                if (dest->isFull(bot))
                    out << " 拥挤";

                botAI->TellMaster(out);

                limit++;
            }
        }
    }

    return count;
}

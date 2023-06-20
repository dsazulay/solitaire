#pragma once

struct PlayerData
{
    int gamesPlayed;
    int gamesWon;
    float bestTime;
};

struct MatchData
{
    float startTime;
    float currentTime;
    float timePaused;
    float timePausedStart;
};

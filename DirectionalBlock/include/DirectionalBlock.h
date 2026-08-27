#pragma once
#include "syati.h"

class DirectionalBlock : public LiveActor {
public:
    DirectionalBlock(const char *pName);
    virtual ~DirectionalBlock();

    virtual void init(const JMapInfoIter &rIter);
    virtual void control();

    void exeWait();
    void exeMove();
    void exeFall();
    void exeMoveRail();
    void exeFallRail();

    void calcFallTime();
    void setRailRotation();


    bool receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver);

    f32 mStepSize;
    s32 mMaxSteps;
    s32 mFalling;
    s32 mFallDelay;
    f32 mFallSpeed;
    s32 mSwitchBBehavior;
    s32 mColorFrame;

    s32 mCurrentStep;
    f32 mFallTime;
    s32 mFallSteps;

};

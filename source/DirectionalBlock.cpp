#include "DirectionalBlock.h"

namespace pt {
    extern void initRailToNearestAndRepositionWithGravity(LiveActor* pActor);
    extern void turnToDirectionUpFront(LiveActor *pActor, TVec3f rUp, TVec3f rFront);
}

namespace NrvDirectionalBlock {
   FULL_NERVE(NrvWait, DirectionalBlock, Wait);
   FULL_NERVE(NrvMove, DirectionalBlock, Move);
   FULL_NERVE(NrvFall, DirectionalBlock, Fall);
   FULL_NERVE(NrvMoveRail, DirectionalBlock, MoveRail);
   FULL_NERVE(NrvFallRail, DirectionalBlock, FallRail);
}

/*
ObjArg0: Step Size
ObjArg1: Max Steps
ObjArg2: Does Block Fall?
    0 = No
    1 = Only at Max Steps
    2 = Always
ObjArg3: Fall Delay
ObjArg4: Fall Speed
ObjArg5: Switch B Activation 
    0 = Activate Switch B on hit
    1 = Activate Switch B when block reaches max steps
ObjArg6: Color
    0 = Red
    1 = Orange
    2 = Yellow
    3 = Light Green
    4 = Green
    5 = Light Blue
    6 = Blue
    7 = Purple
    8 = Pink
    9 = White
    10 = Black
*/

DirectionalBlock::DirectionalBlock(const char *pName) : LiveActor(pName) {

    // OSReport("constructor\n");
    mStepSize = 100.0f;
    mMaxSteps = 5;
    mFalling = 0;
    mFallDelay = 120;
    mFallSpeed = 15.0f;
    mSwitchBBehavior = 0;
    mColorFrame = 7;

    mCurrentStep = 0;
    mFallTime = 0.0f;
    mFallSteps = 0;
}

DirectionalBlock::~DirectionalBlock() { }

void DirectionalBlock::init(const JMapInfoIter &rIter) {

    // OSReport("init\n");
    MR::processInitFunction(this, rIter, false);
    MR::onCalcGravity(this);
    MR::connectToSceneMapObjStrongLight(this);

    MR::getJMapInfoArg0NoInit(rIter, &mStepSize);
    MR::getJMapInfoArg1NoInit(rIter, &mMaxSteps);
    MR::getJMapInfoArg2NoInit(rIter, &mFalling);
    MR::getJMapInfoArg3NoInit(rIter, &mFallDelay);
    MR::getJMapInfoArg4NoInit(rIter, &mFallSpeed);
    MR::getJMapInfoArg5NoInit(rIter, &mSwitchBBehavior);
    MR::getJMapInfoArg6NoInit(rIter, &mColorFrame);
    MR::useStageSwitchReadA(this, rIter);
    MR::useStageSwitchWriteB(this, rIter);

    if(mMaxSteps < 0) {
        mMaxSteps = 5;
    }
    if (mFallDelay < 0) {
        mFallDelay = 120;
    }
    if (mFallSpeed <= 0.0f) {
        mFallSpeed = 5.0f;
    }

    if (MR::isConnectedWithRail(rIter)) {
        initRailRider(rIter);
        pt::initRailToNearestAndRepositionWithGravity(this);
        setRailRotation();
    }


    initHitSensor(1);
    MR::addHitSensorMapObj(this, "Platform");
    MR::initCollisionParts(this, "DirectionalBlock", getSensor("Platform"), NULL);
    MR::validateCollisionParts(this);

    MR::startBrkAndSetFrameAndStop(this, "ColorChange", mColorFrame);

    initNerve(&NrvDirectionalBlock::NrvWait::sInstance, 0);


    makeActorAppeared();
}

void DirectionalBlock::control() {

}

bool DirectionalBlock::receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    // OSReport("receiveMsgPlayerAttack\n");
    if (MR::isMsgPlayerUpperPunch(msg)) {
        MR::sendArbitraryMsg(ACTMES_REFLECT_V, pSender, pReceiver);
        if (isNerve(&NrvDirectionalBlock::NrvWait::sInstance) && mCurrentStep < mMaxSteps) {
            if (mRailRider != NULL) {
                setNerve(&NrvDirectionalBlock::NrvMoveRail::sInstance);
            }
            else {
                setNerve(&NrvDirectionalBlock::NrvMove::sInstance);
            }
        }
        return true;
    }
    return false;
}

void DirectionalBlock::exeWait() {
    if(MR::isFirstStep(this)) {

        //OSReport("wait\n");
        
        mVelocity.setAll(0);
        if (mRailRider != NULL) {
            mRailRider->setSpeed(0);
        }

        if (MR::isValidSwitchB(this)) {
            MR::offSwitchB(this);
        }
        
        if (mCurrentStep >= mMaxSteps) {
            MR::startBrkAndSetFrameAndStop(this, "ColorChange", 11);
        }
        else {
            MR::startBrkAndSetFrameAndStop(this, "ColorChange", mColorFrame);
        }

    }

    if(MR::isValidSwitchA(this) && MR::isOnSwitchA(this) && mCurrentStep < mMaxSteps) {
        if (mRailRider != NULL) {
            setNerve(&NrvDirectionalBlock::NrvMoveRail::sInstance);
        }
        else {
            setNerve(&NrvDirectionalBlock::NrvMove::sInstance);
        }
    }

    if (mFalling == 1 && mCurrentStep >= mMaxSteps) {
        if (MR::isStep(this, mFallDelay)) {
            if (mRailRider != NULL) {
                setNerve(&NrvDirectionalBlock::NrvFallRail::sInstance);
            }
            else {
                setNerve(&NrvDirectionalBlock::NrvFall::sInstance);
            }
        }
    }

    if (mFalling == 2 && mCurrentStep > 0) {
        if (MR::isStep(this, mFallDelay)) {
            if (mRailRider != NULL) {
                setNerve(&NrvDirectionalBlock::NrvFallRail::sInstance);
            }
            else {
                setNerve(&NrvDirectionalBlock::NrvFall::sInstance);
            }
        }
    }

}

void DirectionalBlock::exeMove() {
    if (MR::isFirstStep(this)) {

        // OSReport("move\n");

        mCurrentStep++;
        
        TVec3f upVec;
        MR::calcUpVec(&upVec, this);
        mVelocity = upVec * mStepSize / 5.0f;

        if (MR::isValidSwitchB(this)) {
            if (mSwitchBBehavior == 0 || (mSwitchBBehavior == 1 && mCurrentStep >= mMaxSteps)) {
                MR::onSwitchB(this);
            }
        }

        MR::startBck(this, "Move", NULL);
        MR::startActionSound(this, "OjBlockMove", -1, -1, -1);
    }

    if (MR::isGreaterEqualStep(this, 5)) {

        mVelocity.setAll(0);
        setNerve(&NrvDirectionalBlock::NrvWait::sInstance);

        if (mCurrentStep >= mMaxSteps) {
            MR::startActionSound(this, "OjBlockStopped", -1, -1, -1);
        }
    }
}

void DirectionalBlock::exeFall() {

    TVec3f upVec;
    MR::calcUpVec(&upVec, this);
    
    if (MR::isFirstStep(this)) {

        //OSReport("fall\n");

        if (mStepSize >= 0.0f) {
            mVelocity = -upVec * mFallSpeed;
            mFallTime = (mCurrentStep * mStepSize / mFallSpeed);
            mFallSteps = (s32)mFallTime;
        }
        else {
            mVelocity = upVec * mFallSpeed;
            mFallTime = (mCurrentStep * -mStepSize / mFallSpeed);
            mFallSteps = (s32)mFallTime;
        }
        

        MR::startActionSound(this, "OjBlockFall", -1, -1, -1);
    }

    if (MR::isGreaterEqualStep(this, mFallSteps)) {
        if (mStepSize >= 0.0f) {
            mVelocity = -upVec * (mFallSpeed * (mFallTime - mFallSteps));
        }
        else {
            mVelocity = upVec * (mFallSpeed * (mFallTime - mFallSteps));
        }
        MR::startActionSound(this, "OjBlockCrash", -1, -1, -1);
        mCurrentStep = 0;
        setNerve(&NrvDirectionalBlock::NrvWait::sInstance); 
    }

}

void DirectionalBlock::exeMoveRail() {
    if (MR::isFirstStep(this)) {

        // OSReport("move rail\n");
        mCurrentStep++;

        mRailRider->setSpeed(mStepSize / 5.0f);

        if (MR::isValidSwitchB(this)) {
            if (mSwitchBBehavior == 0 || (mSwitchBBehavior == 1 && mCurrentStep >= mMaxSteps)) {
                MR::onSwitchB(this);
            }
        }

        MR::startBck(this, "Move", NULL);
        MR::startActionSound(this, "OjBlockMove", -1, -1, -1);
    }

    if(MR::isGreaterEqualStep(this, 5)) {
        mRailRider->setSpeed(0);
        setNerve(&NrvDirectionalBlock::NrvWait::sInstance);


        if (mCurrentStep >= mMaxSteps || (mRailRider->isReachedGoal() || mRailRider->isReachedEdge())) {
            if (MR::isValidSwitchB(this)) {
                MR::onSwitchB(this);
            }
            MR::startActionSound(this, "OjBlockStopped", -1, -1, -1);
            mCurrentStep = mMaxSteps;
        }
    }

    mRailRider->move();
    MR::moveTransToCurrentRailPos(this);
    setRailRotation();

}

void DirectionalBlock::exeFallRail() {

    if (MR::isFirstStep(this)) {

        // OSReport("fall rail\n");

        if (mStepSize >= 0.0f) {
            mRailRider->setSpeed(-mFallSpeed);
            mFallTime = ((mCurrentStep * mStepSize) / mFallSpeed);
            mFallSteps = (s32)mFallTime;
            // OSReport("fall time: %f, fall steps: %d\n", mFallTime, mFallSteps);
        }
        else {
            mRailRider->setSpeed(mFallSpeed);
            mFallTime = (mCurrentStep * -mStepSize / mFallSpeed);
            mFallSteps = (s32)mFallTime;
            // OSReport("fall time: %f, fall steps: %d\n", mFallTime, mFallSteps);
        }

        MR::startActionSound(this, "OjBlockFall", -1, -1, -1);

    }

    mRailRider->move();
    MR::moveTransToCurrentRailPos(this);
    setRailRotation();

    if (MR::isGreaterEqualStep(this, mFallSteps) || mRailRider->isReachedGoal() || mRailRider->isReachedEdge()) {
        if (mStepSize >= 0.0f) {
            mRailRider->setSpeed(-mFallSpeed * (mFallTime - mFallSteps));
        }
        else {
            mRailRider->setSpeed(mFallSpeed * (mFallTime - mFallSteps));
        }
        mCurrentStep = 0;
        MR::startActionSound(this, "OjBlockCrash", -1, -1, -1);
        setNerve(&NrvDirectionalBlock::NrvWait::sInstance);
    }

    

}

void DirectionalBlock::setRailRotation() {
    
    // This staticVec feels like some straight BS imma be so real.
    TVec3f staticVec(0.0f, 1.0f, 0.0f);

    TVec3f railDirection;
    MR::calcNearestRailDirection(&railDirection, this, mTranslation);

    if (staticVec.dot(railDirection) > 0.999f) {
        staticVec.set(0.05f, 1.0f, 0.0f);
        staticVec.normalize(staticVec);
    }
    else{
        staticVec.set(0.0f, 1.0f, 0.0f);
    }

    TVec3f rotationVec;
    rotationVec = staticVec.cross(railDirection);

    pt::turnToDirectionUpFront(this, railDirection, rotationVec);


}
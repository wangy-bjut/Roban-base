#include "robanbehavior.h"
#include "../rvdraw/rvdraw.h"

extern int agentBodyType;

/*
 * Real game beaming.
 * Filling params x y angle
 */
void RobanBehavior::beam(double &beamX, double &beamY, double &beamAngle)
{
    // beamX = -HALF_FIELD_X + worldModel->getUNum();
    int number = worldModel->getUNum();

    switch (number)
    {
    case 1:
    { // 我是1号球员
        beamX = -HALF_FIELD_X + worldModel->getUNum();
        beamY = 0;
        break;
    }
    case 2:
    { // 我是2号球员
        beamX = -6;
        beamY = 0;
        break;
    }
    case 3:
    { // 我是3号球员
        beamX = -3;
        beamY = 0;
        break;
    }
    case 4:
    { // 我是4号球员
        beamX = -7;
        beamY = 6;
        break;
    }
    case 5:
    { // 我是5号球员
        beamX = -7;
        beamY = -6;
        break;
    }
    default:
        break;
    }
    beamAngle = 0;
}

SkillType RobanBehavior::selectSkill()
{
    // My position and angle
    // cout << worldModel->getUNum() << ": " << worldModel->getMyPosition() << ",\t" << worldModel->getMyAngDeg() << "\n";

    // Position of the ball
    // cout << worldModel->getBall() << "\n";

    // Example usage of the roboviz drawing system and RVSender in rvdraw.cc.
    // Agents draw the position of where they think the ball is
    // Also see example in robanbahevior.cc for drawing agent position and
    // orientation.
    /*
    worldModel->getRVSender()->clear(); // erases drawings from previous cycle
    worldModel->getRVSender()->drawPoint("ball", ball.getX(), ball.getY(), 10.0f, RVSender::MAGENTA);
    */

    // ### Demo Behaviors ###

    // Walk in different directions
    // return goToTargetRelative(VecPosition(1,0,0), 0); // Forward
    // return goToTargetRelative(VecPosition(-1,0,0), 0); // Backward
    // return goToTargetRelative(VecPosition(0,1,0), 0); // Left
    // return goToTargetRelative(VecPosition(0,-1,0), 0); // Right
    // return goToTargetRelative(VecPosition(1,1,0), 0); // Diagonal
    // return goToTargetRelative(VecPosition(0,1,0), 90); // Turn counter-clockwise
    // return goToTargetRelative(VecPdosition(0,-1,0), -90); // Turn clockwise
    // return goToTargetRelative(VecPosition(1,0,0), 15); // Circle

    // Walk to the ball
    // return goToTarget(ball);

    // Turn in place to face ball
    /*double distance, angle;
    getTargetDistanceAndAngle(ball, distance, angle);
    if (abs(angle) > 10) {
      return goToTargetRelative(VecPosition(), angle);
    } else {
      return SKILL_STAND;
    }*/

    // Walk to ball while always facing forward
    // return goToTargetRelative(worldModel->g2l(ball), -worldModel->getMyAngDeg());

    // Dribble ball toward opponent's goal
    // return kickBall(KICK_DRIBBLE, VecPosition(HALF_FIELD_X, 0, 0));

    // Kick ball toward opponent's goal
    // return kickBall(KICK_FORWARD, VecPosition(HALF_FIELD_X, 0, 0)); // Basic kick
    // return kickBall(KICK_IK, VecPosition(HALF_FIELD_X, 0, 0)); // IK kick

    // Just stand in place
    // return SKILL_STAND;

    // Demo behavior where players form a rotating circle and kick the ball
    // back and forth
    // return demoKickingCircle();
    return MeAdd();
}

/*
 * Demo behavior where players form a rotating circle and kick the ball
 * back and forth
 */
SkillType RobanBehavior::demoKickingCircle()
{
    // Parameters for circle
    VecPosition center = VecPosition(-HALF_FIELD_X / 2.0, 0, 0);
    double circleRadius = 5.0;
    double rotateRate = 2.5;

    // Find closest player to ball
    int playerClosestToBall = -1;
    double closestDistanceToBall = 10000;
    for (int i = WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; ++i)
    {
        VecPosition temp;
        int playerNum = i - WO_TEAMMATE1 + 1;
        if (worldModel->getUNum() == playerNum)
        {
            // This is us
            temp = worldModel->getMyPosition();
        }
        else
        {
            WorldObject *teammate = worldModel->getWorldObject(i);
            if (teammate->validPosition)
            {
                temp = teammate->pos;
            }
            else
            {
                continue;
            }
        }
        temp.setZ(0);

        double distanceToBall = temp.getDistanceTo(ball);
        if (distanceToBall < closestDistanceToBall)
        {
            playerClosestToBall = playerNum;
            closestDistanceToBall = distanceToBall;
        }
    }

    if (playerClosestToBall == worldModel->getUNum())
    {
        // Have closest player kick the ball toward the center
        return kickBall(KICK_FORWARD, center);
    }
    else
    {
        // Move to circle position around center and face the center
        VecPosition localCenter = worldModel->g2l(center);
        SIM::AngDeg localCenterAngle = atan2Deg(localCenter.getY(), localCenter.getX());

        // Our desired target position on the circle
        // Compute target based on uniform number, rotate rate, and time
        VecPosition target = center + VecPosition(circleRadius, 0, 0).rotateAboutZ(360.0 / (NUM_AGENTS - 1) * (worldModel->getUNum() - (worldModel->getUNum() > playerClosestToBall ? 1 : 0)) + worldModel->getTime() * rotateRate);

        // Adjust target to not be too close to teammates or the ball
        target = collisionAvoidance(true /*teammate*/, false /*opponent*/, true /*ball*/, 1 /*proximity thresh*/, .5 /*collision thresh*/, target, true /*keepDistance*/);

        if (me.getDistanceTo(target) < .25 && abs(localCenterAngle) <= 10)
        {
            // Close enough to desired position and orientation so just stand
            return SKILL_STAND;
        }
        else if (me.getDistanceTo(target) < .5)
        {
            // Close to desired position so start turning to face center
            return goToTargetRelative(worldModel->g2l(target), localCenterAngle);
        }
        else
        {
            // Move toward target location
            return goToTarget(target);
        }
    }
}

SkillType RobanBehavior::MeAdd()
{
    int number = worldModel->getUNum(); // 获取球员号，不同的球员执行不同的任务
    switch (number)
    {

    case 1:
    { // 我是1号球员

        int playerClosestToBall = -1;
        double closestDistanceToBall = 10000;

        if ((ball.getX() < -12.5 && ball.getX() > -15) && (ball.getY() < 2 && ball.getY() > -2))
        {
            if (me.getDistanceTo(ball) < 0.5)
            {
                for (int i = WO_TEAMMATE1; i < WO_TEAMMATE1 + NUM_AGENTS; ++i)
                {
                    VecPosition temp;
                    int playerNum = i - WO_TEAMMATE1 + 1;
                    if (worldModel->getUNum() == playerNum)
                    {
                        // This is us
                        // temp = worldModel->getMyPosition();
                        continue;
                    }
                    else
                    {
                        WorldObject *teammate = worldModel->getWorldObject(i);
                        if (teammate->validPosition)
                        {
                            temp = teammate->pos;
                        }
                        else
                        {
                            continue;
                        }
                    }
                    temp.setZ(0);

                    double distanceToBall = temp.getDistanceTo(ball);
                    if (distanceToBall < closestDistanceToBall)
                    {
                        playerClosestToBall = playerNum;
                        closestDistanceToBall = distanceToBall;
                    }
                }

                WorldObject *teammate = worldModel->getWorldObject(playerClosestToBall);
                SIM::AngDeg meToPlayerAngle = me.getAngleBetweenPoints(teammate->pos.getY(), teammate->pos.getX());

                SIM::AngDeg tempAngle;
                bool mark = false;
                for (int i = WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; ++i)
                {

                    VecPosition temp;
                    int playerNum = i - WO_TEAMMATE1 + 1;

                    WorldObject *oppo = worldModel->getWorldObject(i);

                    tempAngle = me.getAngleBetweenPoints(oppo->pos.getY(), oppo->pos.getX());

                    if (tempAngle == meToPlayerAngle)
                    {
                        mark = true;
                        break;
                    }
                }

                if (mark == true)
                {
                    return kickBall(KICK_FORWARD, VecPosition(15, 0, 0));
                }
                else
                {
                    if (teammate->pos.getX() < -13)
                    {
                        return kickBall(KICK_FORWARD, VecPosition(teammate->pos.getX() + 2, teammate->pos.getY(), 0));
                    }
                    else
                    {
                        return kickBall(KICK_FORWARD, teammate->pos);
                    }
                }
            }
            else
            {
                return goToTarget(ball);
            }
        }
        else
        {
            // SIM::AngDeg localCenterAngle = me.getAngleBetweenPoints(ball.getY(),ball.getX());
            // return  goToTargetRelative(VecPosition(-14,0,0), localCenterAngle);//站在球门前1米距离，防守对方球队进球
            if ((ball.getX() < -12.5 && ball.getX() > -13) && (ball.getY() < 1.5 && ball.getY() > -1.5))
            {
                double distance, angle;

                while (me.getDistanceTo(ball) > 0.5)
                {
                    return goToTarget(ball);
                }
                VecPosition target;
                target.setY(ball.getY());
                target.setX(-13.5);

                getTargetDistanceAndAngle(ball, distance, angle);
                if (abs(angle) > 10 && abs(me.getY() - ball.getY()) > 0.2)
                {
                    return goToTargetRelative(target, angle);
                }
                else
                {
                    if (me.getX() > -13 || me.getY() < -1.5 || me.getY() > 1.5)
                    {
                        return goToTarget(VecPosition(-14, 0, 0));
                    }
                    else
                        return SKILL_STAND;
                }
            }
            else
            {

                double distance, angle;
                getTargetDistanceAndAngle(ball, distance, angle);
                if (abs(angle) > 10)
                {
                    return goToTargetRelative(VecPosition(), angle);
                }
                else
                {
                    return goToTarget(VecPosition(-14, 0, 0));
                }
            }
        }

        break;
    }

    case 2:
    {

        int playerClosestToBall = -1;
        int playerClosestTome = -1;

        double closestDistanceToBall = 10000;
        double closestDistanceTome = 10000;

        if (ball.getX() >= -2 && ball.getX() < 15 && ball.getY() <= 0)
        {

            // WorldObject* teammate = worldModel->getWorldObject( playerClosestToBall );
            // SIM::AngDeg meToPlayerAngle =  me.getAngleBetweenPoints(teammate->pos.getY(),teammate->pos.getX());

            // SIM::AngDeg tempAngle;

            if (me.getDistanceTo(ball) < 1)
            {

                for (int i = WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; ++i)
                {

                    VecPosition temp_opp;
                    int playerNum_opp = i - WO_OPPONENT1 + 1;

                    WorldObject *opp = worldModel->getWorldObject(i);
                    if (opp->validPosition)
                    {
                        temp_opp = opp->pos;
                    }
                    else
                    {
                        continue;
                    }

                    temp_opp.setZ(0);

                    double distanceTome = me.getDistanceTo(temp_opp);
                    if (distanceTome < closestDistanceTome)
                    {
                        playerClosestTome = playerNum_opp;
                        closestDistanceTome = distanceTome;
                    }
                }

                if (closestDistanceTome < 1 && (me.getDistanceTo(WO_TEAMMATE3) > 1.5 && me.getDistanceTo(WO_TEAMMATE3) < 3.5) && ball.getX() > 2)
                {

                    WorldObject *teammate = worldModel->getWorldObject(WO_TEAMMATE3);

                    return kickBall(KICK_FORWARD, teammate->pos);
                }
                else
                {
                    if (me.getDistanceTo(VecPosition(15, 0, 0)) >= 3)
                    {
                        return kickBall(KICK_DRIBBLE, VecPosition(15, 0, 0));
                    }
                    else
                    {
                        WorldObject *opponent = worldModel->getWorldObject(WO_OPPONENT1);
                        VecPosition target = opponent->pos;
                        target.setX(15);
                        if (opponent->pos.getY() > 0)
                        {
                            int y = opponent->pos.getY() - 1;
                            target.setY(y);
                        }
                        else if (opponent->pos.getY() < 0)
                        {
                            int y = opponent->pos.getY() + 1;
                            target.setY(y);
                        }

                        return kickBall(KICK_FORWARD, target);
                        // return kickBall(KICK_FORWARD, VecPosition(15, 0, 0));
                    }
                }
            }
            else if (ball.getDistanceTo(WO_TEAMMATE3) > me.getDistanceTo(ball))
            {
                return goToTarget(ball);
            }
            else
            {
                WorldObject *teammate = worldModel->getWorldObject(WO_TEAMMATE3);
                VecPosition target;
                if (teammate->pos.getY() <= 0)
                {
                    target.setX(teammate->pos.getX());
                    target.setY(teammate->pos.getY() - 2);
                }
                else
                {
                    target.setX(teammate->pos.getX());
                    target.setY(-2);
                }

                return goToTarget(target);
            }
        }
        else if (ball.getX() >= -2 && ball.getX() < 15 && ball.getY() > 0)
        {

            WorldObject *teammate = worldModel->getWorldObject(WO_TEAMMATE3);
            if (teammate->pos.getY() > 10 || teammate->pos.getY() < -10)
            {
                if (me.getDistanceTo(ball) < 1)
                {
                    if (me.getDistanceTo(VecPosition(15, 0, 0)) >= 3)
                    {
                        return kickBall(KICK_DRIBBLE, VecPosition(15, 0, 0));
                    }
                    else
                    {
                        WorldObject *opponent = worldModel->getWorldObject(WO_OPPONENT1);
                        VecPosition target = opponent->pos;
                        target.setX(15);
                        if (opponent->pos.getY() > 0)
                        {
                            int y = opponent->pos.getY() - 1;
                            target.setY(y);
                        }
                        else if (opponent->pos.getY() < 0)
                        {
                            int y = opponent->pos.getY() + 1;
                            target.setY(y);
                        }

                        return kickBall(KICK_FORWARD, target);
                        // return kickBall(KICK_FORWARD, VecPosition(15, 0, 0));
                    }
                }
                else
                {
                    return goToTarget(ball);
                }
            }
            else
            {
                if (teammate->pos.getDistanceTo(me) > 1)
                {
                    if (me.getDistanceTo(ball) < 1)
                    {
                        if (me.getDistanceTo(VecPosition(15, 0, 0)) >= 3)
                        {
                            return kickBall(KICK_DRIBBLE, VecPosition(15, 0, 0));
                        }
                        else
                        {
                            WorldObject *opponent = worldModel->getWorldObject(WO_OPPONENT1);
                            VecPosition target = opponent->pos;
                            target.setX(15);
                            if (opponent->pos.getY() > 0)
                            {
                                int y = opponent->pos.getY() - 1;
                                target.setY(y);
                            }
                            else if (opponent->pos.getY() < 0)
                            {
                                int y = opponent->pos.getY() + 1;
                                target.setY(y);
                            }

                            return kickBall(KICK_FORWARD, target);
                            // return kickBall(KICK_FORWARD, VecPosition(15, 0, 0));
                        }
                    }
                    else
                    {
                        return goToTarget(ball);
                    }
                    // return kickBall(KICK_DRIBBLE,WO_TEAMMATE3);
                }
                else
                {
                    VecPosition target;
                    target.setX(teammate->pos.getX());
                    target.setY(-2);
                    return goToTarget(target);
                }
            }
        }
        else if (ball.getX() < -2)
        {

            return goToTarget(VecPosition(3, 2, 0));
        }

        break;
    }
    case 3:
    {
        int playerClosestToBall = -1;
        int playerClosestTome = -1;
        double closestDistanceToBall = 10000;
        double closestDistanceTome = 10000;
        if (ball.getX() > -2 && ball.getX() < 15 && ball.getY() > 0) // 前场球
        {

            for (int i = WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; ++i)
            {

                VecPosition temp_opp;
                int playerNum_opp = i - WO_OPPONENT1 + 1;

                WorldObject *opp = worldModel->getWorldObject(i);
                if (opp->validPosition)
                {
                    temp_opp = opp->pos;
                }
                else
                {
                    continue;
                }

                temp_opp.setZ(0);

                double distanceTome = me.getDistanceTo(temp_opp);
                if (distanceTome < closestDistanceTome)
                {
                    playerClosestTome = playerNum_opp;
                    closestDistanceTome = distanceTome;
                }
            }

            if (closestDistanceTome < 1 && (me.getDistanceTo(WO_TEAMMATE2) > 1.5 && me.getDistanceTo(WO_TEAMMATE2) < 3.5) && ball.getX() > 3)
            {

                WorldObject *teammate = worldModel->getWorldObject(WO_TEAMMATE2);

                return kickBall(KICK_FORWARD, teammate->pos);
            }
            else
            {

                if (me.getDistanceTo(ball) > 1)
                {
                    return goToTarget(ball);
                }
                else
                {

                    if (me.getDistanceTo(VecPosition(15, 0, 0)) > 3)
                    {
                        return kickBall(KICK_DRIBBLE, VecPosition(15, 0, 0));
                    }
                    else
                    {
                        WorldObject *opponent = worldModel->getWorldObject(WO_OPPONENT1);
                        VecPosition target = opponent->pos;
                        target.setX(15);
                        if (opponent->pos.getY() > 0)
                        {
                            int y = opponent->pos.getY() - 1;
                            target.setY(y);
                        }
                        else if (opponent->pos.getY() < 0)
                        {
                            int y = opponent->pos.getY() + 1;
                            target.setY(y);
                        }

                        return kickBall(KICK_FORWARD, target);
                        // return kickBall(KICK_FORWARD, VecPosition(15, 0, 0));
                    }
                }
            }
        }
        else if (ball.getX() > -2 && ball.getX() < 15 && ball.getY() < 0)
        {

            WorldObject *teammate = worldModel->getWorldObject(WO_TEAMMATE2);
            if (teammate->pos.getY() > 10 || teammate->pos.getY() < -10)
            {
                if (me.getDistanceTo(ball) < 1)
                {
                    if (me.getDistanceTo(VecPosition(15, 0, 0)) >= 3)
                    {
                        return kickBall(KICK_DRIBBLE, VecPosition(15, 0, 0));
                    }
                    else
                    {
                        WorldObject *opponent = worldModel->getWorldObject(WO_OPPONENT1);
                        VecPosition target = opponent->pos;
                        target.setX(15);
                        if (opponent->pos.getY() > 0)
                        {
                            int y = opponent->pos.getY() - 1;
                            target.setY(y);
                        }
                        else if (opponent->pos.getY() < 0)
                        {
                            int y = opponent->pos.getY() + 1;
                            target.setY(y);
                        }

                        return kickBall(KICK_FORWARD, target);

                        // return kickBall(KICK_FORWARD, VecPosition(15, 0, 0));
                    }
                }
                else
                {
                    return goToTarget(ball);
                }
            }
            else
            {
                if (teammate->pos.getDistanceTo(me) > 1)
                {
                    if (me.getDistanceTo(ball) < 1)
                    {
                        if (me.getDistanceTo(VecPosition(15, 0, 0)) >= 3)
                        {
                            return kickBall(KICK_DRIBBLE, VecPosition(15, 0, 0));
                        }
                        else
                        {
                            WorldObject *opponent = worldModel->getWorldObject(WO_OPPONENT1);
                            VecPosition target = opponent->pos;
                            target.setX(15);
                            if (opponent->pos.getY() > 0)
                            {
                                int y = opponent->pos.getY() - 1;
                                target.setY(y);
                            }
                            else if (opponent->pos.getY() < 0)
                            {
                                int y = opponent->pos.getY() + 1;
                                target.setY(y);
                            }

                            return kickBall(KICK_FORWARD, target);

                            // return kickBall(KICK_FORWARD, VecPosition(15, 0, 0));
                        }
                    }
                    else
                    {
                        return goToTarget(ball);
                    }
                    // return kickBall(KICK_DRIBBLE,WO_TEAMMATE2);
                }
                else
                {
                    VecPosition target;
                    target.setX(teammate->pos.getX());
                    target.setY(2);
                    return goToTarget(target);
                }
            }
        }
        else if (ball.getX() < -2)
        {

            return goToTarget(VecPosition(3, -2, 0));
        }

        break;
    }

    case 4:
    {
        int playerClosestToBall = -1;
        int playerClosestTome = -1;
        double closestDistanceToBall = 10000;
        double closestDistanceTome = 10000;
        if (ball.getX() < 0 && ball.getX() > -14 && ball.getY() >= 0) // hou场球&&ball.getY()>0&&ball.getY()<11
        {

            // WorldObject* teammate = worldModel->getWorldObject( playerClosestToBall );
            // SIM::AngDeg meToPlayerAngle =  me.getAngleBetweenPoints(teammate->pos.getY(),teammate->pos.getX());

            // SIM::AngDeg tempAngle;
            //  bool mark = false;
            if (me.getDistanceTo(ball) > 1 && ball.getX() < -2)
            {
                return goToTarget(ball);
            }
            else
            {
                WorldObject *teammate = worldModel->getWorldObject(WO_TEAMMATE3);
                if (ball.getDistanceTo(WO_TEAMMATE2) > 1 && ball.getDistanceTo(WO_TEAMMATE3) > 1)
                {

                    if (me.getDistanceTo(WO_TEAMMATE2) < me.getDistanceTo(WO_TEAMMATE3))
                    {
                        return kickBall(KICK_FORWARD, WO_TEAMMATE2);
                    }
                    else
                    {
                        return kickBall(KICK_FORWARD, WO_TEAMMATE3);
                    }
                }
                else
                {
                    return SKILL_STAND;
                }
            }
        }
        else if (ball.getX() < 0 && ball.getX() > -14 && ball.getY() < 0)
        {
            WorldObject *teammate = worldModel->getWorldObject(WO_TEAMMATE5);
            if (teammate->pos.getY() > 10 || teammate->pos.getY() < -10)
            {
                if (me.getDistanceTo(ball) > 1)
                {
                    return goToTarget(ball);
                }
                else
                {
                    if (ball.getX() <= -2.5)
                    {
                        return kickBall(KICK_DRIBBLE, VecPosition(0, 0, 0));
                    }
                    else
                    {
                        if (me.getDistanceTo(WO_TEAMMATE2) < me.getDistanceTo(WO_TEAMMATE3))
                        {
                            return kickBall(KICK_FORWARD, WO_TEAMMATE2);
                        }
                        else
                        {
                            return kickBall(KICK_FORWARD, WO_TEAMMATE3);
                        };
                    }
                }
            }
            else
            {
                if (ball.getDistanceTo(WO_TEAMMATE5) < 1)
                {
                    WorldObject *teammate = worldModel->getWorldObject(WO_TEAMMATE5);

                    VecPosition target;
                    target.setX(teammate->pos.getX() - 2);
                    target.setY(teammate->pos.getY() - 2);

                    return goToTarget(target);
                }
                else
                {
                    if (me.getDistanceTo(ball) > 1)
                    {
                        return goToTarget(ball);
                    }
                    else
                    {
                        if (ball.getX() <= -2)
                        {
                            return kickBall(KICK_DRIBBLE, VecPosition(0, 0, 0));
                        }
                        else
                        {
                            if (me.getDistanceTo(WO_TEAMMATE2) < me.getDistanceTo(WO_TEAMMATE3))
                            {
                                return kickBall(KICK_FORWARD, WO_TEAMMATE2);
                            }
                            else
                            {
                                return kickBall(KICK_FORWARD, WO_TEAMMATE3);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            return goToTarget(VecPosition(-2, 2, 0));
        }

        break;
    }
    case 5:
    {
        int playerClosestToBall = -1;
        int playerClosestTome = -1;
        double closestDistanceToBall = 10000;
        double closestDistanceTome = 10000;
        if (ball.getX() < -2 && ball.getX() > -15 && ball.getY() < 0) //   &&ball.getY()>-11&&ball.getY()<1 //hou场球
        {

            for (int i = WO_OPPONENT1; i < WO_OPPONENT1 + NUM_AGENTS; ++i)
            {
                VecPosition temp;
                int playerNum = i - WO_OPPONENT1 + 1;
                if (worldModel->getUNum() == playerNum)
                {
                    // This is us
                    // temp = worldModel->getMyPosition();
                    continue;
                }
                else
                {
                    WorldObject *opp = worldModel->getWorldObject(i);
                    if (opp->validPosition)
                    {
                        temp = opp->pos;
                    }
                    else
                    {
                        continue;
                    }
                }
                temp.setZ(0);

                double distanceToBall = temp.getDistanceTo(ball);
                if (distanceToBall < closestDistanceToBall)
                {
                    playerClosestToBall = playerNum;
                    closestDistanceToBall = distanceToBall;
                }
            }
            if (me.getDistanceTo(WO_TEAMMATE4) > 2)
            {
                if (me.getDistanceTo(ball) < 1)
                {
                    WorldObject *teammate = worldModel->getWorldObject(WO_TEAMMATE4);
                    if (me.getDistanceTo(VecPosition(0, 0, 0)) > teammate->pos.getDistanceTo(VecPosition(0, 0, 0)))
                    {
                        if (ball.getX() < -12)
                        {
                            return kickBall(KICK_FORWARD, VecPosition(15, 0, 0));
                        }
                        else
                        {
                            return kickBall(KICK_DRIBBLE, teammate->pos);
                        }
                    }
                    else
                    {

                        return kickBall(KICK_FORWARD, VecPosition(0, 0, 0));
                    }
                }
                else
                {
                    return goToTarget(ball);
                }
            }
            else
            {
                return goToTarget(playerClosestToBall);
            }
        }
        else if (ball.getX() < -2 && ball.getX() > -15 && ball.getY() > 0)
        {
            WorldObject *teammate = worldModel->getWorldObject(WO_TEAMMATE4);
            if (teammate->pos.getY() > 10 || teammate->pos.getY() < -10)
            {
                if (me.getDistanceTo(ball) > 1)
                {
                    return goToTarget(ball);
                }
                else
                {
                    if (ball.getX() <= -2.5)
                    {
                        return kickBall(KICK_DRIBBLE, VecPosition(0, 0, 0));
                    }
                    else
                    {
                        if (me.getDistanceTo(WO_TEAMMATE2) < me.getDistanceTo(WO_TEAMMATE3))
                        {
                            return kickBall(KICK_FORWARD, WO_TEAMMATE2);
                        }
                        else
                        {
                            return kickBall(KICK_FORWARD, WO_TEAMMATE3);
                        }
                    }
                }
            }
            else
            {
                if (ball.getDistanceTo(WO_TEAMMATE4) < 1)
                {
                    WorldObject *teammate = worldModel->getWorldObject(WO_TEAMMATE4);

                    VecPosition target;
                    target.setX(teammate->pos.getX() - 2);
                    target.setY(teammate->pos.getY() - 2);

                    return goToTarget(target);
                }
                else
                {
                    if (me.getDistanceTo(ball) > 1)
                    {
                        return goToTarget(ball);
                    }
                    else
                    {
                        if (ball.getX() <= -2)
                        {
                            return kickBall(KICK_DRIBBLE, VecPosition(0, 0, 0));
                        }
                        else
                        {
                            if (me.getDistanceTo(WO_TEAMMATE2) < me.getDistanceTo(WO_TEAMMATE3))
                            {
                                return kickBall(KICK_FORWARD, WO_TEAMMATE2);
                            }
                            else
                            {
                                return kickBall(KICK_FORWARD, WO_TEAMMATE3);
                            }
                        }
                    }
                }
            }
        }
        else
        {

            return goToTarget(VecPosition(-3, -1, 0));
        }

        break;
    }

    default:
        break;
    }
}

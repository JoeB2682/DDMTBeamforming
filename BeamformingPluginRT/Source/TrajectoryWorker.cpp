//===============================================================================
#include "trajectoryWorker.h"
//===============================================================================
trajectoryWorker::trajectoryWorker(std::shared_ptr<MotionTrackerHandler> mtrackhandler)
    : Thread("Trajectory Worker"),
    trackerhandler(std::move(mtrackhandler)),
    frameSize(35),
    updaterate(120)
{
    // Initialise position vectors
    xBuff.resize(frameSize, 0.0f);
    yBuff.resize(frameSize, 0.0f);
    zBuff.resize(frameSize, 0.0f);

    // Initialise velocity vectors
    vxBuff.resize(frameSize - 1, 0.0f);
    vyBuff.resize(frameSize - 1, 0.0f);
    vzBuff.resize(frameSize - 1, 0.0f);

    // Initialise trajectory vector
    trajectory.resize(frameSize, 0.0f);
}
//===============================================================================
trajectoryWorker::~trajectoryWorker()
{
    stopThread(1000);
}
//===============================================================================
// Trajectory vector Getter
std::vector<float> trajectoryWorker::getTrajectory() 
{
    return trajectory;
}
//===============================================================================
void trajectoryWorker::run()
{
    while (!threadShouldExit())
    {
        // Get current motion tracking position
        float x = trackerhandler->x.load();
        float y = trackerhandler->y.load();
        float z = trackerhandler->z.load();

        // Populate frame buffers
        xBuff.push_back(x);
        yBuff.push_back(y);
        zBuff.push_back(z);

        // Wait until frame is populated
        if (xBuff.size() >= frameSize)
        {
            // Calculate mean position
            float xsum = std::accumulate(xBuff.begin(), xBuff.end(), 0.0f);
            mx = xsum / xBuff.size();

            float ysum = std::accumulate(yBuff.begin(), yBuff.end(), 0.0f);
            my = ysum / yBuff.size();

            float zsum = std::accumulate(zBuff.begin(), zBuff.end(), 0.0f);
            mz = zsum / zBuff.size();

            // Calculate position variance
            float xVariance = 0.0f;
            float yVariance = 0.0f;
            float zVariance = 0.0f;

            for (int i = 0; i < xBuff.size(); i++)
            {
                xVariance += std::pow(xBuff[i] - mx, 2.0f);
                yVariance += std::pow(yBuff[i] - my, 2.0f);
                zVariance += std::pow(zBuff[i] - mz, 2.0f);
            }

            xVariance /= xBuff.size();
            yVariance /= yBuff.size();
            zVariance /= zBuff.size();

            // Position standard deviation
            sx = std::sqrt(xVariance);
            sy = std::sqrt(yVariance);
            sz = std::sqrt(zVariance);

            // Calculate velocity for each frame
            float dt = 1.0f / updaterate;

            vxBuff.clear();
            vyBuff.clear();
            vzBuff.clear();

            for (int i = 1; i < xBuff.size(); i++)
            {
                vxBuff.push_back((xBuff[i] - xBuff[i - 1]) / dt);
                vyBuff.push_back((yBuff[i] - yBuff[i - 1]) / dt);
                vzBuff.push_back((zBuff[i] - zBuff[i - 1]) / dt);
            }

            // Calculate mean velocity
            float vxsum = std::accumulate(vxBuff.begin(), vxBuff.end(), 0.0f);
            float vysum = std::accumulate(vyBuff.begin(), vyBuff.end(), 0.0f);
            float vzsum = std::accumulate(vzBuff.begin(), vzBuff.end(), 0.0f);

            mvx = vxsum / vxBuff.size();
            mvy = vysum / vyBuff.size();
            mvz = vzsum / vzBuff.size();

            // Calculate velocity variance
            float vxVariance = 0.0f;
            float vyVariance = 0.0f;
            float vzVariance = 0.0f;

            for (int i = 0; i < vxBuff.size(); i++)
            {
                vxVariance += std::pow(vxBuff[i] - mvx, 2.0f);
                vyVariance += std::pow(vyBuff[i] - mvy, 2.0f);
                vzVariance += std::pow(vzBuff[i] - mvz, 2.0f);
            }

            vxVariance /= vxBuff.size();
            vyVariance /= vyBuff.size();
            vzVariance /= vzBuff.size();

            // Velocity standard deviation
            svx = std::sqrt(vxVariance);
            svy = std::sqrt(vyVariance);
            svz = std::sqrt(vzVariance);

            // Populate 35-element trajectory vector
            // Same ordering as Python DatasetHandler

            trajectory.resize(35, 0.0f);

            // Start position
            trajectory[0] = xBuff[0];
            trajectory[1] = yBuff[0];
            trajectory[2] = zBuff[0];

            // Mean position
            trajectory[3] = mx;
            trajectory[4] = my;
            trajectory[5] = mz;

            // Position standard deviation
            trajectory[6] = sx;
            trajectory[7] = sy;
            trajectory[8] = sz;

            // Mean velocity
            trajectory[9] = mvx;
            trajectory[10] = mvy;
            trajectory[11] = mvz;

            // Velocity standard deviation
            trajectory[12] = svx;
            trajectory[13] = svy;
            trajectory[14] = svz;

            // Clear buffers for next trajectory
            xBuff.clear();
            yBuff.clear();
            zBuff.clear();

            vxBuff.clear();
            vyBuff.clear();
            vzBuff.clear();
        }
        wait(8);
    }
}
//===============================================================================
#include <ToyViewer.h>

#include <osgGA/TrackballManipulator>
#include <osg/os_utils>

namespace toy
{

int ToyViewer::run()
{
    if (!getCameraManipulator() && getCamera()->getAllowEventFocus())
    {
        setCameraManipulator(new osgGA::TrackballManipulator());
    }

    if (!isRealized())
    {
        realize();
    }

    unsigned int runTillFrameNumber = osg::UNINITIALIZED_FRAME_NUMBER;
    osg::getEnvVar("OSG_RUN_FRAME_COUNT", runTillFrameNumber);

    auto lastTick = osg::Timer::instance()->tick();
    double simulationTime = 0;

    while (!done() && (runTillFrameNumber == osg::UNINITIALIZED_FRAME_NUMBER ||
                          getViewerFrameStamp()->getFrameNumber() < runTillFrameNumber))
    {
        double minFrameTime = _runMaxFrameRate > 0.0 ? 1.0 / _runMaxFrameRate : 0.0;
        osg::Timer_t startFrameTick = osg::Timer::instance()->tick();
        if (!_pause || _debugSteps > 0)
        {
            auto dt = osg::Timer::instance()->delta_s(lastTick, startFrameTick);
            if (dt < 2)
            {
                simulationTime += dt;
                --_debugSteps;
            }
        }

        lastTick = startFrameTick;

        if (_runFrameScheme == ON_DEMAND)
        {
            if (checkNeedToDoFrame())
            {
                frame(simulationTime);
            }
            else
            {
                // we don't need to render a frame but we don't want to spin the run loop so
                // make sure the minimum loop time is 1/100th of second, if not otherwise
                // set, so enabling the frame microSleep below to avoid consume excessive
                // CPU resources.
                if (minFrameTime == 0.0)
                    minFrameTime = 0.01;
            }
        }
        else
        {
            frame(simulationTime);
        }

        // work out if we need to force a sleep to hold back the frame rate
        osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
        double frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
        if (frameTime < minFrameTime)
            OpenThreads::Thread::microSleep(
                static_cast<unsigned int>(1000000.0 * (minFrameTime - frameTime)));
    }

    return 0;
}

bool ViewerDebugHandler::handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    switch (ea.getEventType())
    {
        case osgGA::GUIEventAdapter::KEYDOWN:
            switch (ea.getKey())
            {
                case osgGA::GUIEventAdapter::KEY_F9:
                    _viewer->setPause(!_viewer->getPause());
                    break;

                case osgGA::GUIEventAdapter::KEY_F10:
                    _viewer->setDebugSteps(1);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return false;  // return true will stop event
}

}  // namespace toy
